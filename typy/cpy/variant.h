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

#define MetaVariant_CLEAR(m, s, i) \
	(abstract_Clear[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value))
#define MetaVariant_GET(m, s, i) \
	(abstract_GetPyObject[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value))
#define MetaVariant_SET(m, s, i, f) \
	(abstract_CopyFrom[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (f)))
#define MetaVariant_CHECKSET(m, s, i, v, e) \
	(abstract_CheckAndSet[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (v), (e)))
#define MetaVariant_MERGEFROM(m, s, i, f) \
	(abstract_MergeFrom[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (f)))
#define MetaVariant_BYTESIZE(m, s, i, t) \
	(abstract_ByteSize[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (t)))
#define MetaVariant_WRITE(m, s, i, t, o) \
	(abstract_Write[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (t), (o)))
#define MetaVariant_READ(m, s, i, t, l) \
	(abstract_Read[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (t), (l)))

void    MetaVariant_Clear(TypyMetaObject* type, TypyVariant* self);
#define TypyVariant_Clear(ob) MetaVariant_Clear(Typy_TYPE(ob), (ob))

TypyVariant* TypyVariant_New          (TypyMetaObject*);
PyObject*    TypyVariant_GetPyObject  (TypyMetaObject*, TypyVariant**);
size_t       TypyVariant_ByteSize     (TypyMetaObject*, TypyVariant**, int);
size_t       TypyVariant_Write        (TypyMetaObject*, TypyVariant**, int, byte*);
bool         TypyVariant_Read         (TypyMetaObject*, TypyVariant**, byte**, size_t*);
bool         TypyVariant_CheckAndSet  (TypyMetaObject*, TypyVariant**, PyObject*, const char*);
void         TypyVariant_MergeFrom    (TypyMetaObject*, TypyVariant**, TypyVariant*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_VARIANT_H__
