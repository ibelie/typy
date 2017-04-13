// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

TypyPython* Typy_RegisterPython(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyPython* type;
	if (!PyArg_ParseTuple(args, "s#", &name, &nameLen)) {
		return NULL;
	}

	type = (TypyPython*)malloc(sizeof(TypyPython) + nameLen);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Register Python out of memory %lu.", sizeof(TypyPython) + nameLen);
		return NULL;
	}

	type->python_type = &PyBaseObject_Type;
	type->python_name[nameLen] = 0;
	memcpy(type->python_name, name, nameLen);
	(void)PyObject_INIT(type, &TypyPythonType);

	return type;
}

bool TypyPython_CheckAndSet(TypyPython* type, PyObject** value, PyObject* arg, const char* err) {
	if (arg == Py_None) {
		Py_XDECREF(*value);
		return true;
	} else if (PyObject_TypeCheck(arg, type->python_type)) {
		Py_INCREF(arg);
		*value = arg;
		return true;
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

bool TypyPython_Read(TypyPython* type, PyObject** value, byte** input, size_t* length) {
	uint32 size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return false;
	} else if (size > *length) {
		return false;
	} else if (!(*value)) {
		*value = PyObject_CallObject((PyObject*)type->python_type, NULL);
	}
	if (!size) { return true; }
	register PyObject* data = PyBytes_FromStringAndSize((const char*)*input, size);
	*input += size;
	*length -= size;
	if (*value) {
		Py_XDECREF(PyObject_CallMethod(*value, "Deserialize", "O", data));
	}
	Py_XDECREF(data);
	return true;
}

size_t TypyPython_Write(TypyPython* type, PyObject** value, int tag, byte* output) {
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	if (*value) {
		register PyObject* data = PyObject_CallMethod(*value, "Serialize", NULL);
		if (data) {
			register size_t length = PyBytes_GET_SIZE(data);
			size += IblPutUvarint(output + size, length);
			memcpy(output + size, PyBytes_AS_STRING(data), length);
			Py_XDECREF(data);
			return size + length;
		}
	}
	output[size] = 0;
	return size + 1;
}

size_t TypyPython_ByteSize(TypyPython* type, PyObject** value, int tagsize) {
	register size_t size = 0;
	if (*value) {
		register PyObject* s = PyObject_CallMethod(*value, "ByteSize", NULL);
		if (s) { size = PyInt_AsLong(s); Py_DECREF(s); }
	}
	return tagsize + IblSizeVarint(size) + size;
}

static TypyPython* TypyPython_Initialize(TypyPython* type, PyObject* args) {
	PyObject* python_type = Py_None;
	if (PyArg_ParseTuple(args, "|O", &python_type)) {
		Py_XDECREF(type->python_type);
		Py_INCREF(python_type);
		type->python_type = (PyTypeObject*)python_type;
	}
	Py_INCREF(type);
	return type;
}

static void TypyPython_Dealloc(TypyPython* type) {
	Py_XDECREF(type->python_type);
	free(type);
}

static PyObject* TypyPython_Repr(TypyPython* type) {
	return PyString_FromFormat("<Python '" FULL_MODULE_NAME ".%s'>", type->python_name);
}

PyTypeObject TypyPythonType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Python",               /* tp_name           */
	sizeof(TypyPython),                       /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)TypyPython_Dealloc,           /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	0,                                        /* tp_compare        */
	(reprfunc)TypyPython_Repr,                /* tp_repr           */
	0,                                        /* tp_as_number      */
	0,                                        /* tp_as_sequence    */
	0,                                        /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	(ternaryfunc)TypyPython_Initialize,       /* tp_call           */
	(reprfunc)TypyPython_Repr,                /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy Python",                          /* tp_doc            */
};
