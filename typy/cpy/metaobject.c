// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

inline PyTypeObject* _InheritTypyObjectType(void) {
	register PyTypeObject* type = (PyTypeObject*)malloc(sizeof(PyTypeObject));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Inherit TypyObjectType out of memory %lu.", sizeof(PyTypeObject));
		return NULL;
	}
	memcpy(type, &BaseTypyObjectType, sizeof(PyTypeObject));
	type->tp_base = TypyObjectType;
	if (PyType_Ready(type) < 0) {
		free(type);
		return NULL;
	}
	return type;
}

TypyObject* Typy_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	TypyObject* object = (TypyObject*)calloc(1, sizeof(TypyObject) + sizeof(TypyField) * type->meta_size);
	if (!object) {
		PyErr_Format(PyExc_RuntimeError, "Alloc Object out of memory %lu.", sizeof(TypyObject) + sizeof(TypyField) * type->meta_size);
		return NULL;
	}
	(void)PyObject_INIT(object, type->py_type);
	Typy_TYPE(object) = type;
	if (kwargs) {
		while (PyDict_Next(kwargs, &pos, &k, &v)) {
			if (PyObject_SetAttr((PyObject*)object, k, v) == -1) {
				break;
			}
		}
	}
	return object;
}

inline int Meta_PropertyIndex(TypyMetaObject* type, char* key) {
	register TypyFieldMap field = (TypyFieldMap)IblMap_Get(type->meta_field2index, &key);
	return field ? field->index : -1;
}

inline char* Meta_PropertyName(TypyMetaObject* type, int index) {
	if (index < 0 || (size_t)index > type->meta_size) {
		return NULL;
	} else {
		return type->meta_index2field[index];
	}
}

//=============================================================================

inline TypyMetaObject* _Typy_RegisterMeta(PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	PyObject* descriptors;
	size_t i, meta_size;
	PyObject* typy_type;
	uint8 tagsize, wire_type, field_type;
	uint32 tag;

	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &descriptors)) {
		return NULL;
	} else if (!PySequence_Check(descriptors)) {
		FormatTypeError(descriptors, "RegisterMeta descriptors expect sequence type, but ");
		return NULL;
	} else if ((meta_size = PySequence_Size(descriptors)) < 0) {
		FormatTypeError(descriptors, "RegisterMeta descriptors cannot get sequence size, and ");
		return NULL;
	}

	register size_t size = sizeof(TypyMetaObject) + sizeof(TypyDescriptor) * meta_size + nameLen;
	register TypyMetaObject* type = (TypyMetaObject*)malloc(size);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Register Meta out of memory %lu.", size);
		return NULL;
	}

	type->meta_size = meta_size;
	Meta_NAME(type)[nameLen] = 0;
	memcpy(Meta_NAME(type), name, nameLen);
	(void)PyObject_INIT(type, &TypyMetaObjectType);
	type->meta_index2field = (char**)malloc(meta_size * sizeof(char*));
	if (!type->meta_index2field) {
		PyErr_Format(PyExc_RuntimeError, "Register Meta index2field out of memory %lu.", meta_size * sizeof(char*));
		Py_DECREF(type); return NULL;
	}
	type->meta_field2index = TypyFieldMap_New();
	if (!type->meta_field2index) {
		PyErr_Format(PyExc_RuntimeError, "Register Meta field2index out of memory.");
		Py_DECREF(type); return NULL;
	}

	register uint32 max_tag = 0;
	for (i = 0; i < meta_size; i++) {
		register PyObject* item = PySequence_GetItem(descriptors, i);
		typy_type = NULL;
		if (!PyArg_ParseTuple(item, "sIBBB|O", &name, &tag, &tagsize, &wire_type, &field_type, &typy_type)) {
			Py_XDECREF(item);
			Py_DECREF(type);
			return NULL;
		}
		max_tag = Ibl_Max(max_tag, tag);
		type->meta_descriptor[i].desc_tag       = tag;
		type->meta_descriptor[i].desc_tagsize   = tagsize;
		type->meta_descriptor[i].desc_WireType  = wire_type;
		type->meta_descriptor[i].desc_FieldType = field_type;
		type->meta_descriptor[i].desc_type      = Meta_FromInitializer;
		register TypyFieldMap field = (TypyFieldMap)IblMap_Set(type->meta_field2index, &name);
		if (!field) {
			PyErr_Format(PyExc_RuntimeError, "Register Meta cannot set field2index.");
			Py_XDECREF(item);
			Py_DECREF(type);
			return NULL;
		}
		field->index = i;
		type->meta_index2field[i] = field->key;
		Py_XDECREF(item);
	}
	type->meta_cutoff = max_tag <= 0x7F ? 0x7F : (max_tag <= 0x3FFF ? 0x3FFF : max_tag);

	return type;
}

