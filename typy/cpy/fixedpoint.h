// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_FIXEDPOINT_H__
#define TYPY_FIXEDPOINT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	PyObject_HEAD
	uint8 fixedpoint_precision;
	int32 fixedpoint_floor;
} TypyFixedPoint;

extern PyTypeObject TypyFixedPointType;
PyObject* Typy_RegisterFixedPoint(PyObject*, PyObject*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_FIXEDPOINT_H__
