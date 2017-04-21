// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TypyDictMap {
	IblMap_KEY(TypyField);
	TypyField value;
} *TypyDictMap;

static TypyDictMap _TypyDictMap_New(TypyField* key) {
	TypyDictMap item = (TypyDictMap)calloc(1, sizeof(struct _TypyDictMap));
	if (item) { item->key = *key; }
	return item;
}

static void _TypyDictMap_NumericFree(TypyDictMap item) {
	if (item) { free(item); }
}

static void _TypyDictMap_PyObjectFree(TypyDictMap item) {
	if (item) {
		Py_XDECREF(*((PyObject**)&item->key));
		free(item);
	}
}

static inline IblMap TypyDictMap_New(FieldType field_type) {
	IblMap map = (IblMap)calloc(1, sizeof(struct _IblMap));
	if (map) {
		map->hash    = (IblMap_Hash)abstract_Hash[field_type];
		map->alloc   = (IblMap_NewItem)_TypyDictMap_New;
		map->dealloc = (IblMap_Dealloc)(field_type < MAX_PRIMITIVE_TYPE ? _TypyDictMap_NumericFree : _TypyDictMap_PyObjectFree);
		map->compare = (IblMap_Compare)abstract_Compare[field_type];
	}
	return map;
}

//=============================================================================

TypyMetaDict* Typy_RegisterDict(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyMetaDict* type;
	PyObject* key_desc;
	PyObject* value_desc;
	PyObject* typy_type;
	uint8 wire_type, field_type;
	if (!PyArg_ParseTuple(args, "s#OO", &name, &nameLen, &key_desc, &value_desc)) {
		return NULL;
	}

	type = (TypyMetaDict*)malloc(IBL_ALIGNED_SIZE(sizeof(TypyMetaDict) + sizeof(char) * nameLen));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Register Dict out of memory %lu.", sizeof(TypyMetaDict) + nameLen);
		return NULL;
	}

	type->dict_name[nameLen] = 0;
	memcpy(type->dict_name, name, nameLen);
	(void)PyObject_INIT(type, &TypyMetaDictType);

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
	type->value_desc.desc_type      = Meta_FromInitializer;
	type->value_desc.desc_FieldType = field_type;
	type->value_desc.desc_WireType  = wire_type;

	return type;
}

static inline bool MetaDict_SetItem(TypyMetaDict* type, TypyDict* self, PyObject* key, PyObject* value) {
	TypyField k = 0;
	if (!MetaKey_CHECKSET(type, &k, key, "Dict key type error: ")) {
		return false;
	}
	register TypyDictMap entry = (TypyDictMap)IblMap_Set(self->dict_map, &k);
	if (!entry) { return false; }
	return MetaValue_CHECKSET(type, &entry->value, value, "Dict value type error: ");
}

static inline bool MetaDict_MergeDict(TypyMetaDict* type, TypyDict* self, PyObject* dict) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	while (PyDict_Next(dict, &pos, &k, &v)) {
		if (!MetaDict_SetItem(type, self, k, v)) {
			return false;
		}
	}
	return true;
}

static inline bool MetaDict_MergeIter(TypyMetaDict* type, TypyDict* self, PyObject* iter, Py_ssize_t size) {
	register Py_ssize_t i;
	for (i = 0; i < size; i++) {
		register PyObject* item = PyIter_Next(iter);
		if (!MetaDict_SetItem(type, self, PyTuple_GET_ITEM(item, 0), PyTuple_GET_ITEM(item, 1))) {
			return false;
		}
		Py_XDECREF(item);
	}
	return true;
}

static void MetaDict_Dealloc(TypyMetaDict* type) {
	free(type);
}