inline void Typy_Clear(TypyObject* self) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (Typy_FIELD(self, i)) {
			Typy_CLEAR(self, i);
		}
	}
}

void Typy_Dealloc(TypyObject* self) {
	Typy_Clear(self);
	free(self);
}

inline void Typy_MergeFrom(TypyObject* self, TypyObject* from) {
	if (from == self) { return; }
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_MERGEFROM(self, i, Typy_FIELD(from, i));
	}
}

inline void Typy_CopyFrom(TypyObject* self, TypyObject* from) {
	if (from == self) { return; }
	Typy_Clear(self);
	Typy_MergeFrom(self, from);
}

inline size_t Typy_ByteSize(TypyObject* self) {
	register size_t size = 0, i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (!Typy_TAG(self, i) || !Typy_FIELD(self, i)) { continue; }
		size += Typy_BYTESIZE(self, i, Typy_TAGSIZE(self, i));
	}
	return size;
}

inline void Typy_SerializeString(TypyObject* self, byte* output) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (!Typy_TAG(self, i) || !Typy_FIELD(self, i)) { continue; }
		output += Typy_WRITE(self, i, Typy_TAG(self, i), output);
	}
}

inline size_t Typy_MergeFromString(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	for (;;) {
		if (!Typy_ReadTag(&input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
			goto handle_unusual;
		}
		register int index = TAG_INDEX(tag);
		if (index < 0 || (size_t)index >= Typy_SIZE(self)) { goto handle_unusual; }
		if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
			if (!Typy_READ(self, index, &input, &remain)) {
				return 0;
			}
		} else if (TAG_WIRETYPE(tag) == MetaList_WIRETYPE(Typy_TYPYTYPE(self, index))) {
			if (!TypyList_ReadRepeated(Typy_TYPYTYPE(self, index), (TypyList**)&Typy_FIELD(self, index), &input, &remain)) {
				return 0;
			}
		}

		if (!remain) {
			return length;
		} else {
			continue;
		}

	handle_unusual:
		if (tag == 0) { return length - remain; }
		if (!Typy_SkipField(&input, &remain, tag)) { return 0; }
	}
}

inline size_t Typy_PropertyByteSize(TypyObject* self, int index) {
	if (!Typy_TAG(self, index)) { return 0; }
	if (Typy_FIELD(self, index)) {
		return Typy_BYTESIZE(self, index, Typy_TAGSIZE(self, index));
	} else {
		return Typy_TAGSIZE(self, index);
	}
}

inline void Typy_SerializeProperty(TypyObject* self, byte* output, int index) {
	if (!Typy_TAG(self, index)) { return; }
	if (Typy_FIELD(self, index)) {
		Typy_WRITE(self, index, Typy_TAG(self, index), output);
	} else {
		Typy_WriteTag(output, Typy_TAG(self, index));
	}
}

