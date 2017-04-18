// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_PYTHON_H__
#define TYPY_PYTHON_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef PyObject* (*TypyPyI_FromJson)   (PyTypeObject*, PyObject*);
typedef PyObject* (*TypyPyI_ToJson)     (PyObject*);
typedef size_t    (*TypyPyI_Write)      (PyObject*, byte*);
typedef size_t    (*TypyPyI_ByteSize)   (PyObject*);
typedef size_t    (*TypyPyI_CachedSize) (PyObject*);
typedef bool      (*TypyPyI_Read)       (PyObject*, byte*, size_t);

typedef struct {
	PyObject_HEAD
	PyTypeObject*      python_type;
	TypyPyI_FromJson   python_fromjson;
	TypyPyI_ToJson     python_tojson;
	TypyPyI_Write      python_write;
	TypyPyI_ByteSize   python_bytesize;
	TypyPyI_CachedSize python_cachedsize;
	TypyPyI_Read       python_read;
	char               python_name[1];
} TypyPython;

#define TypyPython_OFFSET(F)  ((size_t)(&((TypyPython*)(0))->F))
#define TypyPython_COPY(L, R) memcpy((char*)(L) + TypyPython_OFFSET(python_type), (char*)(R) + TypyPython_OFFSET(python_type), TypyPython_OFFSET(python_name) - TypyPython_OFFSET(python_type))

extern PyTypeObject TypyPythonType;
extern TypyPython*  TypyPythonBuiltin;

TypyPython* Typy_RegisterPython(PyObject*, PyObject*);

PyObject* TypyPython_ToJson      (TypyPython*, PyObject**, bool);
bool      TypyPython_FromJson    (TypyPython*, PyObject**, PyObject*);
bool      TypyPython_CheckAndSet (TypyPython*, PyObject**, PyObject*, const char*);
bool      TypyPython_Read        (TypyPython*, PyObject**, byte**, size_t*);
size_t    TypyPython_Write       (TypyPython*, PyObject**, int, byte*);
size_t    TypyPython_ByteSize    (TypyPython*, PyObject**, int);

#ifdef __cplusplus
}
#endif

#endif // TYPY_PYTHON_H__
