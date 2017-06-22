// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_PROTOBUF_H__
#define TYPY_PROTOBUF_H__

#include "port.h"

#define MaxVarint32Len 5

#define TAG_TYPE_BITS 3
#define TAG_TYPE_MASK ((1 << TAG_TYPE_BITS) - 1)
#define MAKE_TAG(FIELD_NUMBER, TYPE) ((uint32)(((FIELD_NUMBER) << TAG_TYPE_BITS) | (TYPE)))
#define MAX_TAG(TAG) MAKE_TAG(TAG, TAG_TYPE_MASK)
#define TAG_INDEX(TAG) ((size_t)(((TAG) >> TAG_TYPE_BITS) - 1))
#define TAG_WIRETYPE(TAG) ((WireType)((TAG) & TAG_TYPE_MASK))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	WIRETYPE_VARINT  = 0,
	WIRETYPE_FIXED64 = 1,
	WIRETYPE_BYTES   = 2,
	WIRETYPE_START   = 3,
	WIRETYPE_END     = 4,
	WIRETYPE_FIXED32 = 5,
} WireType;

bool Typy_ReadTag      (byte**, size_t*, uint32*, uint32);
bool Typy_ReadVarint32 (byte**, size_t*, uint32*);
bool Typy_ReadVarint64 (byte**, size_t*, uint64*);
bool Typy_Read64       (byte**, size_t*, uint64*);
bool Typy_Read32       (byte**, size_t*, uint32*);
bool Typy_ReadByte     (byte**, size_t*, byte*);
bool Typy_SkipField    (byte**, size_t*, uint32);

size_t Typy_WriteByte  (byte*, byte*);
size_t Typy_WriteTag   (byte*, uint32);
size_t Typy_Write32    (byte*, uint32*);
size_t Typy_Write64    (byte*, uint64*);


#ifdef __cplusplus
}
#endif

#endif // TYPY_PROTOBUF_H__
