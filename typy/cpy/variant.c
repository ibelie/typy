// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MetaVariant_GET(m, s) \
	(abstract_GetPyObject [Meta_FIELDTYPE(m, (s)->variant_index)](Meta_TYPYTYPE(m, (s)->variant_index), &(s)->variant_value))
#define MetaVariant_TOJSON(m, s, l) \
	(abstract_ToJson      [Meta_FIELDTYPE(m, (s)->variant_index)](Meta_TYPYTYPE(m, (s)->variant_index), &(s)->variant_value, (l)))
#define MetaVariant_WRITE(m, s, o) \
	(abstract_Write       [Meta_FIELDTYPE(m, (s)->variant_index)](Meta_TYPYTYPE(m, (s)->variant_index), &(s)->variant_value, Meta_TAG(m, (s)->variant_index), (o)))
#define MetaVariant_BYTESIZE(m, s) \
	(abstract_ByteSize    [Meta_FIELDTYPE(m, (s)->variant_index)](Meta_TYPYTYPE(m, (s)->variant_index), &(s)->variant_value, Meta_TAGSIZE(m, (s)->variant_index)))
#define MetaVariant_MERGEFROM(m, s, r) \
	(abstract_MergeFrom   [Meta_FIELDTYPE(m, (r)->variant_index)](Meta_TYPYTYPE(m, (r)->variant_index), &(s)->variant_value, (r)->variant_value))
#define MetaVariant_READ(m, s, i, l) \
	(abstract_Read        [Meta_FIELDTYPE(m, (s)->variant_index)](Meta_TYPYTYPE(m, (s)->variant_index), &(s)->variant_value, (i), (l)))
#define MetaVariant_CHECKSET(m, s, r, e) \
	(abstract_CheckAndSet [Meta_FIELDTYPE(m, (s)->variant_index)](Meta_TYPYTYPE(m, (s)->variant_index), &(s)->variant_value, (r), (e)))
#define MetaVariant_FROMJSON(m, s, j) \
	(abstract_FromJson    [Meta_FIELDTYPE(m, (s)->variant_index)](Meta_TYPYTYPE(m, (s)->variant_index), &(s)->variant_value, (j)))

#define MetaVariant_DEL_OWNER(m, s) \
	if ((s)->variant_index >= 0 && (size_t)(s)->variant_index < Meta_SIZE(m) &&              \
		FIELD_TYPE_COMPOSITE(Meta_FIELDTYPE(m, (s)->variant_index)) && (s)->variant_value) { \
		TypyComposite_DelOwner((TypyComposite*)((s)->variant_value), (TypyComposite*)(s));   \
	}                                                                                        \
	TypyField_Clr(Meta_FIELDTYPE(m, (s)->variant_index), (s)->variant_value)

#define MetaVariant_ADD_OWNER(m, s) do { \
	if ((s)->variant_index >= 0 && (size_t)(s)->variant_index < Meta_SIZE(m) &&              \
		FIELD_TYPE_COMPOSITE(Meta_FIELDTYPE(m, (s)->variant_index)) && (s)->variant_value) { \
		TypyComposite_AddOwner((TypyComposite*)((s)->variant_value),                         \
			(TypyComposite*)(s), FIELD_TYPE_VARIANT, Meta_PROPFLAG(m, (s)->variant_index));  \
	}                                                                                        \
} while (0)

#define MetaVariant_CLEAR(m, s) do { \
	MetaVariant_DEL_OWNER((m), (s));                                                         \
	(s)->variant_index = -1;                                                                 \
	MetaVariant_ADD_OWNER((m), (s));                                                         \
} while (0)

//=============================================================================

TypyMetaObject* Typy_RegisterVariant(PyObject* m, PyObject* args) {
	register TypyMetaObject* type = _Typy_RegisterMeta(args);
	type->py_type = NULL;
	return type;
}

static TypyVariant* TypyVariant_New(TypyMetaObject* type) {
	TypyVariant* variant = (TypyVariant*)calloc(1, sizeof(TypyVariant));
	if (!variant) {
		PyErr_Format(PyExc_RuntimeError, "Alloc Variant out of memory %zu.", sizeof(TypyVariant));
		return NULL;
	}
	(void)PyObject_INIT(variant, &TypyVariantType);
	Py_INCREF(type);
	Typy_TYPE(variant) = type;
	variant->variant_index = -1;
	TypyComposite_INIT(variant);
	return variant;
}

static void TypyVariant_Dealloc(TypyVariant* self) {
	TypyComposite_FREE(self);
	MetaVariant_CLEAR(Typy_TYPE(self), self);
	Py_DECREF(Typy_TYPE(self));
	free(self);
}

static PyObject* TypyVariant_Repr(TypyMetaObject* type) {
	return PyString_FromFormat("<Variant '" FULL_MODULE_NAME ".%.100s'>", Meta_NAME(type));
}

//=============================================================================

#define TypyVariant_FromValueOrNew(s, v, t, r) \
	register TypyVariant* s = *(v);            \
	if (!s) {                                  \
		s = TypyVariant_New(t);  \
		if (!s) { return r; }                  \
		*(v) = s;                              \
	}

PyObject* TypyVariant_GetPyObject(TypyMetaObject* type, TypyVariant** value) {
	register TypyVariant* self = *value;
	if (!self) { Py_RETURN_NONE; }
	if (self->variant_index >= 0 && (size_t)self->variant_index < Meta_SIZE(type)) {
		return MetaVariant_GET(type, self);
	}
	Py_RETURN_NONE;
}

