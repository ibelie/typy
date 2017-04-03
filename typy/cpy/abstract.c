// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"


static PyObject* TypyInt32_GetPyObject(TypeType t, int32* v) { return PyInt_FromLong(*v); }
static PyObject* TypyInt64_GetPyObject(TypeType t, int64* v) { return PyLong_FromLongLong(*v); }
static PyObject* TypyUint32_GetPyObject(TypeType t, uint32* v) { return PyInt_FromSize_t(*v); }
static PyObject* TypyUint64_GetPyObject(TypeType t, uint64* v) { return PyLong_FromUnsignedLongLong(*v); }
static PyObject* TypyBool_GetPyObject(TypeType t, bool* v) { return PyBool_FromLong(*v); }
static PyObject* TypyDouble_GetPyObject(TypeType t, double* v) { return PyFloat_FromDouble(*v); }
static PyObject* TypyFloat_GetPyObject(TypeType t, float* v) { PyFloat_FromDouble(*v); }

static PyObject* TypyObject_GetPyObject(TypeType type, PyObject** value) {
	if (!(*value)) {
		Py_RETURN_NONE;
	}
	Py_INCREF(*value);
	return *value;
}

abstract_GetPyObject = {
	TypyEnum_GetPyObject,       /* TYPE_ENUM       */
	TypyInt32_GetPyObject,      /* TYPE_INT32      */
	TypyInt64_GetPyObject,      /* TYPE_INT64      */
	TypyUint32_GetPyObject,     /* TYPE_UINT32     */
	TypyUint64_GetPyObject,     /* TYPE_UINT64     */
	TypyDouble_GetPyObject,     /* TYPE_DOUBLE     */
	TypyFloat_GetPyObject,      /* TYPE_FLOAT      */
	TypyBool_GetPyObject,       /* TYPE_BOOL       */
	TypyObject_GetPyObject,     /* TYPE_BYTES      */
	TypyObject_GetPyObject,     /* TYPE_STRING     */
	TypyObject_GetPyObject,     /* TYPE_OBJECT     */
	TypyObject_GetPyObject,     /* TYPE_VARIANT    */
	TypyList_GetPyObject,       /* TYPE_LIST       */
	TypyDict_GetPyObject,       /* TYPE_DICT       */
	TypyFixedPoint_GetPyObject, /* TYPE_FIXEDPOINT */
	TypyObject_GetPyObject,     /* TYPE_PYTHON     */
};

//=============================================================================

static bool CheckAndSetInteger(int64* value, PyObject* arg, const char* err, PyObject* min, PyObject* max) {
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
			register PyObject* s = PyObject_Str(arg);
			if (s) {
				PyErr_Format(PyExc_ValueError, "Value out of range: %.100s", PyString_AsString(s));
				Py_DECREF(s);
			}
		}
		return false;
	}
#if PY_MAJOR_VERSION < 3
	if (!is_long) {
		*value = PyInt_AsLong(arg);
	} else  // NOLINT
#endif
	{
		if (min == kPythonZero) {
			*value = PyLong_AsUnsignedLongLong(arg);
		} else {
			*value = PyLong_AsLongLong(arg);
		}
	}
	return true;
}

static bool TypyInt32_CheckAndSet(TypeType t, int32* value, PyObject* arg, const char* err) {
	TypyField v;
	if (CheckAndSetInteger(&v, arg, err, kint32min_py, kint32max_py)) {
		*value = (int32)v;
		return true;
	}
	return false;
}

static bool TypyInt64_CheckAndSet(TypeType t, int64* value, PyObject* arg, const char* err) {
	TypyField v;
	if (CheckAndSetInteger(&v, arg, err, kint64min_py, kint64max_py)) {
		*value = (int64)v;
		return true;
	}
	return false;
}

static bool TypyUint32_CheckAndSet(TypeType t, uint32* value, PyObject* arg, const char* err) {
	TypyField v;
	if (CheckAndSetInteger(&v, arg, err, kPythonZero, kuint32max_py)) {
		*value = (uint32)v;
		return true;
	}
	return false;
}

static bool TypyUint64_CheckAndSet(TypeType t, uint64* value, PyObject* arg, const char* err) {
	TypyField v;
	if (CheckAndSetInteger(&v, arg, err, kPythonZero, kuint64max_py)) {
		*value = (uint64)v;
		return true;
	}
	return false;
}

static bool TypyDouble_CheckAndSet(TypeType t, double* value, PyObject* arg, const char* err) {
	if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	*value = PyFloat_AsDouble(arg);
	return true;
}

static bool TypyFloat_CheckAndSet(TypeType t, float* value, PyObject* arg, const char* err) {
	if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	*value = (float)PyFloat_AsDouble(arg);
	return true;
}

