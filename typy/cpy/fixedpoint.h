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

inline double TypyFixedPoint_toDouble(TypyFixedPoint* type, TypyField value) {
	return (double)value / type->fp_precision + type->fp_floor;
}

inline TypyField TypyFixedPoint_fromDouble(TypyFixedPoint* type, double value) {
	return (TypyField)((value - type->fp_floor) * type->fp_precision);
}

PyObject* TypyFixedPoint_GetPyObject (TypyFixedPoint*, TypyField*);
bool      TypyFixedPoint_CheckAndSet (TypyFixedPoint*, TypyField*, PyObject*, const char*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_FIXEDPOINT_H__
