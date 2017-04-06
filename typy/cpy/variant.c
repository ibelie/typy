// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

TypyMetaObject* Typy_RegisterVariant(PyObject* m, PyObject* args) {
	register TypyMetaObject* type = _Typy_RegisterMeta(args);
	type->meta_new = NULL;
	type->py_type = NULL;
	return type;
}

TypyVariant* TypyVariant_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	TypyVariant* variant = (TypyVariant*)calloc(1, sizeof(TypyVariant));
	if (!variant) {
		PyErr_Format(PyExc_RuntimeError, "Alloc Variant out of memory %d.", sizeof(TypyVariant));
		return NULL;
	}
	PyObject_INIT(variant, &TypyVariantType);
	Typy_TYPE(variant) = type;
	variant->variant_index = -1;
	return variant;
}

PyObject* TypyVariant_GetPyObject(TypyMetaObject* type, TypyVariant** value) {
	register TypyVariant* self = *value;
	if (!self) { Py_RETURN_NONE; }
	register int i = self->variant_index;
	if (i >= 0 && (size_t)i < Meta_SIZE(type)) {
		return MetaVariant_GET(type, self, i);
	}
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

	register int index;
	if (PyBool_Check(arg)) {
		if ((index = Meta_PropertyIndex(type, "Boolean")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Integer")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Enum")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Double")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Float")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "FixedPoint")) >= 0) {
		} else {
			PyErr_Format(PyExc_TypeError, "SetVariant no suitable type for Boolean.");
			return false;
		}
	} else if (PyInt_Check(arg) || PyLong_Check(arg)) {
		if ((index = Meta_PropertyIndex(type, "Integer")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Enum")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Boolean")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Double")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Float")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "FixedPoint")) >= 0) {
		} else {
			PyErr_Format(PyExc_TypeError, "SetVariant no suitable type for Integer.");
			return false;
		}
	} else if (PyFloat_Check(arg)) {
		if ((index = Meta_PropertyIndex(type, "Double")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Float")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "FixedPoint")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Integer")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Enum")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Boolean")) >= 0) {
		} else {
			PyErr_Format(PyExc_TypeError, "SetVariant no suitable type for Float.");
			return false;
		}
	} else if (PyUnicode_Check(arg)) {
		if ((index = Meta_PropertyIndex(type, "String")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "Bytes")) >= 0) {
		} else {
			PyErr_Format(PyExc_TypeError, "SetVariant no suitable type for String.");
			return false;
		}
	} else if (PyBytes_Check(arg)) {
		if ((index = Meta_PropertyIndex(type, "Bytes")) >= 0) {} else
		if ((index = Meta_PropertyIndex(type, "String")) >= 0) {
		} else {
			PyErr_Format(PyExc_TypeError, "SetVariant no suitable type for Bytes.");
			return false;
		}
	} else if (PyObject_HasAttrString(arg, "iteritems") && (index = Meta_PropertyIndex(type, "Dict")) >= 0) {
	} else if (PySequence_Check(arg) && (index = Meta_PropertyIndex(type, "List")) >= 0) {
	} else if (PyObject_TypeCheck(arg, TypyObjectType)) {
		if ((index = Meta_PropertyIndex(type, Typy_NAME(arg))) < 0) {
			FormatTypeError(arg, "SetVariant no suitable type for Typy Object, ");
			return false;
		}
	} else if ((index = Meta_PropertyIndex(type, (char*)Py_TYPE(arg)->tp_name)) < 0) {
		FormatTypeError(arg, "SetVariant no suitable type for Python Object, ");
		return false;
	}
	if (self->variant_index >= 0 && self->variant_index < Meta_SIZE(type)) {
		MetaVariant_CLEAR(type, self, self->variant_index);
	}
	register bool success = MetaVariant_CHECKSET(type, self, index, arg, "SetVariant ");
	if (success) {
		self->variant_index = index;
	}
	return success;
}

size_t TypyVariant_ByteSize(TypyMetaObject* type, TypyVariant** value, int tagsize) {
	register TypyVariant* self = *value;
	register size_t size = 0;
	if (self) {
		register int i = self->variant_index;
		if (i >= 0 && (size_t)i < Meta_SIZE(type)) {
			size = MetaVariant_BYTESIZE(type, self, i, Meta_TAGSIZE(type, i));
		}
		self->cached_size = size;
	}
	return tagsize + IblSizeVarint(size) + size;
}

size_t TypyVariant_Write(TypyMetaObject* type, TypyVariant** value, int tag, byte* output) {
	register TypyVariant* self = *value;
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	size += IblPutUvarint(output + size, self->cached_size);
	register int i = self->variant_index;
	if (i >= 0 && (size_t)i < Meta_SIZE(type)) {
		size += MetaVariant_WRITE(type, self, i, Meta_TAG(type, i), output + size);
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
	if (index < 0 || (size_t)index >= Meta_SIZE(type)) { return false; }
	if (TAG_WIRETYPE(tag) == Meta_WIRETYPE(type, index)) {
		TypyVariant_Clear(self);
		if (!MetaVariant_READ(type, self, index, input, &remain)) {
			return false;
		}
	} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
		TypyVariant_Clear(self);
		if (!Typy_ReadPacked(Meta_TYPYTYPE(type, index), &self->variant_value, input, &remain)) {
			return false;
		}
	}
	self->variant_index = index;
	*input += remain;
	*length -= limit;
	return remain == 0;
}

void TypyVariant_MergeFrom(TypyMetaObject* type, TypyVariant** lvalue, TypyVariant* rvalue) {
	if (!rvalue || rvalue->variant_index < 0 || (size_t)rvalue->variant_index >= Meta_SIZE(type)) {
		return;
	}
	TypyVariant_FromValueOrNew(self, lvalue, type, );
	if (self->variant_index != rvalue->variant_index) {
		TypyVariant_Clear(self);
	}
	MetaVariant_MERGEFROM(type, self, self->variant_index, rvalue->variant_value);
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
