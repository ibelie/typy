// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MetaList_TYPYTYPE(m)  (((TypyMetaList*)(m))->list_desc.desc_type)
#define MetaList_FIELDTYPE(m) (((TypyMetaList*)(m))->list_desc.desc_FieldType)

#define MetaList_GET(m, f) \
	(abstract_GetPyObject [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f)))
#define MetaList_TOJSON(m, f, s) \
	(abstract_ToJson      [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (s)))
#define MetaList_WRITE(m, f, t, o) \
	(abstract_Write       [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (t), (o)))
#define MetaList_BYTESIZE(m, f, t) \
	(abstract_ByteSize    [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (t)))

#define MetaList_MERGEFROM(m, ob, l, r) do { \
	register TypyField* _l = (TypyField*)(l);                                         \
	register TypyField old = TypyField_CopyFrom(MetaList_FIELDTYPE(m), *_l);          \
	TypyComposite_DEL_OWNER(MetaList_FIELDTYPE(m), *_l, (ob));                        \
	abstract_MergeFrom[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), _l, (r));         \
	TypyComposite_ADD_OWNER(MetaList_FIELDTYPE(m), *_l, (ob), FIELD_TYPE_LIST, 0);    \
	TypyComposite_NOTIFY((ob), FIELD_TYPE_LIST, 0, MetaList_FIELDTYPE(m), old, *_l);  \
} while (0)

#define MetaList_CLEAR(m, ob, f) do { \
	register TypyField* _f = (TypyField*)(f);                                         \
	register TypyField old = TypyField_CopyFrom(MetaList_FIELDTYPE(m), *_f);          \
	TypyComposite_DEL_OWNER(MetaList_FIELDTYPE(m), *_f, (ob));                        \
	abstract_Clear[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), _f);                  \
	TypyComposite_NOTIFY((ob), FIELD_TYPE_LIST, 0, MetaList_FIELDTYPE(m), old, NULL); \
} while (0)

#define MetaList_SET(m, ob, l, r) do { \
	register TypyField* _l = (TypyField*)(l);                                         \
	register TypyField old = TypyField_CopyFrom(MetaList_FIELDTYPE(m), *_l);          \
	TypyComposite_DEL_OWNER(MetaList_FIELDTYPE(m), *_l, (ob));                        \
	abstract_CopyFrom[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), _l, (r));          \
	TypyComposite_ADD_OWNER(MetaList_FIELDTYPE(m), *_l, (ob), FIELD_TYPE_LIST, 0);    \
	TypyComposite_NOTIFY((ob), FIELD_TYPE_LIST, 0, MetaList_FIELDTYPE(m), old, *_l);  \
} while (0)

#define MetaList_Clear(m, ob) { \
	register size_t i;                                   \
	for (i = 0; i < (ob)->list_length; i++) {            \
		MetaList_CLEAR((m), (ob), &(ob)->list_items[i]); \
	}                                                    \
	(ob)->list_length = 0;                               \
}

static inline bool MetaList_READ(TypyMetaList* type, TypyList* self, TypyField* item, byte** input, size_t* length) {
	register TypyField old = TypyField_CopyFrom(MetaList_FIELDTYPE(type), *item);
	TypyComposite_DEL_OWNER(MetaList_FIELDTYPE(type), *item, self);
	register bool result = abstract_Read[MetaList_FIELDTYPE(type)](MetaList_TYPYTYPE(type), item, input, length);
	if (result) {
		result = TypyComposite_ADD_OWNER(MetaList_FIELDTYPE(type), *item, self, FIELD_TYPE_LIST, 0);
		TypyComposite_NOTIFY(self, FIELD_TYPE_LIST, 0, MetaList_FIELDTYPE(type), old, *item);
	}
	return result;
}

static inline bool MetaList_CHECKSET(TypyMetaList* type, TypyList* self, TypyField* item, PyObject* arg, const char* err) {
	register TypyField old = TypyField_CopyFrom(MetaList_FIELDTYPE(type), *item);
	TypyComposite_DEL_OWNER(MetaList_FIELDTYPE(type), *item, self);
	register bool result = abstract_CheckAndSet[MetaList_FIELDTYPE(type)](MetaList_TYPYTYPE(type), item, arg, err);
	if (result) {
		result = TypyComposite_ADD_OWNER(MetaList_FIELDTYPE(type), *item, self, FIELD_TYPE_LIST, 0);
		TypyComposite_NOTIFY(self, FIELD_TYPE_LIST, 0, MetaList_FIELDTYPE(type), old, *item);
	}
	return result;
}

