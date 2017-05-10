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
	PyObject_HEAD
	TypyMetaList* list_type;
	size_t        cached_size;
	size_t        list_capacity;
	size_t        list_length;
	TypyField*    list_items;
} TypyList;

#define MetaList_DESC(m)      (((TypyMetaList*)(m))->list_desc)
#define MetaList_TYPYTYPE(m)  (MetaList_DESC(m).desc_type)
#define MetaList_FIELDTYPE(m) (MetaList_DESC(m).desc_FieldType)
#define MetaList_WIRETYPE(m)  (MetaList_DESC(m).desc_WireType)
#define MetaList_CLEAR(m, f) \
	(abstract_Clear       [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f)))
#define MetaList_READ(m, f, i, l) \
	(abstract_Read        [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (i), (l)))
#define MetaList_WRITE(m, f, t, o) \
	(abstract_Write       [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (t), (o)))
#define MetaList_BYTESIZE(m, f, t) \
	(abstract_ByteSize    [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (t)))
#define MetaList_COPYFROM(m, l, r) \
	(abstract_CopyFrom   [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (l), (r)))
#define MetaList_MERGEFROM(m, l, r) \
	(abstract_MergeFrom   [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (l), (r)))
#define MetaList_GET(m, f) \
	(abstract_GetPyObject [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f)))
#define MetaList_SET(m, l, r) \
	(abstract_CopyFrom    [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (l), (r)))
#define MetaList_CHECKSET(m, l, r, e) \
	(abstract_CheckAndSet [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (l), (r), (e)))
#define MetaList_TOJSON(m, f, s) \
	(abstract_ToJson      [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (s)))
#define MetaList_FROMJSON(m, f, j) \
	(abstract_FromJson    [MetaList_FIELDTYPE(m)](MetaList_TYPYTYPE(m), (f), (j)))
#define MetaList_IsPrimitive(m) (MetaList_FIELDTYPE(m) < MAX_PRIMITIVE_TYPE)

#define MetaList_Clear(m, ob) { \
	register size_t i;                             \
	for (i = 0; i < (ob)->list_length; i++) {      \
		MetaList_CLEAR((m), &(ob)->list_items[i]); \
	}                                              \
	(ob)->list_length = 0;                         \
}

#define TypyList_TYPE(ob)              (((TypyList*)(ob))->list_type)
#define TypyList_CLEAR(ob, f)          MetaList_CLEAR(TypyList_TYPE(ob), (f))
#define TypyList_READ(ob, f, i, l)     MetaList_READ(TypyList_TYPE(ob), (f), (i), (l))
#define TypyList_WRITE(ob, f, t, o)    MetaList_WRITE(TypyList_TYPE(ob), (f), (t), (o))
#define TypyList_BYTESIZE(ob, f, t)    MetaList_BYTESIZE(TypyList_TYPE(ob), (f), (t))
#define TypyList_MERGEFROM(ob, l, r)   MetaList_MERGEFROM(TypyList_TYPE(ob), (l), (r))
#define TypyList_GET(ob, f)            MetaList_GET(TypyList_TYPE(ob), (f))
#define TypyList_SET(ob, l, r)         MetaList_SET(TypyList_TYPE(ob), (l), (r))
#define TypyList_CHECKSET(ob, l, r, e) MetaList_CHECKSET(TypyList_TYPE(ob), (l), (r), (e))
#define TypyList_Clear(ob)             MetaList_Clear(TypyList_TYPE(ob), (ob))

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