bool TypyVariant_CheckAndSet(TypyMetaObject* type, TypyVariant** value, PyObject* arg, const char* err) {
	if (!arg || arg == Py_None) {
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
	MetaVariant_DEL_OWNER(type, self);
	self->variant_index = index;
	if (MetaVariant_CHECKSET(type, self, arg, "SetVariant ")) {
		MetaVariant_ADD_OWNER(type, self);
		return true;
	} else {
		self->variant_index = -1;
		return false;
	}
}

size_t TypyVariant_ByteSize(TypyMetaObject* type, TypyVariant** value, int tagsize) {
	register TypyVariant* self = *value;
	register size_t size = 0;
	if (self) {
		if (self->variant_index >= 0 && (size_t)self->variant_index < Meta_SIZE(type)) {
			size = MetaVariant_BYTESIZE(type, self);
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
	if (self) {
		size += IblPutUvarint(output + size, self->cached_size);
		if (self->variant_index >= 0 && (size_t)self->variant_index < Meta_SIZE(type)) {
			return size + MetaVariant_WRITE(type, self, output + size);
		}
	}
	output[size] = 0;
	return size + 1;
}

bool TypyVariant_Read(TypyMetaObject* type, TypyVariant** value, byte** input, size_t* length) {
	uint32 tag, limit;
	size_t remain;
	if (!Typy_ReadVarint32(input, length, &limit)) {
		return false;
	} else if (!limit) {
		return true;
	} else if (limit > *length) {
		return false;
	}
	remain = limit;
	TypyVariant_FromValueOrNew(self, value, type, false);

	for (;;) {
		if (!Typy_ReadTag(input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
			goto handle_unusual;
		}
		register int index = TAG_INDEX(tag);
		if (index < 0 || (size_t)index >= Meta_SIZE(type)) { goto handle_unusual; }
		if (TAG_WIRETYPE(tag) == Meta_WIRETYPE(type, index)) {
			MetaVariant_DEL_OWNER(type, self);
			self->variant_index = index;
			if (!MetaVariant_READ(type, self, input, &remain)) {
				self->variant_index = -1;
				goto handle_unusual;
			}
			MetaVariant_ADD_OWNER(type, self);
		} else if (Meta_FIELDTYPE(type, index) == FIELD_TYPE_LIST &&
			TAG_WIRETYPE(tag) == MetaList_WIRETYPE(Meta_TYPYTYPE(type, index))) {
			if (self->variant_index != index) {
				MetaVariant_CLEAR(type, self);
			}
			if (!TypyList_ReadRepeated(Meta_TYPYTYPE(type, index), (TypyList**)&self->variant_value, input, &remain)) {
				goto handle_unusual;
			}
		}
		self->variant_index = index;

		if (!remain) {
			break;
		} else {
			continue;
		}

	handle_unusual:
		if (tag == 0) { break; }
		if (!Typy_SkipField(input, &remain, tag)) { return false; }
	}
	*input += remain;
	*length -= limit;
	return remain == 0;
}

void TypyVariant_MergeFrom(TypyMetaObject* type, TypyVariant** lvalue, TypyVariant* rvalue) {
	if (!rvalue || rvalue->variant_index < 0 || (size_t)rvalue->variant_index >= Meta_SIZE(type)) {
		return;
	}
	TypyVariant_FromValueOrNew(self, lvalue, type, );
	MetaVariant_DEL_OWNER(type, self);
	MetaVariant_MERGEFROM(type, self, rvalue);
	self->variant_index = rvalue->variant_index;
	MetaVariant_ADD_OWNER(type, self);
}

PyObject* TypyVariant_ToJson(TypyMetaObject* type, TypyVariant** value, bool slim) {
	register TypyVariant* self = *value;
	if (!slim && !self) {
		Py_RETURN_NONE;
	} else if (self) {
		if (self->variant_index >= 0 && (size_t)self->variant_index < Meta_SIZE(type)) {
			return MetaVariant_TOJSON(type, self, slim);
		}
		Py_RETURN_NONE;
	} else {
		return NULL;
	}
}

bool TypyVariant_FromJson(TypyMetaObject* type, TypyVariant** value, PyObject* json) {
	register int index = -1;
	if (!json || json == Py_None) { return true; }
	TypyVariant_FromValueOrNew(self, value, type, false);
	if (PyObject_HasAttrString(json, "iteritems")) {
		register PyObject* _t = PyObject_GetItem(json, k_t);
		PyErr_Clear();
		if (!_t) {
		} else if (PyUnicode_Check(_t)) {
			register PyObject* _b = PyUnicode_AsEncodedObject(_t, "utf-8", NULL);
			Py_DECREF(_t);
			_t = _b;
		} else if (!PyBytes_Check(_t)) {
			Py_DECREF(_t);
			_t = NULL;
		}
		if (_t && (index = Meta_PropertyIndex(type, PyBytes_AS_STRING(_t))) >= 0) {
		} else if ((index = Meta_PropertyIndex(type, "Dict")) >= 0) {
		} else {
			FormatTypeError(json, "FromJson no suitable type, ");
			Py_XDECREF(_t);
			return false;
		}
		Py_XDECREF(_t);
	} else if (PyObject_HasAttrString(json, "__iter__") && (index = Meta_PropertyIndex(type, "List")) >= 0) {
	} else {
		return TypyVariant_CheckAndSet(type, value, json, "FromJson CheckAndSet, ");
	}
	MetaVariant_DEL_OWNER(type, self);
	self->variant_index = index;
	if (MetaVariant_FROMJSON(type, self, json)) {
		MetaVariant_ADD_OWNER(type, self);
		return true;
	} else {
		self->variant_index = -1;
		return false;
	}
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

#ifdef __cplusplus
}
#endif
