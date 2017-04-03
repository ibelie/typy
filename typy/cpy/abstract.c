// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"


static PyObject* TypyInt32_GetPyObject  (TypeType t, int32* v)  { return PyInt_FromLong(*v); }
static PyObject* TypyInt64_GetPyObject  (TypeType t, int64* v)  { return PyLong_FromLongLong(*v); }
static PyObject* TypyUint32_GetPyObject (TypeType t, uint32* v) { return PyInt_FromSize_t(*v); }
static PyObject* TypyUint64_GetPyObject (TypeType t, uint64* v) { return PyLong_FromUnsignedLongLong(*v); }
static PyObject* TypyBool_GetPyObject   (TypeType t, bool* v)   { return PyBool_FromLong(*v); }
static PyObject* TypyDouble_GetPyObject (TypeType t, double* v) { return PyFloat_FromDouble(*v); }
static PyObject* TypyFloat_GetPyObject  (TypeType t, float* v)  { return PyFloat_FromDouble(*v); }

static PyObject* TypyPyObject_GetPyObject(TypeType type, PyObject** value) {
	if (!(*value)) {
		Py_RETURN_NONE;
	}
	Py_INCREF(*value);
	return *value;
}

GetPyObject abstract_GetPyObject[MAX_FIELD_TYPE] = {
	(GetPyObject)TypyEnum_GetPyObject,       /* TYPE_ENUM       */
	(GetPyObject)TypyInt32_GetPyObject,      /* TYPE_INT32      */
	(GetPyObject)TypyInt64_GetPyObject,      /* TYPE_INT64      */
	(GetPyObject)TypyUint32_GetPyObject,     /* TYPE_UINT32     */
	(GetPyObject)TypyUint64_GetPyObject,     /* TYPE_UINT64     */
	(GetPyObject)TypyDouble_GetPyObject,     /* TYPE_DOUBLE     */
	(GetPyObject)TypyFloat_GetPyObject,      /* TYPE_FLOAT      */
	(GetPyObject)TypyBool_GetPyObject,       /* TYPE_BOOL       */
	(GetPyObject)TypyPyObject_GetPyObject,   /* TYPE_BYTES      */
	(GetPyObject)TypyPyObject_GetPyObject,   /* TYPE_STRING     */
	(GetPyObject)TypyPyObject_GetPyObject,   /* TYPE_OBJECT     */
	(GetPyObject)TypyVariant_GetPyObject,    /* TYPE_VARIANT    */
	(GetPyObject)TypyList_GetPyObject,       /* TYPE_LIST       */
	(GetPyObject)TypyDict_GetPyObject,       /* TYPE_DICT       */
	(GetPyObject)TypyFixedPoint_GetPyObject, /* TYPE_FIXEDPOINT */
	(GetPyObject)TypyPyObject_GetPyObject,   /* TYPE_PYTHON     */
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
	int64 v;
	if (CheckAndSetInteger(&v, arg, err, kint32min_py, kint32max_py)) {
		*value = (int32)v;
		return true;
	}
	return false;
}

static bool TypyInt64_CheckAndSet(TypeType t, int64* value, PyObject* arg, const char* err) {
	int64 v;
	if (CheckAndSetInteger(&v, arg, err, kint64min_py, kint64max_py)) {
		*value = (int64)v;
		return true;
	}
	return false;
}

static bool TypyUint32_CheckAndSet(TypeType t, uint32* value, PyObject* arg, const char* err) {
	int64 v;
	if (CheckAndSetInteger(&v, arg, err, kPythonZero, kuint32max_py)) {
		*value = (uint32)v;
		return true;
	}
	return false;
}

static bool TypyUint64_CheckAndSet(TypeType t, uint64* value, PyObject* arg, const char* err) {
	int64 v;
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
		Py_INCREF(arg);
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
	return true;
}

