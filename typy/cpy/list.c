// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

TypyMetaList* Typy_RegisterList(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyMetaList* type;
	PyObject* descriptor;
	PyObject* typy_type = NULL;
	uint8 wire_type, field_type;
	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &descriptor)) {
		return NULL;
	}

	type = (TypyMetaList*)malloc(IBL_ALIGNED_SIZE(sizeof(TypyMetaList) + sizeof(char) * nameLen));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Register List out of memory %lu.", sizeof(TypyMetaList) + nameLen);
		return NULL;
	}

	type->list_name[nameLen] = 0;
	memcpy(type->list_name, name, nameLen);
	(void)PyObject_INIT(type, &TypyMetaListType);

	if (!PyArg_ParseTuple(descriptor, "BB|O", &wire_type, &field_type, &typy_type)) {
		free(type); return NULL;
	}
	type->list_desc.desc_type      = Meta_FromInitializer;
	type->list_desc.desc_FieldType = field_type;
	type->list_desc.desc_WireType  = wire_type;

	return type;
}

static TypyList* TypyList_New(TypyMetaList* type) {
	TypyList* list = (TypyList*)calloc(1, sizeof(TypyList));
	if (!list) {
		PyErr_Format(PyExc_RuntimeError, "Alloc List object out of memory %lu.", sizeof(TypyList));
		return NULL;
	}
	(void)PyObject_INIT(list, &TypyListType);
	TypyList_TYPE(list) = type;
	return list;
}

static TypyField* TypyList_EnsureSize(TypyList* self, size_t size) {
	if (self->list_length + size > self->list_capacity) {
		register size_t capacity = Ibl_Max(2 * self->list_capacity + size, MIN_LIST_CAPACITY);
		register TypyField* buffer = (TypyField*)calloc(capacity, sizeof(TypyField));
		if (!buffer) {
			PyErr_Format(PyExc_RuntimeError, "Alloc List buffer out of memory %lu.\n", capacity * sizeof(TypyField));
			return NULL;
		}
		self->list_capacity = capacity;
		if (self->list_items) {
			memcpy(buffer, self->list_items, self->list_length * sizeof(TypyField));
			free(self->list_items);
		}
		self->list_items = buffer;
	}
	register TypyField* offset = &self->list_items[self->list_length];
	self->list_length += size;
	return offset;
}

static bool MetaList_Extend(TypyMetaList* type, TypyList* self, PyObject* list) {
	if (PyList_CheckExact(list) || PyTuple_CheckExact(list) || (PyObject*)self == list) {
		list = PySequence_Fast(list, "argument must be iterable");
		if (!list) { return false; }
		register Py_ssize_t i, size = PySequence_Fast_GET_SIZE(list);
		if (size == 0) { Py_DECREF(list); return true; }
		register PyObject** src = PySequence_Fast_ITEMS(list);
		register TypyField* offset = TypyList_EnsureSize(self, size);
		if (!offset) { Py_DECREF(list); return false; }
		for (i = 0; i < size; i++) {
			if (!MetaList_CHECKSET(type, offset++, src[i], "List item type error: ")) {
				Py_DECREF(list);
				return false;
			}
		}
		Py_DECREF(list);
		return true;
	}
	register Py_ssize_t i, size = _PyObject_LengthHint(list, 0);
	if (size < 0) { return false; } else if (!size) { return true; }
	register PyObject* it = PyObject_GetIter(list);
	if (!it) { return false; }
	register iternextfunc iternext = *it->ob_type->tp_iternext;
	register TypyField* offset = TypyList_EnsureSize(self, size);
	if (!offset) { Py_DECREF(it); return false; }
	for (i = 0; i < size; i++) {
		register PyObject* item = iternext(it);
		register bool success = MetaList_CHECKSET(type, offset++, item, "List item type error: ");
		Py_XDECREF(item);
		if (!success) {
			Py_DECREF(it);
			return false;
		}
	}
	Py_DECREF(it);
	return true;
}

static void MetaList_Dealloc(TypyMetaList* type) {
	free(type);
}

static void TypyList_Dealloc(TypyList* self) {
	TypyList_Clear(self);
	if (self->list_items) {
		free(self->list_items);
	}
	free(self);
}

static PyObject* TypyList_Repr(TypyMetaList* type) {
	return PyString_FromFormat("<List '" FULL_MODULE_NAME ".%s'>", type->list_name);
}

//=============================================================================

#define TypyList_FromValueOrNew(s, v, t, r) \
	register TypyList* s = *(v);            \
	if (!s) {                               \
		s = (TypyList*)TypyList_New(t);     \
		if (!s) { return r; }               \
		*(v) = s;                           \
	}

