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
	PyObject_HEAD
	uint8 fixedpoint_precision;
	uint8 fixedpoint_loor;
} TypyFixedPoint;

#ifdef __cplusplus
}
#endif

#endif // TYPY_TYPE_H__
