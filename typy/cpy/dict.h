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
	TypyComposite_HEAD
	TypyMetaDict* dict_type;
	IblMap        dict_map;
} TypyDict;

#define TypyDict_TYPE(ob) (((TypyDict*)(ob))->dict_type)

extern PyTypeObject TypyDictType;
extern PyTypeObject TypyMetaDictType;

TypyMetaDict* Typy_RegisterDict(PyObject*, PyObject*);

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
