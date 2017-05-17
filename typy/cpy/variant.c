// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MetaVariant_GET(m, s, i) \
	(abstract_GetPyObject [Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value))
#define MetaVariant_TOJSON(m, s, i, l) \
	(abstract_ToJson      [Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (l)))
#define MetaVariant_WRITE(m, s, i, t, o) \
	(abstract_Write       [Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (t), (o)))
#define MetaVariant_BYTESIZE(m, s, i, t) \
	(abstract_ByteSize    [Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (t)))

#define MetaVariant_DEL_OWNER(m, s, i) \
	TypyComposite_DEL_OWNER(Meta_FIELDTYPE(m, i), (s)->variant_value, (s))
#define MetaVariant_ADD_OWNER(m, s, i) \
	TypyComposite_ADD_OWNER(Meta_FIELDTYPE(m, i), (s)->variant_value, (s), FIELD_TYPE_VARIANT, Meta_PROPFLAG(m, i))
#define MetaVariant_COPY_OLD(m, s, i) \
	TypyComposite_COPY_OLD(Meta_FIELDTYPE(m, i), (s)->variant_value)
#define MetaVariant_NOTIFY(m, s, i) \
	TypyComposite_NOTIFY((s), FIELD_TYPE_VARIANT, Meta_PROPFLAG(m, i), Meta_FIELDTYPE(m, i), Meta_TYPYTYPE(m, i), old, (s)->variant_value)

#define MetaVariant_MERGEFROM(m, s, i, f) do { \
	MetaVariant_COPY_OLD((m), (s), (i));                                                     \
	MetaVariant_DEL_OWNER((m), (s), (i));                                                    \
	abstract_MergeFrom[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (f)); \
	MetaVariant_ADD_OWNER((m), (s), (i));                                                    \
	MetaVariant_NOTIFY((m), (s), (i));                                                       \
} while (0)

#define MetaVariant_CLEAR(m, s, i) do { \
	MetaVariant_COPY_OLD((m), (s), (i));                                                     \
	MetaVariant_DEL_OWNER((m), (s), (i));                                                    \
	abstract_Clear[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value);          \
	MetaVariant_NOTIFY((m), (s), (i));                                                       \
} while (0)

#define MetaVariant_SET(m, s, i, f) do { \
	MetaVariant_COPY_OLD((m), (s), (i));                                                     \
	MetaVariant_DEL_OWNER((m), (s), (i));                                                    \
	abstract_CopyFrom[Meta_FIELDTYPE(m, i)](Meta_TYPYTYPE(m, i), &(s)->variant_value, (f));  \
	MetaVariant_ADD_OWNER((m), (s), (i));                                                    \
	MetaVariant_NOTIFY((m), (s), (i));                                                       \
} while (0)

#define MetaVariant_Clear(m, ob) { \
	if ((ob)->variant_index >= 0 && (size_t)(ob)->variant_index < Meta_SIZE(m)) { \
		MetaVariant_CLEAR((m), (ob), (ob)->variant_index);                        \
		(ob)->variant_index = -1;                                                 \
	}                                                                             \
}

static inline bool MetaVariant_READ(TypyMetaObject* type, TypyVariant* self, size_t index, byte** input, size_t* length) {
	MetaVariant_COPY_OLD(type, self, index);
	MetaVariant_DEL_OWNER(type, self, index);
	register bool result = abstract_Read[Meta_FIELDTYPE(type, index)](Meta_TYPYTYPE(type, index), &self->variant_value, input, length);
	if (result) {
		result = MetaVariant_ADD_OWNER(type, self, index);
		MetaVariant_NOTIFY(type, self, index);
	}
	return result;
}

static inline bool MetaVariant_CHECKSET(TypyMetaObject* type, TypyVariant* self, size_t index, PyObject* arg, const char* err) {
	MetaVariant_COPY_OLD(type, self, index);
	MetaVariant_DEL_OWNER(type, self, index);
	register bool result = abstract_CheckAndSet[Meta_FIELDTYPE(type, index)](Meta_TYPYTYPE(type, index), &self->variant_value, arg, err);
	if (result) {
		result = MetaVariant_ADD_OWNER(type, self, index);
		MetaVariant_NOTIFY(type, self, index);
	}
	return result;
}

static inline bool MetaVariant_FROMJSON(TypyMetaObject* type, TypyVariant* self, size_t index, PyObject* json) {
	MetaVariant_COPY_OLD(type, self, index);
	MetaVariant_DEL_OWNER(type, self, index);
	register bool result = abstract_FromJson[Meta_FIELDTYPE(type, index)](Meta_TYPYTYPE(type, index), &self->variant_value, json);
	if (result) {
		result = MetaVariant_ADD_OWNER(type, self, index);
		MetaVariant_NOTIFY(type, self, index);
	}
	return result;
}

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
	variant->composite_active = true;
	return variant;
}

static void TypyVariant_Dealloc(TypyVariant* self) {
	TypyComposite_FREE(self);
	MetaVariant_Clear(Typy_TYPE(self), self);
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
	register int i = self->variant_index;
	if (i >= 0 && (size_t)i < Meta_SIZE(type)) {
		return MetaVariant_GET(type, self, i);
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
	if (self->variant_index >= 0 && (size_t)self->variant_index < Meta_SIZE(type)) {
		MetaVariant_CLEAR(type, self, self->variant_index);
	}
	register bool success = MetaVariant_CHECKSET(type, self, index, arg, "SetVariant ");
	if (success) {
		self->variant_index = index;
	} else {
		self->variant_index = -1;
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
	if (self) {
		size += IblPutUvarint(output + size, self->cached_size);
		register int i = self->variant_index;
		if (i >= 0 && (size_t)i < Meta_SIZE(type)) {
			return size + MetaVariant_WRITE(type, self, i, Meta_TAG(type, i), output + size);
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
			if (self->variant_index != index) {
				MetaVariant_Clear(type, self);
			}
			if (!MetaVariant_READ(type, self, index, input, &remain)) {
				goto handle_unusual;
			}
		} else if (Meta_FIELDTYPE(type, index) == FIELD_TYPE_LIST &&
			TAG_WIRETYPE(tag) == MetaList_WIRETYPE(Meta_TYPYTYPE(type, index))) {
			if (self->variant_index != index) {
				MetaVariant_Clear(type, self);
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
	if (self->variant_index != rvalue->variant_index) {
		MetaVariant_Clear(type, self);
	}
	MetaVariant_MERGEFROM(type, self, self->variant_index, rvalue->variant_value);
}

PyObject* TypyVariant_ToJson(TypyMetaObject* type, TypyVariant** value, bool slim) {
	register TypyVariant* self = *value;
	if (!slim && !self) {
		Py_RETURN_NONE;
	} else if (self) {
		register int i = self->variant_index;
		if (i >= 0 && (size_t)i < Meta_SIZE(type)) {
			return MetaVariant_TOJSON(type, self, i, slim);
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
	if (self->variant_index >= 0 && (size_t)self->variant_index < Meta_SIZE(type)) {
		MetaVariant_CLEAR(type, self, self->variant_index);
	}
	register bool success = MetaVariant_FROMJSON(type, self, index, json);
	if (success) {
		self->variant_index = index;
	} else {
		self->variant_index = -1;
	}
	return success;
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
