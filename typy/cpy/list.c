// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject* Typy_RegisterList(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyMetaList* type;
	PyObject* descriptor = Py_None;
	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &descriptor)) {
		return NULL;
	}

	type = (TypyMetaList*)malloc(sizeof(TypyMetaList) + nameLen);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register List: MetaList out of memory %d.", sizeof(TypyMetaList) + nameLen);
		return NULL;
	}

	type->list_name[nameLen] = 0;
	memcpy(type->list_name, name, nameLen);
	PyObject_INIT(type, &TypyMetaListType);
	/* todo: Typy_RegisterList */

	return (PyObject*)type;
}

static void MetaList_Dealloc(TypyMetaList* type) {
	free(type);
}

PyObject* TypyList_GetPyObject(TypyMetaList* type, TypyList** value) {
	register PyObject* list = (PyObject*)(*value);
	if (!list) {
		list = TypyList_New(type, NULL, NULL);
		*value = (TypyList*)list;
	}
	Py_INCREF(list);
	return list;
}

bool TypyList_Read(TypyMetaList* type, TypyList** value, byte** input, size_t* length) {
	TypyField item;
	register TypyList* list = *value;
	if (!list) {
		list = (TypyList*)TypyList_New(type, NULL, NULL);
		*value = list;
	}
	if (!TypyList_METHOD(list, Read, Typy_ARGS(&item, input, length))) {
		return false;
	}
	return TypyList_Append(list, item);
}

bool TypyList_ReadPacked(TypyMetaList* type, TypyList** value, byte** input, size_t* length) {
	uint32 i, size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return false;
	}
	register TypyList* list = *value;
	if (!list) {
		list = (TypyList*)TypyList_New(type, NULL, NULL);
		*value = list;
	} else {
		TypyList_Clear(list);
	}
	TypyList_EnsureSize(list, (size_t)size);
	for (i = 0; i < size; i ++) {
		if (!TypyList_METHOD(list, Read, Typy_ARGS(&list->list_items[i], input, length))) {
			return false;
		}
	}
	return true;
}

static void TypyList_Dealloc(TypyList* self) {
	TypyList_Clear(self);
	if (self->list_items) {
		free(self->list_items);
	}
	free(self);
}

static PyObject* TypyList_Repr(TypyMetaList* type) {
	return PyString_FromFormat("<List '" FULL_MODULE_NAME ".%s'>", type->list_name);
}

PyTypeObject TypyMetaListType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaList",            /* tp_name           */
	sizeof(TypyMetaList),                    /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)MetaList_Dealloc,            /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	(reprfunc)TypyList_Repr,                 /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	PyObject_HashNotImplemented,             /* tp_hash           */
	0,                                       /* tp_call           */
	(reprfunc)TypyList_Repr,                 /* tp_str            */
	0,                                       /* tp_getattro       */
	0,                                       /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy List Metaclass",               /* tp_doc            */
};

PyTypeObject TypyListType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".List",                 /* tp_name           */
	sizeof(TypyList),                         /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)TypyList_Dealloc,             /* tp_dealloc        */
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
	"A Typy List",                            /* tp_doc            */
};
