// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_TYPE_H__
#define TYPY_TYPE_H__

#define HAVE_ROUND

#include "map.h"
#include "protobuf.h"
#include "Python.h"

#define FULL_MODULE_NAME "_typyd"

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
		(PyUnicode_Check(ob) ? PyUnicode_AsUTF8(ob) : PyBytes_AsString(ob))
	#define PyString_AsStringAndSize(ob, charpp, sizep) \
		(PyUnicode_Check(ob) ? \
			(!(*(charpp) = PyUnicode_AsUTF8AndSize(ob, (sizep)))? -1 : 0) : \
			PyBytes_AsStringAndSize(ob, (charpp), (sizep)))
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

void FormatTypeError(PyObject* arg, const char* err);

// ===================================================================

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

IblMap_KEY_STRING(TypyFieldMap,
	int index;
);

typedef size_t TypeField;

typedef PyObject* (*GetPyObject)  (TypeField);
typedef bool      (*CheckAndSet)  (TypeField*, PyObject* arg, const char* err);                            \
typedef void      (*CopyFrom)     (TypeField*, TypeField);
typedef void      (*MergeFrom)    (TypeField*, TypeField);
typedef void      (*Clear)        (TypeField*);
typedef bool      (*Read)         (TypeField*, byte**, size_t*);
typedef bool      (*ReadPacked)   (TypeField*, byte**, size_t*);
typedef size_t    (*Write)        (int, TypeField, byte*);
typedef size_t    (*ByteSize)     (int, TypeField);


typedef struct {
	size_t        ty_tagsize;
	WireType      ty_WireType;
	GetPyObject   ty_GetPyObject;
	CheckAndSet   ty_CheckAndSet;
	CopyFrom      ty_CopyFrom;
	MergeFrom     ty_MergeFrom;
	Clear         ty_Clear;
	Read          ty_Read;
	ReadPacked    ty_ReadPacked;
	Write         ty_Write;
	ByteSize      ty_ByteSize;
} TypyDescriptor;

typedef struct {
	PyTypeObject*  py_type;
	PyObject*      ty_new;
	IblMap         ty_field2index;
	char**         ty_index2field;
	size_t         ty_size;
	TypyDescriptor ty_descriptor[1];
} TypyType;

#define Ty_NAME(ty) ((char*)(&((ty)->ty_descriptor[(ty)->ty_size])))

#define TypyObject_HEAD \
    PyObject_HEAD       \
    TypyType* ty_type;

typedef struct {
	TypyObject_HEAD
	size_t ty_cached_size;
	TypeField ty_fields[1];
} TypyObject;

#define Typy_TYPE(ob) (((TypyObject*)(ob))->ty_type)
#define Typy_SIZE(ob) (Typy_TYPE(ob)->ty_size)
#define Typy_NAME(ob) Ty_NAME(Typy_TYPE(ob))
#define Typy_FIELD(ob, i) (((TypyObject*)(ob))->ty_fields[i])
#define Typy_DESCRIPTOR(ob, i) (Typy_TYPE(ob)->ty_descriptor[i])
#define Typy_WIRETYPE(ob, i) (Typy_DESCRIPTOR(ob, i).ty_WireType)
#define Typy_TypeCheck(ob) PyObject_TypeCheck(ob, TypyTypeObject)

inline void Typy_Clear(TypyObject* self) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_DESCRIPTOR(self, i).ty_Clear(&Typy_FIELD(self, i));
	}
}

inline void Typy_MergeFrom(TypyObject* self, TypyObject* from) {
	if (from == self) { return; }
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_DESCRIPTOR(self, i).ty_MergeFrom(&Typy_FIELD(self, i), Typy_FIELD(from, i));
	}
}

inline void Typy_CopyFrom(TypyObject* self, TypyObject* from) {
	if (from == self) { return; }
	Typy_Clear(self);
	Typy_MergeFrom(self, from);
}

inline size_t Typy_ByteSize(TypyObject* self) {
	register size_t size = 0, i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		size += Typy_DESCRIPTOR(self, i).ty_ByteSize(Typy_DESCRIPTOR(self, i).ty_tagsize, Typy_FIELD(self, i));
	}
	self->ty_cached_size = size;
	return size;
}

