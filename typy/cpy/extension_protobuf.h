// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_EXTENSION_PROTOBUF_H__
#define TYPY_EXTENSION_PROTOBUF_H__

#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

bool   Protobuf_TypeCheck  (PyTypeObject*);
size_t Protobuf_CachedSize (PyObject*);
size_t Protobuf_ByteSize   (PyObject*);
size_t Protobuf_Write      (PyObject*, void*);
bool   Protobuf_Read       (PyObject*, void*, size_t);

#ifdef __cplusplus
}
#endif

#endif // TYPY_EXTENSION_PROTOBUF_H__
