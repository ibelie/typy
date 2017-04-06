// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_VARIANT_H__
#define TYPY_VARIANT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TypyObject_HEAD
	size_t    cached_size;
	int       variant_index;
	TypyField variant_value;
} TypyVariant;

extern PyTypeObject TypyVariantType;
extern PyTypeObject TypyMetaVariantType;
TypyMetaObject* Typy_RegisterVariant(PyObject*, PyObject*);

TypyVariant* TypyVariant_New          (TypyMetaObject*, PyObject*, PyObject*);
PyObject*    TypyVariant_GetPyObject  (TypyMetaObject*, TypyVariant**);
size_t       TypyVariant_ByteSize     (TypyMetaObject*, TypyVariant**, int);
size_t       TypyVariant_Write        (TypyMetaObject*, TypyVariant**, int, byte*);
bool         TypyVariant_Read         (TypyMetaObject*, TypyVariant**, byte**, size_t*);
bool         TypyVariant_CheckAndSet  (TypyMetaObject*, TypyVariant**, PyObject*, const char*);
void         TypyVariant_MergeFrom    (TypyMetaObject*, TypyVariant**, TypyVariant*);

inline void TypyVariant_Clear(TypyVariant* self) {
	register int i = self->variant_index;
	if (i < 0 || (size_t)i >= Typy_SIZE(self)) { return; }
	Typy_CLEAR(self, i);
	self->variant_index = -1;
}

#ifdef __cplusplus
}
#endif

#endif // TYPY_VARIANT_H__
