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

#define TypyList_FromValueOrNew(s, v, t) \
	register TypyList* s = *(v);                    \
	if (!s) {                                       \
		s = (TypyList*)TypyList_New(t, NULL, NULL); \
		if (!s) { return false; }                   \
		*(v) = s;                                   \
	}

PyObject* TypyList_GetPyObject(TypyMetaList* type, TypyList** value) {
	TypyList_FromValueOrNew(self, value, type);
	Py_INCREF(self);
	return (PyObject*)self;
}

bool TypyList_Read(TypyMetaList* type, TypyList** value, byte** input, size_t* length) {
	TypyList_FromValueOrNew(self, value, type);
	register TypyField* offset = TypyList_EnsureSize(self, 1);
	if (!offset) { return false; }
	if (!MetaList_READ(type, offset, input, length)) {
		return false;
	}
	return true;
}

static inline bool TypyList_ReadPacked(TypyMetaList* type, TypyList** value, byte** input, size_t* length) {
	uint32 size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return false;
	}
	TypyList_FromValueOrNew(self, value, type);
	register TypyField* offset;
	register byte* limit = *input + size;
	while (*input < limit) {
		if (!(offset = TypyList_EnsureSize(self, 1))) {
			return false;
		} else if (!MetaList_READ(type, offset, input, length)) {
			return false;
		}
	}
	return true;
}

bool Typy_ReadPacked(TypyType type, TypyField* value, byte** input, size_t* length) {
	return TypyList_ReadPacked((TypyMetaList*)type, (TypyList**)value, input, length);
}

size_t TypyList_Write(TypyMetaList* type, TypyList** value, int tag, byte* output) {
	register TypyList* self = *value;
	if (!self) { return 0; }
	register size_t i, size = 0;
	if (MetaList_IsPrimitive(type)) {
		if (tag) {
			size += Typy_WriteTag(output, tag);
		}
		size += IblPutUvarint(output + size, self->list_size);
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_WRITE(type, &self->list_items[i], 0, output);
		}
	} else {
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_WRITE(type, &self->list_items[i], tag, output);
		}
	}
	return size;
}

size_t TypyList_ByteSize(TypyMetaList* type, TypyList** value, int tagsize) {
	register TypyList* self = *value;
	if (!self) { return 0; }
	register size_t i, size = 0;
	if (MetaList_IsPrimitive(type)) {
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_BYTESIZE(type, &self->list_items[i], 0);
		}
		self->list_size = size;
		size += tagsize + IblSizeVarint(size);
	} else {
		for (i = 0; i < self->list_length; i++) {
			size += MetaList_BYTESIZE(type, &self->list_items[i], tagsize);
		}
	}
	return size;
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
