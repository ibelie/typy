// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

void FormatTypeError(PyObject* arg, const char* err) {
	PyObject* repr = PyObject_Repr(arg);
	if (!repr) { return; }
	PyErr_Format(PyExc_TypeError, "%s%.100s has type %.100s",
		err, PyString_AsString(repr), Py_TYPE(arg)->tp_name);
	Py_DECREF(repr);
}

bool isDefaultEncodingUTF8 = false;

static const char module_docstring[] =
"python-proto2 is a module that can be used to enhance proto2 Python API\n"
"performance.\n"
"\n"
"It provides access to the protocol buffers C++ reflection API that\n"
"implements the basic protocol buffer functions.";

static PyObject* SetDefaultEncodingUTF8(PyObject* m) {
	PyUnicode_SetDefaultEncoding("utf-8");
	isDefaultEncodingUTF8 = true;
	Py_RETURN_NONE;
}

static PyMethodDef ModuleMethods[] = {
	{"setDefaultEncodingUTF8", (PyCFunction)SetDefaultEncodingUTF8, METH_NOARGS,
		"sys.setdefaultencoding('utf-8') to get better performance for string."},
	{"Object", (PyCFunction)Typy_RegisterObject, METH_VARARGS,
		"register typy Object with properties."},
	{"Variant", (PyCFunction)Typy_RegisterVariant, METH_VARARGS,
		"register typy Variant with properties."},
	{"Enum", (PyCFunction)Typy_RegisterEnum, METH_VARARGS,
		"register typy Enum with properties."},
	{"List", (PyCFunction)Typy_RegisterList, METH_VARARGS,
		"register typy List with properties."},
	{"Dict", (PyCFunction)Typy_RegisterDict, METH_VARARGS,
		"register typy Dict with properties."},
	{"Python", (PyCFunction)Typy_RegisterPython, METH_VARARGS,
		"register typy Python with properties."},
	{"FixedPoint", (PyCFunction)Typy_RegisterFixedPoint, METH_VARARGS,
		"register typy FixedPoint with properties."},
	{ NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef _module = {
	PyModuleDef_HEAD_INIT,
	FULL_MODULE_NAME,
	module_docstring,
	-1,
	ModuleMethods, /* m_methods */
	NULL,
	NULL,
	NULL,
	NULL
};
#define INITFUNC PyInit__typyd
#define INITFUNC_ERRORVAL NULL
#else // Python 2
#define INITFUNC init_typyd
#define INITFUNC_ERRORVAL
#endif

PyObject* kPythonZero;
PyObject* kint32min_py;
PyObject* kint32max_py;
PyObject* kuint32max_py;
PyObject* kint64min_py;
PyObject* kint64max_py;
PyObject* kuint64max_py;

PyMODINIT_FUNC INITFUNC(void) {
	kPythonZero = PyInt_FromLong(0);
	kint32min_py = PyInt_FromLong(INT32_MIN);
	kint32max_py = PyInt_FromLong(INT32_MAX);
	kuint32max_py = PyLong_FromLongLong(UINT32_MAX);
	kint64min_py = PyLong_FromLongLong(INT64_MIN);
	kint64max_py = PyLong_FromLongLong(INT64_MAX);
	kuint64max_py = PyLong_FromUnsignedLongLong(UINT64_MAX);

	PyObject* m;
#if PY_MAJOR_VERSION >= 3
	m = PyModule_Create(&_module);
#else
	m = Py_InitModule3(FULL_MODULE_NAME, ModuleMethods, module_docstring);
#endif
	if (!m) { return INITFUNC_ERRORVAL; }

	TypyEnumType.ob_type = &PyType_Type;
	if (PyType_Ready(&TypyEnumType) < 0) {
		return INITFUNC_ERRORVAL;
	}
	TypyPythonType.ob_type = &PyType_Type;
	if (PyType_Ready(&TypyPythonType) < 0) {
		return INITFUNC_ERRORVAL;
	}
	TypyFixedPointType.ob_type = &PyType_Type;
	if (PyType_Ready(&TypyFixedPointType) < 0) {
		return INITFUNC_ERRORVAL;
	}

	TypyVariantType.ob_type = &PyType_Type;
	if (PyType_Ready(&TypyVariantType) < 0) {
		return INITFUNC_ERRORVAL;
	}
	TypyMetaVariantType.ob_type = &PyType_Type;
	if (PyType_Ready(&TypyMetaVariantType) < 0) {
		return INITFUNC_ERRORVAL;
	}

	BaseTypyObjectType.ob_type = &PyType_Type;
	TypyMetaObjectType.ob_type = &PyType_Type;
	if (PyType_Ready(&TypyMetaObjectType) < 0) {
		return INITFUNC_ERRORVAL;
	}
	TypyObjectType = _InheritTypyObjectType();
	if (!TypyObjectType) { return INITFUNC_ERRORVAL; }

#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}

#ifdef __cplusplus
}
#endif