static TypyDict* TypyDict_New(TypyMetaDict* type) {
	TypyDict* dict = (TypyDict*)calloc(1, sizeof(TypyDict));
	if (!dict) {
		PyErr_Format(PyExc_RuntimeError, "Alloc Dict object out of memory %lu.", sizeof(TypyDict));
		return NULL;
	}
	dict->dict_map = TypyDictMap_New(MetaKey_FIELDTYPE(type));
	if (!dict->dict_map) {
		free(dict);
		PyErr_Format(PyExc_RuntimeError, "Alloc Dict map out of memory.");
		return NULL;
	}
	(void)PyObject_INIT(dict, &TypyDictType);
	TypyDict_TYPE(dict) = type;
	return dict;
}

static void TypyDict_Dealloc(TypyDict* self) {
	TypyDict_Clear(self);
	IblMap_Free(self->dict_map);
	free(self);
}

static PyObject* TypyDict_Repr(TypyMetaDict* type) {
	return PyString_FromFormat("<Dict '" FULL_MODULE_NAME ".%s'>", type->dict_name);
}

//=============================================================================

#define TypyDict_FromValueOrNew(s, v, t, r) \
	register TypyDict* s = *(v);            \
	if (!s) {                               \
		s = (TypyDict*)TypyDict_New(t);     \
		if (!s) { return r; }               \
		*(v) = s;                           \
	}

TypyDict* TypyDict_GetPyObject(TypyMetaDict* type, TypyDict** value) {
	TypyDict_FromValueOrNew(self, value, type, NULL);
	Py_INCREF(self);
	return self;
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
		MetaDict_Clear(type, self);
		return MetaDict_MergeDict(type, self, arg);
	} else if ((items = PyObject_CallMethod(arg, "iteritems", NULL))) {
		TypyDict_FromValueOrNew(self, value, type, false);
		MetaDict_Clear(type, self);
		register bool success = MetaDict_MergeIter(type, self, items, _PyObject_LengthHint(arg, 0));
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
	} else if (!limit) {
		return true;
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
		} else if (MetaDict_FIELDTYPE(type, index) == FIELD_TYPE_LIST &&
			TAG_WIRETYPE(tag) == MetaList_WIRETYPE(MetaDict_TYPYTYPE(type, index))) {
			if (!TypyList_ReadRepeated(MetaDict_TYPYTYPE(type, index), (TypyList**)(index ? &value : &key), input, &remain)) {
				return false;
			}
		}

		if (!remain) {
			break;
		} else {
			continue;
		}

	handle_unusual:
		if (tag == 0) { break; }
		if (!Typy_SkipField(input, &remain, tag)) { return false; }
	}

	TypyDict_FromValueOrNew(self, dict, type, false);
	register TypyDictMap item = (TypyDictMap)IblMap_Set(self->dict_map, &key);
	if (item) {
		MetaValue_CLEAR(type, &item->value);
		item->value = value;
	} else {
		MetaKey_CLEAR(type, &key);
		MetaValue_CLEAR(type, &value);
	}
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
		size += Typy_WriteTag(output + size, tag);
		register TypyDictMap item = (TypyDictMap)iter;
		register size_t entry_size = MetaKey_BYTESIZE(type, &item->key);
		entry_size += MetaValue_BYTESIZE(type, &item->value);
		size += IblPutUvarint(output + size, entry_size);
		size += MetaKey_WRITE(type, &item->key, output + size);
		size += MetaValue_WRITE(type, &item->value, output + size);
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

PyObject* TypyDict_ToJson(TypyMetaDict* type, TypyDict** value, bool slim) {
	if (!slim && !(*value)) {
		return PyDict_New();
	} else if (*value) {
		register PyObject* dict = PyDict_New();
		register IblMap_Item iter;
		for (iter = IblMap_Begin((*value)->dict_map); iter; iter = IblMap_Next((*value)->dict_map, iter)) {
			register TypyDictMap item = (TypyDictMap)iter;
			register PyObject* k = MetaKey_TOJSON(type, &item->key, slim);
			register PyObject* key = PyObject_Str(k);
			register PyObject* value = MetaValue_TOJSON(type, &item->value, slim);
			PyDict_SetItem(dict, key, value);
			Py_XDECREF(k);
			Py_XDECREF(key);
			Py_XDECREF(value);
		}
		return dict;
	} else {
		return NULL;
	}
}

