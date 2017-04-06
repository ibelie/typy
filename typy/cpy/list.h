// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_LIST_H__
#define TYPY_LIST_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_LIST_CAPACITY 10

typedef struct {
	PyObject_HEAD
	TypyNestDesc list_desc;
	char         list_name[1];
} TypyMetaList;

#define MetaList_DESC(m) ((m)->list_desc)
#define MetaList_TYPYTYPE(m) (MetaList_DESC(m).desc_type)
#define MetaList_FIELDTYPE(m) (MetaList_DESC(m).desc_FieldType)
#define MetaList_CLEAR(m, f) \
	(abstract_Clear[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f)))
#define MetaList_READ(m, f, i, l) \
	(abstract_Read[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (i), (l)))
#define MetaList_WRITE(m, f, t, o) \
	(abstract_Write[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (t), (o)))
#define MetaList_BYTESIZE(m, f, t) \
	(abstract_ByteSize[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (t)))
#define MetaList_MERGEFROM(m, l, r) \
	(abstract_MergeFrom[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (l), (r)))
#define MetaList_GET(m, f) \
	(abstract_GetPyObject[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f)))
#define MetaList_SET(m, l, r) \
	(abstract_CopyFrom[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (l), (r)))
#define MetaList_CHECKSET(m, l, r, e) \
	(abstract_CheckAndSet[MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (l), (r), (e)))
#define MetaList_IsPrimitive(m) (MetaList_FIELDTYPE(m) < MAX_PRIMITIVE_TYPE)

typedef struct {
	PyObject_HEAD
	TypyMetaList* list_type;
	size_t        cached_size;
	size_t        list_capacity;
	size_t        list_length;
	TypyField*    list_items;
} TypyList;

inline void MetaList_Clear(TypyMetaList* type, TypyList* self) {
	register size_t i;
	for (i = 0; i < self->list_length; i++) {
		MetaList_CLEAR(type, &self->list_items[i]);
	}
	self->list_length = 0;
}
#define TypyList_Clear(ob) MetaList_Clear((ob)->list_type, (ob))
#define TypyList_TYPE(ob) (((TypyList*)(ob))->list_type)

inline TypyField* TypyList_EnsureSize(TypyList* self, size_t size) {
	if (self->list_length + size > self->list_capacity) {
		register size_t capacity = Ibl_Max(2 * self->list_capacity + size, MIN_LIST_CAPACITY);
		register TypyField* buffer = (TypyField*)calloc(capacity, sizeof(TypyField));
		if (!buffer) {
			PyErr_Format(PyExc_RuntimeError, "Alloc List buffer out of memory %d.\n", capacity * sizeof(TypyField));
			return NULL;
		}
		self->list_capacity = capacity;
		if (self->list_items) {
			memcpy(buffer, self->list_items, self->list_length * sizeof(TypyField));
			free(self->list_items);
		}
		self->list_items = buffer;
	}
	register TypyField* offset = &self->list_items[self->list_length];
	self->list_length += size;
	return offset;
}

inline bool MetaList_CheckAndSetList(TypyMetaList* type, TypyList* self, PyObject* value) {
	MetaList_Clear(type, self);
	register Py_ssize_t i, size = PySequence_Size(value);
	register TypyField* offset = TypyList_EnsureSize(self, size);
	if (!offset) { return false; }
	for (i = 0; i < size; i++) {
		if (!MetaList_CHECKSET(type, offset++, PySequence_GetItem(value, i), "List item type error: ")) {
			return false;
		}
	}
	return true;
}

extern PyTypeObject TypyListType;
extern PyTypeObject TypyMetaListType;
TypyMetaList* Typy_RegisterList(PyObject*, PyObject*);

inline TypyList* TypyList_New(TypyMetaList* type, PyObject* args, PyObject* kwargs) {
	TypyList* list = (TypyList*)calloc(1, sizeof(TypyList));
	if (!list) {
		PyErr_Format(PyExc_RuntimeError, "Alloc List object out of memory %d.", sizeof(TypyList));
		return NULL;
	}
	PyObject_INIT(list, &TypyListType);
	list->list_type = type;
	return list;
}

TypyList* TypyList_GetPyObject (TypyMetaList*, TypyList**);
bool      TypyList_CheckAndSet (TypyMetaList*, TypyList**, PyObject*, const char*);
bool      TypyList_Read        (TypyMetaList*, TypyList**, byte**, size_t*);
void      TypyList_MergeFrom   (TypyMetaList*, TypyList**, TypyList*);
size_t    TypyList_Write       (TypyMetaList*, TypyList**, int, byte*);
size_t    TypyList_ByteSize    (TypyMetaList*, TypyList**, int);

typedef struct {
	PyObject_HEAD
	size_t    it_index;
	TypyList* it_seq; /* Set to NULL when iterator is exhausted */
} TypyListIterator;

extern PyTypeObject TypyListIteratorType;

#ifdef __cplusplus
}
#endif

#endif // TYPY_LIST_H__
