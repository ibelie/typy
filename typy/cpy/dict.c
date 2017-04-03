// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject* Typy_RegisterDict(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyMetaDict* type;
	PyObject* key_descriptor = Py_None;
	PyObject* value_descriptor = Py_None;
	if (!PyArg_ParseTuple(args, "s#OO", &name, &nameLen, &key_descriptor, &value_descriptor)) {
		return NULL;
	}

	type = (TypyMetaDict*)malloc(sizeof(TypyMetaDict) + nameLen);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register Dict: MetaDict out of memory %d.", sizeof(TypyMetaDict) + nameLen);
		return NULL;
	}

	type->dict_name[nameLen] = 0;
	memcpy(type->dict_name, name, nameLen);
	PyObject_INIT(type, &TypyMetaDictType);
	/* todo: Typy_RegisterDict */

	return (PyObject*)type;
}

static void MetaDict_Dealloc(TypyMetaDict* type) {
	free(type);
}

PyObject* TypyDict_GetPyObject(TypyMetaDict* type, TypyDict** value) {
	register PyObject* dict = (PyObject*)(*value);
	if (!dict) {
		dict = TypyDict_New(type, NULL, NULL);
		*value = (TypyDict*)dict;
	}
	Py_INCREF(dict);
	return dict;
}

static void TypyDict_Dealloc(TypyDict* self) {
	TypyDict_Clear(self);
	free(self);
}

static PyObject* TypyDict_Repr(TypyMetaDict* type) {
	return PyString_FromFormat("<Dict '" FULL_MODULE_NAME ".%s'>", type->dict_name);
}

PyTypeObject TypyMetaDictType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaDict",            /* tp_name           */
	sizeof(TypyMetaDict),                    /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)MetaDict_Dealloc,            /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	(reprfunc)TypyDict_Repr,                 /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	PyObject_HashNotImplemented,             /* tp_hash           */
	0,                                       /* tp_call           */
	(reprfunc)TypyDict_Repr,                 /* tp_str            */
	0,                                       /* tp_getattro       */
	0,                                       /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy Dict Metaclass",               /* tp_doc            */
};

PyTypeObject TypyDictType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Dict",                 /* tp_name           */
	sizeof(TypyDict),                         /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)TypyDict_Dealloc,             /* tp_dealloc        */
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
	"A Typy Dict",                            /* tp_doc            */
};