inline void Typy_SerializeString(TypyObject* self, byte* output) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		output += Typy_DESCRIPTOR(self, i).ty_Write(i + 1, Typy_FIELD(self, i), output);
	}
}

inline size_t Typy_MergeFromString(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	for (;;) {
		if (!Typy_ReadTag(&input, &remain, &tag, TAG_CUTOFF(Typy_SIZE(self) + 1))) {
			goto handle_unusual;
		}
		register int index = TAG_INDEX(tag);
		if (index < 0 || (size_t)index >= Typy_SIZE(self)) { goto handle_unusual; }
		if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
			if (!Typy_DESCRIPTOR(self, index).ty_Read(&Typy_FIELD(self, index), &input, &remain)) {
				return 0;
			}
		} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
			if (!Typy_DESCRIPTOR(self, index).ty_ReadPacked(&Typy_FIELD(self, index), &input, &remain)) {
				return 0;
			}
		}
	handle_unusual:
		if (tag == 0) { return length - remain; }
		if (!Typy_SkipField(&input, &remain, tag)) { return false; }
	}
}

inline char* Typy_PropertyName(TypyObject* self, int index) {
	if (index < 0 || (size_t)index > Typy_SIZE(self)) {
		return NULL;
	} else {
		return Typy_TYPE(self)->ty_index2field[index];
	}
}

inline int Typy_PropertyIndex(TypyObject* self, char* key) {
	register TypyFieldMap field = (TypyFieldMap)IblMap_Get(Typy_TYPE(self)->ty_field2index, key);
	if (field) {
		return field->index;
	} else {
		return -1;
	}
}

inline size_t Typy_PropertyByteSize(TypyObject* self, int index) {
	register size_t size = Typy_DESCRIPTOR(self, index).ty_ByteSize(Typy_DESCRIPTOR(self, index).ty_tagsize, Typy_FIELD(self, index));
	if (size == 0) {
		return Typy_DESCRIPTOR(self, index).ty_tagsize;
	} else {
		return size;
	}
}

inline void Typy_SerializeProperty(TypyObject* self, byte* output, int index) {
	if (Typy_DESCRIPTOR(self, index).ty_Write(index + 1, Typy_FIELD(self, index), output) <= 0) {
		Typy_WriteTag(output, index + 1);
	}
}

inline int Typy_DeserializeProperty(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	if (!Typy_ReadTag(&input, &remain, &tag, TAG_CUTOFF(Typy_SIZE(self) + 1))) {
		return -1;
	}
	register int index = TAG_INDEX(tag);
	if (index < 0 || (size_t)index >= Typy_SIZE(self)) { return -1; }
	Typy_DESCRIPTOR(self, index).ty_Clear(&Typy_FIELD(self, index));
	if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
		if (!Typy_DESCRIPTOR(self, index).ty_Read(&Typy_FIELD(self, index), &input, &remain)) {
			return -1;
		}
	} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
		if (!Typy_DESCRIPTOR(self, index).ty_ReadPacked(&Typy_FIELD(self, index), &input, &remain)) {
			return -1;
		}
	}
	return index;
}

PyObject* Typy_New(TypyType*, PyObject*, PyObject*);
PyObject* Py_CopyFrom(TypyObject*, PyObject*);
PyObject* Py_MergeFrom(TypyObject*, PyObject*);
PyObject* Py_SerializeString(TypyObject*);
PyObject* Py_MergeFromString(TypyObject*, PyObject*);
PyObject* Py_DeserializeProperty(TypyObject*, PyObject*);
PyObject* Py_SerializeProperty(TypyObject*, PyObject*);

inline void Typy_Dealloc(TypyObject* self) { Typy_Clear(self); free(self); }
inline PyObject* Py_Clear(TypyObject* self) { Typy_Clear(self); Py_RETURN_NONE; }
inline PyObject* Py_ParseFromPyString(TypyObject* self, PyObject* arg) { Typy_Clear(self); return Py_MergeFromString(self, arg); }

typedef struct {
	TypyObject_HEAD
	TypeField ty_value;
	int ty_index;
	size_t ty_cached_size;
} TypyVariant;

extern PyTypeObject BaseTypyTypeObject;
extern PyTypeObject* TypyTypeObject;

#ifdef __cplusplus
}
#endif

#endif // TYPY_TYPE_H__