static bool TypyBool_CheckAndSet(TypeType t, bool* value, PyObject* arg, const char* err) {
	if (!PyInt_Check(arg) && !PyBool_Check(arg) && !PyLong_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	*value = PyInt_AsLong(arg) == 0 ? false : true;
	return true;
}

static bool TypyString_CheckAndSet(TypeType t, PyString* value, PyObject* arg, const char* err) {
	if (!arg || arg == Py_None) {
		Py_DECREF(*value);
		*value = NULL;
		return true;
	} else if (PyUnicode_Check(arg)) {
		Py_DECREF(*value);
		*value = (PyString)arg;
		return true;
	}
	register PyString s = (PyString)PyUnicode_FromEncodedObject(arg, "utf-8", NULL);
	if (!s) { return false; }
	Py_DECREF(*value);
	*value = s;
	return true;
}

static bool TypyBytes_CheckAndSet(TypeType t, PyBytes* value, PyObject* arg, const char* err) {
	if (!arg || arg == Py_None) {
		Py_DECREF(*value);
		*value = NULL;
		return true;
	} else if (PyUnicode_Check(arg)) {
		arg = PyUnicode_AsEncodedObject(arg, "utf-8", NULL);
		if (!arg) { return false; }
	} else if (PyBytes_Check(arg)) {
		Py_INCREF(arg);
	} else {
		FormatTypeError(arg, err);
		return false;
	}
	Py_DECREF(*value);
	*value = (PyBytes)arg;
	Py_DECREF(arg);
	return true;
}

abstract_CheckAndSet = {
	TypyEnum_CheckAndSet,       /* TYPE_ENUM       */
	TypyInt32_CheckAndSet,      /* TYPE_INT32      */
	TypyInt64_CheckAndSet,      /* TYPE_INT64      */
	TypyUint32_CheckAndSet,     /* TYPE_UINT32     */
	TypyUint64_CheckAndSet,     /* TYPE_UINT64     */
	TypyDouble_CheckAndSet,     /* TYPE_DOUBLE     */
	TypyFloat_CheckAndSet,      /* TYPE_FLOAT      */
	TypyBool_CheckAndSet,       /* TYPE_BOOL       */
	TypyBytes_CheckAndSet,      /* TYPE_BYTES      */
	TypyString_CheckAndSet,     /* TYPE_STRING     */
	/* TYPE_OBJECT     */
	/* TYPE_VARIANT    */
	/* TYPE_LIST       */
	/* TYPE_DICT       */
	TypyFixedPoint_CheckAndSet, /* TYPE_FIXEDPOINT */
	TypyPython_CheckAndSet,     /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_CopyFrom(TypeType type, TypyField* lvalue, TypyField rvalue) {
	*lvalue = rvalue;
}

static void TypyObject_CopyFrom(TypeType type, PyObject** lvalue, PyObject* rvalue) {
	Py_XDECREF(*lvalue);
	if (!rvalue) { *lvalue = NULL; return; }
	Py_INCREF(rvalue);
	*lvalue = rvalue;
}

abstract_CopyFrom = {
	TypyNumeric_CopyFrom, /* TYPE_ENUM       */
	TypyNumeric_CopyFrom, /* TYPE_INT32      */
	TypyNumeric_CopyFrom, /* TYPE_INT64      */
	TypyNumeric_CopyFrom, /* TYPE_UINT32     */
	TypyNumeric_CopyFrom, /* TYPE_UINT64     */
	TypyNumeric_CopyFrom, /* TYPE_DOUBLE     */
	TypyNumeric_CopyFrom, /* TYPE_FLOAT      */
	TypyNumeric_CopyFrom, /* TYPE_BOOL       */
	TypyObject_CopyFrom,  /* TYPE_BYTES      */
	TypyObject_CopyFrom,  /* TYPE_STRING     */
	TypyObject_CopyFrom,  /* TYPE_OBJECT     */
	TypyObject_CopyFrom,  /* TYPE_VARIANT    */
	TypyObject_CopyFrom,  /* TYPE_LIST       */
	TypyObject_CopyFrom,  /* TYPE_DICT       */
	TypyNumeric_CopyFrom, /* TYPE_FIXEDPOINT */
	TypyObject_CopyFrom,  /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_MergeFrom(TypeType type, TypyField* lvalue, TypyField rvalue) {
	if (rvalue != 0) { *lvalue = rvalue; }
}

static void TypyObject_MergeFrom(TypeType type, PyObject** lvalue, PyObject* rvalue) {
	if (rvalue != 0) {
		Py_XDECREF(*lvalue);
		Py_INCREF(rvalue);
		*lvalue = rvalue;
	}
}

abstract_MergeFrom = {
	TypyNumeric_MergeFrom, /* TYPE_ENUM       */
	TypyNumeric_MergeFrom, /* TYPE_INT32      */
	TypyNumeric_MergeFrom, /* TYPE_INT64      */
	TypyNumeric_MergeFrom, /* TYPE_UINT32     */
	TypyNumeric_MergeFrom, /* TYPE_UINT64     */
	TypyNumeric_MergeFrom, /* TYPE_DOUBLE     */
	TypyNumeric_MergeFrom, /* TYPE_FLOAT      */
	TypyNumeric_MergeFrom, /* TYPE_BOOL       */
	TypyObject_MergeFrom,  /* TYPE_BYTES      */
	TypyObject_MergeFrom,  /* TYPE_STRING     */
	/* TYPE_OBJECT     */
	/* TYPE_VARIANT    */
	/* TYPE_LIST       */
	/* TYPE_DICT       */
	TypyNumeric_MergeFrom, /* TYPE_FIXEDPOINT */
	TypyObject_MergeFrom,  /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_Clear(TypeType type, TypyField* value) {
	*value = 0;
}

static void TypyObject_Clear(TypeType type, PyObject** value) {
	Py_XDECREF(value);
	*value = 0;
}

abstract_Clear = {
	TypyNumeric_Clear, /* TYPE_ENUM       */
	TypyNumeric_Clear, /* TYPE_INT32      */
	TypyNumeric_Clear, /* TYPE_INT64      */
	TypyNumeric_Clear, /* TYPE_UINT32     */
	TypyNumeric_Clear, /* TYPE_UINT64     */
	TypyNumeric_Clear, /* TYPE_DOUBLE     */
	TypyNumeric_Clear, /* TYPE_FLOAT      */
	TypyNumeric_Clear, /* TYPE_BOOL       */
	TypyObject_Clear,  /* TYPE_BYTES      */
	TypyObject_Clear,  /* TYPE_STRING     */
	TypyObject_Clear,  /* TYPE_OBJECT     */
	TypyObject_Clear,  /* TYPE_VARIANT    */
	TypyObject_Clear,  /* TYPE_LIST       */
	TypyObject_Clear,  /* TYPE_DICT       */
	TypyNumeric_Clear, /* TYPE_FIXEDPOINT */
	TypyObject_Clear,  /* TYPE_PYTHON     */
};

//=============================================================================

abstract_Read = {
	TypyEnum_Read,       /* TYPE_ENUM       */
	                     /* TYPE_INT32      */
	                     /* TYPE_INT64      */
	                     /* TYPE_UINT32     */
	                     /* TYPE_UINT64     */
	                     /* TYPE_DOUBLE     */
	                     /* TYPE_FLOAT      */
	                     /* TYPE_BOOL       */
	                     /* TYPE_BYTES      */
	                     /* TYPE_STRING     */
	                     /* TYPE_OBJECT     */
	                     /* TYPE_VARIANT    */
	TypyList_Read,       /* TYPE_LIST       */
	                     /* TYPE_DICT       */
	TypyFixedPoint_Read, /* TYPE_FIXEDPOINT */
	TypyPython_Read,     /* TYPE_PYTHON     */
};

//=============================================================================

abstract_ReadPacked = {
	NULL,                /* TYPE_ENUM       */
	NULL,                /* TYPE_INT32      */
	NULL,                /* TYPE_INT64      */
	NULL,                /* TYPE_UINT32     */
	NULL,                /* TYPE_UINT64     */
	NULL,                /* TYPE_DOUBLE     */
	NULL,                /* TYPE_FLOAT      */
	NULL,                /* TYPE_BOOL       */
	NULL,                /* TYPE_BYTES      */
	NULL,                /* TYPE_STRING     */
	NULL,                /* TYPE_OBJECT     */
	NULL,                /* TYPE_VARIANT    */
	TypyList_ReadPacked, /* TYPE_LIST       */
	NULL,                /* TYPE_DICT       */
	NULL,                /* TYPE_FIXEDPOINT */
	NULL,                /* TYPE_PYTHON     */
};

//=============================================================================

abstract_Write = {
	TypyEnum_Write,       /* TYPE_ENUM       */
	                      /* TYPE_INT32      */
	                      /* TYPE_INT64      */
	                      /* TYPE_UINT32     */
	                      /* TYPE_UINT64     */
	                      /* TYPE_DOUBLE     */
	                      /* TYPE_FLOAT      */
	                      /* TYPE_BOOL       */
	                      /* TYPE_BYTES      */
	                      /* TYPE_STRING     */
	                      /* TYPE_OBJECT     */
	                      /* TYPE_VARIANT    */
	                      /* TYPE_LIST       */
	                      /* TYPE_DICT       */
	TypyFixedPoint_Write, /* TYPE_FIXEDPOINT */
	TypyPython_Write,     /* TYPE_PYTHON     */
};

//=============================================================================

abstract_ByteSize = {
	TypyEnum_ByteSize,       /* TYPE_ENUM       */
	                         /* TYPE_INT32      */
	                         /* TYPE_INT64      */
	                         /* TYPE_UINT32     */
	                         /* TYPE_UINT64     */
	                         /* TYPE_DOUBLE     */
	                         /* TYPE_FLOAT      */
	                         /* TYPE_BOOL       */
	                         /* TYPE_BYTES      */
	                         /* TYPE_STRING     */
	                         /* TYPE_OBJECT     */
	                         /* TYPE_VARIANT    */
	                         /* TYPE_LIST       */
	                         /* TYPE_DICT       */
	TypyFixedPoint_ByteSize, /* TYPE_FIXEDPOINT */
	TypyPython_ByteSize,     /* TYPE_PYTHON     */
};
