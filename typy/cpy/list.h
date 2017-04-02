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

typedef struct {
	PyObject_HEAD
	TypyMetaList* list_type;
	size_t        list_capacity;
	size_t        list_size;
	TypyField*    list_items;
} TypyList;

extern PyTypeObject TypyListType;
extern PyTypeObject TypyMetaListType;
PyObject* Typy_RegisterList(PyObject* m, PyObject* args);

#define TypyList_DESCRIPTOR(ob) ((ob)->list_type->list_descriptor)
#define TypyList_TAG(ob) (TypyList_DESCRIPTOR(ob).desc_tag)
#define TypyList_TAGSIZE(ob) (TypyList_DESCRIPTOR(ob).desc_tagsize)
#define TypyList_WIRETYPE(ob) (TypyList_DESCRIPTOR(ob).desc_WireType)
#define TypyList_METHOD(ob, NAME, ARGS) \
	(TypyList_DESCRIPTOR(ob).desc_##NAME(TypyList_DESCRIPTOR(ob).desc_type, ARGS))

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

inline void TypyList_Clear(TypyList* self) {
	register size_t i;
	for (i = 0; i < self->list_size; i++) {
		TypyList_METHOD(self, Clear, &self->list_items[i]);
	}
	self->list_size = 0;
}

inline void TypyList_Insert(TypyList* self) {
	/* todo: TypyList_Insert */
}

inline void TypyList_CheckAndSet(TypyList* self) {
	/* todo: TypyList_CheckAndSet */
}

inline void TypyList_Remove(TypyList* self) {
	/* todo: TypyList_Remove */
}

#ifdef __cplusplus
}
#endif

#endif // TYPY_LIST_H__
