// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject* Typy_RegisterVariant(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyMetaObject* type;
	PyObject* attrs = Py_None;
	size_t meta_size = 1;
	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &attrs)) {
		return NULL;
	}

	register size_t size = sizeof(TypyMetaObject) + sizeof(TypyDescriptor) * meta_size + nameLen;
	type = (TypyMetaObject*)malloc(size);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Variant: MetaObject out of memory %d.", size);
		return NULL;
	}

	type->py_type = NULL;
	type->meta_new = NULL;
	type->meta_size = meta_size;
	Meta_NAME(type)[nameLen] = 0;
	memcpy(Meta_NAME(type), name, nameLen);
	PyObject_INIT(type, &TypyMetaVariantType);
	/* todo: Typy_RegisterVariant */
	type->meta_index2field = (char**)malloc(meta_size * sizeof(char*));
	if (!type->meta_index2field) {
		free(type);
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Variant: index2field out of memory %d.", meta_size * sizeof(char*));
		return NULL;
	}
	type->meta_field2index = TypyFieldMap_New();
	if (!type->meta_field2index) {
		free(type->meta_index2field);
		free(type);
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Variant: field2index out of memory.");
		return NULL;
	}

	return (PyObject*)type;
}

PyObject* Variant_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	PyObject* variant = (PyObject*)calloc(1, sizeof(TypyVariant));
	PyObject_INIT(variant, &TypyVariantType);
	((TypyVariant*)variant)->variant_index = -1;
	return variant;
}

PyObject* Variant_toPyObject(TypyVariant* self) {
	/* todo: Variant_toPyObject */
	Py_RETURN_NONE;
}

bool Variant_fromPyObject(TypyVariant* self, PyObject* from) {
	/* todo: Variant_fromPyObject */
	return false;
}

void Variant_Dealloc(TypyVariant* self) {
	Variant_Clear(self);
	free(self);
}

static PyObject* Variant_Repr(TypyMetaObject* type) {
	return PyString_FromFormat("<Variant '" FULL_MODULE_NAME ".%s'>", Meta_NAME(type));
}

PyTypeObject TypyMetaVariantType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaVariant",         /* tp_name           */
	0,                                       /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)TypyMeta_Dealloc,            /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	(reprfunc)Variant_Repr,                  /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	PyObject_HashNotImplemented,             /* tp_hash           */
	(ternaryfunc)Variant_New,                /* tp_call           */
	(reprfunc)Variant_Repr,                  /* tp_str            */
	0,                                       /* tp_getattro       */
	0,                                       /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy Variant Metaclass",            /* tp_doc            */
};

PyTypeObject TypyVariantType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Variant",              /* tp_name           */
	sizeof(TypyVariant),                      /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)Variant_Dealloc,              /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	0,                                        /* tp_compare        */
	0,                                        /* tp_repr           */
	0,                                        /* tp_as_number      */
	0,                                        /* tp_as_sequence    */
	0,                                        /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	0,                                        /* tp_call           */
	0,                                        /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy Variant",                         /* tp_doc            */
};
