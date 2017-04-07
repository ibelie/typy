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

typedef struct _TypyDictMap {
	IblMap_KEY(TypyField);
	TypyField value;
} *TypyDictMap;

static inline TypyDictMap _TypyDictMap_New(TypyField* key) {
	TypyDictMap item = (TypyDictMap)calloc(1, sizeof(struct _TypyDictMap));
	if (item) { item->key = *key; }
	return item;
}

static inline void _TypyDictMap_Free(TypyDictMap item) {
	if (item) { free(item); }
}

inline IblMap TypyDictMap_New(byte field_type) {
	IblMap map = (IblMap)calloc(1, sizeof(struct _IblMap));
	if (map) {
		map->hash = (IblMap_Hash)abstract_Hash[field_type];
		map->alloc = (IblMap_NewItem)_TypyDictMap_New;
		map->dealloc = (IblMap_Dealloc)_TypyDictMap_Free;
		map->compare = (IblMap_Compare)abstract_Compare[field_type];
	}
	return map;
}

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

#define MetaDict_DESC(m, i) (i ? MetaValue_DESC(m) : MetaKey_DESC(m))
#define MetaDict_FIELDTYPE(m, i) (MetaDict_DESC(m, i).desc_FieldType)
#define MetaDict_TYPYTYPE(m, i) (MetaDict_DESC(m, i).desc_type)
#define MetaDict_WIRETYPE(m, i) (MetaDict_DESC(m, i).desc_WireType)
#define MetaDict_READ(m, i, f, s, l) \
	(abstract_Read[MetaDict_FIELDTYPE(m, i)](MetaDict_TYPYTYPE(m, i), (f), (s), (l)))
#define MetaDict_MERGEFROM(m, l, r) \
	(abstract_MergeFrom[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (l), (r)))

inline void MetaDict_Clear(TypyMetaDict* type, TypyDict* self) {
	register TypyDictMap item;
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		item = (TypyDictMap)iter;
		MetaKey_CLEAR(type, &item->key);
		MetaValue_CLEAR(type, &item->value);
	}
}

inline bool MetaDict_SetItem(TypyMetaDict* type, TypyDict* self, PyObject* key, PyObject* value) {
	TypyField k = 0;
	if (!MetaKey_CHECKSET(type, &k, key, "Dict key type error: ")) {
		return false;
	}
	register TypyDictMap entry = (TypyDictMap)IblMap_Set(self->dict_map, &k);
	if (!entry) { return false; }
	return MetaValue_CHECKSET(type, &entry->value, value, "Dict value type error: ");
}

inline bool MetaDict_MergeDict(TypyMetaDict* type, TypyDict* self, PyObject* dict) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	while (PyDict_Next(dict, &pos, &k, &v)) {
		if (!MetaDict_SetItem(type, self, k, v)) {
			return false;
		}
	}
	return true;
}

inline bool MetaDict_MergeIter(TypyMetaDict* type, TypyDict* self, PyObject* iter) {
	register Py_ssize_t i, n = _PyObject_LengthHint(iter, 0);
	for (i = 0; i < n; i++) {
		register PyObject* item = PyIter_Next(iter);
		if (!MetaDict_SetItem(type, self, PyTuple_GET_ITEM(item, 0), PyTuple_GET_ITEM(item, 1))) {
			return false;
		}
	}
	return true;
}

#define TypyDict_TYPE(ob)               (((TypyDict*)(ob))->dict_type)
#define TypyDict_Clear(ob)              MetaDict_Clear(TypyDict_TYPE(ob), (ob))
#define TypyDict_SetItem(ob, k, v)      MetaDict_SetItem(TypyDict_TYPE(ob), (ob), (k), (v))
#define TypyDict_MergeDict(ob, d)       MetaDict_MergeDict(TypyDict_TYPE(ob), (ob), (d))
#define TypyDict_MergeIter(ob, i)       MetaDict_MergeIter(TypyDict_TYPE(ob), (ob), (i))
#define TypyKey_GET(ob, k)              MetaKey_GET(TypyDict_TYPE(ob), (k))
#define TypyKey_CHECKSET(ob, l, r, e)   MetaKey_CHECKSET(TypyDict_TYPE(ob), (l), (r), (e))
#define TypyValue_GET(ob, v)            MetaValue_GET(TypyDict_TYPE(ob), (v))
#define TypyValue_CHECKSET(ob, l, r, e) MetaValue_CHECKSET(TypyDict_TYPE(ob), (l), (r), (e))

inline TypyDict* TypyDict_New(TypyMetaDict* type, PyObject* args, PyObject* kwargs) {
	TypyDict* dict = (TypyDict*)calloc(1, sizeof(TypyDict));
	if (!dict) {
		PyErr_Format(PyExc_RuntimeError, "Alloc Dict object out of memory %d.", sizeof(TypyDict));
		return NULL;
	}
	dict->dict_map = TypyDictMap_New(MetaKey_FIELDTYPE(type));
	if (!dict->dict_map) {
		free(dict);
		PyErr_Format(PyExc_RuntimeError, "Alloc Dict map out of memory.");
		return NULL;
	}
	PyObject_INIT(dict, &TypyDictType);
	TypyDict_TYPE(dict) = type;
	return dict;
}

TypyDict* TypyDict_GetPyObject (TypyMetaDict*, TypyDict**);
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
extern PyTypeObject TypyDictIterItemType;

#ifdef __cplusplus
}
#endif

#endif // TYPY_DICT_H__
