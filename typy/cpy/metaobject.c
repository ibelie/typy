// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

TypyObject* Typy_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	TypyObject* object = (TypyObject*)calloc(1, sizeof(TypyObject) + sizeof(TypyField) * type->meta_size);
	if (!object) {
		PyErr_Format(PyExc_RuntimeError, "Alloc Object out of memory %d.", sizeof(TypyObject) + sizeof(TypyField) * type->meta_size);
		return NULL;
	}
	PyObject_INIT(object, type->py_type);
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

bool TypyObject_CheckAndSet(TypyMetaObject* type, TypyObject** value, PyObject* arg, const char* err) {
	if (arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	}
	if (!Typy_TypeCheck(arg) || Typy_TYPE(arg) != type) {
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
		self->object_size = size;
	}
	return tagsize + IblSizeVarint(size) + size;
}

size_t TypyObject_Write(TypyMetaObject* type, TypyObject** value, int tag, byte* output) {
	register TypyObject* self = *value;
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	size += IblPutUvarint(output + size, self->object_size);
	if (self->object_size) {
		Typy_SerializeString(self, output + size);
	}
	return size + self->object_size;
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
	register size_t size = Typy_MergeFromString(self, *input, limit);
	*input += (limit - size);
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

// ===================================================================

PyTypeObject* TypyObjectType = NULL;

void TypyMeta_Dealloc(TypyMetaObject* type) {
	Py_XDECREF(type->meta_new);
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

static PyObject* MetaObject_Initialize(TypyMetaObject* type, PyObject* args) {
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
	Py_INCREF(type->meta_new);
	return type->meta_new;
}

static PyObject* MetaObject_Repr(TypyMetaObject* type) {
	return PyString_FromFormat("<MetaObject '" FULL_MODULE_NAME ".%s'>", Meta_NAME(type));
}

static PyMethodDef TypyNewDef = { "TypyNew", (PyCFunction)Typy_New, METH_VARARGS | METH_KEYWORDS,
	"Create Object Type." };

TypyMetaObject* Typy_RegisterObject(PyObject* m, PyObject* args) {
	register TypyMetaObject* type = _Typy_RegisterMeta(args);
	register PyCFunctionObject* meta_new = (PyCFunctionObject*)PyType_GenericAlloc(&PyCFunction_Type, 0);
	if (!meta_new) { Py_DECREF(type); return NULL; }
	meta_new->m_ml = &TypyNewDef;
	meta_new->m_self = (PyObject*)type;
	meta_new->m_module = NULL;
	type->meta_new = (PyObject*)meta_new;
	type->py_type = TypyObjectType;

	return type;
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
	PyObject_HashNotImplemented,             /* tp_hash           */
	(ternaryfunc)MetaObject_Initialize,      /* tp_call           */
	(reprfunc)MetaObject_Repr,               /* tp_str            */
	0,                                       /* tp_getattro       */
	0,                                       /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy Object Metaclass",             /* tp_doc            */
};
