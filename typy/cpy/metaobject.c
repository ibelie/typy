// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject* Typy_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	PyObject* object = (PyObject*)calloc(1, sizeof(TypyObject) + sizeof(TypyField) * type->meta_size);
	if (!object) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Alloc Object: out of memory %d.", sizeof(TypyObject) + sizeof(TypyField) * type->meta_size);
		return NULL;
	}
	PyObject_INIT(object, type->py_type);
	if (kwargs) {
		while (PyDict_Next(kwargs, &pos, &k, &v)) {
			if (PyObject_SetAttr(object, k, v) == -1) {
				break;
			}
		}
	}
	return object;
}

size_t TypyObject_ByteSize(TypyMetaObject* type, TypyObject** value, int tagsize) {
	register size_t size = Typy_ByteSize(*value);
	(*value)->object_size = size;
	return size ? tagsize + IblSizeVarint(size) + size : 0;
}

size_t TypyObject_Write(TypyMetaObject* type, TypyObject** value, int tag, byte* output) {
	register TypyObject* self = *value;
	if (self->object_size <= 0) { return 0;}
	register size_t size = Typy_WriteTag(output, tag);
	size += IblPutUvarint(output + size, self->object_size);
	Typy_SerializeString(self, output + size);
	return size + self->object_size;
}

bool TypyObject_Read(TypyMetaObject* type, TypyObject** value, byte** input, size_t* length) {
	register TypyObject* self = *value;
	if (!self) {
		self = (TypyObject*)Typy_New(type, NULL, NULL);
		*value = self;
	}
	register size_t size = Typy_MergeFromString(self, *input, *length);
	*input += size;
	*length -= size;
	return size > 0;
}

void TypyObject_MergeFrom(TypyMetaObject* type, TypyObject** lvalue, TypyObject* rvalue) {
	if (!rvalue) { return; }
	register TypyObject* self = *lvalue;
	if (!self) {
		self = (TypyObject*)Typy_New(type, NULL, NULL);
		*lvalue = self;
	}
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
	if (!arg || arg == Py_None) {
		FormatTypeError(arg, "SerializeProperty expect property name, but ");
		return NULL;
	} else if (PyUnicode_Check(arg)) {
		arg = PyUnicode_AsEncodedObject(arg, "utf-8", NULL);
		if (!arg) { return NULL; }
	} else if (PyBytes_Check(arg)) {
		Py_INCREF(arg);
	} else {
		FormatTypeError(arg, "SerializeProperty expect property name, but ");
		return NULL;
	}

	register int index = Typy_PropertyIndex(self, PyBytes_AS_STRING(arg));
	if (index < 0) {
		FormatTypeError(arg, "SerializeProperty expect property name, but ");
		Py_DECREF(arg);
		return NULL;
	}
	register size_t size = Typy_PropertyByteSize(self, index);
	if (size <= 0) {
		PyErr_Format(PyExc_RuntimeError, "Error serializing object");
		Py_DECREF(arg);
		return NULL;
	}
	register PyObject* result = PyBytes_FromStringAndSize(NULL, size);
	if (!result) {
		Py_DECREF(arg);
		return NULL;
	}
	Typy_SerializeProperty(self, (byte*)PyBytes_AS_STRING(result), index);
	Py_DECREF(arg);
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

PyObject* Typy_RegisterObject(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyMetaObject* type;
	PyObject* descriptors = Py_None;
	size_t meta_size = 1;
	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &descriptors)) {
		return NULL;
	}

	register size_t size = sizeof(TypyMetaObject) + sizeof(TypyDescriptor) * meta_size + nameLen;
	type = (TypyMetaObject*)malloc(size);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Object: MetaObject out of memory %d.", size);
		return NULL;
	}

	type->py_type = TypyObjectType;
	type->meta_size = meta_size;
	Meta_NAME(type)[nameLen] = 0;
	memcpy(Meta_NAME(type), name, nameLen);
	PyObject_INIT(type, &TypyMetaObjectType);
	/* todo: Typy_RegisterObject */
	type->meta_index2field = (char**)malloc(meta_size * sizeof(char*));
	if (!type->meta_index2field) {
		free(type);
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Object: index2field out of memory %d.", meta_size * sizeof(char*));
		return NULL;
	}
	type->meta_field2index = TypyFieldMap_New();
	if (!type->meta_field2index) {
		free(type->meta_index2field);
		free(type);
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Object: field2index out of memory.");
		return NULL;
	}

	register PyCFunctionObject* meta_new = (PyCFunctionObject*)PyType_GenericAlloc(&PyCFunction_Type, 0);
	if (!meta_new) {
		IblMap_Free(type->meta_field2index);
		free(type->meta_index2field);
		free(type);
		return NULL;
	}
	meta_new->m_ml = &TypyNewDef;
	meta_new->m_self = (PyObject*)type;
	meta_new->m_module = NULL;
	type->meta_new = (PyObject*)meta_new;

	return (PyObject*)type;
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
