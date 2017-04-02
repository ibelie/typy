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
	IblMap enum_map;
	char   enum_name[1];
} TypyEnum;

extern PyTypeObject TypyEnumType;
PyObject* Typy_RegisterEnum(PyObject* m, PyObject* args);

#ifdef __cplusplus
}
#endif

#endif // TYPY_ENUM_H__
