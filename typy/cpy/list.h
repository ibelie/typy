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

typedef struct {
	TypyComposite_HEAD
	TypyMetaList* list_type;
	size_t        cached_size;
	size_t        list_capacity;
	size_t        list_length;
	TypyField*    list_items;
} TypyList;

#define MetaList_WIRETYPE(m)    (((TypyMetaList*)(m))->list_desc.desc_WireType)
#define MetaList_IsPrimitive(m) (((TypyMetaList*)(m))->list_desc.desc_FieldType < MAX_PRIMITIVE_TYPE)
#define TypyList_TYPE(ob)       (((TypyList*)(ob))->list_type)

extern PyTypeObject TypyListType;
extern PyTypeObject TypyMetaListType;

TypyMetaList* Typy_RegisterList(PyObject*, PyObject*);

TypyList* TypyList_GetPyObject  (TypyMetaList*, TypyList**);
PyObject* TypyList_ToJson       (TypyMetaList*, TypyList**, bool);
bool      TypyList_FromJson     (TypyMetaList*, TypyList**, PyObject*);
bool      TypyList_CheckAndSet  (TypyMetaList*, TypyList**, PyObject*, const char*);
bool      TypyList_ReadRepeated (TypyMetaList*, TypyList**, byte**, size_t*);
bool      TypyList_Read         (TypyMetaList*, TypyList**, byte**, size_t*);
void      TypyList_MergeFrom    (TypyMetaList*, TypyList**, TypyList*);
size_t    TypyList_Write        (TypyMetaList*, TypyList**, int, byte*);
size_t    TypyList_ByteSize     (TypyMetaList*, TypyList**, int);


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
