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
	size_t    object_size;
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
#define Typy_DESC(ob, i) (Typy_TYPE(ob)->meta_descriptor[i])
#define Typy_TypeCheck(ob) PyObject_TypeCheck(ob, TypyObjectType)

#define Typy_TAG(ob, i) (Typy_DESC(ob, i).desc_tag)
#define Typy_TAGSIZE(ob, i) (Typy_DESC(ob, i).desc_tagsize)
#define Typy_WIRETYPE(ob, i) (Typy_DESC(ob, i).desc_WireType)
#define Typy_CLEAR(ob, i) \
	(abstract_Clear[Typy_DESC(ob, i).desc_FieldType](Typy_DESC(ob, i).desc_type, &Typy_FIELD(ob, i)))
#define Typy_MERGEFROM(ob, i, f) \
	(abstract_MergeFrom[Typy_DESC(ob, i).desc_FieldType](Typy_DESC(ob, i).desc_type, &Typy_FIELD(ob, i), (f)))
#define Typy_BYTESIZE(ob, i, t) \
	(abstract_ByteSize[Typy_DESC(ob, i).desc_FieldType](Typy_DESC(ob, i).desc_type, &Typy_FIELD(ob, i), (t)))
#define Typy_WRITE(ob, i, t, o) \
	(abstract_Write[Typy_DESC(ob, i).desc_FieldType](Typy_DESC(ob, i).desc_type, &Typy_FIELD(ob, i), (t), (o)))
#define Typy_READ(ob, i, s, l) \
	(abstract_Read[Typy_DESC(ob, i).desc_FieldType](Typy_DESC(ob, i).desc_type, &Typy_FIELD(ob, i), (s), (l)))

inline void Typy_Clear(TypyObject* self) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_CLEAR(self, i);
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
		Typy_MERGEFROM(self, i, Typy_FIELD(from, i));
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
		if (!Typy_TAG(self, i) || !Typy_FIELD(self, i)) { continue; }
		size += Typy_BYTESIZE(self, i, Typy_TAGSIZE(self, i));
	}
	return size;
}

inline void Typy_SerializeString(TypyObject* self, byte* output) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (!Typy_TAG(self, i) || !Typy_FIELD(self, i)) { continue; }
		output += Typy_WRITE(self, i, Typy_TAG(self, i), output);
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
			if (!Typy_READ(self, index, &input, &remain)) {
				return 0;
			}
		} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
			if (!Typy_ReadPacked(Typy_DESC(self, index).desc_type, &Typy_FIELD(self, index), &input, &remain)) {
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
	register size_t size = Typy_BYTESIZE(self, index, Typy_TAGSIZE(self, index));
	if (size == 0) {
		return Typy_TAGSIZE(self, index);
	} else {
		return size;
	}
}

inline void Typy_SerializeProperty(TypyObject* self, byte* output, int index) {
	if (!Typy_TAG(self, index)) { return; }
	if (Typy_WRITE(self, index, Typy_TAG(self, index), output) <= 0) {
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
	Typy_CLEAR(self, index);
	if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
		if (!Typy_READ(self, index, &input, &remain)) {
			return -1;
		}
	} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
		if (!Typy_ReadPacked(Typy_DESC(self, index).desc_type, &Typy_FIELD(self, index), &input, &remain)) {
			return -1;
		}
	}
	return index;
}

PyObject* Typy_New               (TypyMetaObject*, PyObject*, PyObject*);
size_t    TypyObject_ByteSize    (TypyMetaObject*, TypyObject**, int);
size_t    TypyObject_Write       (TypyMetaObject*, TypyObject**, int, byte*);
bool      TypyObject_Read        (TypyMetaObject*, TypyObject**, byte**, size_t*);
bool      TypyObject_CheckAndSet (TypyMetaObject*, TypyObject**, PyObject*, const char*);
void      TypyObject_MergeFrom   (TypyMetaObject*, TypyObject**, TypyObject*);

PyObject* Py_CopyFrom            (TypyObject*, PyObject*);
PyObject* Py_MergeFrom           (TypyObject*, PyObject*);
PyObject* Py_SerializeString     (TypyObject*);
PyObject* Py_MergeFromString     (TypyObject*, PyObject*);
PyObject* Py_DeserializeProperty (TypyObject*, PyObject*);
PyObject* Py_SerializeProperty   (TypyObject*, PyObject*);

inline PyObject* Py_Clear(TypyObject* self) { Typy_Clear(self); Py_RETURN_NONE; }
inline PyObject* Py_ParseFromPyString(TypyObject* self, PyObject* arg) { Typy_Clear(self); return Py_MergeFromString(self, arg); }

#ifdef __cplusplus
}
#endif

#endif // TYPY_OBJECT_H__
