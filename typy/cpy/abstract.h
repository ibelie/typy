// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_ABSTRACT_H__
#define TYPY_ABSTRACT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VARINT_TYPE          6
#define MAX_PRIMITIVE_TYPE       9
#define MAX_FIELD_TYPE          16
#define FIELD_TYPE_OBJECT       11
#define FIELD_TYPE_VARIANT      12
#define FIELD_TYPE_LIST         13
#define FIELD_TYPE_DICT         14
#define FIELD_TYPE_COMPOSITE(f) ((f) >= FIELD_TYPE_OBJECT && (f) <= FIELD_TYPE_DICT)

typedef void* TypyType;
typedef uint8 FieldType;

#ifdef TYPY_FIELD_SIZE_64
	typedef uint64 TypyField;
#else
	typedef size_t TypyField;
#endif

#ifdef TYPY_PROPERTY_HANDLER

#define MIN_OWNER_CAPACITY 1

#define TypyComposite_HEAD             \
	PyObject_HEAD                      \
	size_t            owners_capacity; \
	size_t            owners_length;   \
	TypyPropertyOwner owners_list;

#define TypyComposite_FREE(ob) \
	((TypyComposite*)(ob))->owners_capacity = 0;   \
	((TypyComposite*)(ob))->owners_length = 0;     \
	if (((TypyComposite*)(ob))->owners_list) {     \
		free(((TypyComposite*)(ob))->owners_list); \
	}

typedef struct _TypyPropertyOwner {
	FieldType               owner_type;
	struct _TypyComposite * prop_owner;
	size_t                  prop_flag;
} *TypyPropertyOwner;

typedef struct _TypyComposite {
	TypyComposite_HEAD
} TypyComposite;

bool TypyComposite_AddOwner(TypyComposite*, TypyComposite*, FieldType, size_t);
void TypyComposite_DelOwner(TypyComposite*, TypyComposite*);

#define TypyComposite_ADD_OWNER(child, parent, type, flag) \
	(FIELD_TYPE_COMPOSITE(type) ? TypyComposite_AddOwner((TypyComposite*)(child), (TypyComposite*)(parent), (FieldType)(type), (size_t)(flag)) : true)
#define TypyComposite_DEL_OWNER(child, parent) \
	(FIELD_TYPE_COMPOSITE(type) ? TypyComposite_DelOwner((TypyComposite*)(child), (TypyComposite*)(parent)) : true)

#else
#	define TypyComposite_HEAD PyObject_HEAD
#	define TypyComposite_FREE(ob)
#	define TypyComposite_ADD_OWNER(child, parent, type, flag)
#	define TypyComposite_DEL_OWNER(child, parent)
#endif

typedef PyObject* (*GetPyObject) (TypyType, TypyField*);
typedef PyObject* (*ToJson)      (TypyType, TypyField*, bool);
typedef bool      (*FromJson)    (TypyType, TypyField*, PyObject*);
typedef bool      (*FromJsonKey) (TypyType, TypyField*, PyObject*);
typedef bool      (*CheckAndSet) (TypyType, TypyField*, PyObject*, const char*);
typedef void      (*CopyFrom)    (TypyType, TypyField*, TypyField);
typedef void      (*MergeFrom)   (TypyType, TypyField*, TypyField);
typedef void      (*Clear)       (TypyType, TypyField*);
typedef bool      (*Read)        (TypyType, TypyField*, byte**, size_t*);
typedef size_t    (*Write)       (TypyType, TypyField*, int, byte*);
typedef size_t    (*ByteSize)    (TypyType, TypyField*, int);

typedef struct {
	uint8         desc_tagsize;
	uint32        desc_tag;
	TypyType      desc_type;
	FieldType     desc_FieldType;
	WireType      desc_WireType;
} TypyDescriptor;

typedef struct {
	TypyType      desc_type;
	FieldType     desc_FieldType;
	WireType      desc_WireType;
} TypyNestDesc;

extern GetPyObject    abstract_GetPyObject [MAX_FIELD_TYPE];
extern CheckAndSet    abstract_CheckAndSet [MAX_FIELD_TYPE];
extern CopyFrom       abstract_CopyFrom    [MAX_FIELD_TYPE];
extern MergeFrom      abstract_MergeFrom   [MAX_FIELD_TYPE];
extern Clear          abstract_Clear       [MAX_FIELD_TYPE];
extern Read           abstract_Read        [MAX_FIELD_TYPE];
extern Write          abstract_Write       [MAX_FIELD_TYPE];
extern ByteSize       abstract_ByteSize    [MAX_FIELD_TYPE];
extern ToJson         abstract_ToJson      [MAX_FIELD_TYPE];
extern FromJson       abstract_FromJson    [MAX_FIELD_TYPE];
extern FromJsonKey    abstract_FromJsonKey [MAX_FIELD_TYPE];

extern IblMap_Hash    abstract_Hash        [MAX_FIELD_TYPE];
extern IblMap_Compare abstract_Compare     [MAX_FIELD_TYPE];

#ifdef __cplusplus
}
#endif

#endif // TYPY_ABSTRACT_H__