bool TypyDict_FromJson(TypyMetaDict* type, TypyDict** dict, PyObject* json) {
	PyObject* item = NULL;
	TypyField key = 0, value = 0;
	TypyDict_FromValueOrNew(self, dict, type, false);
	if (!json || json == Py_None) { return true; }
	PyObject* iter = PyObject_CallMethod(json, "iteritems", NULL);
	if (!iter) {
		FormatTypeError(json, "FromJson expect dict, but ");
		goto fromjson_fail;
	}
	register Py_ssize_t i, size = _PyObject_LengthHint(json, 0);
	for (i = 0; i < size; i++) {
		item = PyIter_Next(iter);
		if (!item) { goto fromjson_fail; }
		if (!MetaKey_FROMJSON(type, &key, PyTuple_GET_ITEM(item, 0))) {
			goto fromjson_fail;
		} else if (!MetaValue_FROMJSON(type, &value, PyTuple_GET_ITEM(item, 1))) {
			goto fromjson_fail;
		}
		register TypyDictMap entry = (TypyDictMap)IblMap_Set(self->dict_map, &key);
		if (!entry) { goto fromjson_fail; }
		MetaValue_CLEAR(type, &entry->value);
		entry->value = value;
		key = 0;
		value = 0;
	}
	return true;

fromjson_fail:
	Py_XDECREF(iter);
	Py_XDECREF(item);
	MetaKey_CLEAR(type, &key);
	MetaValue_CLEAR(type, &value);
	return false;
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

//=============================================================================

static int dict_AssSubscript(TypyDict* self, PyObject* key, PyObject* value) {
	return MetaDict_SetItem(TypyDict_TYPE(self), self, key, value) ? 0 : -1;
}

static Py_ssize_t dict_Len(TypyDict* self) {
	return IblMap_Size(self->dict_map);
}

static PyObject* dict_Clear(TypyDict* self) {
	TypyDict_Clear(self);
	Py_RETURN_NONE;
}

static PyObject* dict_Contains(TypyDict* self, PyObject* key) {
	TypyField k = 0;
	if (!TypyKey_CHECKSET(self, &k, key, "")) {
		PyErr_Clear();
		Py_RETURN_FALSE;
	}
	if (IblMap_Get(self->dict_map, &k)) {
		Py_RETURN_FALSE;
	} else {
		Py_RETURN_TRUE;
	}
}

static PyObject* dict_Get(TypyDict* self, PyObject* args) {
	PyObject* key;
	PyObject* failobj = Py_None;
	TypyField k = 0;
	if (!PyArg_UnpackTuple(args, "get", 1, 2, &key, &failobj)) {
		return NULL;
	}
	if (!TypyKey_CHECKSET(self, &k, key, "")) {
		PyErr_Clear();
		Py_INCREF(failobj);
		return failobj;
	}
	register TypyDictMap entry = (TypyDictMap)IblMap_Get(self->dict_map, &k);
	if (!entry) {
		Py_INCREF(failobj);
		return failobj;
	} else {
		return TypyValue_GET(self, &entry->value);
	}
}

static PyObject* dict_Keys(TypyDict* self) {
	PyObject* keys = PyList_New(0);
	if (!keys) { return NULL; }
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		register TypyDictMap item = (TypyDictMap)iter;
		register PyObject* key = TypyKey_GET(self, &item->key);
		PyList_Append(keys, key);
		Py_XDECREF(key);
	}
	return keys;
}

static PyObject* dict_Values(TypyDict* self) {
	PyObject* values = PyList_New(0);
	if (!values) { return NULL; }
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		register TypyDictMap item = (TypyDictMap)iter;
		register PyObject* value = TypyKey_GET(self, &item->value);
		PyList_Append(values, value);
		Py_XDECREF(value);
	}
	return values;
}