static inline bool MetaList_FROMJSON(TypyMetaList* type, TypyList* self, TypyField* item, PyObject* json) {
	register TypyField old = TypyField_CopyFrom(MetaList_FIELDTYPE(type), *item);
	TypyComposite_DEL_OWNER(MetaList_FIELDTYPE(type), *item, self);
	register bool result = abstract_FromJson[MetaList_FIELDTYPE(type)](MetaList_TYPYTYPE(type), item, json);
	if (result) {
		result = TypyComposite_ADD_OWNER(MetaList_FIELDTYPE(type), *item, self, FIELD_TYPE_LIST, 0);
		TypyComposite_NOTIFY(self, FIELD_TYPE_LIST, 0, MetaList_FIELDTYPE(type), old, *item);
	}
	return result;
}

#define TypyList_GET(ob, f)            MetaList_GET      (TypyList_TYPE(ob), (f))
#define TypyList_SET(ob, l, r)         MetaList_SET      (TypyList_TYPE(ob), (ob), (l), (r))
#define TypyList_CLEAR(ob, f)          MetaList_CLEAR    (TypyList_TYPE(ob), (ob), (f))
#define TypyList_CHECKSET(ob, l, r, e) MetaList_CHECKSET (TypyList_TYPE(ob), (ob), (l), (r), (e))
#define TypyList_Clear(ob)             MetaList_Clear    (TypyList_TYPE(ob), (ob))

//=============================================================================

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
		PyErr_Format(PyExc_RuntimeError, "Register List out of memory %zu.", sizeof(TypyMetaList) + nameLen);
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
		PyErr_Format(PyExc_RuntimeError, "Alloc List object out of memory %zu.", sizeof(TypyList));
		return NULL;
	}
	(void)PyObject_INIT(list, &TypyListType);
	Py_INCREF(type);
	TypyList_TYPE(list) = type;
	return list;
}

