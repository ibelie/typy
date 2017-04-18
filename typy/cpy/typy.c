// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"
#include "longintrepr.h"

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

PyBytes Typy_CheckBytes(PyObject* arg, const char* err) {
	if (!arg || arg == Py_None) {
		FormatTypeError(arg, err);
		return NULL;
	} else if (PyUnicode_Check(arg)) {
		return (PyBytes)PyUnicode_AsEncodedObject(arg, "utf-8", NULL);
	} else if (PyBytes_Check(arg)) {
		Py_INCREF(arg);
		return (PyBytes)arg;
	} else {
		FormatTypeError(arg, err);
		return NULL;
	}
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

static PyIntObject kPythonZeroObject;
static PyIntObject kint32min_obj;
static PyIntObject kint32max_obj;

#define TypyIntObject_INIT(O, N) do { \
	PyObject_INIT((O), &PyInt_Type);  \
	((PyIntObject*)(O))->ob_ival = (N); \
} while (0)

#define TypyLongObject struct { \
	PyObject_VAR_HEAD           \
	digit ob_digit[5];          \
}

static TypyLongObject kuint32max_obj;
static TypyLongObject kuint64max_obj;
static TypyLongObject kint64min_obj;
static TypyLongObject kint64max_obj;

#define TypyLongObject_INIT(O, N) do { \
	PyObject_INIT((O), &PyLong_Type);                                                           \
	register int i = 0;                                                                         \
	register unsigned PY_LONG_LONG t = (N) < 0 ? ((unsigned PY_LONG_LONG)(-1 - (N)) + 1) : (N); \
	while (t) {                                                                                 \
		((PyLongObject*)(O))->ob_digit[i++] = (digit)(t & PyLong_MASK);                         \
		t >>= PyLong_SHIFT;                                                                     \
	}                                                                                           \
	((PyLongObject*)(O))->ob_size = (N) < 0 ? -i : i;                                           \
} while (0)

#define TypyStringObject(N) struct { \
	PyObject_VAR_HEAD                \
	long ob_shash;                   \
	int  ob_sstate;                  \
	char ob_sval[N + 1];             \
}

static TypyStringObject(2) k_t_obj;

#define TypyStringObject_INIT(O, S) do { \
	PyObject_INIT_VAR((O), &PyString_Type, strlen(S));       \
	((PyStringObject*)(O))->ob_shash = -1;                   \
	((PyStringObject*)(O))->ob_sstate = SSTATE_NOT_INTERNED; \
	strcpy(((PyStringObject*)(O))->ob_sval, (S));            \
} while (0)

PyObject* kPythonZero   = (PyObject*)&kPythonZeroObject;
PyObject* kint32min_py  = (PyObject*)&kint32min_obj;
PyObject* kint32max_py  = (PyObject*)&kint32max_obj;
PyObject* kuint32max_py = (PyObject*)&kuint32max_obj;
PyObject* kint64min_py  = (PyObject*)&kint64min_obj;
PyObject* kint64max_py  = (PyObject*)&kint64max_obj;
PyObject* kuint64max_py = (PyObject*)&kuint64max_obj;
PyObject* k_t           = (PyObject*)&k_t_obj;

static PyTypeObject* PyTypeObjects[] = {
	&TypyMetaDictType,
	&TypyDictType,
	&TypyDictIterKeyType,
	&TypyDictIterValueType,
	&TypyDictIterItemType,
	&TypyEnumType,
	&TypyPythonType,
	&TypyFixedPointType,
	&TypyMetaListType,
	&TypyListType,
	&TypyListIteratorType,
	&TypyMetaObjectType,
	&TypyMetaVariantType,
	&TypyVariantType,
	NULL,
};

PyMODINIT_FUNC INITFUNC(void) {
	TypyIntObject_INIT(kPythonZero, 0);
	TypyIntObject_INIT(kint32min_py, INT32_MIN);
	TypyIntObject_INIT(kint32max_py, INT32_MAX);
	TypyLongObject_INIT(kuint32max_py, UINT32_MAX);
	TypyLongObject_INIT(kuint64max_py, UINT64_MAX);
	TypyLongObject_INIT(kint64min_py, INT64_MIN);
	TypyLongObject_INIT(kint64max_py, INT64_MAX);
	TypyStringObject_INIT(k_t, "_t");

	PyObject* m;
#if PY_MAJOR_VERSION >= 3
	m = PyModule_Create(&_module);
#else
	m = Py_InitModule3(FULL_MODULE_NAME, ModuleMethods, module_docstring);
#endif
	if (!m) { return INITFUNC_ERRORVAL; }

	register int i;
	for (i = 0; PyTypeObjects[i]; i++) {
		PyTypeObjects[i]->ob_type = &PyType_Type;
		if (PyType_Ready(PyTypeObjects[i]) < 0) {
			return INITFUNC_ERRORVAL;
		}
	}

	BaseTypyObjectType.ob_type = &PyType_Type;
	TypyObjectType = _InheritTypyObjectType();
	if (!TypyObjectType) { return INITFUNC_ERRORVAL; }

#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}

#ifdef __cplusplus
}
#endif
