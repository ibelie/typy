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

static PyTypeObject* _TypyTypeObject;

static inline PyTypeObject* _CopyTypyTypeObject() {
	register PyTypeObject* type = (PyTypeObject*)malloc(sizeof(PyTypeObject));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Copy TypyTypeObject: out of memory %d.", sizeof(PyTypeObject));
		return NULL;
	}
	memcpy(type, &TypyTypeObject, sizeof(PyTypeObject));
	if (PyType_Ready(type) < 0) {
		free(type);
		return NULL;
	}
	return type;
}

static PyObject* InitObject(TypyType* type, PyObject* args) {
	PyObject* attrs = Py_None;
	if (PyArg_ParseTuple(args, "|O", &attrs)) {
		if (PyDict_Check(attrs)) {
			PyObject *k, *v;
			Py_ssize_t pos = 0;
			while (PyDict_Next(attrs, &pos, &k, &v)) {
				if (type->py_type == _TypyTypeObject) {
					type->py_type = _CopyTypyTypeObject();
					if (!type->py_type) {
						type->py_type = _TypyTypeObject;
						return NULL;
					}
				}
				if (PyFunction_Check(v)) {
					v = PyMethod_New(v, NULL, object);
				}
				PyDict_SetItem(type->py_type->tp_dict, k, v);
			}
			PyObject* metaclass = PyDict_GetItemString(attrs, "__metaclass__");
			if (metaclass) { object->ob_type = (PyTypeObject*)metaclass; }
		}
	}
	return object;
}

PyMethodDef InitObjectDef = { "InitObject", (PyCFunction)InitObject, METH_VARARGS,
	"Initialize Object Type." };

static PyObject* registerObject(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyType* type;
	PyObject* attrs = Py_None;
	size_t ty_size = 1;
	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &attrs)) {
		return NULL;
	}

	type = (TypyType*)malloc(sizeof(TypyType) + sizeof(TypyDescriptor) * ty_size + nameLen);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register: out of memory %d.", sizeof(TypyType));
		return NULL;
	}

	type->py_type = _TypyTypeObject;
	type->ty_size = ty_size;
	Ty_NAME(type)[nameLen] = 0;
	memcpy(Ty_NAME(type), name, nameLen);

	PyCFunctionObject* method = (PyCFunctionObject*)PyType_GenericAlloc(&PyCFunction_Type, 0);
	if (!method) {
		free(type);
		return NULL;
	}
	method->m_ml = &InitObjectDef;
	method->m_self = (PyObject*)type;
	method->m_module = NULL;
	return (PyObject*)method;
}

static PyMethodDef ModuleMethods[] = {
	{"setDefaultEncodingUTF8", (PyCFunction)SetDefaultEncodingUTF8, METH_NOARGS,
		"sys.setdefaultencoding('utf-8') to get better performance for string."},
	{"register", (PyCFunction)registerObject, METH_VARARGS,
		"register typy with properties."},
	{ NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef _module = {
	PyModuleDef_HEAD_INIT,
	#FULL_MODULE_NAME,
	module_docstring,
	-1,
	ModuleMethods, /* m_methods */
	NULL,
	NULL,
	NULL,
	NULL
};
#define INITFUNC PyInit_##FULL_MODULE_NAME
#define INITFUNC_ERRORVAL NULL
#else // Python 2
#define INITFUNC init##FULL_MODULE_NAME
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
	m = Py_InitModule3(#FULL_MODULE_NAME, ModuleMethods, module_docstring);
#endif
	if (!m) { return INITFUNC_ERRORVAL; }

	_TypyTypeObject = _CopyTypyTypeObject();
	if (!_TypyTypeObject) { return INITFUNC_ERRORVAL; }

#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}

#ifdef __cplusplus
}
#endif
