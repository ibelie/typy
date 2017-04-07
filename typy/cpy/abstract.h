// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_ABSTRACT_H__
#define TYPY_ABSTRACT_H__

#include "typy.h"

#define MAX_VARINT_TYPE 6
#define MAX_PRIMITIVE_TYPE 9
#define MAX_FIELD_TYPE 16

typedef void*  TypyType;
typedef size_t TypyField;

typedef PyObject* (*GetPyObject) (TypyType, TypyField*);
typedef bool      (*CheckAndSet) (TypyType, TypyField*, PyObject*, const char*);
typedef void      (*CopyFrom)    (TypyType, TypyField*, TypyField);
typedef void      (*MergeFrom)   (TypyType, TypyField*, TypyField);
typedef void      (*Clear)       (TypyType, TypyField*);
typedef bool      (*Read)        (TypyType, TypyField*, byte**, size_t*);
typedef size_t    (*Write)       (TypyType, TypyField*, int, byte*);
typedef size_t    (*ByteSize)    (TypyType, TypyField*, int);

typedef struct {
	byte          desc_tagsize;
	uint32        desc_tag;
	TypyType      desc_type;
	byte          desc_FieldType;
	WireType      desc_WireType;
} TypyDescriptor;

typedef struct {
	TypyType      desc_type;
	byte          desc_FieldType;
	WireType      desc_WireType;
} TypyNestDesc;

extern GetPyObject    abstract_GetPyObject[MAX_FIELD_TYPE];
extern CheckAndSet    abstract_CheckAndSet[MAX_FIELD_TYPE];
extern CopyFrom       abstract_CopyFrom[MAX_FIELD_TYPE];
extern MergeFrom      abstract_MergeFrom[MAX_FIELD_TYPE];
extern Clear          abstract_Clear[MAX_FIELD_TYPE];
extern Read           abstract_Read[MAX_FIELD_TYPE];
extern Write          abstract_Write[MAX_FIELD_TYPE];
extern ByteSize       abstract_ByteSize[MAX_FIELD_TYPE];

extern IblMap_Hash    abstract_Hash[MAX_FIELD_TYPE];
extern IblMap_Compare abstract_Compare[MAX_FIELD_TYPE];

#ifdef __cplusplus
}
#endif

#endif // TYPY_ABSTRACT_H__