inline int Typy_DeserializeProperty(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	register int index = -1;
	for (;;) {
		if (!Typy_ReadTag(&input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
			goto handle_unusual;
		} else if (index >= 0 && index != TAG_INDEX(tag)) {
			goto handle_unusual;
		}
		index = TAG_INDEX(tag);
		if (index < 0 || (size_t)index >= Typy_SIZE(self)) { goto handle_unusual; }
		Typy_CLEAR(self, index);
		if (!remain) { break; }
		if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
			if (!Typy_READ(self, index, &input, &remain)) {
				goto handle_unusual;
			}
		} else if (TAG_WIRETYPE(tag) == MetaList_WIRETYPE(Typy_TYPYTYPE(self, index))) {
			if (!TypyList_ReadRepeated(Typy_TYPYTYPE(self, index), (TypyList**)&Typy_FIELD(self, index), &input, &remain)) {
				goto handle_unusual;
			}
		}

		if (!remain) {
			break;
		} else {
			continue;
		}

	handle_unusual:
		if (tag == 0 || !Typy_SkipField(&input, &remain, tag)) { break; }
	}
	return index;
}

PyObject* Typy_GetAttr(TypyObject* self, PyObject* arg) {
	register PyBytes name = Typy_CheckBytes(arg, "GetAttr expect property name, but ");
	if (name) {
		register int index = Typy_PropertyIndex(self, PyBytes_AS_STRING(name));
		Py_DECREF(name);
		if (index >= 0) {
			return Typy_GET(self, index);
		}
	}
	return PyObject_GenericGetAttr((PyObject*)self, arg);
}

int Typy_SetAttr(TypyObject* self, PyObject* arg, PyObject* value) {
	register PyBytes name = Typy_CheckBytes(arg, "SetAttr expect property name, but ");
	if (name) {
		register int index = Typy_PropertyIndex(self, PyBytes_AS_STRING(name));
		Py_DECREF(name);
		if (index >= 0) {
			return Typy_CHECKSET(self, index, value, "SetAttr ") ? 0 : -1;
		}
	}
	return PyObject_GenericSetAttr((PyObject*)self, arg, value);
}

//=============================================================================

bool TypyObject_CheckAndSet(TypyMetaObject* type, TypyObject** value, PyObject* arg, const char* err) {
	if (arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	}
	if (!Typy_TypeCheck(arg, type)) {
		FormatTypeError(arg, err);
		return false;
	}
	Py_XDECREF(*value);
	Py_INCREF(arg);
	*value = (TypyObject*)arg;
	return true;
}

size_t TypyObject_ByteSize(TypyMetaObject* type, TypyObject** value, int tagsize) {
	register TypyObject* self = *value;
	register size_t size = 0;
	if (self) {
		size = Typy_ByteSize(self);
		self->cached_size = size;
	}
	return tagsize + IblSizeVarint(size) + size;
}

size_t TypyObject_Write(TypyMetaObject* type, TypyObject** value, int tag, byte* output) {
	register TypyObject* self = *value;
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	size += IblPutUvarint(output + size, self->cached_size);
	if (self->cached_size) {
		Typy_SerializeString(self, output + size);
	}
	return size + self->cached_size;
}

#define TypyObject_FromValueOrNew(s, v, t, r) \
	register TypyObject* s = *(v);                \
	if (!s) {                                     \
		s = (TypyObject*)Typy_New(t, NULL, NULL); \
		if (!s) { return r; }                     \
		*(v) = s;                                 \
	}

bool TypyObject_Read(TypyMetaObject* type, TypyObject** value, byte** input, size_t* length) {
	uint32 limit;
	if (!Typy_ReadVarint32(input, length, &limit)) {
		return false;
	} else if (limit > *length) {
		return false;
	}
	TypyObject_FromValueOrNew(self, value, type, false);
	if (!limit) { return true; }
	register size_t size = Typy_MergeFromString(self, *input, limit);
	*input  += limit;
	*length -= limit;
	return size == limit;
}

void TypyObject_MergeFrom(TypyMetaObject* type, TypyObject** lvalue, TypyObject* rvalue) {
	if (!rvalue) { return; }
	TypyObject_FromValueOrNew(self, lvalue, type, )
	Typy_MergeFrom(self, rvalue);
}

PyObject* Py_CopyFrom(TypyObject* self, PyObject* arg) {
	register TypyObject* from = (TypyObject*)arg;
	if (self == from) {
		Py_RETURN_NONE;
	}
	if (Typy_TYPE(self) != Typy_TYPE(from)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to CopyFrom() must be instance of same class: "
			"expected %.100s got %.100s(%.100s).",
			Typy_NAME(self), Py_TYPE(arg)->tp_name, Typy_NAME(from));
		return NULL;
	}
	Typy_CopyFrom(self, from);
	Py_RETURN_NONE;
}

