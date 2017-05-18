// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_OBJECT_H__
#define TYPY_OBJECT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TYPY_PROPERTY_HANDLER

#define MIN_HANDLER_CAPACITY 10

struct _TypyObject;
struct _TypyMetaObject;

typedef void*  TypyHandlerData;
typedef void (*TypyHandlerFunc)(struct _TypyObject *, size_t, TypyHandlerData, FieldType, TypyType, TypyField, TypyField);

typedef struct _TypyPropertyHandler {
	IblBitmap       handler_flag;
	TypyHandlerData handler_data;
	TypyHandlerFunc handler_func;
} *TypyPropertyHandler;

IblBitmap TypyProperty_Register   (struct _TypyMetaObject *, TypyHandlerData, TypyHandlerFunc);
void      TypyProperty_Unregister (struct _TypyMetaObject *, TypyHandlerData, TypyHandlerFunc);
void      TypyProperty_Changed    (struct _TypyObject *, PropertyFlag, FieldType, TypyType, TypyField, TypyField);
bool      Meta_HandleProperty     (struct _TypyMetaObject *, size_t, TypyHandlerData, TypyHandlerFunc);

#endif

typedef struct _TypyMetaObject {
	PyObject_HEAD
#ifdef TYPY_PROPERTY_HANDLER
	PropertyFlag        prop_flagmax;
	size_t              handlers_capacity;
	size_t              handlers_length;
	TypyPropertyHandler handlers_list;
#endif
	PyTypeObject*  py_type;
	IblMap         meta_field2index;
	char**         meta_index2field;
	uint32         meta_cutoff;
	size_t         meta_size;
	TypyDescriptor meta_descriptor[1];
} TypyMetaObject;

#define TypyObject_HEAD \
    TypyComposite_HEAD  \
    TypyMetaObject* meta_type;

typedef struct _TypyObject {
	TypyObject_HEAD
	size_t    cached_size;
	TypyField object_fields[1];
} TypyObject;

void TypyMeta_Dealloc(TypyMetaObject*);

#define     Meta_NAME(m)            ((char*)(&(((TypyMetaObject*)(m))->meta_descriptor[((TypyMetaObject*)(m))->meta_size])))
#define     Meta_SIZE(m)            (((TypyMetaObject*)(m))->meta_size)
#define     Meta_DESC(m, i)         (((TypyMetaObject*)(m))->meta_descriptor[i])
#define     Meta_TAG(m, i)          (Meta_DESC(m, i).desc_tag)
#define     Meta_TAGSIZE(m, i)      (Meta_DESC(m, i).desc_tagsize)
#define     Meta_FIELDTYPE(m, i)    (Meta_DESC(m, i).desc_FieldType)
#define     Meta_TYPYTYPE(m, i)     (Meta_DESC(m, i).desc_type)
#define     Meta_WIRETYPE(m, i)     (Meta_DESC(m, i).desc_WireType)
#define     Meta_FromInitializer    (field_type == FIELD_TYPE_OBJECT ? ((PyCFunctionObject*)typy_type)->m_self : typy_type)
#define     Meta_PropertyName(m, i) (((i) < 0 || (size_t)(i) >= (m)->meta_size) ? NULL : (m)->meta_index2field[i])

int         Meta_PropertyIndex      (TypyMetaObject*, char*);
PyObject*   Meta_ToJson             (TypyMetaObject*, TypyObject*, bool);
TypyObject* Meta_FromJson           (TypyMetaObject*, PyObject*);


extern PyTypeObject TypyMetaObjectType;
extern PyTypeObject BaseTypyObjectType;
extern PyTypeObject* TypyObjectType;

PyTypeObject*   _InheritTypyObjectType (void);
TypyMetaObject* _Typy_RegisterMeta     (PyObject*);
PyObject*       Typy_RegisterObject    (PyObject*, PyObject*);

#define Typy_NAME(ob)     Meta_NAME(Typy_TYPE(ob))
#define Typy_TYPE(ob)     (((TypyObject*)(ob))->meta_type)
#define Typy_FIELD(ob, i) (((TypyObject*)(ob))->object_fields[i])
#define Typy_SIZE(ob)     (Typy_TYPE(ob)->meta_size)
#define Typy_DESC(ob, i)  (Typy_TYPE(ob)->meta_descriptor[i])
#define Typy_TypeCheck(ob, m) \
	PyObject_TypeCheck(ob, TypyObjectType) && PyObject_IsSubclass((PyObject*)Typy_TYPE(ob), (PyObject*)m)
#define Typy_TypeCheckExact(ob, m) \
	(PyObject_TypeCheck((ob), TypyObjectType) && Typy_TYPE(ob) == (m))

#define Typy_TAG(ob, i)       (Typy_DESC(ob, i).desc_tag)
#define Typy_TAGSIZE(ob, i)   (Typy_DESC(ob, i).desc_tagsize)
#define Typy_FIELDTYPE(ob, i) (Typy_DESC(ob, i).desc_FieldType)
#define Typy_TYPYTYPE(ob, i)  (Typy_DESC(ob, i).desc_type)
#define Typy_WIRETYPE(ob, i)  (Typy_DESC(ob, i).desc_WireType)
#define Typy_TOJSON(ob, i, s) \
	(abstract_ToJson      [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (s)))
#define Typy_WRITE(ob, i, o) \
	(abstract_Write       [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), Typy_TAG(ob, i), (o)))
#define Typy_BYTESIZE(ob, i) \
	(abstract_ByteSize    [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), Typy_TAGSIZE(ob, i)))
#define _Typy_GET(ob, i) \
	(abstract_GetPyObject [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i)))
