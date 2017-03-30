// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_TYPE_H__
#define TYPY_TYPE_H__

#define HAVE_ROUND

#include "map.h"
#include "Python.h"

#define FULL_MODULE_NAME "_typyd"
#define FULL_NAME_LEN 7

#ifndef PyVarObject_HEAD_INIT
#define PyVarObject_HEAD_INIT(type, size) PyObject_HEAD_INIT(type) size,
#endif
#ifndef Py_TYPE
#define Py_TYPE(ob) (((PyObject*)(ob))->ob_type)
#endif

#if PY_MAJOR_VERSION >= 3
	#define PyInt_Check PyLong_Check
	#define PyInt_AsLong PyLong_AsLong
	#define PyInt_FromLong PyLong_FromLong
	#define PyInt_FromSize_t PyLong_FromSize_t
	#define PyString_Check PyUnicode_Check
	#define PyString_FromString PyUnicode_FromString
	#define PyString_FromStringAndSize PyUnicode_FromStringAndSize
	#if PY_VERSION_HEX < 0x03030000
		#error "Python 3.0 - 3.2 are not supported."
	#else
	#define PyString_AsString(ob) \
		(PyUnicode_Check(ob)? PyUnicode_AsUTF8(ob): PyBytes_AsString(ob))
	#define PyString_AsStringAndSize(ob, charpp, sizep) \
		(PyUnicode_Check(ob)? \
			(!(*(charpp) = PyUnicode_AsUTF8AndSize(ob, (sizep)))? -1: 0): \
			PyBytes_AsStringAndSize(ob, (charpp), (sizep)))
	#endif
#endif

void FormatTypeError(PyObject* arg, const char* err);

// ===================================================================

#define TAG_TYPE_BITS 3
#define TAG_TYPE_MASK ((1 << TAG_TYPE_BITS) - 1)
#define MAKE_TAG(FIELD_NUMBER, TYPE) (uint32)(((FIELD_NUMBER) << TAG_TYPE_BITS) | (TYPE))
#define MAX_TAG(TAG) MAKE_TAG(TAG, TAG_TYPE_MASK)

enum WireType {
	WIRETYPE_VARINT           = 0,
	WIRETYPE_FIXED64          = 1,
	WIRETYPE_LENGTH_DELIMITED = 2,
	WIRETYPE_START_GROUP      = 3,
	WIRETYPE_END_GROUP        = 4,
	WIRETYPE_FIXED32          = 5,
};

// Lite alternative to FieldDescriptor::Type.  Must be kept in sync.
enum FieldType {
	TYPE_DOUBLE         = 1,
	TYPE_FLOAT          = 2,
	TYPE_INT64          = 3,
	TYPE_UINT64         = 4,
	TYPE_INT32          = 5,
	TYPE_FIXED64        = 6,
	TYPE_FIXED32        = 7,
	TYPE_BOOL           = 8,
	TYPE_STRING         = 9,
	TYPE_GROUP          = 10,
	TYPE_MESSAGE        = 11,
	TYPE_BYTES          = 12,
	TYPE_UINT32         = 13,
	TYPE_ENUM           = 14,
	TYPE_SFIXED32       = 15,
	TYPE_SFIXED64       = 16,
	TYPE_SINT32         = 17,
	TYPE_SINT64         = 18,
	MAX_FIELD_TYPE      = 18,
};

extern bool isDefaultEncodingUTF8;
extern PyObject* kPythonZero;
extern PyObject* kint32min_py;
extern PyObject* kint32max_py;
extern PyObject* kuint32max_py;
extern PyObject* kint64min_py;
extern PyObject* kint64max_py;
extern PyObject* kuint64max_py;

typedef PyBytesObject* PyBytes;
typedef PyUnicodeObject* PyString;

typedef size_t TypeField

typedef PyObject* (*GetPyObject)(TypeField);
typedef bool (*CheckAndSet)(PyObject* arg, TypeField*, const char* err);                            \
typedef bool (*Read)(TypeField*, byte*, size_t*);
typedef void (*Write)(int, TypeField, byte*, size_t*);
typedef void (*WriteTag)(int, TypeField, byte*, size_t*);
typedef void (*CopyFrom)(TypeField*, TypeField);
typedef void (*MergeFrom)(TypeField*, TypeField);
typedef void (*Clear)(TypeField*);
typedef int (*ByteSize)(int, TypeField);
typedef int (*GetCachedSize)(int, TypeField);


typedef struct {
	PyObject_VAR_HEAD
	size_t fields[1];
} TypyObject;

typedef struct {
	PyObject_VAR_HEAD
	size_t value;
} TypyVariant;

typedef struct {
	PyTypeObject py_type;
	char py_name[1];
} TypyTypeObject;

extern TypyTypeObject templateTypeObject;

#endif // TYPY_TYPE_H__