static TypyField* TypyList_EnsureSize(TypyList* self, size_t size) {
	if (self->list_length + size > self->list_capacity) {
		register size_t capacity = Ibl_Max(2 * self->list_capacity + size, MIN_LIST_CAPACITY);
		register TypyField* buffer = (TypyField*)calloc(capacity, sizeof(TypyField));
		if (!buffer) {
			PyErr_Format(PyExc_RuntimeError, "Alloc List buffer out of memory %zu.\n", capacity * sizeof(TypyField));
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

static bool TypyList_Extend(TypyList* self, PyObject* list) {
	if (PyList_CheckExact(list) || PyTuple_CheckExact(list) || (PyObject*)self == list) {
		list = PySequence_Fast(list, "argument must be iterable");
		if (!list) { return false; }
		register Py_ssize_t i, size = PySequence_Fast_GET_SIZE(list);
		if (size == 0) { Py_DECREF(list); return true; }
		register PyObject** src = PySequence_Fast_ITEMS(list);
		register TypyField* offset = TypyList_EnsureSize(self, size);
		if (!offset) { Py_DECREF(list); return false; }
		for (i = 0; i < size; i++) {
			if (!TypyList_CHECKSET(self, offset++, src[i], "List item type error: ")) {
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
		register bool success = TypyList_CHECKSET(self, offset++, item, "List item type error: ");
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
	TypyComposite_FREE(self);
	TypyList_Clear(self);
	if (self->list_items) {
		free(self->list_items);
	}
	Py_DECREF(TypyList_TYPE(self));
	free(self);
}

static PyObject* TypyList_Repr(TypyMetaList* type) {
	return PyString_FromFormat("<List '" FULL_MODULE_NAME ".%.100s'>", type->list_name);
}

//=============================================================================

#define TypyList_FromValueOrNew(s, v, t, r) \
	register TypyList* s = *(v);            \
	if (!s) {                               \
		s = TypyList_New(t);                \
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
	if (!MetaList_READ(type, self, offset, input, length)) {
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
			} else if (!MetaList_READ(type, self, offset, input, length)) {
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
			if (self->list_items[i]) {
				size += MetaList_WRITE(type, &self->list_items[i], tag, output + size);
			} else {
				size += Typy_WriteTag(output + size, tag);
				size += IblPutUvarint(output + size, 0);
			}
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
			if (self->list_items[i]) {
				size += MetaList_BYTESIZE(type, &self->list_items[i], tagsize);
			} else {
				size += tagsize + IblSizeVarint(0);
			}
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
		return TypyList_Extend(self, arg);
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
		MetaList_MERGEFROM(type, self, offset++, rvalue->list_items[i]);
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
			register PyObject* item = NULL;
			if ((*value)->list_items[i]) {
				item = MetaList_TOJSON(type, &(*value)->list_items[i], slim);
			}
			if (!item) {
				Py_INCREF(Py_None);
				item = Py_None;
			}
			PyList_SetItem(list, i, item);
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
		if (size == 0) { Py_DECREF(list); return true; }
		register TypyField* offset = TypyList_EnsureSize(self, size);
		if (!offset) { Py_DECREF(list); return false; }
		register PyObject** src = PySequence_Fast_ITEMS(list);
		for (i = 0; i < size; i++) {
			if (!MetaList_FROMJSON(type, self, offset++, src[i])) {
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
		register bool success = MetaList_FROMJSON(type, self, offset++, item);
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
		PyErr_Format(PyExc_IndexError, "List index (%zd) out of range (%zu).\n", index, self->list_length);
		return NULL;
	}
	return TypyList_GET(self, &self->list_items[index]);
}

static int list_AssignItem(TypyList* self, Py_ssize_t index, PyObject* arg) {
	if (index < 0) {
		index = self->list_length + index;
	}
	if (index < 0 || (size_t)index >= self->list_length) {
		PyErr_Format(PyExc_IndexError, "List index (%zd) out of range (%zu).\n", index, self->list_length);
		return -1;
	}
	if (!arg) {
		TypyList_CLEAR(self, &self->list_items[index]);
		for (; (size_t)index < self->list_length - 1; index++) {
			self->list_items[index] = self->list_items[index + 1];
		}
		self->list_items[index] = 0;
		self->list_length--;
		return 0;
	}
	return TypyList_CHECKSET(self, &self->list_items[index], arg, "List item type error: ") ? 0 : -1;
}

static PyObject* list_Extend(TypyList* self, PyObject* other) {
	if (!PyObject_Not(other) && Py_TYPE(other)->tp_as_sequence) {
		if (!TypyList_Extend(self, other)) {
			return NULL;
		}
	}
	Py_RETURN_NONE;
}

static PyObject* list_PyList(TypyList* self) {
	PyObject* list = PyList_New(self->list_length);
	if (!list) { return NULL; }
	register size_t i;
	for (i = 0; i < self->list_length; i++) {
		PyList_SET_ITEM(list, i, TypyList_GET(self, &self->list_items[i]));
	}
	return list;
}

static PyObject* list_Concat(TypyList* self, PyObject* other) {
	if (PyObject_TypeCheck(other, &TypyListType) && TypyList_TYPE(other) == TypyList_TYPE(self)) {
		register TypyList* rvalue = (TypyList*)other;
		register TypyList* list = TypyList_New(TypyList_TYPE(self));
		if (!list) { return NULL; }
		if (self->list_length + rvalue->list_length == 0) { return (PyObject*)list; }
		register TypyField* offset = TypyList_EnsureSize(list, self->list_length + rvalue->list_length);
		if (!offset) { Py_DECREF(list); return NULL; }
		register size_t i;
		for (i = 0; i < self->list_length; i++) {
			TypyList_SET(list, offset++, self->list_items[i]);
		}
		for (i = 0; i < rvalue->list_length; i++) {
			TypyList_SET(list, offset++, rvalue->list_items[i]);
		}
		return (PyObject*)list;
	} else {
		register PyObject* list = list_PyList(self);
		if (!list) { return NULL; }
		register PyObject* result = PySequence_InPlaceConcat(list, other);
		Py_XDECREF(result);
		return list;
	}
}

static PyObject* list_InplaceConcat(TypyList* self, PyObject* other) {
	if (!PyObject_Not(other) && Py_TYPE(other)->tp_as_sequence) {
		if (!TypyList_Extend(self, other)) {
			return NULL;
		}
	}
	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject* list_Repeat(TypyList* self, Py_ssize_t n) {
	register size_t size = self->list_length;
	if (n <= 0 || size == 0) {
		return (PyObject*)TypyList_New(TypyList_TYPE(self));
	} else if (n > 0 && size > (size_t)PY_SSIZE_T_MAX / n) {
		return PyErr_NoMemory();
	}
	register TypyList* list = TypyList_New(TypyList_TYPE(self));
	if (!list) { return NULL; }
	register TypyField* offset = TypyList_EnsureSize(list, size * n);
	if (!offset) { Py_DECREF(list); return NULL; }
	register Py_ssize_t i;
	for (i = 0; i < n; i++) {
		register size_t j;
		for (j = 0; j < size; j++) {
			TypyList_SET(list, offset++, self->list_items[j]);
		}
	}
	return (PyObject*)list;
}

static PyObject* list_InplaceRepeat(TypyList* self, Py_ssize_t n) {
	register size_t size = self->list_length;
	if (n < 1) { TypyList_Clear(self); }
	if (size == 0 || n <= 1) {
		Py_INCREF(self);
		return (PyObject*)self;
	} else if (size > (size_t)PY_SSIZE_T_MAX / n) {
		return PyErr_NoMemory();
	}
	register TypyField* offset = TypyList_EnsureSize(self, size * (n - 1));
	if (!offset) { return PyErr_NoMemory(); }
	register Py_ssize_t i;
	for (i = 1; i < n; i++) {
		register size_t j;
		for (j = 0; j < size; j++) {
			TypyList_SET(self, offset++, self->list_items[j]);
		}
	}
	Py_INCREF(self);
	return (PyObject*)self;
}

static int list_Contains(TypyList* self, PyObject* value) {
	register size_t i;
	register int cmp = 0;
	for (i = 0; cmp == 0 && i < self->list_length; i++) {
		register PyObject* item = TypyList_GET(self, &self->list_items[i]);
		cmp = PyObject_RichCompareBool(value, item, Py_EQ);
		Py_XDECREF(item);
	}
	return cmp;
}

static PyObject* list_Slice(TypyList* self, Py_ssize_t ilow, Py_ssize_t ihigh) {
	if (ilow < 0) {
		ilow = 0;
	} else if ((size_t)ilow > self->list_length) {
		ilow = self->list_length;
	}
	if (ihigh < ilow) {
		ihigh = ilow;
	} else if ((size_t)ihigh > self->list_length) {
		ihigh = self->list_length;
	}

	register TypyList* slice = TypyList_New(TypyList_TYPE(self));
	if (!slice) { return NULL; } else if (ihigh == ilow) { return (PyObject*)slice; }
	register TypyField* offset = TypyList_EnsureSize(slice, ihigh - ilow);
	if (!offset) { Py_DECREF(slice); return NULL; }
	register Py_ssize_t i;
	for (i = ilow; i < ihigh; i++) {
		TypyList_SET(slice, offset++, self->list_items[i]);
	}
	return (PyObject*)slice;
}

static int list_AssignSlice(TypyList* self, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject* value) {
	register PyObject* list = list_PyList(self);
	if (!list) { return -1; }
	if (PySequence_SetSlice(list, ilow, ihigh, value) < 0) {
		Py_DECREF(list);
		return -1;
	}

	TypyList_Clear(self);
	return TypyList_Extend(self, list) ? 0 : -1;
}

static PyObject* list_Subscript(TypyList* self, PyObject* slice) {
	Py_ssize_t from, to, step, slicelength;
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		return list_Item(self, PyInt_AsLong(slice));
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		return list_Item(self, PyLong_AsLong(slice));
	} else if (PySlice_Check(slice)) {
		register Py_ssize_t length = (Py_ssize_t)self->list_length;
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx((PySliceObject*)slice,
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return NULL;
		}

		register TypyList* slice = TypyList_New(TypyList_TYPE(self));
		if (!slice) { return NULL; }
		register Py_ssize_t i;
		if (from <= to) {
			if (step < 0) { return (PyObject*)slice; }
			for (i = from; i < to; i += step) {
				if (i < 0 || i >= length) { break; }
				register TypyField* offset = TypyList_EnsureSize(slice, 1);
				if (!offset) { Py_DECREF(slice); return NULL; }
				TypyList_SET(slice, offset, self->list_items[i]);
			}
		} else {
			if (step > 0) { return (PyObject*)slice; }
			for (i = from; i > to; i += step) {
				if (i < 0 || i >= length) { break; }
				register TypyField* offset = TypyList_EnsureSize(slice, 1);
				if (!offset) { Py_DECREF(slice); return NULL; }
				TypyList_SET(slice, offset, self->list_items[i]);
			}
		}
		return (PyObject*)slice;

	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return NULL;
	}
}

static int list_AssSubscript(TypyList* self, PyObject* slice, PyObject* value) {
	Py_ssize_t from, to, step, slicelength;
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		return list_AssignItem(self, PyInt_AsLong(slice), value);
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		return list_AssignItem(self, PyLong_AsLong(slice), value);
	} else if (PySlice_Check(slice)) {
		register Py_ssize_t length = self->list_length;
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx((PySliceObject*)slice,
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return -1;
		}
		return list_AssignSlice(self, from, to, value);
	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return -1;
	}
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
	self->list_items[i] = 0;
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
	self->list_items[i] = 0;
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

static PyObject* list_DeepCopy(TypyList* self, PyObject* args) {
	TypyList* list = NULL;
	register PyObject* json = TypyList_ToJson(TypyList_TYPE(self), &self, false);
	if (!json) { return NULL; }
	if (!TypyList_FromJson(TypyList_TYPE(self), &list, json)) {
		Py_XDECREF(list);
		list = NULL;
	}
	Py_DECREF(json);
	return (PyObject*)list;
}

static PyObject* list_Copy(PyTypeObject* cls, TypyList* self) {
	if (!PyObject_TypeCheck(self, &TypyListType)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to __copy__() must be instance of Typy List, bug got %.100s.",
			Py_TYPE(self)->tp_name);
		return NULL;
	}
	register TypyList* list = TypyList_New(TypyList_TYPE(self));
	if (!list) { return NULL; }
	register TypyField* offset = TypyList_EnsureSize(list, self->list_length);
	if (!offset) { Py_DECREF(list); return NULL; }
	register size_t i;
	for (i = 0; i < self->list_length; i++) {
		TypyList_SET(self, offset++, self->list_items[i]);
	}
	return (PyObject*)list;
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
	(lenfunc)list_Len,                      /* sq_length         */
	(binaryfunc)list_Concat,                /* sq_concat         */
	(ssizeargfunc)list_Repeat,              /* sq_repeat         */
	(ssizeargfunc)list_Item,                /* sq_item           */
	(ssizessizeargfunc)list_Slice,          /* sq_slice          */
	(ssizeobjargproc)list_AssignItem,       /* sq_ass_item       */
	(ssizessizeobjargproc)list_AssignSlice, /* sq_ass_slice      */
	(objobjproc)list_Contains,              /* sq_contains       */
	(binaryfunc)list_InplaceConcat,         /* sq_inplace_concat */
	(ssizeargfunc)list_InplaceRepeat,       /* sq_inplace_repeat */
};

static PyMappingMethods TypyList_MpMethods = {
	(lenfunc)list_Len,                /* mp_length        */
	(binaryfunc)list_Subscript,       /* mp_subscript     */
	(objobjargproc)list_AssSubscript, /* mp_ass_subscript */
};

static PyMethodDef TypyList_Methods[] = {
	{ "__copy__", (PyCFunction)list_Copy, METH_O | METH_CLASS,
		"Shallow copy the list." },
	{ "__deepcopy__", (PyCFunction)list_DeepCopy, METH_VARARGS,
		"Deep copy the list." },
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

#ifdef __cplusplus
}
#endif