TypyList* TypyList_GetPyObject(TypyMetaList* type, TypyList** value) {
	TypyList_FromValueOrNew(self, value, type, NULL);
	Py_INCREF(self);
	return self;
}

bool TypyList_ReadRepeated(TypyMetaList* type, TypyList** value, byte** input, size_t* length) {
	TypyList_FromValueOrNew(self, value, type, false);
	register TypyField* offset = TypyList_EnsureSize(self, 1);
	if (!offset) { return false; }
	if (!MetaList_READ(type, offset, input, length)) {
		return false;
	}
	return true;
}

bool TypyList_Read(TypyMetaList* type, TypyList** value, byte** input, size_t* length) {
	uint32 size;
	register TypyField* offset;
	if (MetaList_IsPrimitive(type)) {
		if (!Typy_ReadVarint32(input, length, &size)) {
			return false;
		} else if (!size) {
			return true;
		} else if (size > *length) {
			return false;
		}
		TypyList_FromValueOrNew(self, value, type, false);
		register byte* limit = *input + size;
		while (*input < limit) {
			if (!(offset = TypyList_EnsureSize(self, 1))) {
				return false;
			} else if (!MetaList_READ(type, offset, input, length)) {
				return false;
			}
		}
	} else {
		return TypyList_ReadRepeated(type, value, input, length);
	}
	return true;
}

size_t TypyList_Write(TypyMetaList* type, TypyList** value, int tag, byte* output) {
	register TypyList* self = *value;
	if (!self) { return 0; }
	register size_t i, size = 0;
	if (MetaList_IsPrimitive(type)) {
		size += Typy_WriteTag(output, tag);
		size += IblPutUvarint(output + size, self->cached_size);
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_WRITE(type, &self->list_items[i], 0, output + size);
		}
	} else {
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_WRITE(type, &self->list_items[i], tag, output + size);
		}
	}
	return size;
}

size_t TypyList_ByteSize(TypyMetaList* type, TypyList** value, int tagsize) {
	register TypyList* self = *value;
	if (!self) { return 0; }
	register size_t i, size = 0;
	if (MetaList_IsPrimitive(type)) {
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_BYTESIZE(type, &self->list_items[i], 0);
		}
		self->cached_size = size;
		size += tagsize + IblSizeVarint(size);
	} else {
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_BYTESIZE(type, &self->list_items[i], tagsize);
		}
	}
	return size;
}

bool TypyList_CheckAndSet(TypyMetaList* type, TypyList** value, PyObject* arg, const char* err) {
	if (!arg || arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	} else if (PyObject_TypeCheck(arg, &TypyListType) && TypyList_TYPE(arg) == type) {
		Py_XDECREF(*value);
		Py_INCREF(arg);
		*value = (TypyList*)arg;
		return true;
	} else if (PySequence_Check(arg)) {
		TypyList_FromValueOrNew(self, value, type, false);
		MetaList_Clear(type, self);
		return MetaList_Extend(type, self, arg);
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

void TypyList_MergeFrom(TypyMetaList* type, TypyList** lvalue, TypyList* rvalue) {
	if (!rvalue) { return; }
	TypyList_FromValueOrNew(self, lvalue, type, );
	register TypyField* offset = TypyList_EnsureSize(self, rvalue->list_length);
	if (!offset) { return; }
	register size_t i;
	for (i = 0; i < rvalue->list_length; i++) {
		MetaList_MERGEFROM(type, offset++, rvalue->list_items[i]);
	}
}

PyObject* TypyList_ToJson(TypyMetaList* type, TypyList** value, bool slim) {
	if (!slim && !(*value)) {
		return PyList_New(0);
	} else if (*value) {
		register size_t i;
		register PyObject* list = PyList_New((*value)->list_length);
		if (!list) { return NULL; }
		for (i = 0; i < (*value)->list_length; i++) {
			PyList_SetItem(list, i, MetaList_TOJSON(type, &(*value)->list_items[i], slim));
		}
		return list;
	} else {
		return NULL;
	}
}

bool TypyList_FromJson(TypyMetaList* type, TypyList** value, PyObject* json) {
	TypyList_FromValueOrNew(self, value, type, false);
	if (!json || json == Py_None) { return true; }
	if (PyList_CheckExact(json) || PyTuple_CheckExact(json)) {
		register PyObject* list = PySequence_Fast(json, "argument must be iterable");
		if (!list) { return false; }
		register Py_ssize_t i, size = PySequence_Fast_GET_SIZE(list);
		register TypyField* offset = TypyList_EnsureSize(self, size);
		if (!offset) { Py_DECREF(list); return false; }
		register PyObject** src = PySequence_Fast_ITEMS(list);
		for (i = 0; i < size; i++) {
			if (!MetaList_FROMJSON(type, offset++, src[i])) {
				Py_DECREF(list);
				return false;
			}
		}
		Py_DECREF(list);
		return true;
	}
	register Py_ssize_t i, size = _PyObject_LengthHint(json, 0);
	if (size < 0) { return false; } else if (!size) { return true; }
	register PyObject* it = PyObject_GetIter(json);
	if (!it) { return false; }
	register iternextfunc iternext = *it->ob_type->tp_iternext;
	register TypyField* offset = TypyList_EnsureSize(self, size);
	if (!offset) { Py_DECREF(it); return false; }
	for (i = 0; i < size; i++) {
		register PyObject* item = iternext(it);
		register bool success = MetaList_FROMJSON(type, offset++, item);
		Py_XDECREF(item);
		if (!success) {
			Py_DECREF(it);
			return false;
		}
	}
	Py_DECREF(it);
	return true;
}

PyTypeObject TypyMetaListType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaList",            /* tp_name           */
	sizeof(TypyMetaList),                    /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)MetaList_Dealloc,            /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	(reprfunc)TypyList_Repr,                 /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	PyObject_HashNotImplemented,             /* tp_hash           */
	0,                                       /* tp_call           */
	(reprfunc)TypyList_Repr,                 /* tp_str            */
	0,                                       /* tp_getattro       */
	0,                                       /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy List Metaclass",               /* tp_doc            */
};

