// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef TYPY_PROPERTY_HANDLER

void TypyComposite_AddOwner(TypyComposite* child, TypyComposite* parent, FieldType type, PropertyFlag flag) {
	if (child->owners_length) {
		register size_t i;
		for (i = 0; i < child->owners_length && child->owners_list[i].prop_owner != parent; i++);
		if (i < child->owners_length) { return; }
	}
	if (child->owners_length + 1 > child->owners_capacity) {
		register size_t capacity = Ibl_Max(2 * child->owners_capacity + 1, MIN_OWNER_CAPACITY);
		register TypyPropertyOwner buffer = (TypyPropertyOwner)calloc(capacity, sizeof(struct _TypyPropertyOwner));
		if (!buffer) { return; }
		child->owners_capacity = capacity;
		if (child->owners_list) {
			memcpy(buffer, child->owners_list, child->owners_length * sizeof(struct _TypyPropertyOwner));
			free(child->owners_list);
		}
		child->owners_list = buffer;
	}
	register TypyPropertyOwner owner = &child->owners_list[child->owners_length];
	owner->owner_type = type;
	owner->prop_owner = parent;
	owner->prop_flag  = flag;
	child->owners_length++;
}

void TypyComposite_DelOwner(TypyComposite* child, TypyComposite* parent) {
	if (child->owners_length) {
		register size_t i;
		for (i = 0; i < child->owners_length && child->owners_list[i].prop_owner != parent; i++);
		for (; i < child->owners_length - 1; i++) {
			memcpy(&child->owners_list[i], &child->owners_list[i + 1], sizeof(struct _TypyPropertyOwner));
		}
		if (i < child->owners_length) {
			memset(&child->owners_list[i], 0, sizeof(struct _TypyPropertyOwner));
			child->owners_length--;
		}
	}
}

void TypyComposite_Notify(TypyComposite* composite, FieldType type, PropertyFlag f, FieldType ft, TypyType tt, TypyField o, TypyField n) {
	register size_t i;
	switch (type) {
	case FIELD_TYPE_OBJECT:
		TypyProperty_Changed((TypyObject*)composite, f, ft, tt, o, n);
	case FIELD_TYPE_VARIANT:
		for (i = 0; i < composite->owners_length; i++) {
			register TypyPropertyOwner owner = &composite->owners_list[i];
			TypyComposite_Notify(owner->prop_owner, owner->owner_type, owner->prop_flag + f, ft, tt, o, n);
		}
		break;
	case FIELD_TYPE_LIST:
	case FIELD_TYPE_DICT:
		for (i = 0; i < composite->owners_length; i++) {
			register TypyPropertyOwner owner = &composite->owners_list[i];
			TypyComposite_Notify(owner->prop_owner, owner->owner_type, owner->prop_flag, ft, tt, o, n);
		}
		break;
	}
}

#endif

//=============================================================================

static PyObject* TypyInt32_GetPyObject  (TypyType t, int32* v)  { return PyInt_FromLong(*v); }
static PyObject* TypyUint32_GetPyObject (TypyType t, uint32* v) { return PyInt_FromSize_t(*v); }
static PyObject* TypyBool_GetPyObject   (TypyType t, bool* v)   { return PyBool_FromLong(*v); }
static PyObject* TypyFloat_GetPyObject  (TypyType t, float* v)  { return PyFloat_FromDouble(*v); }

static PyObject* TypyUint64_GetPyObject(TypyType t, uint64* value) {
	if (sizeof(TypyField) < sizeof(uint64)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %zu-bit, cannot use uint64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return NULL;
	} else {
		return PyLong_FromUnsignedLongLong(*value);
	}
}

static PyObject* TypyInt64_GetPyObject(TypyType t, int64* value) {
	if (sizeof(TypyField) < sizeof(int64)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %zu-bit, cannot use int64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return NULL;
	} else {
		return PyLong_FromLongLong(*value);
	}
}

static PyObject* TypyDouble_GetPyObject(TypyType t, double* value) {
	if (sizeof(TypyField) < sizeof(double)) {
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %zu-bit, cannot use double. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
		return NULL;
	} else {
		return PyFloat_FromDouble(*value);
	}
}

static PyObject* TypyBytes_GetPyObject(TypyType t, PyBytes* value) {
	if (*value) {
		Py_INCREF(*value);
		return (PyObject*)(*value);
	}
	return PyString_FromStringAndSize(NULL, 0);
}

