// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"


static PyObject* TypyInt32_GetPyObject  (TypyType t, int32* v)  { return PyInt_FromLong(*v); }
static PyObject* TypyUint32_GetPyObject (TypyType t, uint32* v) { return PyInt_FromSize_t(*v); }
static PyObject* TypyBool_GetPyObject   (TypyType t, bool* v)   { return PyBool_FromLong(*v); }
static PyObject* TypyFloat_GetPyObject  (TypyType t, float* v)  { return PyFloat_FromDouble(*v); }

static PyObject* TypyUint64_GetPyObject (TypyType t, uint64* v) {
	if (sizeof(TypyField) < sizeof(uint64)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %lu-bit, cannot use uint64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return NULL;
	} else {
		return PyLong_FromUnsignedLongLong(*v);
	}
}

static PyObject* TypyInt64_GetPyObject  (TypyType t, int64* v)  {
	if (sizeof(TypyField) < sizeof(int64)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %lu-bit, cannot use int64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return NULL;
	} else {
		return PyLong_FromLongLong(*v);
	}
}

static PyObject* TypyDouble_GetPyObject (TypyType t, double* v) {
	if (sizeof(TypyField) < sizeof(double)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %lu-bit, cannot use double. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return NULL;
	} else {
		return PyFloat_FromDouble(*v);
	}
}

static PyObject* TypyPyObject_GetPyObject(TypyType type, PyObject** value) {
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
	(GetPyObject)TypyFixedPoint_GetPyObject, /* TYPE_FIXEDPOINT */
	(GetPyObject)TypyDouble_GetPyObject,     /* TYPE_DOUBLE     */
	(GetPyObject)TypyFloat_GetPyObject,      /* TYPE_FLOAT      */
	(GetPyObject)TypyBool_GetPyObject,       /* TYPE_BOOL       */
	(GetPyObject)TypyPyObject_GetPyObject,   /* TYPE_BYTES      */
	(GetPyObject)TypyPyObject_GetPyObject,   /* TYPE_STRING     */
	(GetPyObject)TypyPyObject_GetPyObject,   /* TYPE_OBJECT     */
	(GetPyObject)TypyVariant_GetPyObject,    /* TYPE_VARIANT    */
	(GetPyObject)TypyList_GetPyObject,       /* TYPE_LIST       */
	(GetPyObject)TypyDict_GetPyObject,       /* TYPE_DICT       */
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

static bool TypyInt32_CheckAndSet(TypyType t, int32* value, PyObject* arg, const char* err) {
	int64 v;
	if (CheckAndSetInteger(&v, arg, err, kint32min_py, kint32max_py)) {
		*value = (int32)v;
		return true;
	}
	return false;
}

static bool TypyInt64_CheckAndSet(TypyType t, int64* value, PyObject* arg, const char* err) {
	if (sizeof(TypyField) < sizeof(int64)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %lu-bit, cannot use int64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return false;
	} else {
		int64 v;
		if (CheckAndSetInteger(&v, arg, err, kint64min_py, kint64max_py)) {
			*value = (int64)v;
			return true;
		}
		return false;
	}
}

static bool TypyUint32_CheckAndSet(TypyType t, uint32* value, PyObject* arg, const char* err) {
	int64 v;
	if (CheckAndSetInteger(&v, arg, err, kPythonZero, kuint32max_py)) {
		*value = (uint32)v;
		return true;
	}
	return false;
}

static bool TypyUint64_CheckAndSet(TypyType t, uint64* value, PyObject* arg, const char* err) {
	if (sizeof(TypyField) < sizeof(uint64)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %lu-bit, cannot use uint64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return false;
	} else {
		int64 v;
		if (CheckAndSetInteger(&v, arg, err, kPythonZero, kuint64max_py)) {
			*value = (uint64)v;
			return true;
		}
		return false;
	}
}

static bool TypyDouble_CheckAndSet(TypyType t, double* value, PyObject* arg, const char* err) {
	if (sizeof(TypyField) < sizeof(double)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %lu-bit, cannot use double. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return false;
	} else {
		if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
			FormatTypeError(arg, err);
			return false;
		}
		*value = PyFloat_AsDouble(arg);
		return true;
	}
}

