// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_OBJECT_H__
#define TYPY_OBJECT_H__

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	PyObject_HEAD
	PyTypeObject*  py_type;
	IblMap         meta_field2index;
	char**         meta_index2field;
	uint32         meta_cutoff;
	size_t         meta_size;
	TypyDescriptor meta_descriptor[1];
} TypyMetaObject;

#define TypyObject_HEAD \
    PyObject_HEAD       \
    TypyMetaObject* meta_type;

typedef struct {
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
#define     Meta_PropertyName(m, i) (((i) < 0 || (size_t)(i) > (m)->meta_size) ? NULL : (m)->meta_index2field[i])

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
#define Typy_CLEAR(ob, i) \
	(abstract_Clear       [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i)))
#define Typy_GET(ob, i) \
	(abstract_GetPyObject [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i)))
#define Typy_SET(ob, i, f) \
	(abstract_CopyFrom    [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (f)))
#define Typy_CHECKSET(ob, i, v, e) \
	(abstract_CheckAndSet [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (v), (e)))
#define Typy_MERGEFROM(ob, i, f) \
	(abstract_MergeFrom   [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (f)))
#define Typy_BYTESIZE(ob, i, t) \
	(abstract_ByteSize    [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (t)))
#define Typy_WRITE(ob, i, t, o) \
	(abstract_Write       [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (t), (o)))
#define Typy_READ(ob, i, s, l) \
	(abstract_Read        [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (s), (l)))
#define Typy_TOJSON(ob, i, s) \
	(abstract_ToJson      [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (s)))
#define Typy_FROMJSON(ob, i, j) \
	(abstract_FromJson    [Typy_FIELDTYPE(ob, i)](Typy_TYPYTYPE(ob, i), &Typy_FIELD(ob, i), (j)))

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

TypyObject* Py_FromJson            (TypyMetaObject*, PyObject*);
PyObject*   Py_ToJson              (TypyObject*, PyObject*);
PyObject*   Py_DeepCopy            (TypyObject*, PyObject*);
PyObject*   Py_CopyFrom            (TypyObject*, PyObject*);
PyObject*   Py_MergeFrom           (TypyObject*, PyObject*);
PyObject*   Py_SerializeString     (TypyObject*);
PyObject*   Py_MergeFromString     (TypyObject*, PyObject*);
PyObject*   Py_DeserializeProperty (TypyObject*, PyObject*);
PyObject*   Py_SerializeProperty   (TypyObject*, PyObject*);
PyObject*   Py_Clear               (TypyObject*);
PyObject*   Py_ParseFromPyString   (TypyObject*, PyObject*);

#ifdef __cplusplus
}
#endif

#endif // TYPY_OBJECT_H__