static PyObject* TypyString_GetPyObject(TypyType t, PyString* value) {
	if (*value) {
		Py_INCREF(*value);
		return (PyObject*)(*value);
	}
	return PyUnicode_FromStringAndSize(NULL, 0);
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
	(GetPyObject)TypyBytes_GetPyObject,      /* TYPE_BYTES      */
	(GetPyObject)TypyString_GetPyObject,     /* TYPE_STRING     */
	(GetPyObject)TypyBytes_GetPyObject,      /* TYPE_SYMBOL     */
	(GetPyObject)TypyObject_GetPyObject,     /* TYPE_OBJECT     */
	(GetPyObject)TypyVariant_GetPyObject,    /* TYPE_VARIANT    */
	(GetPyObject)TypyList_GetPyObject,       /* TYPE_LIST       */
	(GetPyObject)TypyDict_GetPyObject,       /* TYPE_DICT       */
	(GetPyObject)TypyPython_GetPyObject,     /* TYPE_PYTHON     */
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
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %zu-bit, cannot use int64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
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
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %zu-bit, cannot use uint64. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
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
		PyErr_Format(PyExc_RuntimeError, "TypyField size is %zu-bit, cannot use double. Please rebuild typyd with TYPY_FIELD_SIZE_64.", 8 * sizeof(TypyField));
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

static bool TypyBytes_CheckAndSet(TypyType t, PyBytes* value, PyObject* arg, const char* err) {
	if (!arg) {
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

static bool TypyString_CheckAndSet(TypyType t, PyString* value, PyObject* arg, const char* err) {
	if (!arg) {
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
	(CheckAndSet)TypyBytes_CheckAndSet,      /* TYPE_SYMBOL     */
	(CheckAndSet)TypyObject_CheckAndSet,     /* TYPE_OBJECT     */
	(CheckAndSet)TypyVariant_CheckAndSet,    /* TYPE_VARIANT    */
	(CheckAndSet)TypyList_CheckAndSet,       /* TYPE_LIST       */
	(CheckAndSet)TypyDict_CheckAndSet,       /* TYPE_DICT       */
	(CheckAndSet)TypyPython_CheckAndSet,     /* TYPE_PYTHON     */
};

//=============================================================================

static void TypyNumeric_MergeFrom(TypyType type, TypyField* lvalue, TypyField rvalue) {
	if (rvalue != 0) { *lvalue = rvalue; }
}

static void TypyPyObject_MergeFrom(TypyType type, PyObject** lvalue, PyObject* rvalue) {
	if (!rvalue || *lvalue == rvalue) { return; }
	Py_XDECREF(*lvalue);
	Py_INCREF(rvalue);
	*lvalue = rvalue;
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
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_SYMBOL     */
	(MergeFrom)TypyObject_MergeFrom,   /* TYPE_OBJECT     */
	(MergeFrom)TypyVariant_MergeFrom,  /* TYPE_VARIANT    */
	(MergeFrom)TypyList_MergeFrom,     /* TYPE_LIST       */
	(MergeFrom)TypyDict_MergeFrom,     /* TYPE_DICT       */
	(MergeFrom)TypyPyObject_MergeFrom, /* TYPE_PYTHON     */
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

static bool TypySymbol_Read(TypyType t, PyBytes* value, byte** input, size_t* length) {
	uint32 size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return false;
	} else if (size > *length) {
		return false;
	}

	register PyBytes symbol = (PyBytes)PyBytes_FromStringAndSize(NULL, Typy_SymbolDecodedLen(size));
	if (!symbol) { return false; }

	Py_XDECREF(*value);
	PyString_GET_SIZE(symbol) = Typy_DecodeSymbol(PyBytes_AS_STRING(symbol), *input, size);
	*value = symbol;
	*input += size;
	*length -= size;

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
	(Read)TypySymbol_Read,     /* TYPE_SYMBOL     */
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

static size_t TypySymbol_Write(TypyType t, PyBytes* value, int tag, byte* output) {
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	if (*value) {
		Py_ssize_t length = PyBytes_GET_SIZE(*value);
		if (length > 0) {
			register size_t l = Typy_SymbolEncodedLen(length);
			size += IblPutUvarint(output + size, (l));
			Typy_EncodeSymbol(output + size, PyBytes_AS_STRING(*value), length);
			return size + l;
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
	(Write)TypySymbol_Write,     /* TYPE_SYMBOL     */
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

static size_t TypySymbol_ByteSize(TypyType t, PyBytes* value, int tagsize) {
	register size_t size = 0;
	if (*value) {
		size = Typy_SymbolEncodedLen(PyBytes_GET_SIZE(*value));
	}
	return tagsize + IblSizeVarint((uint64)size) + size;
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
	(ByteSize)TypySymbol_ByteSize,     /* TYPE_SYMBOL     */
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
	(IblMap_Hash)TypyObject_Hash,      /* TYPE_SYMBOL     */
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
	(IblMap_Compare)TypyBytes_Compare,   /* TYPE_SYMBOL     */
	(IblMap_Compare)TypyObject_Compare,  /* TYPE_OBJECT     */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_VARIANT    */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_LIST       */
	(IblMap_Compare)TypyNumeric_Compare, /* TYPE_DICT       */
	(IblMap_Compare)TypyObject_Compare,  /* TYPE_PYTHON     */
};

//=============================================================================

#define TO_JSON_SIMPLE(NAME, TYPE) \
static PyObject* Typy##NAME##_ToJson(TypyType t, TYPE* v, bool s) { \
	return (!s || *v) ? Typy##NAME##_GetPyObject(t, v) : NULL;      \
}

TO_JSON_SIMPLE(Enum,       TypyField);
TO_JSON_SIMPLE(Int32,      int32);
TO_JSON_SIMPLE(Int64,      int64);
TO_JSON_SIMPLE(Uint32,     uint32);
TO_JSON_SIMPLE(Uint64,     uint64);
TO_JSON_SIMPLE(FixedPoint, TypyField);
TO_JSON_SIMPLE(Double,     double);
TO_JSON_SIMPLE(Float,      float);
TO_JSON_SIMPLE(Bool,       bool);
TO_JSON_SIMPLE(Bytes,      PyBytes);
TO_JSON_SIMPLE(String,     PyString);

#undef TO_JSON_SIMPLE

ToJson abstract_ToJson[MAX_FIELD_TYPE] = {
	(ToJson)TypyEnum_ToJson,       /* TYPE_ENUM       */
	(ToJson)TypyInt32_ToJson,      /* TYPE_INT32      */
	(ToJson)TypyInt64_ToJson,      /* TYPE_INT64      */
	(ToJson)TypyUint32_ToJson,     /* TYPE_UINT32     */
	(ToJson)TypyUint64_ToJson,     /* TYPE_UINT64     */
	(ToJson)TypyFixedPoint_ToJson, /* TYPE_FIXEDPOINT */
	(ToJson)TypyDouble_ToJson,     /* TYPE_DOUBLE     */
	(ToJson)TypyFloat_ToJson,      /* TYPE_FLOAT      */
	(ToJson)TypyBool_ToJson,       /* TYPE_BOOL       */
	(ToJson)TypyBytes_ToJson,      /* TYPE_BYTES      */
	(ToJson)TypyString_ToJson,     /* TYPE_STRING     */
	(ToJson)TypyBytes_ToJson,      /* TYPE_SYMBOL     */
	(ToJson)TypyObject_ToJson,     /* TYPE_OBJECT     */
	(ToJson)TypyVariant_ToJson,    /* TYPE_VARIANT    */
	(ToJson)TypyList_ToJson,       /* TYPE_LIST       */
	(ToJson)TypyDict_ToJson,       /* TYPE_DICT       */
	(ToJson)TypyPython_ToJson,     /* TYPE_PYTHON     */
};

//=============================================================================

#define FROM_JSON_SIMPLE(NAME, TYPE) \
static bool Typy##NAME##_FromJson(TypyType t, TYPE* v, PyObject* arg) {     \
	return Typy##NAME##_CheckAndSet(t, v, arg, #NAME " json type error, "); \
}

FROM_JSON_SIMPLE(Enum,       TypyField);
FROM_JSON_SIMPLE(Int32,      int32);
FROM_JSON_SIMPLE(Int64,      int64);
FROM_JSON_SIMPLE(Uint32,     uint32);
FROM_JSON_SIMPLE(Uint64,     uint64);
FROM_JSON_SIMPLE(FixedPoint, TypyField);
FROM_JSON_SIMPLE(Double,     double);
FROM_JSON_SIMPLE(Float,      float);
FROM_JSON_SIMPLE(Bool,       bool);
FROM_JSON_SIMPLE(Bytes,      PyBytes);
FROM_JSON_SIMPLE(String,     PyString);

#undef FROM_JSON_SIMPLE

FromJson abstract_FromJson[MAX_FIELD_TYPE] = {
	(FromJson)TypyEnum_FromJson,       /* TYPE_ENUM       */
	(FromJson)TypyInt32_FromJson,      /* TYPE_INT32      */
	(FromJson)TypyInt64_FromJson,      /* TYPE_INT64      */
	(FromJson)TypyUint32_FromJson,     /* TYPE_UINT32     */
	(FromJson)TypyUint64_FromJson,     /* TYPE_UINT64     */
	(FromJson)TypyFixedPoint_FromJson, /* TYPE_FIXEDPOINT */
	(FromJson)TypyDouble_FromJson,     /* TYPE_DOUBLE     */
	(FromJson)TypyFloat_FromJson,      /* TYPE_FLOAT      */
	(FromJson)TypyBool_FromJson,       /* TYPE_BOOL       */
	(FromJson)TypyBytes_FromJson,      /* TYPE_BYTES      */
	(FromJson)TypyString_FromJson,     /* TYPE_STRING     */
	(FromJson)TypyBytes_FromJson,      /* TYPE_SYMBOL     */
	(FromJson)TypyObject_FromJson,     /* TYPE_OBJECT     */
	(FromJson)TypyVariant_FromJson,    /* TYPE_VARIANT    */
	(FromJson)TypyList_FromJson,       /* TYPE_LIST       */
	(FromJson)TypyDict_FromJson,       /* TYPE_DICT       */
	(FromJson)TypyPython_FromJson,     /* TYPE_PYTHON     */
};

//=============================================================================

#define FROM_JSON_KEY_NUMBER(NAME, TYPE, PYTHON) \
static bool Typy##NAME##_FromJsonKey(TypyType t, TYPE* v, PyObject* arg) { \
	register PyObject* value = PyNumber_##PYTHON(arg);                     \
	if (!value) { return false; }                                          \
	register bool success = Typy##NAME##_CheckAndSet(t, v, value,          \
		"FromJsonKey expect a string of '" #TYPE "', but ");               \
	Py_DECREF(value);                                                      \
	return success;                                                        \
}

FROM_JSON_KEY_NUMBER(Enum,       TypyField, Int);
FROM_JSON_KEY_NUMBER(Int32,      int32,     Int);
FROM_JSON_KEY_NUMBER(Int64,      int64,     Long);
FROM_JSON_KEY_NUMBER(Uint32,     uint32,    Int);
FROM_JSON_KEY_NUMBER(Uint64,     uint64,    Long);
FROM_JSON_KEY_NUMBER(FixedPoint, TypyField, Float);
FROM_JSON_KEY_NUMBER(Float,      float,     Float);
FROM_JSON_KEY_NUMBER(Double,     double,    Float);
FROM_JSON_KEY_NUMBER(Bool,       bool,      Int);

#undef FROM_JSON_KEY_NUMBER

#define FROM_JSON_KEY_ERROR(NAME) \
static bool Typy##NAME##_FromJsonKey(TypyType t, TypyField* v, PyObject* arg) { \
	PyErr_Format(PyExc_TypeError, #NAME " cannot used as dict key.");           \
	return false;                                                               \
}

FROM_JSON_KEY_ERROR(Object);
FROM_JSON_KEY_ERROR(Variant);
FROM_JSON_KEY_ERROR(List);
FROM_JSON_KEY_ERROR(Dict);
FROM_JSON_KEY_ERROR(Python);

#undef FROM_JSON_KEY_ERROR

FromJsonKey abstract_FromJsonKey[MAX_FIELD_TYPE] = {
	(FromJsonKey)TypyEnum_FromJsonKey,       /* TYPE_ENUM       */
	(FromJsonKey)TypyInt32_FromJsonKey,      /* TYPE_INT32      */
	(FromJsonKey)TypyInt64_FromJsonKey,      /* TYPE_INT64      */
	(FromJsonKey)TypyUint32_FromJsonKey,     /* TYPE_UINT32     */
	(FromJsonKey)TypyUint64_FromJsonKey,     /* TYPE_UINT64     */
	(FromJsonKey)TypyFixedPoint_FromJsonKey, /* TYPE_FIXEDPOINT */
	(FromJsonKey)TypyDouble_FromJsonKey,     /* TYPE_DOUBLE     */
	(FromJsonKey)TypyFloat_FromJsonKey,      /* TYPE_FLOAT      */
	(FromJsonKey)TypyBool_FromJsonKey,       /* TYPE_BOOL       */
	(FromJsonKey)TypyBytes_FromJson,         /* TYPE_BYTES      */
	(FromJsonKey)TypyString_FromJson,        /* TYPE_STRING     */
	(FromJsonKey)TypyBytes_FromJson,         /* TYPE_SYMBOL     */
	(FromJsonKey)TypyObject_FromJsonKey,     /* TYPE_OBJECT     */
	(FromJsonKey)TypyVariant_FromJsonKey,    /* TYPE_VARIANT    */
	(FromJsonKey)TypyList_FromJsonKey,       /* TYPE_LIST       */
	(FromJsonKey)TypyDict_FromJsonKey,       /* TYPE_DICT       */
	(FromJsonKey)TypyPython_FromJsonKey,     /* TYPE_PYTHON     */
};

//=============================================================================