#define _Typy_READ(ob, i, s, l) \
	(abstract_Read        [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (s), (l)))
#define _Typy_CHECKSET(ob, i, v, e) \
	(abstract_CheckAndSet [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (v), (e)))
#define _Typy_FROMJSON(ob, i, j) \
	(abstract_FromJson    [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (j)))

#ifdef TYPY_PROPERTY_HANDLER
#	define    Meta_PROPFLAG(m, i)  (Meta_DESC(m, i).desc_PropFlag)
#	define    Typy_PROPFLAG(ob, i) (Typy_DESC(ob, i).desc_PropFlag)
	PyObject* Typy_GET             (TypyObject*, size_t);
	bool      Typy_READ            (TypyObject*, size_t, byte**, size_t*);
	bool      Typy_CHECKSET        (TypyObject*, size_t, PyObject*, const char*);
	bool      Typy_FROMJSON        (TypyObject*, size_t, PyObject*);
#else
#	define Meta_PROPFLAG(m, i)  0
#	define Typy_PROPFLAG(ob, i) 0
#	define Typy_GET(ob, i)            _Typy_GET((ob), (i))
#	define Typy_READ(ob, i, s, l)     _Typy_READ((ob), (i), (s), (l))
#	define Typy_CHECKSET(ob, i, v, e) _Typy_CHECKSET((ob), (i), (v), (e))
#	define Typy_FROMJSON(ob, i, j)    _Typy_FROMJSON((ob), (i), (j))
#endif

#define Typy_RECORD(ob, i) \
	TypyComposite_RECORD(Typy_FIELDTYPE(ob, i), Typy_FIELD(ob, i), (ob))
#define Typy_NOTIFY(ob, i) \
	TypyComposite_NOTIFY(FIELD_TYPE_OBJECT, (ob), Typy_PROPFLAG(ob, i), Typy_FIELDTYPE(ob, i), Typy_TYPYTYPE(ob, i), Typy_FIELD(ob, i))

#define Typy_MERGEFROM(ob, i, f) do { \
	Typy_RECORD((ob), (i));                                         \
	abstract_MergeFrom[Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), \
		&Typy_FIELD(ob, i), (f));                                   \
	Typy_NOTIFY((ob), (i));                                         \
} while (0)

#define Typy_CLEAR(ob, i) do { \
	Typy_RECORD((ob), (i));                                         \
	TypyField_Clr(Typy_FIELDTYPE(ob, i), Typy_FIELD(ob, i));        \
	Typy_NOTIFY((ob), (i));                                         \
} while (0)

#define Typy_SET(ob, i, f) do { \
	Typy_RECORD((ob), (i));                                         \
	Typy_FIELD(ob, i) = TypyField_Set(Typy_FIELDTYPE(ob, i), (f));  \
	Typy_NOTIFY((ob), (i));                                         \
} while (0)

void      Typy_Clear               (TypyObject*);
void      Typy_Dealloc             (TypyObject*);
void      Typy_MergeFrom           (TypyObject*, TypyObject*);
void      Typy_CopyFrom            (TypyObject*, TypyObject*);
void      Typy_SerializeString     (TypyObject*, byte*);
size_t    Typy_MergeFromString     (TypyObject*, byte*, size_t);
size_t    Typy_ByteSize            (TypyObject*);
size_t    Typy_PropertyByteSize    (TypyObject*, int);
void      Typy_SerializeProperty   (TypyObject*, byte*, int);
int       Typy_DeserializeProperty (TypyObject*, byte*, size_t);
int       Typy_SetAttr             (TypyObject*, PyObject*, PyObject*);
PyObject* Typy_GetAttr             (TypyObject*, PyObject*);
PyObject* Typy_Args                (TypyObject*);

#define   Typy_PropertyName(ob, i)  Meta_PropertyName  (Typy_TYPE(ob), (i))
#define   Typy_PropertyIndex(ob, k) Meta_PropertyIndex (Typy_TYPE(ob), (k))


TypyObject* Typy_New               (TypyMetaObject*, PyObject*, PyObject*);
PyObject*   TypyObject_ToJson      (TypyMetaObject*, TypyObject**, bool);
size_t      TypyObject_ByteSize    (TypyMetaObject*, TypyObject**, int);
size_t      TypyObject_Write       (TypyMetaObject*, TypyObject**, int, byte*);
bool        TypyObject_FromJson    (TypyMetaObject*, TypyObject**, PyObject*);
bool        TypyObject_Read        (TypyMetaObject*, TypyObject**, byte**, size_t*);
bool        TypyObject_CheckAndSet (TypyMetaObject*, TypyObject**, PyObject*, const char*);
void        TypyObject_MergeFrom   (TypyMetaObject*, TypyObject**, TypyObject*);

PyObject*   Py_FromJson            (TypyMetaObject*, PyObject*);
PyObject*   Py_Copy                (PyTypeObject*, TypyObject*);
PyObject*   Py_CopyFrom            (TypyObject*, TypyObject*);
PyObject*   Py_MergeFrom           (TypyObject*, TypyObject*);
PyObject*   Py_ToJson              (TypyObject*, PyObject*);
PyObject*   Py_DeepCopy            (TypyObject*, PyObject*);
PyObject*   Py_MergeFromString     (TypyObject*, PyObject*);
PyObject*   Py_ParseFromPyString   (TypyObject*, PyObject*);
PyObject*   Py_DeserializeProperty (TypyObject*, PyObject*);
PyObject*   Py_SerializeProperty   (TypyObject*, PyObject*);
PyObject*   Py_SerializeString     (TypyObject*);
PyObject*   Py_Clear               (TypyObject*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_OBJECT_H__