static PyObject* dict_Items(TypyDict* self) {
	PyObject* items = PyList_New(0);
	if (!items) { return NULL; }
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		register TypyDictMap item = (TypyDictMap)iter;
		register PyObject* key = TypyKey_GET(self, &item->key);
		register PyObject* value = TypyValue_GET(self, &item->value);
		register PyObject* pare = PyTuple_Pack(2, key, value);
		PyList_Append(items, pare);
		Py_XDECREF(key);
		Py_XDECREF(value);
		Py_XDECREF(pare);
	}
	return items;
}

static PyObject* dict_SetDefault(TypyDict* self, PyObject* args) {
	PyObject* key;
	PyObject* failobj = Py_None;
	TypyField k = 0;
	if (!PyArg_UnpackTuple(args, "get", 1, 2, &key, &failobj)) {
		return NULL;
	} else if (!TypyKey_CHECKSET(self, &k, key, "")) {
		return NULL;
	}
	register TypyDictMap entry = (TypyDictMap)IblMap_Get(self->dict_map, &k);
	if (!entry) {
		entry = (TypyDictMap)IblMap_Set(self->dict_map, &k);
		if (!entry || !TypyValue_CHECKSET(self, &entry->value, failobj, "Dict value type error: ")) {
			return NULL;
		}
		Py_INCREF(failobj);
		return failobj;
	} else {
		return TypyValue_GET(self, &entry->value);
	}
}

static PyObject* dict_Update(TypyDict* self, PyObject* arg) {
	PyObject* items;
	if (!arg || arg == Py_None) {
		Py_RETURN_NONE;
	} else if (PyDict_Check(arg)) {
		MetaDict_MergeDict(TypyDict_TYPE(self), self, arg);
		Py_RETURN_NONE;
	} else if ((items = PyObject_CallMethod(arg, "iteritems", NULL))) {
		MetaDict_MergeIter(TypyDict_TYPE(self), self, items, _PyObject_LengthHint(arg, 0));
		Py_DECREF(items);
		Py_RETURN_NONE;
	} else {
		FormatTypeError(arg, "update expect dict, but");
		return NULL;
	}
}

static PyObject* dict_Subscript(TypyDict* self, PyObject* key) {
	TypyField k = 0;
	if (!TypyKey_CHECKSET(self, &k, key, "Dict key type error: ")) {
		return NULL;
	}
	register TypyDictMap entry = (TypyDictMap)IblMap_Get(self->dict_map, &k);
	if (!entry) {
		register PyObject* repr = PyObject_Repr(key);
		if (!repr) { return NULL; }
		PyErr_Format(PyExc_KeyError, "Dict key (%.100s) not found.", PyString_AsString(repr));
		Py_DECREF(repr);
		return NULL;
	} else {
		return TypyValue_GET(self, &entry->value);
	}
}

static PyObject* dict_Pop(TypyDict* self, PyObject* args) {
	PyObject* key;
	PyObject* failobj = Py_None;
	TypyField k = 0;
	if (!PyArg_UnpackTuple(args, "pop", 1, 2, &key, &failobj)) {
		return NULL;
	}
	if (!TypyKey_CHECKSET(self, &k, key, "")) {
		PyErr_Clear();
		Py_INCREF(failobj);
		return failobj;
	}
	register TypyDictMap entry = (TypyDictMap)IblMap_Get(self->dict_map, &k);
	if (!entry) {
		Py_INCREF(failobj);
		return failobj;
	} else {
		register PyObject* value = TypyValue_GET(self, &entry->value);
		TypyValue_CLEAR(self, &entry->value);
		IblMap_Del(self->dict_map, &k);
		return value;
	}
}

static PyObject* dict_Repr(TypyDict* self) {
	register PyObject* json = TypyDict_ToJson(TypyDict_TYPE(self), &self, false);
	if (!json) { return NULL; }
	register PyObject* repr = PyObject_Repr(json);
	Py_DECREF(json);
	return repr;
}

