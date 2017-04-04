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
#define MetaKey_CLEAR(m, k) \
	(abstract_Clear[MetaKey_DESC(m).desc_FieldType](MetaKey_DESC(m).desc_type, (k)))

#define MetaValue_DESC(m) ((m)->value_descriptor)
#define MetaValue_CLEAR(m, v) \
	(abstract_Clear[MetaValue_DESC(m).desc_FieldType](MetaValue_DESC(m).desc_type, (v)))

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

inline void TypyDict_Set(TypyDict* self) {
	/* todo: TypyDict_Set */
}

inline void TypyDict_Get(TypyDict* self) {
	/* todo: TypyDict_Get */
}

inline void TypyDict_Del(TypyDict* self) {
	/* todo: TypyDict_Del */
}

inline void TypyDict_CheckAndSetDict(TypyDict* self) {
	/* todo: TypyDict_CheckAndSetDict */
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
