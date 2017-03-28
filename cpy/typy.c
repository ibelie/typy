#include "typy.h"

void FormatTypeError(PyObject* arg, const char* err) {
	ScopedPyObjectPtr repr(PyObject_Repr(arg));
	if (repr != NULL) {
		PyErr_Format(PyExc_TypeError, "%s%.100s has type %.100s",
			err, PyString_AsString(repr.get()), Py_TYPE(arg)->tp_name);
	}
}

bool isDefaultEncodingUTF8 = false;

static const char module_docstring[] =
"python-proto2 is a module that can be used to enhance proto2 Python API\n"
"performance.\n"
"\n"
"It provides access to the protocol buffers C++ reflection API that\n"
"implements the basic protocol buffer functions.";

static PyObject* setDefaultEncodingUTF8(PyObject* m) {
	PyUnicode_SetDefaultEncoding("utf-8");
	isDefaultEncodingUTF8 = true;
	Py_RETURN_NONE;
}

static PyObject* registerType(PyObject* m, PyObject* args) {
	Py_RETURN_NONE;
}

static PyMethodDef ModuleMethods[] = {
	{"setDefaultEncodingUTF8", (PyCFunction)setDefaultEncodingUTF8, METH_NOARGS,
		"sys.setdefaultencoding('utf-8') to get better performance for string."},
	{"register", (PyCFunction)registerType, METH_VARARGS,
		"register typy with properties."},
	{ NULL, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef _module = {
	PyModuleDef_HEAD_INIT,
	"_typyd",
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
	m = Py_InitModule3("_typyd", ModuleMethods, module_docstring);
#endif
	if (m == NULL) {
		return INITFUNC_ERRORVAL;
	}

#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}
