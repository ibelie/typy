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

bool   TypyPython_CheckAndSet (TypyPython*, PyObject**, PyObject*, const char*);
bool   TypyPython_Read        (TypyPython*, PyObject**, byte**, size_t*);
size_t TypyPython_Write       (TypyPython*, PyObject**, int, byte*);
size_t TypyPython_ByteSize    (TypyPython*, PyObject**, int);

#ifdef __cplusplus
}
#endif

#endif // TYPY_PYTHON_H__