static bool TypyFloat_CheckAndSet(TypyType t, float* value, PyObject* arg, const char* err) {
	if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	*value = (float)PyFloat_AsDouble(arg);
	return true;
}

static bool TypyBool_CheckAndSet(TypyType t, bool* value, PyObject* arg, const char* err) {
	if (!PyInt_Check(arg) && !PyBool_Check(arg) && !PyLong_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	*value = PyInt_AsLong(arg) == 0 ? false : true;
	return true;
}

static bool TypyString_CheckAndSet(TypyType t, PyString* value, PyObject* arg, const char* err) {
	if (!arg || arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	} else if (PyUnicode_Check(arg)) {
		Py_XDECREF(*value);
		Py_INCREF(arg);
		*value = (PyString)arg;
		return true;
	}
	register PyString s = (PyString)PyUnicode_FromEncodedObject(arg, "utf-8", NULL);
	if (!s) { return false; }
	Py_XDECREF(*value);
	*value = s;
	return true;
}

static bool TypyBytes_CheckAndSet(TypyType t, PyBytes* value, PyObject* arg, const char* err) {
	if (!arg || arg == Py_None) {
		Py_XDECREF(*value);
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
	Py_XDECREF(*value);
	*value = (PyBytes)arg;
	return true;
}

CheckAndSet abstract_CheckAndSet[MAX_FIELD_TYPE] = {
	(CheckAndSet)TypyEnum_CheckAndSet,       /* TYPE_ENUM       */
	(CheckAndSet)TypyInt32_CheckAndSet,      /* TYPE_INT32      */
	(CheckAndSet)TypyInt64_CheckAndSet,      /* TYPE_INT64      */
	(CheckAndSet)TypyUint32_CheckAndSet,     /* TYPE_UINT32     */
	(CheckAndSet)TypyUint64_CheckAndSet,     /* TYPE_UINT64     */
	(CheckAndSet)TypyFixedPoint_CheckAndSet, /* TYPE_FIXEDPOINT */
	(CheckAndSet)TypyDouble_CheckAndSet,     /* TYPE_DOUBLE     */
	(CheckAndSet)TypyFloat_CheckAndSet,      /* TYPE_FLOAT      */
	(CheckAndSet)TypyBool_CheckAndSet,       /* TYPE_BOOL       */
	(CheckAndSet)TypyBytes_CheckAndSet,      /* TYPE_BYTES      */
	(CheckAndSet)TypyString_CheckAndSet,     /* TYPE_STRING     */
	(CheckAndSet)TypyObject_CheckAndSet,     /* TYPE_OBJECT     */
	(CheckAndSet)TypyVariant_CheckAndSet,    /* TYPE_VARIANT    */
	(CheckAndSet)TypyList_CheckAndSet,       /* TYPE_LIST       */
	(CheckAndSet)TypyDict_CheckAndSet,       /* TYPE_DICT       */
	(CheckAndSet)TypyPython_CheckAndSet,     /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_CopyFrom(TypyType type, TypyField* lvalue, TypyField rvalue) {
	*lvalue = rvalue;
}

static void TypyPyObject_CopyFrom(TypyType type, PyObject** lvalue, PyObject* rvalue) {
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
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_FIXEDPOINT */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_DOUBLE     */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_FLOAT      */
	(CopyFrom)TypyNumeric_CopyFrom,  /* TYPE_BOOL       */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_BYTES      */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_STRING     */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_OBJECT     */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_VARIANT    */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_LIST       */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_DICT       */
	(CopyFrom)TypyPyObject_CopyFrom, /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_MergeFrom(TypyType type, TypyField* lvalue, TypyField rvalue) {
	if (rvalue != 0) { *lvalue = rvalue; }
}

static void TypyPyObject_MergeFrom(TypyType type, PyObject** lvalue, PyObject* rvalue) {
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
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_FIXEDPOINT */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_DOUBLE     */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_FLOAT      */
	(MergeFrom)TypyNumeric_MergeFrom,  /* TYPE_BOOL       */
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_BYTES      */
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_STRING     */
	(MergeFrom)TypyObject_MergeFrom,   /* TYPE_OBJECT     */
	(MergeFrom)TypyVariant_MergeFrom,  /* TYPE_VARIANT    */
	(MergeFrom)TypyList_MergeFrom,     /* TYPE_LIST       */
	(MergeFrom)TypyDict_MergeFrom,     /* TYPE_DICT       */
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_Clear(TypyType type, TypyField* value) {
	*value = 0;
}

static void TypyPyObject_Clear(TypyType type, PyObject** value) {
	Py_XDECREF(*value);
	*value = 0;
}

Clear abstract_Clear[MAX_FIELD_TYPE] = {
	(Clear)TypyNumeric_Clear,  /* TYPE_ENUM       */
	(Clear)TypyNumeric_Clear,  /* TYPE_INT32      */
	(Clear)TypyNumeric_Clear,  /* TYPE_INT64      */
	(Clear)TypyNumeric_Clear,  /* TYPE_UINT32     */
	(Clear)TypyNumeric_Clear,  /* TYPE_UINT64     */
	(Clear)TypyNumeric_Clear,  /* TYPE_FIXEDPOINT */
	(Clear)TypyNumeric_Clear,  /* TYPE_DOUBLE     */
	(Clear)TypyNumeric_Clear,  /* TYPE_FLOAT      */
	(Clear)TypyNumeric_Clear,  /* TYPE_BOOL       */
	(Clear)TypyPyObject_Clear, /* TYPE_BYTES      */
	(Clear)TypyPyObject_Clear, /* TYPE_STRING     */
	(Clear)TypyPyObject_Clear, /* TYPE_OBJECT     */
	(Clear)TypyPyObject_Clear, /* TYPE_VARIANT    */
	(Clear)TypyPyObject_Clear, /* TYPE_LIST       */
	(Clear)TypyPyObject_Clear, /* TYPE_DICT       */
	(Clear)TypyPyObject_Clear, /* TYPE_PYTHON     */
};

//=============================================================================

#define READ(NAME, TYPE, READER) \
static bool NAME(TypyType t, TYPE* value, byte** input, size_t* length) { \
	return READER(input, length, value);                                  \
}
READ(TypyVarint32_Read, uint32, Typy_ReadVarint32)
READ(TypyVarint64_Read, uint64, Typy_ReadVarint64)
READ(TypyDouble_Read,   uint64, Typy_Read64)
READ(TypyFloat_Read,    uint32, Typy_Read32)
READ(TypyBool_Read,     bool,   Typy_ReadByte)
#undef READ

static bool TypyField_Read(TypyType t, TypyField* value, byte** input, size_t* length) {
	uint64 data;
	if (Typy_ReadVarint64(input, length, &data)) {
		*value = (TypyField)data;
		return true;
	} else {
		return false;
	}
}

static inline PyBytes Typy_ReadBytes(byte** input, size_t* length) {
	uint32 size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return NULL;
	} else if (size > *length) {
		return NULL;
	}
	register PyBytes bytes = (PyBytes)PyBytes_FromStringAndSize(NULL, size);
	if (!bytes) { return NULL; }
	memcpy(PyBytes_AS_STRING(bytes), *input, size);
	*input += size;
	*length -= size;
	return bytes;
}

static bool TypyBytes_Read(TypyType t, PyBytes* value, byte** input, size_t* length) {
	register PyBytes bytes = Typy_ReadBytes(input, length);
	if (!bytes) { return false; }
	Py_XDECREF(*value);
	*value = bytes;
	return true;
}

static bool TypyString_Read(TypyType t, PyString* value, byte** input, size_t* length) {
	register PyBytes bytes = Typy_ReadBytes(input, length);
	if (!bytes) { return false; }
	Py_XDECREF(*value);
	*value = (PyString)PyUnicode_FromEncodedObject((PyObject*)bytes, "utf-8", NULL);
	Py_DECREF(bytes);
	return true;
}

Read abstract_Read[MAX_FIELD_TYPE] = {
	(Read)TypyField_Read,      /* TYPE_ENUM       */
	(Read)TypyVarint32_Read,   /* TYPE_INT32      */
	(Read)TypyVarint64_Read,   /* TYPE_INT64      */
	(Read)TypyVarint32_Read,   /* TYPE_UINT32     */
	(Read)TypyVarint64_Read,   /* TYPE_UINT64     */
	(Read)TypyField_Read,      /* TYPE_FIXEDPOINT */
	(Read)TypyDouble_Read,     /* TYPE_DOUBLE     */
	(Read)TypyFloat_Read,      /* TYPE_FLOAT      */
	(Read)TypyBool_Read,       /* TYPE_BOOL       */
	(Read)TypyBytes_Read,      /* TYPE_BYTES      */
	(Read)TypyString_Read,     /* TYPE_STRING     */
	(Read)TypyObject_Read,     /* TYPE_OBJECT     */
	(Read)TypyVariant_Read,    /* TYPE_VARIANT    */
	(Read)TypyList_Read,       /* TYPE_LIST       */
	(Read)TypyDict_Read,       /* TYPE_DICT       */
	(Read)TypyPython_Read,     /* TYPE_PYTHON     */
};

//=============================================================================

#define WRITE(NAME, TYPE) \
static size_t NAME(TypyType t, TYPE* value, int tag, byte* output) { \
	register size_t size = 0;                                        \
	if (tag) {                                                       \
		size += Typy_WriteTag(output, tag);                          \
	}                                                                \
	return size + IblPutUvarint(output + size, *value);              \
}
WRITE(TypyEnum_Write,       TypyField)
WRITE(TypyInt32_Write,      int32)
WRITE(TypyInt64_Write,      int64)
WRITE(TypyUint32_Write,     uint32)
WRITE(TypyUint64_Write,     uint64)
WRITE(TypyFixedPoint_Write, TypyField)
#undef WRITE

#define WRITE(NAME, TYPE, PARAMTYPE, WRITER) \
static size_t NAME(TypyType t, TYPE* value, int tag, byte* output) { \
	register size_t size = 0;                                        \
	if (tag) {                                                       \
		size += Typy_WriteTag(output, tag);                          \
	}                                                                \
	return size + WRITER(output + size, (PARAMTYPE*)value);          \
}
WRITE(TypyDouble_Write, double, uint64, Typy_Write64)
WRITE(TypyFloat_Write,  float,  uint32, Typy_Write32)
WRITE(TypyBool_Write,   bool,   bool,   Typy_WriteByte)
#undef WRITE

static inline PyObject* _DecodeString(PyString value) {
	PyObject* result = NULL;
	if (isDefaultEncodingUTF8) {
		result = _PyUnicode_AsDefaultEncodedString((PyObject*)value, NULL);
		Py_INCREF(result);
	} else {
		result = PyUnicode_AsEncodedObject((PyObject*)value, "utf-8", NULL);
	}
	return result;
}

static size_t TypyBytes_Write(TypyType t, PyBytes* value, int tag, byte* output) {
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	if (*value) {
		Py_ssize_t length = PyBytes_GET_SIZE(*value);
		if (length > 0) {
			size += IblPutUvarint(output + size, length);
			memcpy(output + size, PyBytes_AS_STRING(*value), length);
			return size + length;
		}
	}
	output[size] = 0;
	return size + 1;
}

static size_t TypyString_Write(TypyType t, PyString* value, int tag, byte* output) {
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	if (*value) {
		register PyObject* bytes = _DecodeString(*value);
		if (bytes) {
			Py_ssize_t length = PyBytes_GET_SIZE(bytes);
			if (length > 0) {
				size += IblPutUvarint(output + size, length);
				memcpy(output + size, PyBytes_AS_STRING(bytes), length);
				Py_DECREF(bytes);
				return size + length;
			}
			Py_DECREF(bytes);
		}
	}
	output[size] = 0;
	return size + 1;
}

Write abstract_Write[MAX_FIELD_TYPE] = {
	(Write)TypyEnum_Write,       /* TYPE_ENUM       */
	(Write)TypyInt32_Write,      /* TYPE_INT32      */
	(Write)TypyInt64_Write,      /* TYPE_INT64      */
	(Write)TypyUint32_Write,     /* TYPE_UINT32     */
	(Write)TypyUint64_Write,     /* TYPE_UINT64     */
	(Write)TypyFixedPoint_Write, /* TYPE_FIXEDPOINT */
	(Write)TypyDouble_Write,     /* TYPE_DOUBLE     */
	(Write)TypyFloat_Write,      /* TYPE_FLOAT      */
	(Write)TypyBool_Write,       /* TYPE_BOOL       */
	(Write)TypyBytes_Write,      /* TYPE_BYTES      */
	(Write)TypyString_Write,     /* TYPE_STRING     */
	(Write)TypyObject_Write,     /* TYPE_OBJECT     */
	(Write)TypyVariant_Write,    /* TYPE_VARIANT    */
	(Write)TypyList_Write,       /* TYPE_LIST       */
	(Write)TypyDict_Write,       /* TYPE_DICT       */
	(Write)TypyPython_Write,     /* TYPE_PYTHON     */
};

//=============================================================================

#define BYTESIZE(NAME, TYPE) \
static size_t NAME(TypyType t, TYPE* value, int tagsize) { \
	return tagsize + IblSizeVarint(*value);                \
}
BYTESIZE(TypyEnum_ByteSize,       TypyField)
BYTESIZE(TypyInt32_ByteSize,      int32)
BYTESIZE(TypyInt64_ByteSize,      int64)
BYTESIZE(TypyUint32_ByteSize,     uint32)
BYTESIZE(TypyUint64_ByteSize,     uint64)
BYTESIZE(TypyFixedPoint_ByteSize, TypyField)
#undef BYTESIZE

#define BYTESIZE(NAME, TYPE) \
static size_t NAME(TypyType t, TYPE* value, int tagsize) { \
	return tagsize + sizeof(TYPE);                         \
}
BYTESIZE(TypyDouble_ByteSize, double)
BYTESIZE(TypyFloat_ByteSize,  float)
BYTESIZE(TypyBool_ByteSize,   bool)
#undef BYTESIZE

static size_t TypyBytes_ByteSize(TypyType t, PyBytes* value, int tagsize) {
	register size_t size = 0;
	if (*value) {
		size = PyBytes_GET_SIZE(*value);
	}
	return tagsize + IblSizeVarint((uint64)size) + size;
}

static size_t TypyString_ByteSize(TypyType t, PyString* value, int tagsize) {
	register size_t size = 0;
	if (*value) {
		register PyObject* bytes = _DecodeString(*value);
		if (bytes) {
			size = PyBytes_GET_SIZE(bytes);
			Py_DECREF(bytes);
		}
	}
	return tagsize + IblSizeVarint(size) + size;
}

ByteSize abstract_ByteSize[MAX_FIELD_TYPE] = {
	(ByteSize)TypyEnum_ByteSize,       /* TYPE_ENUM       */
	(ByteSize)TypyInt32_ByteSize,      /* TYPE_INT32      */
	(ByteSize)TypyInt64_ByteSize,      /* TYPE_INT64      */
	(ByteSize)TypyUint32_ByteSize,     /* TYPE_UINT32     */
	(ByteSize)TypyUint64_ByteSize,     /* TYPE_UINT64     */
	(ByteSize)TypyFixedPoint_ByteSize, /* TYPE_FIXEDPOINT */
	(ByteSize)TypyDouble_ByteSize,     /* TYPE_DOUBLE     */
	(ByteSize)TypyFloat_ByteSize,      /* TYPE_FLOAT      */
	(ByteSize)TypyBool_ByteSize,       /* TYPE_BOOL       */
	(ByteSize)TypyBytes_ByteSize,      /* TYPE_BYTES      */
	(ByteSize)TypyString_ByteSize,     /* TYPE_STRING     */
	(ByteSize)TypyObject_ByteSize,     /* TYPE_OBJECT     */
	(ByteSize)TypyVariant_ByteSize,    /* TYPE_VARIANT    */
	(ByteSize)TypyList_ByteSize,       /* TYPE_LIST       */
	(ByteSize)TypyDict_ByteSize,       /* TYPE_DICT       */
	(ByteSize)TypyPython_ByteSize,     /* TYPE_PYTHON     */
};

//=============================================================================

static size_t TypyNumeric_Hash(TypyField* value) { return (size_t)(*value); }
static size_t TypyObject_Hash(PyObject** value) { return (size_t)PyObject_Hash(*value); }

IblMap_Hash abstract_Hash[MAX_FIELD_TYPE] = {
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_ENUM       */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_INT32      */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_INT64      */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_UINT32     */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_UINT64     */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_FIXEDPOINT */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_DOUBLE     */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_FLOAT      */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_BOOL       */
	(IblMap_Hash)TypyObject_Hash,      /* TYPE_BYTES      */
	(IblMap_Hash)TypyObject_Hash,      /* TYPE_STRING     */
	(IblMap_Hash)TypyObject_Hash,      /* TYPE_OBJECT     */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_VARIANT    */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_LIST       */
	(IblMap_Hash)TypyNumeric_Hash,     /* TYPE_DICT       */
	(IblMap_Hash)TypyObject_Hash,      /* TYPE_PYTHON     */
};

//=============================================================================

static int TypyNumeric_Compare(TypyField* lvalue, TypyField* rvalue) {
	return (int)((*lvalue) - (*rvalue));
}

static int TypyObject_Compare(PyObject** lvalue, PyObject** rvalue) {
	return (int)PyObject_Compare(*lvalue, *rvalue);
}

static int TypyBytes_Compare(PyBytes* lvalue, PyBytes* rvalue) {
	if (Py_SIZE(*lvalue) != Py_SIZE(*rvalue)) {
		return Py_SIZE(*lvalue) - Py_SIZE(*rvalue);
	} else if ((*lvalue)->ob_sval[0] != (*rvalue)->ob_sval[0]) {
		return (*lvalue)->ob_sval[0] - (*rvalue)->ob_sval[0];
	} else {
		return memcmp((*lvalue)->ob_sval, (*rvalue)->ob_sval, Py_SIZE(*lvalue));
	}
}

static int TypyString_Compare(PyString* lvalue, PyString* rvalue) {
	if ((*lvalue)->length != (*rvalue)->length) {
		return (*lvalue)->length - (*rvalue)->length;
	} else {
		return memcmp((*lvalue)->str, (*rvalue)->str, sizeof(Py_UNICODE) * (*lvalue)->length);
	}
}

IblMap_Compare abstract_Compare[MAX_FIELD_TYPE] = {
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_ENUM       */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_INT32      */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_INT64      */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_UINT32     */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_UINT64     */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_FIXEDPOINT */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_DOUBLE     */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_FLOAT      */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_BOOL       */
	(IblMap_Compare)TypyBytes_Compare,   /* TYPE_BYTES      */
	(IblMap_Compare)TypyString_Compare,  /* TYPE_STRING     */
	(IblMap_Compare)TypyObject_Compare,  /* TYPE_OBJECT     */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_VARIANT    */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_LIST       */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_DICT       */
	(IblMap_Compare)TypyObject_Compare,  /* TYPE_PYTHON     */
};

//=============================================================================