//=============================================================================

static TypyDictIterator* dict_IterKey(TypyDict* self) {
	TypyDictIterator* it = (TypyDictIterator*)PyType_GenericAlloc(&TypyDictIterKeyType, 0);
	if (!it) { return NULL; }
	it->it_result = NULL;
	it->it_index = 0;
	it->it = IblMap_Begin(self->dict_map);
	Py_INCREF(self);
	it->it_dict = self;
	return it;
}

static TypyDictIterator* dict_IterValue(TypyDict* self) {
	TypyDictIterator* it = (TypyDictIterator*)PyType_GenericAlloc(&TypyDictIterValueType, 0);
	if (!it) { return NULL; }
	it->it_result = NULL;
	it->it_index = 0;
	it->it = IblMap_Begin(self->dict_map);
	Py_INCREF(self);
	it->it_dict = self;
	return it;
}

static TypyDictIterator* dict_IterItem(TypyDict* self) {
	TypyDictIterator* it = (TypyDictIterator*)PyType_GenericAlloc(&TypyDictIterItemType, 0);
	if (!it) { return NULL; }
	it->it_result = PyTuple_Pack(2, Py_None, Py_None);
	if (!it->it_result) {
		Py_DECREF(it);
		return NULL;
	}
	it->it_index = 0;
	it->it = IblMap_Begin(self->dict_map);
	Py_INCREF(self);
	it->it_dict = self;
	return it;
}

static void iter_Dealloc(TypyDictIterator* it)
{
	Py_XDECREF(it->it_dict);
	Py_XDECREF(it->it_result);
	Py_TYPE(it)->tp_free(it);
}

static PyObject* iter_Len(TypyDictIterator* it)
{
	Py_ssize_t len;
	if (it->it_dict) {
		len = IblMap_Size(it->it_dict->dict_map) - it->it_index;
		if (len >= 0) { return PyInt_FromSsize_t(len); }
	}
	return PyInt_FromLong(0);
}

static int iter_Traverse(TypyDictIterator* it, visitproc visit, void* arg)
{
	Py_VISIT(it->it_dict);
	Py_VISIT(it->it_result);
	return 0;
}

static PyObject* iter_NextKey(TypyDictIterator* it)
{
	assert(it);
	TypyDict* dict = it->it_dict;
	if (!dict) { return NULL; }
	if (it->it) {
		register TypyDictMap entry = (TypyDictMap)it->it;
		register PyObject* key = TypyKey_GET(dict, &entry->key);
		it->it_index++;
		it->it = IblMap_Next(dict->dict_map, it->it);
		return key;
	}
	it->it_dict = NULL;
	Py_DECREF(dict);
	return NULL;
}

static PyObject* iter_NextValue(TypyDictIterator* it)
{
	assert(it);
	TypyDict* dict = it->it_dict;
	if (!dict) { return NULL; }
	if (it->it) {
		register TypyDictMap entry = (TypyDictMap)it->it;
		register PyObject* value = TypyValue_GET(dict, &entry->value);
		it->it_index++;
		it->it = IblMap_Next(dict->dict_map, it->it);
		return value;
	}
	it->it_dict = NULL;
	Py_DECREF(dict);
	return NULL;
}

static PyObject* iter_NextItem(TypyDictIterator* it)
{
	assert(it);
	TypyDict* dict = it->it_dict;
	if (!dict) { return NULL; }
	if (it->it) {
		register TypyDictMap entry = (TypyDictMap)it->it;
		register PyObject* key = TypyKey_GET(dict, &entry->key);
		if (!key) { return NULL; }
		register PyObject* value = TypyValue_GET(dict, &entry->value);
		if (!value) { Py_DECREF(key); return NULL; }
		PyObject* result = it->it_result;
		if (result->ob_refcnt == 1) {
			Py_INCREF(result);
			Py_DECREF(PyTuple_GET_ITEM(result, 0));
			Py_DECREF(PyTuple_GET_ITEM(result, 1));
		} else {
			result = PyTuple_New(2);
			if (!result) {
				Py_DECREF(key);
				Py_DECREF(value);
				return NULL;
			}
		}
		PyTuple_SET_ITEM(result, 0, key);
		PyTuple_SET_ITEM(result, 1, value);
		it->it = IblMap_Next(dict->dict_map, it->it);
		it->it_index++;
		return result;
	}
	it->it_dict = NULL;
	Py_DECREF(dict);
	return NULL;
}

