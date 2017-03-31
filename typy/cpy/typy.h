// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_TYPE_H__
#define TYPY_TYPE_H__

#define HAVE_ROUND

#include "map.h"
#include "Python.h"

#define FULL_MODULE_NAME _typyd

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

#ifdef __cplusplus
extern "C" {
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

typedef PyObject* (*GetPyObject)  (TypeField);
typedef bool      (*CheckAndSet)  (TypeField*, PyObject* arg, const char* err);                            \
typedef void      (*CopyFrom)     (TypeField*, TypeField);
typedef void      (*MergeFrom)    (TypeField*, TypeField);
typedef void      (*Clear)        (TypeField*);
typedef bool      (*Read)         (TypeField*, byte**);
typedef size_t    (*Write)        (int, TypeField, byte*);
typedef size_t    (*WriteTag)     (int, TypeField, byte*);
typedef size_t    (*ByteSize)     (size_t, TypeField);
typedef size_t    (*GetCachedSize)(size_t, TypeField);


typedef struct {
	GetPyObject   ty_GetPyObject;
	CheckAndSet   ty_CheckAndSet;
	CopyFrom      ty_CopyFrom;
	MergeFrom     ty_MergeFrom;
	Clear         ty_Clear;
	Read          ty_Read;
	Write         ty_Write;
	WriteTag      ty_WriteTag;
	ByteSize      ty_ByteSize;
	GetCachedSize ty_GetCachedSize;
} TypyDescriptor;

typedef struct {
	PyTypeObject* py_type;
	size_t ty_size;
	TypyDescriptor ty_descriptor[1];
} TypyType;

#define Ty_NAME(ty) (char*)(&ty->ty_descriptor[ty->ty_size])

#define TypyObject_HEAD \
    PyObject_HEAD       \
    TypyType* ty_type;

#define Typy_TYPE(ob) (((TypyObject*)(ob))->ty_type)
#define Typy_SIZE(ob) (Typy_TYPE(ob)->ty_size)
#define Typy_NAME(ob) Ty_NAME(Typy_TYPE(ob))
#define Typy_FIELD(ob, i) (((TypyObject*)(ob))->ty_fields[i])

PyObject* Typy_New(TypyType* type, PyObject* args, PyObject* kwargs);

inline void Typy_Clear(TypyObject* self) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_TYPE(self)->ty_descriptor[i].ty_Clear(&Typy_FIELD(self, i));
	}
}

inline void Typy_Dealloc(TypyObject* self) { Typy_Clear(self); free(self); }

inline void Typy_CopyFrom(TypyObject* self, TypyObject* other) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_TYPE(self)->ty_descriptor[i].ty_CopyFrom(&Typy_FIELD(self, i), Typy_FIELD(other, i));
	}
}

inline void Typy_MergeFrom(TypyObject* self, TypyObject* other) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_TYPE(self)->ty_descriptor[i].ty_MergeFrom(&Typy_FIELD(self, i), Typy_FIELD(other, i));
	}
}

void Typy_Serialize(TypyObject* self, byte* output) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		output += Typy_TYPE(self)->ty_descriptor[i].ty_Write(i + 1, Typy_FIELD(self, i), output);
	}
}

bool Typy_MergeFromString(TypyObject* self, byte*, size_t);

inline size_t Typy_ByteSize(TypyObject* self) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_TYPE(self)->ty_descriptor[i].ty_MergeFrom(&Typy_FIELD(self, i), Typy_FIELD(other, i));
	}
}

char* Typy_PropertyName(int);
void  Typy_SerializeProperty(byte*, int);
int   Typy_DeserializeProperty(byte*, size_t);
int   Typy_PropertyIndex(char*);
int   Typy_PropertyByteSize(int);

typedef struct {
	TypyObject_HEAD
	size_t ty_cached_size;
	TypeField ty_fields[1];
} TypyObject;

typedef struct {
	TypyObject_HEAD
	TypeField ty_value;
	int ty_tag;
	size_t ty_cached_size;
} TypyVariant;

extern PyTypeObject TypyTypeObject;

#ifdef __cplusplus
}
#endif

#endif // TYPY_TYPE_H__
