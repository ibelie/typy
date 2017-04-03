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
	int       variant_index;
	size_t    variant_size;
	TypyField variant_value;
} TypyVariant;

extern PyTypeObject TypyVariantType;
extern PyTypeObject TypyMetaVariantType;
PyObject* Typy_RegisterVariant(PyObject*, PyObject*);

PyObject* TypyVariant_New          (TypyMetaObject*, PyObject*, PyObject*);
size_t    TypyVariant_ByteSize     (TypyMetaObject*, TypyVariant**, int);
size_t    TypyVariant_Write        (TypyMetaObject*, TypyVariant**, int, byte*);
bool      TypyVariant_Read         (TypyMetaObject*, TypyVariant**, byte**, size_t*);
void      TypyVariant_MergeFrom    (TypyMetaObject*, TypyVariant**, TypyVariant*);

PyObject* TypyVariant_toPyObject   (TypyVariant*);
bool      TypyVariant_fromPyObject (TypyVariant*, PyObject*);

inline void TypyVariant_Clear(TypyVariant* self) {
	register int i = self->variant_index;
	if (i < 0 || (size_t)i >= Typy_SIZE(self)) { return; }
	Typy_METHOD_NOARG(self, i, Clear);
	self->variant_index = -1;
}

#ifdef __cplusplus
}
#endif

#endif // TYPY_VARIANT_H__
