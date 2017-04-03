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
	TypyField variant_value;
} TypyVariant;

extern PyTypeObject TypyVariantType;
extern PyTypeObject TypyMetaVariantType;
PyObject* Typy_RegisterVariant(PyObject*, PyObject*);

PyObject* TypyVariant_New(TypyMetaObject*, PyObject*, PyObject*);
PyObject* TypyVariant_toPyObject(TypyVariant*);
bool      TypyVariant_fromPyObject(TypyVariant*, PyObject*);

inline void TypyVariant_Clear(TypyVariant* self) {
	register int i = self->variant_index;
	if (i < 0 || (size_t)i >= Typy_SIZE(self)) { return; }
	Typy_METHOD_NOARG(self, i, Clear);
	self->variant_index = -1;
}

inline size_t TypyVariant_ByteSize(TypyVariant* self) {
	register size_t size = 0;
	register int i = self->variant_index;
	if (i >= 0 && (size_t)i < Typy_SIZE(self)) {
		size = Typy_METHOD(self, i, ByteSize, Typy_TAGSIZE(self, i));
	}
	return size;
}

inline void TypyVariant_SerializeString(TypyVariant* self, byte* output) {
	register int i = self->variant_index;
	if (i < 0 || (size_t)i >= Typy_SIZE(self)) { return; }
	output += Typy_METHOD(self, i, Write, Typy_ARGS(Typy_TAG(self, i), output));
}

inline size_t TypyVariant_MergeFromString(TypyVariant* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	if (!Typy_ReadTag(&input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
		return 0;
	}
	register int index = TAG_INDEX(tag);
	if (index < 0 || (size_t)index >= Typy_SIZE(self)) { return 0; }
	if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
		TypyVariant_Clear(self);
		if (!Typy_METHOD(self, index, Read, Typy_ARGS(&input, &remain))) {
			return 0;
		}
	} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
		TypyVariant_Clear(self);
		if (!Typy_METHOD(self, index, ReadPacked, Typy_ARGS(&input, &remain))) {
			return 0;
		}
	}
	self->variant_index = index;
	return length - remain;
}

#ifdef __cplusplus
}
#endif

#endif // TYPY_VARIANT_H__
