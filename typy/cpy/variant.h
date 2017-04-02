// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_ENUM_H__
#define TYPY_ENUM_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TypyObject_HEAD
	int       variant_index;
	size_t    variant_cached_size;
	TypyField variant_value;
} TypyVariant;

#ifdef __cplusplus
}
#endif

#endif // TYPY_TYPE_H__