PyMappingMethods TypyDict_MpMethods = {
	(lenfunc)dict_Len,                /* mp_length        */
	(binaryfunc)dict_Subscript,       /* mp_subscript     */
	(objobjargproc)dict_AssSubscript, /* mp_ass_subscript */
};

PyMethodDef TypyDict_Methods[] = {
	{ "__contains__", (PyCFunction)dict_Contains, METH_O,
		"Tests whether a key is a member of the map." },
	{ "clear", (PyCFunction)dict_Clear, METH_NOARGS,
		"Removes all elements from the map." },
	{ "get", (PyCFunction)dict_Get, METH_VARARGS,
		"Get value or None." },
	{ "pop", (PyCFunction)dict_Pop, METH_VARARGS,
		"Remove specified key and return the corresponding value." },
	{ "keys", (PyCFunction)dict_Keys, METH_NOARGS,
		"Get key list of the map." },
	{ "values", (PyCFunction)dict_Values, METH_NOARGS,
		"Get value list of the map." },
	{ "items", (PyCFunction)dict_Items, METH_NOARGS,
		"Get item list of the map." },
	{ "itervalues", (PyCFunction)dict_IterValue, METH_NOARGS,
		"Iterator over values of the map." },
	{ "iteritems", (PyCFunction)dict_IterItem, METH_NOARGS,
		"Iterator over the (key, value) items of the map." },
	{ "setdefault", (PyCFunction)dict_SetDefault, METH_VARARGS,
		"Get value of the key, also set the default value if key not in the map." },
	{ "update", (PyCFunction)dict_Update, METH_O,
		"Update items from another map." },
	{ NULL, NULL }
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
	(reprfunc)dict_Repr,                      /* tp_repr           */
	0,                                        /* tp_as_number      */
	0,                                        /* tp_as_sequence    */
	&TypyDict_MpMethods,                      /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	0,                                        /* tp_call           */
	(reprfunc)dict_Repr,                      /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy Dict",                            /* tp_doc            */
	0,                                        /* tp_traverse       */
	0,                                        /* tp_clear          */
	0,                                        /* tp_richcompare    */
	0,                                        /* tp_weaklistoffset */
	(getiterfunc)dict_IterKey,                /* tp_iter           */
	0,                                        /* tp_iternext       */
	TypyDict_Methods,                         /* tp_methods        */
	0,                                        /* tp_members        */
	0,                                        /* tp_getset         */
	0,                                        /* tp_base           */
	0,                                        /* tp_dict           */
	0,                                        /* tp_descr_get      */
	0,                                        /* tp_descr_set      */
	0,                                        /* tp_dictoffset     */
	0,                                        /* tp_init           */
};

PyMethodDef TypyDict_IteratorMethods[] = {
	{ "__length_hint__", (PyCFunction)iter_Len, METH_NOARGS,
		"Private method returning an estimate of len(list(it))." },
	{ NULL, NULL }
};

