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

#define TypyKey_DESCRIPTOR(ob) ((ob)->dict_type->key_descriptor)
#define TypyKey_TAG(ob) (TypyKey_DESCRIPTOR(ob).desc_tag)
#define TypyKey_TAGSIZE(ob) (TypyKey_DESCRIPTOR(ob).desc_tagsize)
#define TypyKey_WIRETYPE(ob) (TypyKey_DESCRIPTOR(ob).desc_WireType)
#define TypyKey_METHOD(ob, NAME, ARGS) \
	(TypyKey_DESCRIPTOR(ob).desc_##NAME(TypyKey_DESCRIPTOR(ob).desc_type, ARGS))

#define TypyValue_DESCRIPTOR(ob) ((ob)->dict_type->value_descriptor)
#define TypyValue_TAG(ob) (TypyValue_DESCRIPTOR(ob).desc_tag)
#define TypyValue_TAGSIZE(ob) (TypyValue_DESCRIPTOR(ob).desc_tagsize)
#define TypyValue_WIRETYPE(ob) (TypyValue_DESCRIPTOR(ob).desc_WireType)
#define TypyValue_METHOD(ob, NAME, ARGS) \
	(TypyValue_DESCRIPTOR(ob).desc_##NAME(TypyValue_DESCRIPTOR(ob).desc_type, ARGS))

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

inline void TypyDict_Clear(TypyDict* self) {
	register TypyDictMap item;
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		item = (TypyDictMap)iter;
		TypyKey_METHOD(self, Clear, &item->key);
		TypyValue_METHOD(self, Clear, &item->value);
	}
	IblMap_Free(self->dict_map);
}

inline void TypyDict_Set(TypyDict* self) {
	/* todo: TypyDict_Set */
}

inline void TypyDict_Get(TypyDict* self) {
	/* todo: TypyDict_Get */
}

inline void TypyDict_Del(TypyDict* self) {
	/* todo: TypyDict_Del */
}

inline void TypyDict_CheckAndSet(TypyDict* self) {
	/* todo: TypyDict_CheckAndSet */
}

PyObject* TypyDict_GetPyObject(TypyMetaDict*, TypyField*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_DICT_H__
