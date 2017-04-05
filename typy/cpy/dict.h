// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_DICT_H__
#define TYPY_DICT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

IblMap_KEY_NUMERIC(TypyDictMap, TypyField,
	TypyField value;
);

typedef struct {
	PyObject_HEAD
	TypyDescriptor key_descriptor;
	TypyDescriptor value_descriptor;
	char           dict_name[1];
} TypyMetaDict;

typedef struct {
	PyObject_HEAD
	TypyMetaDict* dict_type;
	IblMap        dict_map;
} TypyDict;

extern PyTypeObject TypyDictType;
extern PyTypeObject TypyMetaDictType;
PyObject* Typy_RegisterDict(PyObject*, PyObject*);

#define MetaKey_DESC(m) ((m)->key_descriptor)
#define MetaKey_TAG(m) (MetaKey_DESC(m).desc_tag)
#define MetaKey_FIELDTYPE(m) (MetaKey_DESC(m).desc_FieldType)
#define MetaKey_TYPYTYPE(m) (MetaKey_DESC(m).desc_type)
#define MetaKey_CLEAR(m, k) \
	(abstract_Clear[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k)))
#define MetaKey_BYTESIZE(m, k) \
	(abstract_ByteSize[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k), 1))
#define MetaKey_WRITE(m, k, o) \
	(abstract_Write[MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k), MetaKey_TAG(m), (o)))

#define MetaValue_DESC(m) ((m)->value_descriptor)
#define MetaValue_TAG(m) (MetaValue_DESC(m).desc_tag)
#define MetaValue_FIELDTYPE(m) (MetaValue_DESC(m).desc_FieldType)
#define MetaValue_TYPYTYPE(m) (MetaValue_DESC(m).desc_type)
#define MetaValue_CLEAR(m, v) \
	(abstract_Clear[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v)))
#define MetaValue_BYTESIZE(m, v) \
	(abstract_ByteSize[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), 1))
#define MetaValue_WRITE(m, v, o) \
	(abstract_Write[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), MetaValue_TAG(m), (o)))

#define MetaDict_DESC(m, i) (i ? MetaValue_DESC(m) : MetaKey_DESC(m))
#define MetaDict_FIELDTYPE(m, i) (MetaDict_DESC(m, i).desc_FieldType)
#define MetaDict_TYPYTYPE(m, i) (MetaDict_DESC(m, i).desc_type)
#define MetaDict_WIRETYPE(m, i) (MetaDict_DESC(m, i).desc_WireType)
#define MetaDict_READ(m, i, f, s, l) \
	(abstract_Read[MetaDict_FIELDTYPE(m, i)](MetaDict_TYPYTYPE(m, i), (f), (s), (l)))
#define MetaDict_MERGEFROM(m, l, r) \
	(abstract_MergeFrom[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (l), (r)))

inline PyObject* TypyDict_New(TypyMetaDict* type, PyObject* args, PyObject* kwargs) {
	TypyDict* dict = (TypyDict*)calloc(1, sizeof(TypyDict));
	if (!dict) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Alloc Dict: out of memory %d.", sizeof(TypyDict));
		return NULL;
	}
	dict->dict_map = TypyDictMap_New();
	if (!dict->dict_map) {
		free(dict);
		PyErr_Format(PyExc_RuntimeError, "[typyd] Alloc Dict: map out of memory.");
		return NULL;
	}
	PyObject_INIT(dict, &TypyDictType);
	dict->dict_type = type;
	return (PyObject*)dict;
}

inline void MetaDict_Clear(TypyMetaDict* type, TypyDict* self) {
	register TypyDictMap item;
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		item = (TypyDictMap)iter;
		MetaKey_CLEAR(type, &item->key);
		MetaValue_CLEAR(type, &item->value);
	}
	IblMap_Free(self->dict_map);
}
#define TypyDict_Clear(ob) MetaDict_Clear((ob)->dict_type, (ob))
#define TypyDict_TYPE(ob) (((TypyDict*)(ob))->dict_type)

inline bool TypyDict_CheckAndSetDict(TypyMetaDict* type, TypyDict* self, PyObject* value) {
	/* todo: TypyDict_CheckAndSetDict */
	return false;
}

inline bool TypyDict_CheckAndSetItems(TypyMetaDict* type, TypyDict* self, PyObject* items) {
	/* todo: TypyDict_CheckAndSetDict */
	return false;
}

PyObject* TypyDict_GetPyObject (TypyMetaDict*, TypyDict**);
bool      TypyDict_CheckAndSet (TypyMetaDict*, TypyDict**, PyObject*, const char*);
bool      TypyDict_Read        (TypyMetaDict*, TypyDict**, byte**, size_t*);
void      TypyDict_MergeFrom   (TypyMetaDict*, TypyDict**, TypyDict*);
size_t    TypyDict_Write       (TypyMetaDict*, TypyDict**, int, byte*);
size_t    TypyDict_ByteSize    (TypyMetaDict*, TypyDict**, int);

#ifdef __cplusplus
}
#endif

#endif // TYPY_DICT_H__