PyTypeObject TypyDictIterKeyType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Dict.KeyIterator",            /* tp_name           */
	sizeof(TypyDictIterator),                        /* tp_basicsize      */
	0,                                               /* tp_itemsize       */
	(destructor)iter_Dealloc,                        /* tp_dealloc        */
	0,                                               /* tp_print          */
	0,                                               /* tp_getattr        */
	0,                                               /* tp_setattr        */
	0,                                               /* tp_compare        */
	0,                                               /* tp_repr           */
	0,                                               /* tp_as_number      */
	0,                                               /* tp_as_sequence    */
	0,                                               /* tp_as_mapping     */
	0,                                               /* tp_hash           */
	0,                                               /* tp_call           */
	0,                                               /* tp_str            */
	PyObject_GenericGetAttr,                         /* tp_getattro       */
	0,                                               /* tp_setattro       */
	0,                                               /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                              /* tp_flags          */
	"A Typy Dict Key Iterator",                      /* tp_doc            */
	(traverseproc)iter_Traverse,                     /* tp_traverse       */
	0,                                               /* tp_clear          */
	0,                                               /* tp_richcompare    */
	0,                                               /* tp_weaklistoffset */
	PyObject_SelfIter,                               /* tp_iter           */
	(iternextfunc)iter_NextKey,                      /* tp_iternext       */
	TypyDict_IteratorMethods,                        /* tp_methods        */
	0,                                               /* tp_members        */
};

PyTypeObject TypyDictIterValueType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Dict.ValueIterator",          /* tp_name           */
	sizeof(TypyDictIterator),                        /* tp_basicsize      */
	0,                                               /* tp_itemsize       */
	(destructor)iter_Dealloc,                        /* tp_dealloc        */
	0,                                               /* tp_print          */
	0,                                               /* tp_getattr        */
	0,                                               /* tp_setattr        */
	0,                                               /* tp_compare        */
	0,                                               /* tp_repr           */
	0,                                               /* tp_as_number      */
	0,                                               /* tp_as_sequence    */
	0,                                               /* tp_as_mapping     */
	0,                                               /* tp_hash           */
	0,                                               /* tp_call           */
	0,                                               /* tp_str            */
	PyObject_GenericGetAttr,                         /* tp_getattro       */
	0,                                               /* tp_setattro       */
	0,                                               /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                              /* tp_flags          */
	"A Typy Dict Value Iterator",                    /* tp_doc            */
	(traverseproc)iter_Traverse,                     /* tp_traverse       */
	0,                                               /* tp_clear          */
	0,                                               /* tp_richcompare    */
	0,                                               /* tp_weaklistoffset */
	PyObject_SelfIter,                               /* tp_iter           */
	(iternextfunc)iter_NextValue,                    /* tp_iternext       */
	TypyDict_IteratorMethods,                        /* tp_methods        */
	0,                                               /* tp_members        */
};

PyTypeObject TypyDictIterItemType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Dict.ItemIterator",           /* tp_name           */
	sizeof(TypyDictIterator),                        /* tp_basicsize      */
	0,                                               /* tp_itemsize       */
	(destructor)iter_Dealloc,                        /* tp_dealloc        */
	0,                                               /* tp_print          */
	0,                                               /* tp_getattr        */
	0,                                               /* tp_setattr        */
	0,                                               /* tp_compare        */
	0,                                               /* tp_repr           */
	0,                                               /* tp_as_number      */
	0,                                               /* tp_as_sequence    */
	0,                                               /* tp_as_mapping     */
	0,                                               /* tp_hash           */
	0,                                               /* tp_call           */
	0,                                               /* tp_str            */
	PyObject_GenericGetAttr,                         /* tp_getattro       */
	0,                                               /* tp_setattro       */
	0,                                               /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                              /* tp_flags          */
	"A Typy Dict Item Iterator",                     /* tp_doc            */
	(traverseproc)iter_Traverse,                     /* tp_traverse       */
	0,                                               /* tp_clear          */
	0,                                               /* tp_richcompare    */
	0,                                               /* tp_weaklistoffset */
	PyObject_SelfIter,                               /* tp_iter           */
	(iternextfunc)iter_NextItem,                     /* tp_iternext       */
	TypyDict_IteratorMethods,                        /* tp_methods        */
	0,                                               /* tp_members        */
};

#ifdef __cplusplus
}
#endif
