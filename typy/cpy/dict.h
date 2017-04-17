// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_DICT_H__
#define TYPY_DICT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================

typedef struct {
	PyObject_HEAD
	TypyNestDesc key_desc;
	TypyNestDesc value_desc;
	char         dict_name[1];
} TypyMetaDict;

typedef struct {
	PyObject_HEAD
	TypyMetaDict* dict_type;
	IblMap        dict_map;
} TypyDict;

extern PyTypeObject TypyDictType;
extern PyTypeObject TypyMetaDictType;

TypyMetaDict* Typy_RegisterDict(PyObject*, PyObject*);

#define MetaKey_DESC(m) (((TypyMetaDict*)(m))->key_desc)
#define MetaKey_TYPYTYPE(m) (MetaKey_DESC(m).desc_type)
#define MetaKey_FIELDTYPE(m) (MetaKey_DESC(m).desc_FieldType)
#define MetaKey_WIRETYPE(m) (MetaKey_DESC(m).desc_WireType)
#define MetaKey_TAG(m) (MAKE_TAG(1, MetaKey_WIRETYPE(m)))
#define MetaKey_CLEAR(m, k) \
	(abstract_Clear[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k)))
#define MetaKey_BYTESIZE(m, k) \
	(abstract_ByteSize[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k), 1))
#define MetaKey_WRITE(m, k, o) \
	(abstract_Write[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k), MetaKey_TAG(m), (o)))
#define MetaKey_GET(m, k) \
	(abstract_GetPyObject[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k)))
#define MetaKey_CHECKSET(m, l, r, e) \
	(abstract_CheckAndSet[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (l), (r), (e)))
#define MetaKey_TOJSON(m, v, s) \
	(abstract_ToJson[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (v), (s)))
#define MetaKey_FROMJSON(m, v, j) \
	(abstract_FromJsonKey[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (v), (j)))

#define MetaValue_DESC(m) (((TypyMetaDict*)(m))->value_desc)
#define MetaValue_TYPYTYPE(m) (MetaValue_DESC(m).desc_type)
#define MetaValue_FIELDTYPE(m) (MetaValue_DESC(m).desc_FieldType)
#define MetaValue_WIRETYPE(m) (MetaValue_DESC(m).desc_WireType)
#define MetaValue_TAG(m) (MAKE_TAG(2, MetaValue_WIRETYPE(m)))
#define MetaValue_CLEAR(m, v) \
	(abstract_Clear[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v)))
#define MetaValue_BYTESIZE(m, v) \
	(abstract_ByteSize[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), 1))
#define MetaValue_WRITE(m, v, o) \
	(abstract_Write[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), MetaValue_TAG(m), (o)))
#define MetaValue_GET(m, v) \
	(abstract_GetPyObject[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v)))
#define MetaValue_CHECKSET(m, l, r, e) \
	(abstract_CheckAndSet[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (l), (r), (e)))
#define MetaValue_TOJSON(m, v, s) \
	(abstract_ToJson[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), (s)))
#define MetaValue_FROMJSON(m, v, j) \
	(abstract_FromJson[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), (j)))

#define MetaDict_DESC(m, i) (i ? MetaValue_DESC(m) : MetaKey_DESC(m))
#define MetaDict_FIELDTYPE(m, i) (MetaDict_DESC(m, i).desc_FieldType)
#define MetaDict_TYPYTYPE(m, i) (MetaDict_DESC(m, i).desc_type)
#define MetaDict_WIRETYPE(m, i) (MetaDict_DESC(m, i).desc_WireType)
#define MetaDict_READ(m, i, f, s, l) \
	(abstract_Read[MetaDict_FIELDTYPE(m, i)](MetaDict_TYPYTYPE(m, i), (f), (s), (l)))
#define MetaDict_MERGEFROM(m, l, r) \
	(abstract_MergeFrom[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (l), (r)))

#define MetaDict_Clear(m, ob) { \
	register IblMap_Item iter;                                                                  \
	for (iter = IblMap_Begin((ob)->dict_map); iter; iter = IblMap_Next((ob)->dict_map, iter)) { \
		MetaValue_CLEAR((m), &((TypyDictMap)iter)->value);                                      \
	}                                                                                           \
	IblMap_Clear((ob)->dict_map);                                                               \
}

#define TypyDict_TYPE(ob)               (((TypyDict*)(ob))->dict_type)
#define TypyDict_Clear(ob)              MetaDict_Clear(TypyDict_TYPE(ob), (ob))
#define TypyKey_GET(ob, k)              MetaKey_GET(TypyDict_TYPE(ob), (k))
#define TypyKey_CHECKSET(ob, l, r, e)   MetaKey_CHECKSET(TypyDict_TYPE(ob), (l), (r), (e))
#define TypyValue_GET(ob, v)            MetaValue_GET(TypyDict_TYPE(ob), (v))
#define TypyValue_CLEAR(ob, v)          MetaValue_CLEAR(TypyDict_TYPE(ob), (v))
#define TypyValue_CHECKSET(ob, l, r, e) MetaValue_CHECKSET(TypyDict_TYPE(ob), (l), (r), (e))

TypyDict* TypyDict_GetPyObject (TypyMetaDict*, TypyDict**);
PyObject* TypyDict_ToJson      (TypyMetaDict*, TypyDict**, bool);
bool      TypyDict_FromJson    (TypyMetaDict*, TypyDict**, PyObject*);
bool      TypyDict_CheckAndSet (TypyMetaDict*, TypyDict**, PyObject*, const char*);
bool      TypyDict_Read        (TypyMetaDict*, TypyDict**, byte**, size_t*);
void      TypyDict_MergeFrom   (TypyMetaDict*, TypyDict**, TypyDict*);
size_t    TypyDict_Write       (TypyMetaDict*, TypyDict**, int, byte*);
size_t    TypyDict_ByteSize    (TypyMetaDict*, TypyDict**, int);

typedef struct {
	PyObject_HEAD
	PyObject*   it_result; /* reusable result tuple for iteritems */
	size_t      it_index;
	IblMap_Item it;
	TypyDict*   it_dict; /* Set to NULL when iterator is exhausted */
} TypyDictIterator;

extern PyTypeObject TypyDictIterKeyType;
extern PyTypeObject TypyDictIterValueType;
extern PyTypeObject TypyDictIterItemType;

#ifdef __cplusplus
}
#endif

#endif // TYPY_DICT_H__
