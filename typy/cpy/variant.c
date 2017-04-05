// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject* Typy_RegisterVariant(PyObject* m, PyObject* args) {
	register TypyMetaObject* type = _Typy_RegisterMeta(args);
	type->meta_new = NULL;
	type->py_type = NULL;
	return (PyObject*)type;
}

PyObject* TypyVariant_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	TypyVariant* variant = (TypyVariant*)calloc(1, sizeof(TypyVariant));
	if (!variant) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Alloc Variant: out of memory %d.", sizeof(TypyVariant));
		return NULL;
	}
	PyObject_INIT(variant, &TypyVariantType);
	variant->meta_type = type;
	variant->variant_index = -1;
	return (PyObject*)variant;
}

PyObject* TypyVariant_GetPyObject(TypyMetaObject* type, TypyVariant** value) {
	/* todo: TypyVariant_GetPyObject */
	Py_RETURN_NONE;
}

#define TypyVariant_FromValueOrNew(s, v, t, r) \
	register TypyVariant* s = *(v);                       \
	if (!s) {                                             \
		s = (TypyVariant*)TypyVariant_New(t, NULL, NULL); \
		if (!s) { return r; }                             \
		*(v) = s;                                         \
	}

bool TypyVariant_CheckAndSet(TypyMetaObject* type, TypyVariant** value, PyObject* arg, const char* err) {
	if (arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	}
	TypyVariant_FromValueOrNew(self, value, type, false);
	/* todo: TypyVariant_CheckAndSet */
	return true;
}

size_t TypyVariant_ByteSize(TypyMetaObject* type, TypyVariant** value, int tagsize) {
	register TypyVariant* self = *value;
	register size_t size = 0;
	if (self) {
		register int i = self->variant_index;
		if (i >= 0 && (size_t)i < Typy_SIZE(self)) {
			size = Typy_BYTESIZE(self, i, Typy_TAGSIZE(self, i));
		}
		self->variant_size = size;
	}
	return tagsize + IblSizeVarint(size) + size;
}

size_t TypyVariant_Write(TypyMetaObject* type, TypyVariant** value, int tag, byte* output) {
	register TypyVariant* self = *value;
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	size += IblPutUvarint(output + size, self->variant_size);
	register int i = self->variant_index;
	if (i >= 0 && (size_t)i < Typy_SIZE(self)) {
		size += Typy_WRITE(self, i, Typy_TAG(self, i), output + size);
	}
	return size;
}

bool TypyVariant_Read(TypyMetaObject* type, TypyVariant** value, byte** input, size_t* length) {
	uint32 tag, limit;
	size_t remain;
	if (!Typy_ReadVarint32(input, length, &limit)) {
		return false;
	} else if (limit > *length) {
		return false;
	}
	remain = limit;
	TypyVariant_FromValueOrNew(self, value, type, false);
	if (!Typy_ReadTag(input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
		return false;
	}
	register int index = TAG_INDEX(tag);
	if (index < 0 || (size_t)index >= Typy_SIZE(self)) { return false; }
	if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
		TypyVariant_Clear(self);
		if (!Typy_READ(self, index, input, &remain)) {
			return false;
		}
	} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
		TypyVariant_Clear(self);
		if (!Typy_ReadPacked(Typy_TYPYTYPE(self, index), &Typy_FIELD(self, index), input, &remain)) {
			return false;
		}
	}
	self->variant_index = index;
	*input += remain;
	*length -= limit;
	return remain == 0;
}

void TypyVariant_MergeFrom(TypyMetaObject* type, TypyVariant** lvalue, TypyVariant* rvalue) {
	if (!rvalue || rvalue->variant_index < 0 || (size_t)rvalue->variant_index >= Typy_SIZE(rvalue)) {
		return;
	}
	TypyVariant_FromValueOrNew(self, lvalue, type, );
	if (self->variant_index != rvalue->variant_index) {
		TypyVariant_Clear(self);
	}
	Typy_MERGEFROM(self, self->variant_index, Typy_FIELD(rvalue, self->variant_index));
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
