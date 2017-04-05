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
	PyObject* key_desc;
	PyObject* value_desc;
	PyObject* typy_type;
	byte wire_type, field_type;
	if (!PyArg_ParseTuple(args, "s#OO", &name, &nameLen, &key_desc, &value_desc)) {
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

	typy_type = NULL;
	if (!PyArg_ParseTuple(key_desc, "BB|O", &wire_type, &field_type, &typy_type)) {
		free(type); return NULL;
	}
	type->key_desc.desc_type      = typy_type;
	type->key_desc.desc_FieldType = field_type;
	type->key_desc.desc_WireType  = wire_type;

	typy_type = NULL;
	if (!PyArg_ParseTuple(value_desc, "BB|O", &wire_type, &field_type, &typy_type)) {
		free(type); return NULL;
	}
	type->value_desc.desc_type      = typy_type;
	type->value_desc.desc_FieldType = field_type;
	type->value_desc.desc_WireType  = wire_type;

	return (PyObject*)type;
}

static void MetaDict_Dealloc(TypyMetaDict* type) {
	free(type);
}

#define TypyDict_FromValueOrNew(s, v, t, r) \
	register TypyDict* s = *(v);                    \
	if (!s) {                                       \
		s = (TypyDict*)TypyDict_New(t, NULL, NULL); \
		if (!s) { return r; }                   \
		*(v) = s;                                   \
	}

PyObject* TypyDict_GetPyObject(TypyMetaDict* type, TypyDict** value) {
	TypyDict_FromValueOrNew(self, value, type, NULL);
	Py_INCREF(self);
	return (PyObject*)self;
}

bool TypyDict_CheckAndSet(TypyMetaDict* type, TypyDict** value, PyObject* arg, const char* err) {
	PyObject* items;
	if (!arg || arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	} else if (PyObject_TypeCheck(arg, &TypyDictType) && TypyDict_TYPE(arg) == type) {
		Py_XDECREF(*value);
		Py_INCREF(arg);
		*value = (TypyDict*)arg;
		return true;
	} else if (PyDict_Check(arg)) {
		TypyDict_FromValueOrNew(self, value, type, false);
		return TypyDict_CheckAndSetDict(type, self, arg);
	} else if ((items = PyObject_CallMethod(arg, "iteritems", NULL)) != NULL) {
		TypyDict_FromValueOrNew(self, value, type, false);
		register bool success = TypyDict_CheckAndSetItems(type, self, items);
		Py_DECREF(items);
		return success;
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

bool TypyDict_Read(TypyMetaDict* type, TypyDict** dict, byte** input, size_t* length) {
	size_t remain;
	uint32 tag, limit;
	TypyField key = 0, value = 0;
	if (!Typy_ReadVarint32(input, length, &limit)) {
		return false;
	} else if (limit > *length) {
		return false;
	}
	remain = limit;
	for (;;) {
		if (!Typy_ReadTag(input, &remain, &tag, 0x7F)) {
			goto handle_unusual;
		}
		register int index = TAG_INDEX(tag);
		if (index < 0 || index > 1) { goto handle_unusual; }
		if (TAG_WIRETYPE(tag) == MetaDict_WIRETYPE(type, index)) {
			if (!MetaDict_READ(type, index, index ? &value : &key, input, &remain)) {
				return false;
			}
		} else if (TAG_WIRETYPE(tag) == WIRETYPE_LENGTH_DELIMITED) {
			if (!Typy_ReadPacked(MetaDict_TYPYTYPE(type, index), index ? &value : &key, input, &remain)) {
				return false;
			}
		}
	handle_unusual:
		if (tag == 0) { break; }
		if (!Typy_SkipField(input, &remain, tag)) { return false; }
	}

	TypyDict_FromValueOrNew(self, dict, type, false);
	register TypyDictMap item = (TypyDictMap)IblMap_Set(self->dict_map, &key);
	item->value = value;
	*input += remain;
	*length -= limit;
	return remain == 0;
}

void TypyDict_MergeFrom(TypyMetaDict* type, TypyDict** lvalue, TypyDict* rvalue) {
	if (!rvalue) { return; }
	TypyDict_FromValueOrNew(self, lvalue, type, );
	register IblMap_Item iter;
	for (iter = IblMap_Begin(rvalue->dict_map); iter; iter = IblMap_Next(rvalue->dict_map, iter)) {
		register TypyDictMap ritem = (TypyDictMap)iter;
		register TypyDictMap litem = (TypyDictMap)IblMap_Set(self->dict_map, &ritem->key);
		MetaDict_MERGEFROM(type, &litem->value, ritem->value);
	}
}

size_t TypyDict_Write(TypyMetaDict* type, TypyDict** value, int tag, byte* output) {
	register TypyDict* self = *value;
	if (!self) { return 0; }
	register size_t size = 0;
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		size += Typy_WriteTag(output, tag);
		register TypyDictMap item = (TypyDictMap)iter;
		register size_t entry_size = MetaKey_BYTESIZE(type, &item->key);
		entry_size += MetaValue_BYTESIZE(type, &item->value);
		size += IblPutUvarint(output + size, entry_size);
		size += MetaKey_WRITE(type, &item->key, output);
		size += MetaValue_WRITE(type, &item->value, output);
	}
	return size;
}

size_t TypyDict_ByteSize(TypyMetaDict* type, TypyDict** value, int tagsize) {
	register TypyDict* self = *value;
	if (!self) { return 0; }
	register size_t size = 0;
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		register TypyDictMap item = (TypyDictMap)iter;
		register size_t entry_size = MetaKey_BYTESIZE(type, &item->key);
		entry_size += MetaValue_BYTESIZE(type, &item->value);
		size += tagsize + IblSizeVarint(entry_size) + entry_size;
	}
	return size;
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
