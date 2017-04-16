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

PyObject* TypyPython_ToJson(TypyPython* type, PyObject** value, bool slim) {
	if (!slim || *value) {
		register PyObject* json = !(*value) ? PyDict_New() : PyObject_CallMethod(*value, "Json", NULL);
		PyDict_SetItemString(json, "_t", PyString_FromString(type->python_type->tp_name));
		return json;
	} else {
		return NULL;
	}
}

bool TypyPython_FromJson(TypyPython* type, PyObject** value, PyObject* json) {
	PyObject* item = NULL;
	PyObject* dict = PyDict_New();
	PyObject* iter = PyObject_CallMethod(json, "iteritems", NULL);
	if (!iter) {
		FormatTypeError(json, "FromJson expect dict, but ");
		goto fromjson_fail;
	} else if (!dict) {
		goto fromjson_fail;
	}

	register bool type_check = false;
	register Py_ssize_t i, size = _PyObject_LengthHint(json, 0);
	for (i = 0; i < size; i++) {
		item = PyIter_Next(iter);
		if (!item) { goto fromjson_fail; }
		register PyObject* k = PyTuple_GET_ITEM(item, 0);
		register PyObject* v = PyTuple_GET_ITEM(item, 1);
		if (PyBytes_Check(k) && !strcmp(PyBytes_AS_STRING(k), "_t")) {
			if (!PyBytes_Check(v)) {
				FormatTypeError(v, "Json _t expect String, but ");
				goto fromjson_fail;
			} else if (strcmp(PyBytes_AS_STRING(v), type->python_type->tp_name)) {
				PyErr_Format(PyExc_TypeError, "Python expect '%.100s', but Json has type %.100s",
					type->python_type->tp_name, PyBytes_AS_STRING(v));
				goto fromjson_fail;
			}
			type_check = true;
			continue;
		}
		PyDict_SetItem(dict, k, v);
		Py_DECREF(item);
		item = NULL;
	}
	if (!type_check) {
		FormatTypeError(json, "Json expect _t, ");
		goto fromjson_fail;
	}
	Py_XDECREF(*value);
	*value = PyObject_CallMethod((PyObject*)type->python_type, "FromJson", "O", dict);
	Py_DECREF(iter);
	Py_DECREF(dict);
	return *value ? true : false;

fromjson_fail:
	Py_XDECREF(dict);
	Py_XDECREF(iter);
	Py_XDECREF(item);
	return false;
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
