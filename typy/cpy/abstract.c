// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

PyObject* GetBytes(const ::std::string& value) {
	PyObject* result = PyUnicode_DecodeUTF8(value.c_str(), value.length(), NULL);
	if (result == NULL) {
		PyErr_Clear();
		result = PyBytes_FromStringAndSize(value.c_str(), value.length());
	}
	return result;
}

template <typename T>
bool CheckAndSetInteger(PyObject* arg, T& value, const char* err, PyObject* min, PyObject* max) {
	bool is_long = PyLong_Check(arg);
#if PY_MAJOR_VERSION < 3
	if (!PyInt_Check(arg) && !is_long) {
		FormatTypeError(arg, err);
		return false;
	}
	if (PyObject_Compare(min, arg) > 0 || PyObject_Compare(max, arg) < 0) {
#else
	if (!is_long) {
		FormatTypeError(arg, err);
		return false;
	}
	if (PyObject_RichCompareBool(min, arg, Py_LE) != 1 || PyObject_RichCompareBool(max, arg, Py_GE) != 1) {
#endif
		if (!PyErr_Occurred()) {
			ScopedPyObjectPtr s(PyObject_Str(arg));
			if (s != NULL) {
				PyErr_Format(PyExc_ValueError, "Value out of range: %.100s", PyString_AsString(s.get()));
			}
		}
		return false;
	}
#if PY_MAJOR_VERSION < 3
	if (!is_long) {
		CopyFrom(value, static_cast<T>(PyInt_AsLong(arg)));
	} else  // NOLINT
#endif
	{
		if (min == kPythonZero) {
			CopyFrom(value, static_cast<T>(PyLong_AsUnsignedLongLong(arg)));
		} else {
			CopyFrom(value, static_cast<T>(PyLong_AsLongLong(arg)));
		}
	}
	return true;
}

bool CheckAndSet(PyObject* arg, int32& value, const char* err) {
	return CheckAndSetInteger<int32>(arg, value, err, kint32min_py, kint32max_py);
}

bool CheckAndSet(PyObject* arg, int64& value, const char* err) {
	return CheckAndSetInteger<int64>(arg, value, err, kint64min_py, kint64max_py);
}

bool CheckAndSet(PyObject* arg, uint32& value, const char* err) {
	return CheckAndSetInteger<uint32>(arg, value, err, kPythonZero, kuint32max_py);
}

bool CheckAndSet(PyObject* arg, uint64& value, const char* err) {
	return CheckAndSetInteger<uint64>(arg, value, err, kPythonZero, kuint64max_py);
}

bool CheckAndSet(PyObject* arg, double& value, const char* err) {
	if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	CopyFrom(value, PyFloat_AsDouble(arg));
	return true;
}

bool CheckAndSet(PyObject* arg, float& value, const char* err) {
	if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	CopyFrom(value, static_cast<float>(PyFloat_AsDouble(arg)));
	return true;
}

bool CheckAndSet(PyObject* arg, bool& value, const char* err) {
	if (!PyInt_Check(arg) && !PyBool_Check(arg) && !PyLong_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	CopyFrom(value, PyInt_AsLong(arg) == 0 ? false : true);
	return true;
}

bool CheckAndSet(PyObject* arg, string& value, const char* err) {
	if (arg == NULL || arg == Py_None) {
		CopyFrom(value, reinterpret_cast<string>(NULL));
		return true;
	} else if (PyUnicode_Check(arg)) {
		CopyFrom(value, reinterpret_cast<string>(arg));
		return true;
	}
	ScopedPyObjectPtr s(PyUnicode_FromEncodedObject(arg, "utf-8", NULL));
	if (s == NULL) { return false; }
	CopyFrom(value, reinterpret_cast<string>(s.get()));
	return true;
}

bool CheckAndSet(PyObject* arg, bytes& value, const char* err) {
	if (arg == NULL || arg == Py_None) {
		CopyFrom(value, reinterpret_cast<bytes>(NULL));
		return true;
	} else if (PyUnicode_Check(arg)) {
		arg = PyUnicode_AsEncodedObject(arg, "utf-8", NULL);
		if (arg == NULL) { return false; }
	} else if (PyBytes_Check(arg)) {
		Py_INCREF(arg);
	} else {
		FormatTypeError(arg, err);
		return false;
	}
	CopyFrom(value, reinterpret_cast<bytes>(arg));
	Py_DECREF(arg);
	return true;
}

bool CheckAndSet(PyObject* arg, ::std::string& value, const char* err) {
	if (arg == NULL || arg == Py_None) {
		CopyFrom(value, "");
		return true;
	} else if (PyUnicode_Check(arg)) {
		arg = PyUnicode_AsEncodedObject(arg, "utf-8", NULL);
	} else if (PyBytes_Check(arg)) {
		Py_INCREF(arg);
	} else {
		FormatTypeError(arg, err);
		return false;
	}
	CopyFrom(value, PyBytes_AS_STRING(arg), PyBytes_GET_SIZE(arg));
	Py_DECREF(arg);
	return true;
}