PyObject* Py_MergeFrom(TypyObject* self, PyObject* arg) {
	register TypyObject* from = (TypyObject*)arg;
	if (self == from) {
		Py_RETURN_NONE;
	}
	if (Typy_TYPE(self) != Typy_TYPE(from)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to CopyFrom() must be instance of same class: "
			"expected %.100s got %.100s(%.100s).",
			Typy_NAME(self), Py_TYPE(arg)->tp_name, Typy_NAME(from));
		return NULL;
	}
	Typy_MergeFrom(self, from);
	Py_RETURN_NONE;
}

PyObject* Py_SerializeString(TypyObject* self) {
	size_t size = Typy_ByteSize(self);
	if (size <= 0) {
		return PyBytes_FromString("");
	}
	PyObject* result = PyBytes_FromStringAndSize(NULL, size);
	if (!result) { return NULL; }
	Typy_SerializeString(self, (byte*)PyBytes_AS_STRING(result));
	return result;
}

PyObject* Py_MergeFromString(TypyObject* self, PyObject* arg) {
	const void* data;
	Py_ssize_t size;
	if (PyObject_AsReadBuffer(arg, &data, &size) < 0) {
		return NULL;
	}
	if ((size = Typy_MergeFromString(self, (byte*)data, size)) > 0) {
		return PyInt_FromLong(size);
	} else {
		PyErr_Format(PyExc_RuntimeError, "Error parsing object");
		return NULL;
	}
}

PyObject* Py_SerializeProperty(TypyObject* self, PyObject* arg) {
	register PyBytes name = Typy_CheckBytes(arg, "SerializeProperty expect property name, but ");
	if (!name) { return NULL; }
	register int index = Typy_PropertyIndex(self, PyBytes_AS_STRING(name));
	if (index < 0) {
		FormatTypeError(arg, "SerializeProperty expect property name, but ");
		Py_DECREF(name);
		return NULL;
	}
	register size_t size = Typy_PropertyByteSize(self, index);
	if (size <= 0) {
		PyErr_Format(PyExc_RuntimeError, "Error serializing object");
		Py_DECREF(name);
		return NULL;
	}
	register PyObject* result = PyBytes_FromStringAndSize(NULL, size);
	if (!result) {
		Py_DECREF(name);
		return NULL;
	}
	Typy_SerializeProperty(self, (byte*)PyBytes_AS_STRING(result), index);
	Py_DECREF(name);
	return result;
}

PyObject* Py_DeserializeProperty(TypyObject* self, PyObject* arg) {
	const void* data;
	Py_ssize_t size;
	if (PyObject_AsReadBuffer(arg, &data, &size) < 0) {
		return NULL;
	}
	register int index = Typy_DeserializeProperty(self, (byte*)data, size);
	if (index < 0) {
		PyErr_Format(PyExc_RuntimeError, "Error deserializing object");
		return NULL;
	} else {
		return PyBytes_FromString(Typy_PropertyName(self, index));
	}
}

PyObject* Py_Clear(TypyObject* self) {
	Typy_Clear(self);
	Py_RETURN_NONE;
}

PyObject* Py_ParseFromPyString(TypyObject* self, PyObject* arg) {
	Typy_Clear(self);
	return Py_MergeFromString(self, arg);
}

// ===================================================================

PyTypeObject* TypyObjectType = NULL;

void TypyMeta_Dealloc(TypyMetaObject* type) {
	if (type->py_type != TypyObjectType) {
		Py_XDECREF(type->py_type);
	}
	if (type->meta_index2field) {
		free(type->meta_index2field);
	}
	if (type->meta_field2index) {
		IblMap_Free(type->meta_field2index);
	}
	free(type);
}

