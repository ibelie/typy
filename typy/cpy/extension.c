// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject* Typy_RegisterPython(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyPython* type;
	if (!PyArg_ParseTuple(args, "s#", &name, &nameLen)) {
		return NULL;
	}

	type = (TypyPython*)malloc(sizeof(TypyPython) + nameLen);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Python: out of memory %d.", sizeof(TypyPython) + nameLen);
		return NULL;
	}

	type->python_type = NULL;
	type->python_name[nameLen] = 0;
	memcpy(type->python_name, name, nameLen);
	PyObject_INIT(type, &TypyPythonType);

	return (PyObject*)type;
}

static PyObject* Python_Initialize(TypyPython* type, PyObject* args) {
	PyObject* python_type = Py_None;
	if (PyArg_ParseTuple(args, "|O", &python_type)) {
		Py_XDECREF(type->python_type);
		Py_INCREF(python_type);
		type->python_type = (PyTypeObject*)python_type;
	}
	Py_INCREF(type);
	return (PyObject*)type;
}

void Python_Dealloc(TypyPython* type) {
	Py_XDECREF(type->python_type);
	free(type);
}

static PyObject* Python_Repr(TypyPython* type) {
	return PyString_FromFormat("<Python '" FULL_MODULE_NAME ".%s'>", type->python_name);
}

PyTypeObject TypyPythonType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Python",               /* tp_name           */
	sizeof(TypyPython),                       /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)Python_Dealloc,               /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	0,                                        /* tp_compare        */
	(reprfunc)Python_Repr,                    /* tp_repr           */
	0,                                        /* tp_as_number      */
	0,                                        /* tp_as_sequence    */
	0,                                        /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	(ternaryfunc)Python_Initialize,           /* tp_call           */
	(reprfunc)Python_Repr,                    /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy Python",                          /* tp_doc            */
};
