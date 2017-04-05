// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_LIST_H__
#define TYPY_LIST_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	PyObject_HEAD
	TypyDescriptor list_descriptor;
	char           list_name[1];
} TypyMetaList;

#define MetaList_DESC(m) ((m)->list_descriptor)
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
#define MetaList_IsPrimitive(m) (MetaList_FIELDTYPE(m) < MAX_PRIMITIVE_TYPE)

typedef struct {
	PyObject_HEAD
	TypyMetaList* list_type;
	size_t        list_capacity;
	size_t        list_length;
	size_t        list_size;
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

extern PyTypeObject TypyListType;
extern PyTypeObject TypyMetaListType;
PyObject* Typy_RegisterList(PyObject*, PyObject*);

inline PyObject* TypyList_New(TypyMetaList* type, PyObject* args, PyObject* kwargs) {
	PyObject* list = (PyObject*)calloc(1, sizeof(TypyList));
	if (!list) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Alloc List: out of memory %d.", sizeof(TypyList));
		return NULL;
	}
	PyObject_INIT(list, &TypyListType);
	((TypyList*)list)->list_type = type;
	return list;
}

inline TypyField* TypyList_EnsureSize(TypyList* self, size_t size) {
	/* todo: TypyList_EnsureSize */
	return NULL;
}

inline bool TypyList_Insert(TypyList* self, size_t offset, TypyField item) {
	/* todo: TypyList_Insert */
	return false;
}

inline bool TypyList_CheckAndSetList(TypyMetaList* type, TypyList* self, PyObject* value) {
	/* todo: TypyList_CheckAndSetList */
	return false;
}

inline void TypyList_Remove(TypyList* self, size_t offset) {
	/* todo: TypyList_Remove */
}

PyObject* TypyList_GetPyObject (TypyMetaList*, TypyList**);
bool      TypyList_CheckAndSet (TypyMetaList*, TypyList**, PyObject*, const char*);
bool      TypyList_Read        (TypyMetaList*, TypyList**, byte**, size_t*);
void      TypyList_MergeFrom   (TypyMetaList*, TypyList**, TypyList*);
size_t    TypyList_Write       (TypyMetaList*, TypyList**, int, byte*);
size_t    TypyList_ByteSize    (TypyMetaList*, TypyList**, int);

#ifdef __cplusplus
}
#endif

#endif // TYPY_LIST_H__