//=============================================================================

static PyObject* list_Append(TypyList* self, PyObject* item) {
	register TypyField* offset = TypyList_EnsureSize(self, 1);
	if (!offset) { return NULL; }
	if (!TypyList_CHECKSET(self, offset, item, "List item type error: ")) {
		return NULL;
	}
	Py_RETURN_NONE;
}

static Py_ssize_t list_Len(TypyList* self) {
	return self->list_length;
}

static PyObject* list_Item(TypyList* self, Py_ssize_t index) {
	if (index < 0) {
		index = self->list_length + index;
	}
	if (index < 0 || (size_t)index >= self->list_length) {
		PyErr_Format(PyExc_IndexError, "List index (%ld) out of range (%lu).\n", index, self->list_length);
		return NULL;
	}
	return TypyList_GET(self, &self->list_items[index]);
}

static int list_AssignItem(TypyList* self, Py_ssize_t index, PyObject* arg) {
	if (index < 0) {
		index = self->list_length + index;
	}
	if (index < 0 || (size_t)index >= self->list_length) {
		PyErr_Format(PyExc_IndexError, "List index (%ld) out of range (%lu).\n", index, self->list_length);
		return -1;
	}
	return TypyList_CHECKSET(self, &self->list_items[index], arg, "List item type error: ") ? 0 : -1;
}

static PyObject* list_Extend(TypyList* self, PyObject* value) {
	if (!PyObject_Not(value) && Py_TYPE(value)->tp_as_sequence) {
		MetaList_Extend(TypyList_TYPE(self), self, value);
	}
	Py_RETURN_NONE;
}

static PyObject* list_Subscript(TypyList* self, PyObject* slice) {
	Py_ssize_t from;
	Py_ssize_t to;
	Py_ssize_t step;
	Py_ssize_t length;
	Py_ssize_t slicelength;
	bool return_list = false;
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		from = to = PyInt_AsLong(slice);
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		from = to = PyLong_AsLong(slice);
	} else if (PySlice_Check(slice)) {
		length = self->list_length;
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx((PySliceObject*)slice,
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return NULL;
		}
		return_list = true;
	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return NULL;
	}

	if (!return_list) {
		return list_Item(self, from);
	}

	PyObject* list = PyList_New(0);
	register Py_ssize_t index;
	if (!list) { return NULL; }
	if (from <= to) {
		if (step < 0) {
			return list;
		}
		for (index = from; index < to; index += step) {
			if (index < 0 || index >= length) {
				break;
			}
			register PyObject* s = list_Item(self, index);
			PyList_Append(list, s);
			Py_XDECREF(s);
		}
	} else {
		if (step > 0) {
			return list;
		}
		for (index = from; index > to; index += step) {
			if (index < 0 || index >= length) {
				break;
			}
			register PyObject* s = list_Item(self, index);
			PyList_Append(list, s);
			Py_XDECREF(s);
		}
	}
	return list;
}

