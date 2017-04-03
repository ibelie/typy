// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_OBJECT_H__
#define TYPY_OBJECT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

IblMap_KEY_STRING(TypyFieldMap,
	int index;
);

typedef void*  TypeType;
typedef size_t TypyField;

typedef PyObject* (*GetPyObject) (TypeType, TypyField*);
typedef bool      (*CheckAndSet) (TypeType, TypyField*, PyObject*, const char*);
typedef void      (*CopyFrom)    (TypeType, TypyField*, TypyField);
typedef void      (*MergeFrom)   (TypeType, TypyField*, TypyField);
typedef void      (*Clear)       (TypeType, TypyField*);
typedef bool      (*Read)        (TypeType, TypyField*, byte**, size_t*);
typedef bool      (*ReadPacked)  (TypeType, TypyField*, byte**, size_t*);
typedef size_t    (*Write)       (TypeType, TypyField*, int, byte*);
typedef size_t    (*ByteSize)    (TypeType, TypyField*, int);

typedef struct {
	byte          desc_tagsize;
	uint32        desc_tag;
	TypeType      desc_type;
	WireType      desc_WireType;
	GetPyObject   desc_GetPyObject;
	CheckAndSet   desc_CheckAndSet;
	CopyFrom      desc_CopyFrom;
	MergeFrom     desc_MergeFrom;
	Clear         desc_Clear;
	Read          desc_Read;
	ReadPacked    desc_ReadPacked;
	Write         desc_Write;
	ByteSize      desc_ByteSize;
} TypyDescriptor;

typedef struct {
	PyObject_HEAD
	PyTypeObject*  py_type;
	PyObject*      meta_new;
	IblMap         meta_field2index;
	char**         meta_index2field;
	uint32         meta_cutoff;
	size_t         meta_size;
	TypyDescriptor meta_descriptor[1];
} TypyMetaObject;

void TypyMeta_Dealloc(TypyMetaObject*);

#define Meta_NAME(meta) ((char*)(&((meta)->meta_descriptor[(meta)->meta_size])))

#define TypyObject_HEAD \
    PyObject_HEAD       \
    TypyMetaObject* meta_type;

typedef struct {
	TypyObject_HEAD
	TypyField object_fields[1];
} TypyObject;

extern PyTypeObject TypyMetaObjectType;
extern PyTypeObject BaseTypyObjectType;
extern PyTypeObject* TypyObjectType;

inline PyTypeObject* _InheritTypyObjectType() {
	register PyTypeObject* type = (PyTypeObject*)malloc(sizeof(PyTypeObject));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Inherit TypyObjectType: out of memory %d.", sizeof(PyTypeObject));
		return NULL;
	}
	memcpy(type, &BaseTypyObjectType, sizeof(PyTypeObject));
	type->tp_base = TypyObjectType;
	if (PyType_Ready(type) < 0) {
		free(type);
		return NULL;
	}
	return type;
}

PyObject* Typy_RegisterObject(PyObject*, PyObject*);

#define Typy_TYPE(ob) (((TypyObject*)(ob))->meta_type)
#define Typy_SIZE(ob) (Typy_TYPE(ob)->meta_size)
#define Typy_NAME(ob) Meta_NAME(Typy_TYPE(ob))
#define Typy_FIELD(ob, i) (((TypyObject*)(ob))->object_fields[i])
#define Typy_DESCRIPTOR(ob, i) (Typy_TYPE(ob)->meta_descriptor[i])
#define Typy_TypeCheck(ob) PyObject_TypeCheck(ob, TypyObjectType)

#define Typy_ARGS(...) __VA_ARGS__
#define Typy_TAG(ob, i) (Typy_DESCRIPTOR(ob, i).desc_tag)
#define Typy_TAGSIZE(ob, i) (Typy_DESCRIPTOR(ob, i).desc_tagsize)
#define Typy_WIRETYPE(ob, i) (Typy_DESCRIPTOR(ob, i).desc_WireType)
#define Typy_METHOD_NOARG(ob, i, NAME) \
	(Typy_DESCRIPTOR(ob, i).desc_##NAME(Typy_DESCRIPTOR(ob, i).desc_type, &Typy_FIELD(ob, i)))
#define Typy_METHOD(ob, i, NAME, ARGS) \
	(Typy_DESCRIPTOR(ob, i).desc_##NAME(Typy_DESCRIPTOR(ob, i).desc_type, &Typy_FIELD(ob, i), ARGS))

inline void Typy_Clear(TypyObject* self) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_METHOD_NOARG(self, i, Clear);
	}
}

