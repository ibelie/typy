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
	PyObject* descriptors = Py_None;
	size_t meta_size = 1;
	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &descriptors)) {
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

PyObject* TypyVariant_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	PyObject* variant = (PyObject*)calloc(1, sizeof(TypyVariant));
	if (!variant) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Alloc Variant: out of memory %d.", sizeof(TypyVariant));
		return NULL;
	}
	PyObject_INIT(variant, &TypyVariantType);
	((TypyVariant*)variant)->variant_index = -1;
	return variant;
}

PyObject* TypyVariant_toPyObject(TypyVariant* self) {
	/* todo: TypyVariant_toPyObject */
	Py_RETURN_NONE;
}

bool TypyVariant_fromPyObject(TypyVariant* self, PyObject* from) {
	/* todo: TypyVariant_fromPyObject */
	return false;
}

size_t TypyVariant_ByteSize(TypyMetaObject* type, TypyVariant** value, int tagsize) {
	register TypyVariant* self = *value;
	register int i = self->variant_index;
	if (i >= 0 && (size_t)i < Typy_SIZE(self)) {
		register size_t size = Typy_METHOD(self, i, ByteSize, Typy_TAGSIZE(self, i));
		self->variant_size = size;
		return tagsize + IblSizeVarint(size) + size;
	}
	self->variant_size = 0;
	return 0;
}

size_t TypyVariant_Write(TypyMetaObject* type, TypyVariant** value, int tag, byte* output) {
	register TypyVariant* self = *value;
	if (self->variant_size <= 0) { return 0;}
	register int i = self->variant_index;
	if (i < 0 || (size_t)i >= Typy_SIZE(self)) { return 0; }
	register size_t size = Typy_WriteTag(output, tag);
	size += IblPutUvarint(output + size, self->variant_size);
	size += Typy_METHOD(self, i, Write, Typy_ARGS(Typy_TAG(self, i), output + size));
	return size;
}

bool TypyVariant_Read(TypyMetaObject* type, TypyVariant** value, byte** input, size_t* length) {
	uint32 tag;
	register TypyVariant* self = *value;
	if (!self) {
		self = (TypyVariant*)TypyVariant_New(type, NULL, NULL);
		*value = self;
	}
	if (!Typy_ReadTag(input, length, &tag, Typy_TYPE(self)->meta_cutoff)) {
		return false;
	}
	register int index = TAG_INDEX(tag);
	if (index < 0 || (size_t)index >= Typy_SIZE(self)) { return false; }
	if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
		TypyVariant_Clear(self);
		if (!Typy_METHOD(self, index, Read, Typy_ARGS(input, length))) {
			return false;
		}
	} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
		TypyVariant_Clear(self);
		if (!Typy_METHOD(self, index, ReadPacked, Typy_ARGS(input, length))) {
			return false;
		}
	}
	self->variant_index = index;
	return true;
}

void TypyVariant_MergeFrom(TypyMetaObject* type, TypyVariant** lvalue, TypyVariant* rvalue) {
	if (!rvalue || rvalue->variant_index < 0 || (size_t)rvalue->variant_index >= Typy_SIZE(rvalue)) {
		return;
	}
	register TypyVariant* self = *lvalue;
	if (!self) {
		self = (TypyVariant*)TypyVariant_New(type, NULL, NULL);
		*lvalue = self;
	}
	if (self->variant_index != rvalue->variant_index) {
		TypyVariant_Clear(self);
	}
	Typy_METHOD(self, self->variant_index, MergeFrom, Typy_FIELD(rvalue, self->variant_index));
}

static void TypyVariant_Dealloc(TypyVariant* self) {
	TypyVariant_Clear(self);
	free(self);
}

static PyObject* TypyVariant_Repr(TypyMetaObject* type) {
	return PyString_FromFormat("<Variant '" FULL_MODULE_NAME ".%s'>", Meta_NAME(type));
}

PyTypeObject TypyMetaVariantType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaVariant",         /* tp_name           */
	sizeof(TypyMetaObject),                  /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)TypyMeta_Dealloc,            /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	(reprfunc)TypyVariant_Repr,              /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	PyObject_HashNotImplemented,             /* tp_hash           */
	(ternaryfunc)TypyVariant_New,            /* tp_call           */
	(reprfunc)TypyVariant_Repr,              /* tp_str            */
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
	(destructor)TypyVariant_Dealloc,          /* tp_dealloc        */
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
