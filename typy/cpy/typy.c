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
PyTypeObject* TypyTypeObject = NULL;

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

// ===================================================================

static void MetaObject_Dealloc(TypyMetaObject* type) {
	Py_XDECREF(type->ty_new);
	if (type->py_type != TypyTypeObject) {
		Py_DECREF(type->py_type);
	}
	if (type->ty_index2field) {
		free(type->ty_index2field);
	}
	if (type->ty_field2index) {
		IblMap_Free(type->ty_field2index);
	}
}

static inline PyTypeObject* _InheritTypyTypeObject() {
	register PyTypeObject* type = (PyTypeObject*)malloc(sizeof(PyTypeObject));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Inherit TypyTypeObject: out of memory %d.", sizeof(PyTypeObject));
		return NULL;
	}
	memcpy(type, &BaseTypyTypeObject, sizeof(PyTypeObject));
	type->tp_base = TypyTypeObject;
	if (PyType_Ready(type) < 0) {
		free(type);
		return NULL;
	}
	return type;
}

static PyObject* MetaObject_Initialize(TypyMetaObject* type, PyObject* args) {
	PyObject* attrs = Py_None;
	if (PyArg_ParseTuple(args, "|O", &attrs)) {
		if (PyDict_Check(attrs)) {
			PyObject *k, *v;
			Py_ssize_t pos = 0;
			while (PyDict_Next(attrs, &pos, &k, &v)) {
				if (type->py_type == TypyTypeObject) {
					type->py_type = _InheritTypyTypeObject();
					if (!type->py_type) {
						type->py_type = TypyTypeObject;
						return NULL;
					}
				}
				if (PyFunction_Check(v)) {
					v = PyMethod_New(v, NULL, (PyObject*)type->py_type);
				}
				PyDict_SetItem(type->py_type->tp_dict, k, v);
			}
			PyObject* metaclass = PyDict_GetItemString(attrs, "__metaclass__");
			if (metaclass) { type->py_type->ob_type = (PyTypeObject*)metaclass; }
		}
	}
	Py_INCREF(type->ty_new);
	return type->ty_new;
}

static PyTypeObject TypyMetaObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaType",            /* tp_name           */
	sizeof(TypyMetaObjectType),              /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)MetaObject_Dealloc,          /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	0,                                       /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	PyObject_HashNotImplemented,             /* tp_hash           */
	(ternaryfunc)MetaObject_Initialize,      /* tp_call           */
	0,                                       /* tp_str            */
	0,                                       /* tp_getattro       */
	0,                                       /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy metaclass",                    /* tp_doc            */
};

// ===================================================================

PyMethodDef TypyNewDef = { "TypyNew", (PyCFunction)Typy_New, METH_VARARGS | METH_KEYWORDS,
	"Create Object Type." };

static PyObject* registerObject(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyMetaObject* type;
	PyObject* attrs = Py_None;
	size_t ty_size = 1;
	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &attrs)) {
		return NULL;
	}

	register size_t size = sizeof(TypyMetaObject) + sizeof(TypyDescriptor) * ty_size + nameLen;
	type = (TypyMetaObject*)malloc(size);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register: out of memory %d.", size);
		return NULL;
	}

	type->py_type = TypyTypeObject;
	type->ty_size = ty_size;
	Ty_NAME(type)[nameLen] = 0;
	memcpy(Ty_NAME(type), name, nameLen);
	PyObject_INIT(type, &TypyMetaObjectType);
	type->ty_index2field = NULL;
	type->ty_field2index = NULL;

	register PyCFunctionObject* ty_new = (PyCFunctionObject*)PyType_GenericAlloc(&PyCFunction_Type, 0);
	if (!ty_new) {
		free(type);
		return NULL;
	}
	ty_new->m_ml = &TypyNewDef;
	ty_new->m_self = (PyObject*)type;
	ty_new->m_module = NULL;
	type->ty_new = (PyObject*)ty_new;

	return (PyObject*)type;
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

	TypyMetaObjectType.ob_type = &PyType_Type;
	BaseTypyTypeObject.ob_type = &PyType_Type;
	TypyTypeObject = _InheritTypyTypeObject();
	if (!TypyTypeObject) { return INITFUNC_ERRORVAL; }

#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}

#ifdef __cplusplus
}
#endif