inline void Typy_Dealloc(TypyObject* self) {
	Typy_Clear(self);
	free(self);
}

inline void Typy_MergeFrom(TypyObject* self, TypyObject* from) {
	if (from == self) { return; }
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_METHOD(self, i, MergeFrom, Typy_FIELD(from, i));
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
		if (!Typy_TAG(self, i)) { continue; }
		size += Typy_METHOD(self, i, ByteSize, Typy_TAGSIZE(self, i));
	}
	return size;
}

inline void Typy_SerializeString(TypyObject* self, byte* output) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (!Typy_TAG(self, i)) { continue; }
		output += Typy_METHOD(self, i, Write, Typy_ARGS(Typy_TAG(self, i), output));
	}
}

inline size_t Typy_MergeFromString(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	for (;;) {
		if (!Typy_ReadTag(&input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
			goto handle_unusual;
		}
		register int index = TAG_INDEX(tag);
		if (index < 0 || (size_t)index >= Typy_SIZE(self)) { goto handle_unusual; }
		if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
			if (!Typy_METHOD(self, index, Read, Typy_ARGS(&input, &remain))) {
				return 0;
			}
		} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
			if (!Typy_METHOD(self, index, ReadPacked, Typy_ARGS(&input, &remain))) {
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
		return Typy_TYPE(self)->meta_index2field[index];
	}
}

inline int Typy_PropertyIndex(TypyObject* self, char* key) {
	register TypyFieldMap field = (TypyFieldMap)IblMap_Get(Typy_TYPE(self)->meta_field2index, &key);
	if (field) {
		return field->index;
	} else {
		return -1;
	}
}

inline size_t Typy_PropertyByteSize(TypyObject* self, int index) {
	if (!Typy_TAG(self, index)) { return 0; }
	register size_t size = Typy_METHOD(self, index, ByteSize, Typy_TAGSIZE(self, index));
	if (size == 0) {
		return Typy_TAGSIZE(self, index);
	} else {
		return size;
	}
}

inline void Typy_SerializeProperty(TypyObject* self, byte* output, int index) {
	if (!Typy_TAG(self, index)) { return; }
	if (Typy_METHOD(self, index, Write, Typy_ARGS(Typy_TAG(self, index), output)) <= 0) {
		Typy_WriteTag(output, Typy_TAG(self, index));
	}
}

inline int Typy_DeserializeProperty(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	if (!Typy_ReadTag(&input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
		return -1;
	}
	register int index = TAG_INDEX(tag);
	if (index < 0 || (size_t)index >= Typy_SIZE(self)) { return -1; }
	Typy_METHOD_NOARG(self, index, Clear);
	if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
		if (!Typy_METHOD(self, index, Read, Typy_ARGS(&input, &remain))) {
			return -1;
		}
	} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
		if (!Typy_METHOD(self, index, ReadPacked, Typy_ARGS(&input, &remain))) {
			return -1;
		}
	}
	return index;
}

PyObject* Typy_New(TypyMetaObject*, PyObject*, PyObject*);
PyObject* Py_CopyFrom(TypyObject*, PyObject*);
PyObject* Py_MergeFrom(TypyObject*, PyObject*);
PyObject* Py_SerializeString(TypyObject*);
PyObject* Py_MergeFromString(TypyObject*, PyObject*);
PyObject* Py_DeserializeProperty(TypyObject*, PyObject*);
PyObject* Py_SerializeProperty(TypyObject*, PyObject*);

inline PyObject* Py_Clear(TypyObject* self) { Typy_Clear(self); Py_RETURN_NONE; }
inline PyObject* Py_ParseFromPyString(TypyObject* self, PyObject* arg) { Typy_Clear(self); return Py_MergeFromString(self, arg); }

#ifdef __cplusplus
}
#endif

#endif // TYPY_OBJECT_H__
