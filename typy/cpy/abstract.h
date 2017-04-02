// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_ABSTRACT_H__
#define TYPY_ABSTRACT_H__

#include "typy.h"

#define MAX_FIELD_TYPE 16

extern GetPyObject abstract_GetPyObject[MAX_FIELD_TYPE];
extern CheckAndSet abstract_CheckAndSet[MAX_FIELD_TYPE];
extern CopyFrom abstract_CopyFrom[MAX_FIELD_TYPE];
extern MergeFrom abstract_MergeFrom[MAX_FIELD_TYPE];
extern Clear abstract_Clear[MAX_FIELD_TYPE];
extern Read abstract_Read[MAX_FIELD_TYPE];
extern ReadPacked abstract_ReadPacked[MAX_FIELD_TYPE];
extern Write abstract_Write[MAX_FIELD_TYPE];
extern ByteSize abstract_ByteSize[MAX_FIELD_TYPE];

#ifdef __cplusplus
}
#endif

#endif // TYPY_ABSTRACT_H__
