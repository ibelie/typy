
#include "Corpus.h"

#define ENUM Corpus

namespace typy {

static PyObject* kCorpusUNIVERSAL;
static PyObject* kCorpusWEB;
static PyObject* kCorpusIMAGES;
static PyObject* kCorpusLOCAL;
static PyObject* kCorpusNEWS;
static PyObject* kCorpusPRODUCTS;
static PyObject* kCorpusVIDEO;

PyObject* _InitCorpus(PyObject* m, PyObject* arg) {
	if (PyDict_Check(arg)) {
		PyObject *k, *v;
		Py_ssize_t pos = 0;
		while (PyDict_Next(arg, &pos, &k, &v)) {
			long i = PyInt_AsLong(k);
			if (PyErr_Occurred()) {
				return NULL;
			}
			switch (i) {
			case UNIVERSAL: Py_INCREF(v); kCorpusUNIVERSAL = v; break;
			case WEB: Py_INCREF(v); kCorpusWEB = v; break;
			case IMAGES: Py_INCREF(v); kCorpusIMAGES = v; break;
			case LOCAL: Py_INCREF(v); kCorpusLOCAL = v; break;
			case NEWS: Py_INCREF(v); kCorpusNEWS = v; break;
			case PRODUCTS: Py_INCREF(v); kCorpusPRODUCTS = v; break;
			case VIDEO: Py_INCREF(v); kCorpusVIDEO = v; break;
			}
		}
		Py_RETURN_NONE;
	} else {
		FormatTypeError(arg, "Corpus expect a value => enum dict, but ");
		return NULL;
	}
}

static PyMethodDef CorpusMethod = { "Corpus", (PyCFunction)_InitCorpus, METH_O,
	"Initialize the 'Corpus' Enum with a value dict." };

bool InitCorpus(PyObject* m) {
	kCorpusUNIVERSAL = PyInt_FromLong(0);
	kCorpusWEB = PyInt_FromLong(1);
	kCorpusIMAGES = PyInt_FromLong(2);
	kCorpusLOCAL = PyInt_FromLong(3);
	kCorpusNEWS = PyInt_FromLong(4);
	kCorpusPRODUCTS = PyInt_FromLong(5);
	kCorpusVIDEO = PyInt_FromLong(6);
	PyCFunctionObject* method = reinterpret_cast<PyCFunctionObject*>(
		PyType_GenericAlloc(&PyCFunction_Type, 0));
	method->m_ml = &CorpusMethod;
	method->m_self = NULL;
	method->m_module = NULL;
	PyModule_AddObject(m, CorpusMethod.ml_name, reinterpret_cast<PyObject*>(method));
	return true;
}

PyObject* GetPyObject(const ENUM& value) {
	switch (value) {
		case UNIVERSAL: Py_INCREF(kCorpusUNIVERSAL); return kCorpusUNIVERSAL;
		case WEB: Py_INCREF(kCorpusWEB); return kCorpusWEB;
		case IMAGES: Py_INCREF(kCorpusIMAGES); return kCorpusIMAGES;
		case LOCAL: Py_INCREF(kCorpusLOCAL); return kCorpusLOCAL;
		case NEWS: Py_INCREF(kCorpusNEWS); return kCorpusNEWS;
		case PRODUCTS: Py_INCREF(kCorpusPRODUCTS); return kCorpusPRODUCTS;
		case VIDEO: Py_INCREF(kCorpusVIDEO); return kCorpusVIDEO;
		default:
			return PyInt_FromLong(value);
	}
}

bool CheckAndSet(PyObject* arg, ENUM& value, const char* err) {
	long i = PyInt_AsLong(arg);
	if (PyErr_Occurred()) { return false; }
	switch (i) {
		case UNIVERSAL:
		case WEB:
		case IMAGES:
		case LOCAL:
		case NEWS:
		case PRODUCTS:
		case VIDEO:
			value = static_cast<ENUM>(i);
			return true;
		default:
			FormatTypeError(arg, err);
			return false;
	}
}

} /* namespace typy */

#undef ENUM
