// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_PYTHON_H__
#define TYPY_PYTHON_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	PyObject_HEAD
	PyTypeObject* python_type;
	char          python_name[1];
} TypyPython;

extern PyTypeObject TypyPythonType;
PyObject* Typy_RegisterPython(PyObject*, PyObject*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_PYTHON_H__