static int list_AssSubscript(TypyList* self, PyObject* slice, PyObject* value) {
	Py_ssize_t from;
	Py_ssize_t to;
	Py_ssize_t step;
	Py_ssize_t length;
	Py_ssize_t slicelength;
	bool create_list = false;

#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		from = to = PyInt_AsLong(slice);
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		from = to = PyLong_AsLong(slice);
	} else if (PySlice_Check(slice)) {
		length = self->list_length;
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx((PySliceObject*)slice,
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return -1;
		}
		create_list = true;
	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return -1;
	}

	if (!create_list) {
		return list_AssignItem(self, from, value);
	}

	register PyObject* full_slice = PySlice_New(NULL, NULL, NULL);
	register PyObject* new_list = list_Subscript(self, full_slice);
	if (PySequence_SetSlice(new_list, from, to, value) < 0) {
		goto list_asssubscript_fail;
	}
	TypyList_Clear(self);
	if (!MetaList_Extend(TypyList_TYPE(self), self, new_list)) {
		goto list_asssubscript_fail;
	}
	Py_XDECREF(full_slice);
	Py_XDECREF(new_list);
	return 0;

list_asssubscript_fail:
	Py_XDECREF(full_slice);
	Py_XDECREF(new_list);
	return -1;
}

static PyObject* list_Insert(TypyList* self, PyObject* args) {
	Py_ssize_t i, index;
	PyObject* value;
	if (!PyArg_ParseTuple(args, "nO", &index, &value)) {
		return NULL;
	}
	if (!TypyList_EnsureSize(self, 1)) { return NULL; }
	for (i = self->list_length - 1; i > index; i--) {
		self->list_items[i] = self->list_items[i - 1];
	}
	if (!TypyList_CHECKSET(self, &self->list_items[index], value, "List item type error: ")) {
		return NULL;
	}
	Py_RETURN_NONE;
}

static PyObject* list_Remove(TypyList* self, PyObject* value) {
	register size_t i;
	for (i = 0; i < self->list_length; i++) {
		register PyObject* item = TypyList_GET(self, &self->list_items[i]);
		register int eq = PyObject_RichCompareBool(item, value, Py_EQ);
		Py_XDECREF(item);
		if (eq) { break; }
	}
	if (i == self->list_length) {
		PyErr_SetString(PyExc_ValueError, "remove(x) - x not in container");
		return NULL;
	}
	TypyList_CLEAR(self, &self->list_items[i]);
	for (; i < self->list_length - 1; i++) {
		self->list_items[i] = self->list_items[i + 1];
	}
	self->list_length--;
	Py_RETURN_NONE;
}

static PyObject* list_Pop(TypyList* self, PyObject* args) {
	Py_ssize_t i = -1;
	if (!PyArg_ParseTuple(args, "|n", &i)) {
		return NULL;
	} else if (i < 0 || (size_t)i >= self->list_length) {
		PyErr_SetString(PyExc_ValueError, "pop(i) - i not in container");
		return NULL;
	}
	register PyObject* item = TypyList_GET(self, &self->list_items[i]);
	TypyList_CLEAR(self, &self->list_items[i]);
	for (; (size_t)i < self->list_length - 1; i++) {
		self->list_items[i] = self->list_items[i + 1];
	}
	self->list_length--;
	return item;
}

static PyObject* list_Repr(TypyList* self) {
	register PyObject* json = TypyList_ToJson(TypyList_TYPE(self), &self, false);
	if (!json) { return NULL; }
	register PyObject* repr = PyObject_Repr(json);
	Py_DECREF(json);
	return repr;
}

//=============================================================================

static TypyListIterator* list_Iter(TypyList* self) {
	TypyListIterator* it = (TypyListIterator*) PyType_GenericAlloc(&TypyListIteratorType, 0);
	if (!it) { return NULL; }
	it->it_index = 0;
	Py_INCREF(self);
	it->it_seq = self;
	return it;
}

static void iter_Dealloc(TypyListIterator* it)
{
	Py_XDECREF(it->it_seq);
	Py_TYPE(it)->tp_free(it);
}

static PyObject* iter_Len(TypyListIterator* it)
{
	Py_ssize_t len;
	if (it->it_seq) {
		len = it->it_seq->list_length - it->it_index;
		if (len >= 0) { return PyInt_FromSsize_t(len); }
	}
	return PyInt_FromLong(0);
}

