// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_TYPE_H__
#define TYPY_TYPE_H__

#ifdef _MSC_VER
#	ifndef HAVE_ROUND
#		define HAVE_ROUND
#	endif
#endif
#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "map.h"
#include "protobuf.h"

#define FULL_MODULE_NAME "_typyd"

#ifndef PyVarObject_HEAD_INIT
#	define PyVarObject_HEAD_INIT(type, size) PyObject_HEAD_INIT(type) size,
#endif
#ifndef Py_TYPE
#	define Py_TYPE(ob) (((PyObject*)(ob))->ob_type)
#endif
#ifndef PyUnbound_Check
#	define PyUnbound_Check(ob) (Py_TYPE(ob)->tp_descr_get && !Py_TYPE(ob)->tp_descr_set && \
		!PyMethod_Check(ob) && !PyFunction_Check(ob) && !PyType_Check(ob) && !PyClass_Check(ob) && \
		!PyObject_TypeCheck(ob, &PyClassMethod_Type) && !PyObject_TypeCheck(ob, &PyStaticMethod_Type))
#endif

#if PY_MAJOR_VERSION >= 3
#	define PyInt_Check PyLong_Check
#	define PyInt_AsLong PyLong_AsLong
#	define PyInt_FromLong PyLong_FromLong
#	define PyInt_FromSize_t PyLong_FromSize_t
#	define PyString_Check PyUnicode_Check
#	define PyString_FromString PyUnicode_FromString
#	define PyString_FromStringAndSize PyUnicode_FromStringAndSize
#	if PY_VERSION_HEX < 0x03030000
#		error "Python 3.0 - 3.2 are not supported."
#	else
#		define PyString_AsString(ob) \
			(PyUnicode_Check(ob) ? PyUnicode_AsUTF8(ob) : PyBytes_AsString(ob))
#		define PyString_AsStringAndSize(ob, charpp, sizep) \
			(PyUnicode_Check(ob) ? \
				(!(*(charpp) = PyUnicode_AsUTF8AndSize(ob, (sizep)))? -1 : 0) : \
				PyBytes_AsStringAndSize(ob, (charpp), (sizep)))
#	endif
#endif

#define TypyIntObject_INIT(O, N) do { \
	(void)PyObject_INIT((O), &PyInt_Type); \
	((PyIntObject*)(O))->ob_ival = (N);    \
} while (0)

#define TypyLongObject struct { \
	PyObject_VAR_HEAD           \
	digit ob_digit[5];          \
}

#define TypyLongObject_INIT(O, N) do { \
	(void)PyObject_INIT((O), &PyLong_Type);                                                     \
	register int i = 0;                                                                         \
	register unsigned PY_LONG_LONG t = (N) < 0 ? ((unsigned PY_LONG_LONG)(-1 - (N)) + 1) : (N); \
	while (t) {                                                                                 \
		((PyLongObject*)(O))->ob_digit[i++] = (digit)(t & PyLong_MASK);                         \
		t >>= PyLong_SHIFT;                                                                     \
	}                                                                                           \
	((PyLongObject*)(O))->ob_size = (N) < 0 ? -i : i;                                           \
} while (0)

#define TypyStringObject(S) struct { \
	PyObject_VAR_HEAD                \
	long ob_shash;                   \
	int  ob_sstate;                  \
	char ob_sval[sizeof(S)];         \
}

#define TypyStringObject_INIT(O, S) do { \
	(void)PyObject_INIT_VAR((O), &PyString_Type, strlen(S)); \
	((PyStringObject*)(O))->ob_shash = -1;                   \
	((PyStringObject*)(O))->ob_sstate = SSTATE_NOT_INTERNED; \
	strcpy(((PyStringObject*)(O))->ob_sval, (S));            \
} while (0)

#ifdef __cplusplus
extern "C" {
#endif

extern bool isDefaultEncodingUTF8;
extern PyObject* kPythonZero;
extern PyObject* kint32min_py;
extern PyObject* kint32max_py;
extern PyObject* kuint32max_py;
extern PyObject* kint64min_py;
extern PyObject* kint64max_py;
extern PyObject* kuint64max_py;
extern PyObject* k_t;

typedef PyBytesObject* PyBytes;
typedef PyUnicodeObject* PyString;

void    FormatTypeError(PyObject*, const char*);
PyBytes Typy_CheckBytes(PyObject*, const char*);

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
	struct _TypyComposite * prop_owner;
	size_t                  prop_flag;
} *TypyPropertyOwner;

typedef struct _TypyComposite {
	TypyComposite_HEAD
} TypyComposite;

bool TypyComposite_AddOwner(TypyComposite*, TypyComposite*, size_t);
void TypyComposite_DelOwner(TypyComposite*, TypyComposite*);

#else
#	define TypyComposite_HEAD PyObject_HEAD
#	define TypyComposite_FREE(ob)
#endif

#include "abstract.h"
#include "list.h"
#include "object.h"
#include "variant.h"
#include "enum.h"
#include "fixedpoint.h"
#include "extension.h"
#include "dict.h"

#ifdef __cplusplus
}
#endif

#endif // TYPY_TYPE_H__