static TypyMetaObject* MetaObject_Initialize(TypyMetaObject* type, PyObject* args) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	PyObject* attrs = Py_None;
	if (PyArg_ParseTuple(args, "|O", &attrs)) {
		if (PyDict_Check(attrs)) {
			while (PyDict_Next(attrs, &pos, &k, &v)) {
				if (type->py_type == TypyObjectType) {
					type->py_type = _InheritTypyObjectType();
					if (!type->py_type) {
						type->py_type = TypyObjectType;
						return NULL;
					}
				}
				if (PyFunction_Check(v)) {
					v = PyMethod_New(v, NULL, (PyObject*)type->py_type);
				}
				PyDict_SetItem(type->py_type->tp_dict, k, v);
			}
			register PyObject* metaclass = PyDict_GetItemString(attrs, "__metaclass__");
			if (metaclass) { type->py_type->ob_type = (PyTypeObject*)metaclass; }
		}
	}
	Py_INCREF(type);
	return type;
}

static PyObject* MetaObject_Repr(TypyMetaObject* type) {
	return PyString_FromFormat("<MetaObject '" FULL_MODULE_NAME ".%s'>", Meta_NAME(type));
}

static PyObject* MetaObject_GetAttr(TypyMetaObject* type, PyObject* arg) {
	if (PyBytes_Check(arg) && !strcmp(PyBytes_AS_STRING(arg), "__name__")) {
		return PyBytes_FromString(Meta_NAME(type));
	}
	return PyObject_GetAttr((PyObject*)type->py_type, arg);
}

static int MetaObject_SetAttr(TypyMetaObject* type, PyObject* arg, PyObject* value) {
	if (type->py_type == TypyObjectType) {
		type->py_type = _InheritTypyObjectType();
		if (!type->py_type) {
			type->py_type = TypyObjectType;
			return -1;
		}
	}
	return PyObject_SetAttr((PyObject*)type->py_type, arg, value);
}

static PyObject* MetaObject_New(PyTypeObject* cls, PyObject* args, PyObject* kwargs) {
	register TypyMetaObject* type = (TypyMetaObject*)PyTuple_GET_ITEM(PyTuple_GET_ITEM(args, 1), 0);
	return PyObject_Call((PyObject*)type->py_type->ob_type, args, kwargs);
}

static PyMethodDef _InitDef = { "InitObject", (PyCFunction)MetaObject_Initialize, METH_VARARGS,
	"Initialize Object Type." };

PyCFunctionObject* Typy_RegisterObject(PyObject* m, PyObject* args) {
	register TypyMetaObject* type = _Typy_RegisterMeta(args);
	register PyCFunctionObject* initializer = (PyCFunctionObject*)PyType_GenericAlloc(&PyCFunction_Type, 0);
	if (!initializer) { Py_DECREF(type); return NULL; }
	initializer->m_ml = &_InitDef;
	initializer->m_self = (PyObject*)type;
	initializer->m_module = NULL;
	type->py_type = TypyObjectType;

	return initializer;
}

PyTypeObject TypyMetaObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaType",            /* tp_name           */
	sizeof(TypyMetaObject),                  /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)TypyMeta_Dealloc,            /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	(reprfunc)MetaObject_Repr,               /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	0,                                       /* tp_hash           */
	(ternaryfunc)Typy_New,                   /* tp_call           */
	(reprfunc)MetaObject_Repr,               /* tp_str            */
	(getattrofunc)MetaObject_GetAttr,        /* tp_getattro       */
	(setattrofunc)MetaObject_SetAttr,        /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy Object Metaclass",             /* tp_doc            */
	0,                                       /* tp_traverse       */
	0,                                       /* tp_clear          */
	0,                                       /* tp_richcompare    */
	0,                                       /* tp_weaklistoffset */
	0,                                       /* tp_iter           */
	0,                                       /* tp_iternext       */
	0,                                       /* tp_methods        */
	0,                                       /* tp_members        */
	0,                                       /* tp_getset         */
	0,                                       /* tp_base           */
	0,                                       /* tp_dict           */
	0,                                       /* tp_descr_get      */
	0,                                       /* tp_descr_set      */
	0,                                       /* tp_dictoffset     */
	0,                                       /* tp_init           */
	0,                                       /* tp_alloc          */
	MetaObject_New,                          /* tp_new            */
};