static int iter_Traverse(TypyListIterator* it, visitproc visit, void* arg)
{
	Py_VISIT(it->it_seq);
	return 0;
}

static PyObject* iter_Next(TypyListIterator* it)
{
	assert(it);
	TypyList* seq = it->it_seq;
	if (!seq) { return NULL; }
	if (it->it_index < seq->list_length) {
		return list_Item(seq, it->it_index++);
	}
	it->it_seq = NULL;
	Py_DECREF(seq);
	return NULL;
}

static PySequenceMethods TypyList_SqMethods = {
	(lenfunc)list_Len,                /* sq_length   */
	0,                                /* sq_concat   */
	0,                                /* sq_repeat   */
	(ssizeargfunc)list_Item,          /* sq_item     */
	0,                                /* sq_slice    */
	(ssizeobjargproc)list_AssignItem, /* sq_ass_item */
};

static PyMappingMethods TypyList_MpMethods = {
	(lenfunc)list_Len,                /* mp_length        */
	(binaryfunc)list_Subscript,       /* mp_subscript     */
	(objobjargproc)list_AssSubscript, /* mp_ass_subscript */
};

static PyMethodDef TypyList_Methods[] = {
	{ "append", (PyCFunction)list_Append, METH_O,
		"Appends an object to the list." },
	{ "extend", (PyCFunction)list_Extend, METH_O,
		"Appends objects to the list." },
	{ "insert", (PyCFunction)list_Insert, METH_VARARGS,
		"Appends objects to the list." },
	{ "pop", (PyCFunction)list_Pop, METH_VARARGS,
		"Removes an object from the list and returns it." },
	{ "remove", (PyCFunction)list_Remove, METH_O,
		"Removes an object from the list." },
	{ NULL, NULL }
};

PyTypeObject TypyListType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".List",                 /* tp_name           */
	sizeof(TypyList),                         /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)TypyList_Dealloc,             /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	0,                                        /* tp_compare        */
	(reprfunc)list_Repr,                      /* tp_repr           */
	0,                                        /* tp_as_number      */
	&TypyList_SqMethods,                      /* tp_as_sequence    */
	&TypyList_MpMethods,                      /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	0,                                        /* tp_call           */
	(reprfunc)list_Repr,                      /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy List",                            /* tp_doc            */
	0,                                        /* tp_traverse       */
	0,                                        /* tp_clear          */
	0,                                        /* tp_richcompare    */
	0,                                        /* tp_weaklistoffset */
	(getiterfunc)list_Iter,                   /* tp_iter           */
	0,                                        /* tp_iternext       */
	TypyList_Methods,                         /* tp_methods        */
	0,                                        /* tp_members        */
	0,                                        /* tp_getset         */
	0,                                        /* tp_base           */
	0,                                        /* tp_dict           */
	0,                                        /* tp_descr_get      */
	0,                                        /* tp_descr_set      */
	0,                                        /* tp_dictoffset     */
	0,                                        /* tp_init           */
};

PyMethodDef TypyList_IteratorMethods[] = {
	{ "__length_hint__", (PyCFunction)iter_Len, METH_NOARGS,
		"Private method returning an estimate of len(list(it))." },
	{ NULL, NULL }
};

PyTypeObject TypyListIteratorType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".List.Iterator",            /* tp_name           */
	sizeof(TypyListIterator),                     /* tp_basicsize      */
	0,                                            /* tp_itemsize       */
	(destructor)iter_Dealloc,                     /* tp_dealloc        */
	0,                                            /* tp_print          */
	0,                                            /* tp_getattr        */
	0,                                            /* tp_setattr        */
	0,                                            /* tp_compare        */
	0,                                            /* tp_repr           */
	0,                                            /* tp_as_number      */
	0,                                            /* tp_as_sequence    */
	0,                                            /* tp_as_mapping     */
	0,                                            /* tp_hash           */
	0,                                            /* tp_call           */
	0,                                            /* tp_str            */
	PyObject_GenericGetAttr,                      /* tp_getattro       */
	0,                                            /* tp_setattro       */
	0,                                            /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                           /* tp_flags          */
	"A Typy List Iterator",                       /* tp_doc            */
	(traverseproc)iter_Traverse,                  /* tp_traverse       */
	0,                                            /* tp_clear          */
	0,                                            /* tp_richcompare    */
	0,                                            /* tp_weaklistoffset */
	PyObject_SelfIter,                            /* tp_iter           */
	(iternextfunc)iter_Next,                      /* tp_iternext       */
	TypyList_IteratorMethods,                     /* tp_methods        */
	0,                                            /* tp_members        */
};
