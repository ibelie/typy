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
	uint32 fp_precision;
	int32  fp_floor;
} TypyFixedPoint;

extern PyTypeObject TypyFixedPointType;
TypyFixedPoint* Typy_RegisterFixedPoint(PyObject*, PyObject*);

#define   TypyFixedPoint_PRECISION(t)      (((TypyFixedPoint*)t)->fp_precision)
#define   TypyFixedPoint_FLOOR(t)          (((TypyFixedPoint*)t)->fp_floor)
#define   TypyFixedPoint_TO_DOUBLE(t, v)   ((double)(v) / TypyFixedPoint_PRECISION(t) + TypyFixedPoint_FLOOR(t))
#define   TypyFixedPoint_FROM_DOUBLE(t, v) ((TypyField)(((double)(v) - TypyFixedPoint_FLOOR(t)) * TypyFixedPoint_PRECISION(t)))

PyObject* TypyFixedPoint_GetPyObject (TypyFixedPoint*, TypyField*);
bool      TypyFixedPoint_CheckAndSet (TypyFixedPoint*, TypyField*, PyObject*, const char*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_FIXEDPOINT_H__