CheckAndSet abstract_CheckAndSet[MAX_FIELD_TYPE] = {
	(CheckAndSet)TypyEnum_CheckAndSet,       /* TYPE_ENUM       */
	(CheckAndSet)TypyInt32_CheckAndSet,      /* TYPE_INT32      */
	(CheckAndSet)TypyInt64_CheckAndSet,      /* TYPE_INT64      */
	(CheckAndSet)TypyUint32_CheckAndSet,     /* TYPE_UINT32     */
	(CheckAndSet)TypyUint64_CheckAndSet,     /* TYPE_UINT64     */
	(CheckAndSet)TypyDouble_CheckAndSet,     /* TYPE_DOUBLE     */
	(CheckAndSet)TypyFloat_CheckAndSet,      /* TYPE_FLOAT      */
	(CheckAndSet)TypyBool_CheckAndSet,       /* TYPE_BOOL       */
	(CheckAndSet)TypyBytes_CheckAndSet,      /* TYPE_BYTES      */
	(CheckAndSet)TypyString_CheckAndSet,     /* TYPE_STRING     */
	(CheckAndSet)TypyObject_CheckAndSet,     /* TYPE_OBJECT     */
	(CheckAndSet)TypyVariant_CheckAndSet,    /* TYPE_VARIANT    */
	(CheckAndSet)0,                          /* TYPE_LIST       */
	(CheckAndSet)0,                          /* TYPE_DICT       */
	(CheckAndSet)TypyFixedPoint_CheckAndSet, /* TYPE_FIXEDPOINT */
	(CheckAndSet)TypyPython_CheckAndSet,     /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_CopyFrom(TypeType type, TypyField* lvalue, TypyField rvalue) {
	*lvalue = rvalue;
}

static void TypyPyObject_CopyFrom(TypeType type, PyObject** lvalue, PyObject* rvalue) {
	Py_XDECREF(*lvalue);
	if (!rvalue) { *lvalue = NULL; return; }
	Py_INCREF(rvalue);
	*lvalue = rvalue;
}

CopyFrom abstract_CopyFrom[MAX_FIELD_TYPE] = {
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_ENUM       */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_INT32      */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_INT64      */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_UINT32     */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_UINT64     */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_DOUBLE     */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_FLOAT      */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_BOOL       */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_BYTES      */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_STRING     */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_OBJECT     */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_VARIANT    */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_LIST       */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_DICT       */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_FIXEDPOINT */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_MergeFrom(TypeType type, TypyField* lvalue, TypyField rvalue) {
	if (rvalue != 0) { *lvalue = rvalue; }
}

static void TypyPyObject_MergeFrom(TypeType type, PyObject** lvalue, PyObject* rvalue) {
	if (rvalue != 0) {
		Py_XDECREF(*lvalue);
		Py_INCREF(rvalue);
		*lvalue = rvalue;
	}
}

MergeFrom abstract_MergeFrom[MAX_FIELD_TYPE] = {
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_ENUM       */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_INT32      */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_INT64      */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_UINT32     */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_UINT64     */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_DOUBLE     */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_FLOAT      */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_BOOL       */
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_BYTES      */
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_STRING     */
	(MergeFrom)TypyObject_MergeFrom,   /* TYPE_OBJECT     */
	(MergeFrom)TypyVariant_MergeFrom,  /* TYPE_VARIANT    */
	(MergeFrom)0,                      /* TYPE_LIST       */
	(MergeFrom)0,                      /* TYPE_DICT       */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_FIXEDPOINT */
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_Clear(TypeType type, TypyField* value) {
	*value = 0;
}

static void TypyPyObject_Clear(TypeType type, PyObject** value) {
	Py_XDECREF(value);
	*value = 0;
}

Clear abstract_Clear[MAX_FIELD_TYPE] = {
	(Clear)TypyNumeric_Clear,  /* TYPE_ENUM       */
	(Clear)TypyNumeric_Clear,  /* TYPE_INT32      */
	(Clear)TypyNumeric_Clear,  /* TYPE_INT64      */
	(Clear)TypyNumeric_Clear,  /* TYPE_UINT32     */
	(Clear)TypyNumeric_Clear,  /* TYPE_UINT64     */
	(Clear)TypyNumeric_Clear,  /* TYPE_DOUBLE     */
	(Clear)TypyNumeric_Clear,  /* TYPE_FLOAT      */
	(Clear)TypyNumeric_Clear,  /* TYPE_BOOL       */
	(Clear)TypyPyObject_Clear, /* TYPE_BYTES      */
	(Clear)TypyPyObject_Clear, /* TYPE_STRING     */
	(Clear)TypyPyObject_Clear, /* TYPE_OBJECT     */
	(Clear)TypyPyObject_Clear, /* TYPE_VARIANT    */
	(Clear)TypyPyObject_Clear, /* TYPE_LIST       */
	(Clear)TypyPyObject_Clear, /* TYPE_DICT       */
	(Clear)TypyNumeric_Clear,  /* TYPE_FIXEDPOINT */
	(Clear)TypyPyObject_Clear, /* TYPE_PYTHON     */
};

//=============================================================================

static bool TypyField_Read(TypeType t, TypyField* value, byte** input, size_t* length) {
	uint64 data;
	if (Typy_ReadVarint64(input, length, &data)) {
		*value = (TypyField)data;
		return true;
	} else {
		return false;
	}
}

static bool TypyBytes_Read(TypeType t, PyBytes* value, byte** input, size_t* length) {
	uint32 size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return false;
	} else if (size > *length) {
		return false;
	}
	register PyBytes bytes = (PyBytes)PyBytes_FromStringAndSize(NULL, size);
	if (!bytes) { return false; }
	memcpy(PyBytes_AS_STRING(bytes), *input, size);
	*input += size;
	*length -= size;
	Py_XDECREF(*value);
	*value = bytes;
	return true;
}

static bool TypyString_Read(TypeType t, PyString* value, byte** input, size_t* length) {
	uint32 size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return false;
	} else if (size > *length) {
		return false;
	}
	register PyObject* bytes = PyBytes_FromStringAndSize(NULL, size);
	if (!bytes) { return false; }
	memcpy(PyBytes_AS_STRING(bytes), *input, size);
	*input += size;
	*length -= size;
	Py_XDECREF(*value);
	*value = (PyString)PyUnicode_FromEncodedObject(bytes, "utf-8", NULL);
	Py_DECREF(bytes);
	return true;
}

Read abstract_Read[MAX_FIELD_TYPE] = {
	(Read)TypyField_Read,      /* TYPE_ENUM       */
	(Read)Typy_ReadVarint32,   /* TYPE_INT32      */
	(Read)Typy_ReadVarint64,   /* TYPE_INT64      */
	(Read)Typy_ReadVarint32,   /* TYPE_UINT32     */
	(Read)Typy_ReadVarint64,   /* TYPE_UINT64     */
	(Read)Typy_Read64,         /* TYPE_DOUBLE     */
	(Read)Typy_Read32,         /* TYPE_FLOAT      */
	(Read)Typy_ReadByte,       /* TYPE_BOOL       */
	(Read)TypyBytes_Read,      /* TYPE_BYTES      */
	(Read)TypyString_Read,     /* TYPE_STRING     */
	(Read)TypyObject_Read,     /* TYPE_OBJECT     */
	(Read)TypyVariant_Read,    /* TYPE_VARIANT    */
	(Read)TypyList_Read,       /* TYPE_LIST       */
	(Read)0,                   /* TYPE_DICT       */
	(Read)TypyField_Read,      /* TYPE_FIXEDPOINT */
	(Read)TypyPython_Read,     /* TYPE_PYTHON     */
};

ReadPacked abstract_ReadPacked[MAX_FIELD_TYPE] = {
	NULL,                            /* TYPE_ENUM       */
	NULL,                            /* TYPE_INT32      */
	NULL,                            /* TYPE_INT64      */
	NULL,                            /* TYPE_UINT32     */
	NULL,                            /* TYPE_UINT64     */
	NULL,                            /* TYPE_DOUBLE     */
	NULL,                            /* TYPE_FLOAT      */
	NULL,                            /* TYPE_BOOL       */
	NULL,                            /* TYPE_BYTES      */
	NULL,                            /* TYPE_STRING     */
	NULL,                            /* TYPE_OBJECT     */
	NULL,                            /* TYPE_VARIANT    */
	(ReadPacked)TypyList_ReadPacked, /* TYPE_LIST       */
	NULL,                            /* TYPE_DICT       */
	NULL,                            /* TYPE_FIXEDPOINT */
	NULL,                            /* TYPE_PYTHON     */
};

//=============================================================================

#define WRITE(NAME, TYPE) \
static size_t NAME(TypeType t, TYPE* value, int tag, byte* output) { \
	register size_t size = 0;                                        \
	if (*value) {                                                    \
		size = Typy_WriteTag(output, tag);                           \
		size += IblPutUvarint(output + size, *value);                \
	}                                                                \
	return size;                                                     \
}
WRITE(TypyEnum_Write,   TypyField)
WRITE(TypyInt32_Write,  int32)
WRITE(TypyInt64_Write,  int64)
WRITE(TypyUint32_Write, uint32)
WRITE(TypyUint64_Write, uint64)
#undef WRITE

#define WRITE(NAME, TYPE, PARAMTYPE, WRITER) \
static size_t NAME(TypeType t, TYPE* value, int tag, byte* output) { \
	register size_t size = 0;                                        \
	if (*value) {                                                    \
		size = Typy_WriteTag(output, tag);                           \
		size += WRITER(output + size, (PARAMTYPE*)value);            \
	}                                                                \
	return size;                                                     \
}
WRITE(TypyDouble_Write, double, uint64, Typy_Write64)
WRITE(TypyFloat_Write,  float,  uint32, Typy_Write32)
WRITE(TypyBool_Write,   bool,   byte,   Typy_WriteByte)
#undef WRITE

inline PyObject* _DecodeString(PyString value) {
	PyObject* result = NULL;
	if (isDefaultEncodingUTF8) {
		result = _PyUnicode_AsDefaultEncodedString((PyObject*)value, NULL);
		Py_INCREF(result);
	} else {
		result = PyUnicode_AsEncodedObject((PyObject*)value, "utf-8", NULL);
	}
	return result;
}

static size_t TypyBytes_Write(TypeType t, PyBytes* value, int tag, byte* output) {
	register size_t size = 0;
	if (*value) {
		Py_ssize_t length = PyBytes_GET_SIZE(*value);
		if (length > 0) {
			size = Typy_WriteTag(output, tag);
			size += IblPutUvarint(output + size, (uint64)length);
			memcpy(output + size, PyBytes_AS_STRING(*value), length);
			return size + length;
		}
	}
	return 0;
}

static size_t TypyString_Write(TypeType t, PyString* value, int tag, byte* output) {
	register size_t size = 0;
	if (*value) {
		register PyObject* bytes = _DecodeString(*value);
		if (!bytes) { return 0; }
		Py_ssize_t length = PyBytes_GET_SIZE(bytes);
		if (length > 0) {
			size = Typy_WriteTag(output, tag);
			size += IblPutUvarint(output + size, (uint64)length);
			memcpy(output + size, PyBytes_AS_STRING(bytes), length);
			Py_DECREF(bytes);
			return size + length;
		}
		Py_DECREF(bytes);
	}
	return 0;
}

Write abstract_Write[MAX_FIELD_TYPE] = {
	(Write)TypyEnum_Write,       /* TYPE_ENUM       */
	(Write)TypyInt32_Write,      /* TYPE_INT32      */
	(Write)TypyInt64_Write,      /* TYPE_INT64      */
	(Write)TypyUint32_Write,     /* TYPE_UINT32     */
	(Write)TypyUint64_Write,     /* TYPE_UINT64     */
	(Write)TypyDouble_Write,     /* TYPE_DOUBLE     */
	(Write)TypyFloat_Write,      /* TYPE_FLOAT      */
	(Write)TypyBool_Write,       /* TYPE_BOOL       */
	(Write)TypyBytes_Write,      /* TYPE_BYTES      */
	(Write)TypyString_Write,     /* TYPE_STRING     */
	(Write)TypyObject_Write,     /* TYPE_OBJECT     */
	(Write)TypyVariant_Write,    /* TYPE_VARIANT    */
	(Write)0,                    /* TYPE_LIST       */
	(Write)0,                    /* TYPE_DICT       */
	(Write)TypyFixedPoint_Write, /* TYPE_FIXEDPOINT */
	(Write)TypyPython_Write,     /* TYPE_PYTHON     */
};

//=============================================================================

#define BYTESIZE(NAME, TYPE) \
static size_t NAME(TypeType t, TYPE* value, int tagsize) { \
	return *value ? tagsize + IblSizeVarint(*value) : 0;   \
}
BYTESIZE(TypyEnum_ByteSize,       TypyField)
BYTESIZE(TypyInt32_ByteSize,      int32)
BYTESIZE(TypyInt64_ByteSize,      int64)
BYTESIZE(TypyUint32_ByteSize,     uint32)
BYTESIZE(TypyUint64_ByteSize,     uint64)
BYTESIZE(TypyFixedPoint_ByteSize, TypyField)
#undef BYTESIZE

#define BYTESIZE(NAME, TYPE) \
static size_t NAME(TypeType t, TYPE* value, int tagsize) { \
	return *value ? tagsize + sizeof(TYPE) : 0;            \
}
BYTESIZE(TypyDouble_ByteSize, double)
BYTESIZE(TypyFloat_ByteSize,  float)
BYTESIZE(TypyBool_ByteSize,   bool)
#undef BYTESIZE

static size_t TypyBytes_ByteSize(TypeType t, PyBytes* value, int tagsize) {
	if (*value) {
		register Py_ssize_t size = PyBytes_GET_SIZE(*value);
		if (size > 0) {
			return tagsize + IblSizeVarint((uint64)size) + size;
		}
	}
	return 0;
}

static size_t TypyString_ByteSize(TypeType t, PyString* value, int tagsize) {
	if (*value) {
		register PyObject* bytes = _DecodeString(*value);
		if (!bytes) { return 0; }
		register Py_ssize_t size = PyBytes_GET_SIZE(bytes);
		if (size > 0) {
			Py_DECREF(bytes);
			return tagsize + IblSizeVarint((uint64)size) + size;
		}
		Py_DECREF(bytes);
	}
	return 0;
}

ByteSize abstract_ByteSize[MAX_FIELD_TYPE] = {
	(ByteSize)TypyEnum_ByteSize,       /* TYPE_ENUM       */
	(ByteSize)TypyInt32_ByteSize,      /* TYPE_INT32      */
	(ByteSize)TypyInt64_ByteSize,      /* TYPE_INT64      */
	(ByteSize)TypyUint32_ByteSize,     /* TYPE_UINT32     */
	(ByteSize)TypyUint64_ByteSize,     /* TYPE_UINT64     */
	(ByteSize)TypyDouble_ByteSize,     /* TYPE_DOUBLE     */
	(ByteSize)TypyFloat_ByteSize,      /* TYPE_FLOAT      */
	(ByteSize)TypyBool_ByteSize,       /* TYPE_BOOL       */
	(ByteSize)TypyBytes_ByteSize,      /* TYPE_BYTES      */
	(ByteSize)TypyString_ByteSize,     /* TYPE_STRING     */
	(ByteSize)TypyObject_ByteSize,     /* TYPE_OBJECT     */
	(ByteSize)TypyVariant_ByteSize,    /* TYPE_VARIANT    */
	(ByteSize)0,                       /* TYPE_LIST       */
	(ByteSize)0,                       /* TYPE_DICT       */
	(ByteSize)TypyFixedPoint_ByteSize, /* TYPE_FIXEDPOINT */
	(ByteSize)TypyPython_ByteSize,     /* TYPE_PYTHON     */
};
