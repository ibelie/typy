// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MetaKey_DESC(m)      (((TypyMetaDict*)(m))->key_desc)
#define MetaKey_TYPYTYPE(m)  (MetaKey_DESC(m).desc_type)
#define MetaKey_FIELDTYPE(m) (MetaKey_DESC(m).desc_FieldType)
#define MetaKey_WIRETYPE(m)  (MetaKey_DESC(m).desc_WireType)
#define MetaKey_TAG(m)       (MAKE_TAG(1, MetaKey_WIRETYPE(m)))
#define MetaKey_CLEAR(m, k)  TypyField_Clr(MetaKey_FIELDTYPE(m), (*k))
#define MetaKey_BYTESIZE(m, k) \
	(abstract_ByteSize    [MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k), 1))
#define MetaKey_WRITE(m, k, o) \
	(abstract_Write       [MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k), MetaKey_TAG(m), (o)))
#define MetaKey_GET(m, k) \
	(abstract_GetPyObject [MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (k)))
#define MetaKey_CHECKSET(m, l, r, e) \
	(abstract_CheckAndSet [MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (l), (r), (e)))
#define MetaKey_TOJSON(m, v, s) \
	(abstract_ToJson      [MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (v), (s)))
#define MetaKey_FROMJSON(m, v, j) \
	(abstract_FromJsonKey [MetaKey_FIELDTYPE(m)](MetaKey_TYPYTYPE(m), (v), (j)))

#define MetaValue_DESC(m)      (((TypyMetaDict*)(m))->value_desc)
#define MetaValue_TYPYTYPE(m)  (MetaValue_DESC(m).desc_type)
#define MetaValue_FIELDTYPE(m) (MetaValue_DESC(m).desc_FieldType)
#define MetaValue_WIRETYPE(m)  (MetaValue_DESC(m).desc_WireType)
#define MetaValue_TAG(m)       (MAKE_TAG(2, MetaValue_WIRETYPE(m)))
#define MetaValue_TOJSON(m, v, s) \
	(abstract_ToJson      [MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), (s)))
#define MetaValue_WRITE(m, v, o) \
	(abstract_Write       [MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), MetaValue_TAG(m), (o)))
#define MetaValue_BYTESIZE(m, v) \
	(abstract_ByteSize    [MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), (v), 1))

#define MetaValue_RECORD(m, d, i) \
	TypyComposite_RECORD(MetaValue_FIELDTYPE(m), (i), (d))
#define MetaValue_NOTIFY(m, d, i) \
	TypyComposite_NOTIFY(FIELD_TYPE_DICT, (d), 0, MetaValue_FIELDTYPE(m), MetaValue_TYPYTYPE(m), (i))

#define MetaValue_MERGEFROM(m, ob, l, r) do { \
	register TypyField* _l = (TypyField*)(l);                                   \
	MetaValue_RECORD((m), (ob), *_l);                                           \
	abstract_MergeFrom[MetaValue_FIELDTYPE(m)](MetaValue_TYPYTYPE(m), _l, (r)); \
	MetaValue_NOTIFY((m), (ob), *_l);                                           \
} while (0)

#define MetaValue_CLEAR(m, ob, v) do { \
	register TypyField* _v = (TypyField*)(v);                                   \
	MetaValue_RECORD((m), (ob), *_v);                                           \
	TypyField_Clr(MetaValue_FIELDTYPE(m), *_v);                                 \
	MetaValue_NOTIFY((m), (ob), *_v);                                           \
} while (0)

#define MetaValue_DISCARD(m, v) TypyField_Clr(MetaValue_FIELDTYPE(m), (*v))

#define MetaValue_SET(m, ob, l, r) do { \
	register TypyField* _l = (TypyField*)(l);                                   \
	MetaValue_RECORD((m), (ob), *_l);                                           \
	*_l = TypyField_Set(MetaValue_FIELDTYPE(m), (r));                           \
	MetaValue_NOTIFY((m), (ob), *_l);                                           \
} while (0)

static inline PyObject* MetaValue_GET(TypyMetaDict* type, TypyDict* self, TypyField* value) {
	register PyObject* result = abstract_GetPyObject [MetaValue_FIELDTYPE(type)](MetaValue_TYPYTYPE(type), value);

#ifdef TYPY_PROPERTY_HANDLER
	if ((MetaValue_FIELDTYPE(type) == FIELD_TYPE_LIST ||
		MetaValue_FIELDTYPE(type) == FIELD_TYPE_DICT) && *value) {
		TypyComposite_AddOwner((TypyComposite*)(*value), (TypyComposite*)self, FIELD_TYPE_DICT, 0);
	}
#endif

	return result;
}

static inline bool MetaValue_CHECKSET(TypyMetaDict* type, TypyDict* self, TypyField* value, PyObject* arg, const char* err) {
	MetaValue_RECORD(type, self, *value);
	register bool result = abstract_CheckAndSet[MetaValue_FIELDTYPE(type)](MetaValue_TYPYTYPE(type), value, arg, err);
	if (result) { MetaValue_NOTIFY(type, self, *value); }
	return result;
}

static inline bool MetaValue_FROMJSON(TypyMetaDict* type, TypyDict* self, TypyField* value, PyObject* json) {
	MetaValue_RECORD(type, self, *value);
	register bool result = abstract_FromJson[MetaValue_FIELDTYPE(type)](MetaValue_TYPYTYPE(type), value, json);
	if (result) { MetaValue_NOTIFY(type, self, *value); }
	return result;
}

#define MetaDict_DESC(m, i)      (i ? MetaValue_DESC(m) : MetaKey_DESC(m))
#define MetaDict_FIELDTYPE(m, i) (MetaDict_DESC(m, i).desc_FieldType)
#define MetaDict_TYPYTYPE(m, i)  (MetaDict_DESC(m, i).desc_type)
#define MetaDict_WIRETYPE(m, i)  (MetaDict_DESC(m, i).desc_WireType)
#define MetaDict_READ(m, i, k, s, l) \
	(abstract_Read[MetaDict_FIELDTYPE(m, i)](MetaDict_TYPYTYPE(m, i), (k), (s), (l)))

#define MetaDict_Clear(m, ob) { \
	register IblMap_Item iter;                                                                  \
	for (iter = IblMap_Begin((ob)->dict_map); iter; iter = IblMap_Next((ob)->dict_map, iter)) { \
		MetaValue_CLEAR((m), (ob), &((TypyDictMap)iter)->value);                                \
	}                                                                                           \
	IblMap_Clear((ob)->dict_map);                                                               \
}

#define TypyKey_GET(ob, k)              MetaKey_GET        (TypyDict_TYPE(ob), (k))
#define TypyKey_CHECKSET(ob, l, r, e)   MetaKey_CHECKSET   (TypyDict_TYPE(ob), (l), (r), (e))
#define TypyValue_GET(ob, v)            MetaValue_GET      (TypyDict_TYPE(ob), (ob), (v))
#define TypyValue_CLEAR(ob, v)          MetaValue_CLEAR    (TypyDict_TYPE(ob), (ob), (v))
#define TypyValue_CHECKSET(ob, l, r, e) MetaValue_CHECKSET (TypyDict_TYPE(ob), (ob), (l), (r), (e))
#define TypyDict_Clear(ob)              MetaDict_Clear     (TypyDict_TYPE(ob), (ob))

//=============================================================================

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
		PyErr_Format(PyExc_RuntimeError, "Register Dict out of memory %zu.", sizeof(TypyMetaDict) + nameLen);
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

static inline bool TypyDict_SetItem(TypyDict* self, PyObject* key, PyObject* value) {
	TypyField k = 0;
	if (!TypyKey_CHECKSET(self, &k, key, "Dict key type error: ")) {
		return false;
	} else if (!value) {
		register TypyDictMap entry = (TypyDictMap)IblMap_Get(self->dict_map, &k);
		if (entry) {
			TypyValue_CLEAR(self, &entry->value);
			IblMap_Del(self->dict_map, &k);
		}
		return true;
	}
	register TypyDictMap entry = (TypyDictMap)IblMap_Set(self->dict_map, &k);
	if (!entry) {
		PyErr_Format(PyExc_MemoryError, "Cannot allocate new entry.");
		return false;
	}
	return TypyValue_CHECKSET(self, &entry->value, value, "Dict value type error: ");
}

static inline bool TypyDict_MergeDict(TypyDict* self, PyObject* dict) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	while (PyDict_Next(dict, &pos, &k, &v)) {
		if (!TypyDict_SetItem(self, k, v)) {
			return false;
		}
	}
	return true;
}

static inline bool TypyDict_MergeIter(TypyDict* self, PyObject* iter, Py_ssize_t size) {
	register Py_ssize_t i;
	register bool success = true;
	for (i = 0; i < size && success; i++) {
		register PyObject* item = PyIter_Next(iter);
		if (item) {
			success = TypyDict_SetItem(self, PyTuple_GET_ITEM(item, 0), PyTuple_GET_ITEM(item, 1));
			Py_DECREF(item);
		} else {
			success = false;
		}
	}
	return success;
}

static void MetaDict_Dealloc(TypyMetaDict* type) {
	free(type);
}

static TypyDict* TypyDict_New(TypyMetaDict* type) {
	TypyDict* dict = PyObject_GC_New(TypyDict, &TypyDictType);
	if (!dict) { return NULL; }
	dict->dict_map = TypyDictMap_New(MetaKey_FIELDTYPE(type));
	if (!dict->dict_map) {
		PyObject_GC_Del(dict);
		PyErr_Format(PyExc_RuntimeError, "Alloc Dict map out of memory.");
		return NULL;
	}
	PyObject_GC_Track(dict);
	Py_INCREF(type);
	TypyDict_TYPE(dict) = type;
	TypyComposite_INIT(dict);
	return dict;
}

static void TypyDict_Dealloc(TypyDict* self) {
	PyObject_GC_UnTrack(self);
	TypyComposite_FREE(self);
	TypyDict_Clear(self);
	IblMap_Free(self->dict_map);
	Py_DECREF(TypyDict_TYPE(self));
	PyObject_GC_Del(self);
}

static PyObject* TypyDict_Repr(TypyMetaDict* type) {
	return PyString_FromFormat("<Dict '" FULL_MODULE_NAME ".%.100s'>", type->dict_name);
}

//=============================================================================

#define TypyDict_FromValueOrNew(s, v, t, r) \
	register TypyDict* s = *(v);            \
	if (!s) {                               \
		s = TypyDict_New(t);                \
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
	if (!arg) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	} else if (PyObject_TypeCheck(arg, &TypyDictType) && TypyDict_TYPE(arg) == type) {
		Py_XDECREF(*value);
		Py_INCREF(arg);
		*value = (TypyDict*)arg;
		return true;
	} else if (PyDict_Check(arg)) {
		Py_XDECREF(*value);
		*value = TypyDict_New(type);
		if (!(*value)) { return false; }
		return TypyDict_MergeDict(*value, arg);
	} else if ((items = PyObject_CallMethod(arg, "iteritems", NULL))) {
		Py_XDECREF(*value);
		*value = TypyDict_New(type);
		if (!(*value)) { Py_DECREF(items); return false; }
		register bool success = TypyDict_MergeIter(*value, items, _PyObject_LengthHint(arg, 0));
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
	while (remain) {
		if (!Typy_ReadTag(input, &remain, &tag, 0x7F)) {
			goto handle_unusual;
		}

		register int index = TAG_INDEX(tag);
		if (index < 0 || index > 1) { goto handle_unusual; }
		if (TAG_WIRETYPE(tag) == MetaDict_WIRETYPE(type, index)) {
			if (!MetaDict_READ(type, index, index ? &value : &key, input, &remain)) {
				goto dictread_failed;
			}
			continue;
		} else if (MetaDict_FIELDTYPE(type, index) == FIELD_TYPE_LIST &&
			TAG_WIRETYPE(tag) == MetaList_WIRETYPE(MetaDict_TYPYTYPE(type, index))) {
			if (!TypyList_ReadRepeated(MetaDict_TYPYTYPE(type, index), (TypyList**)(index ? &value : &key), input, &remain)) {
				goto dictread_failed;
			}
			continue;
		}

	handle_unusual:
		if (tag == 0) { break; }
		if (!Typy_SkipField(input, &remain, tag)) { goto dictread_failed; }
	}

	TypyDict_FromValueOrNew(self, dict, type, false);
	if (MetaKey_FIELDTYPE(type) >= MAX_PRIMITIVE_TYPE && !key) {
		register PyObject* defaultKey = MetaKey_GET(type, &key);
		if (!defaultKey) { goto dictread_failed; }
		MetaKey_CHECKSET(type, &key, defaultKey, "");
		Py_DECREF(defaultKey);
		if (!key) { goto dictread_failed; }
	}
	register TypyDictMap item = (TypyDictMap)IblMap_Set(self->dict_map, &key);
	if (!item) { goto dictread_failed; }
	MetaValue_SET(TypyDict_TYPE(self), self, &item->value, value);
	MetaValue_DISCARD(type, &value);
	*input += remain;
	*length -= limit;
	return remain == 0;

dictread_failed:
	MetaKey_CLEAR(type, &key);
	MetaValue_DISCARD(type, &value);
	return false;
}

void TypyDict_MergeFrom(TypyMetaDict* type, TypyDict** lvalue, TypyDict* rvalue) {
	if (!rvalue) { return; }
	TypyDict_FromValueOrNew(self, lvalue, type, );
	register IblMap_Item iter;
	for (iter = IblMap_Begin(rvalue->dict_map); iter; iter = IblMap_Next(rvalue->dict_map, iter)) {
		register TypyDictMap ritem = (TypyDictMap)iter;
		register TypyDictMap litem = (TypyDictMap)IblMap_Set(self->dict_map, &ritem->key);
		MetaValue_MERGEFROM(type, self, &litem->value, ritem->value);
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
		if (item->value) {
			entry_size += MetaValue_BYTESIZE(type, &item->value);
		}
		size += IblPutUvarint(output + size, entry_size);
		size += MetaKey_WRITE(type, &item->key, output + size);
		if (item->value) {
			size += MetaValue_WRITE(type, &item->value, output + size);
		}
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
		if (item->value) {
			entry_size += MetaValue_BYTESIZE(type, &item->value);
		}
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
			register PyObject* k = MetaKey_TOJSON(type, &item->key, false);
			register PyObject* key = PyObject_Str(k);
			register PyObject* value = MetaValue_TOJSON(type, &item->value, slim);
			if (!value) {
				Py_INCREF(Py_None);
				value = Py_None;
			}
			PyDict_SetItem(dict, key, value);
			Py_XDECREF(k);
			Py_XDECREF(key);
			Py_DECREF(value);
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
		goto fromjson_failed;
	}
	register Py_ssize_t i, size = _PyObject_LengthHint(json, 0);
	for (i = 0; i < size; i++) {
		item = PyIter_Next(iter);
		if (!item) { goto fromjson_failed; }
		if (!MetaKey_FROMJSON(type, &key, PyTuple_GET_ITEM(item, 0))) {
			goto fromjson_failed;
		} else if (!MetaValue_FROMJSON(type, self, &value, PyTuple_GET_ITEM(item, 1))) {
			goto fromjson_failed;
		}
		Py_DECREF(item);
		item = NULL;
		register TypyDictMap entry = (TypyDictMap)IblMap_Set(self->dict_map, &key);
		if (!entry) { goto fromjson_failed; }
		MetaValue_CLEAR(type, self, &entry->value);
		entry->value = value;
		key = 0;
		value = 0;
	}
	Py_XDECREF(iter);
	return true;

fromjson_failed:
	Py_XDECREF(iter);
	Py_XDECREF(item);
	MetaKey_CLEAR(type, &key);
	MetaValue_DISCARD(type, &value);
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
	return TypyDict_SetItem(self, key, value) ? 0 : -1;
}

static Py_ssize_t dict_Len(TypyDict* self) {
	return IblMap_Size(self->dict_map);
}

static PyObject* dict_Clear(TypyDict* self) {
	TypyDict_Clear(self);
	Py_RETURN_NONE;
}

static int dict_Contains(TypyDict* self, PyObject* key) {
	TypyField k = 0;
	if (!TypyKey_CHECKSET(self, &k, key, "")) {
		PyErr_Clear();
		return 0;
	}
	return IblMap_Get(self->dict_map, &k) ? 1 : 0;
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
		register PyObject* value = TypyValue_GET(self, &item->value);
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
	if (!arg) {
		Py_RETURN_NONE;
	} else if (PyDict_Check(arg)) {
		TypyDict_MergeDict(self, arg);
		Py_RETURN_NONE;
	} else if ((items = PyObject_CallMethod(arg, "iteritems", NULL))) {
		TypyDict_MergeIter(self, items, _PyObject_LengthHint(arg, 0));
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

static PyObject* dict_DeepCopy(TypyDict* self, PyObject* args) {
	TypyDict* dict = NULL;
	register PyObject* json = TypyDict_ToJson(TypyDict_TYPE(self), &self, false);
	if (!json) { return NULL; }
	if (!TypyDict_FromJson(TypyDict_TYPE(self), &dict, json)) {
		Py_XDECREF(dict);
		dict = NULL;
	}
	Py_DECREF(json);
	return (PyObject*)dict;
}

static PyObject* dict_Copy(PyTypeObject* cls, TypyDict* self) {
	if (!PyObject_TypeCheck(self, &TypyDictType)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to __copy__() must be instance of Typy Dict, bug got %.100s.",
			Py_TYPE(self)->tp_name);
		return NULL;
	}
	register TypyDict* dict = TypyDict_New(TypyDict_TYPE(self));
	if (!dict) { return NULL; }
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		register TypyDictMap item = (TypyDictMap)iter;
		register TypyDictMap entry = (TypyDictMap)IblMap_Set(dict->dict_map, &item->key);
		if (!entry) { Py_DECREF(dict); return NULL; }
		MetaValue_SET(TypyDict_TYPE(self), self, &entry->value, item->value);
	}
	return (PyObject*)dict;
}

static int dict_Traverse(TypyDict* self, visitproc visit, void* arg) {
	register IblMap_Item iter;
	for (iter = IblMap_Begin(self->dict_map); iter; iter = IblMap_Next(self->dict_map, iter)) {
		register TypyDictMap item = (TypyDictMap)iter;
		TypyField_Vst(MetaKey_FIELDTYPE(TypyDict_TYPE(self)), item->key);
		TypyField_Vst(MetaValue_FIELDTYPE(TypyDict_TYPE(self)), item->value);
	}
	return 0;
}

static int dict_GcClear(TypyDict* self) {
	TypyDict_Clear(self);
	return 0;
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

PySequenceMethods TypyDict_SqMethods = {
	0,                                /* sq_length         */
	0,                                /* sq_concat         */
	0,                                /* sq_repeat         */
	0,                                /* sq_item           */
	0,                                /* sq_slice          */
	0,                                /* sq_ass_item       */
	0,                                /* sq_ass_slice      */
	(objobjproc)dict_Contains,        /* sq_contains       */
	0,                                /* sq_inplace_concat */
	0,                                /* sq_inplace_repeat */
};

PyMappingMethods TypyDict_MpMethods = {
	(lenfunc)dict_Len,                /* mp_length        */
	(binaryfunc)dict_Subscript,       /* mp_subscript     */
	(objobjargproc)dict_AssSubscript, /* mp_ass_subscript */
};

PyMethodDef TypyDict_Methods[] = {
	{ "__copy__", (PyCFunction)dict_Copy, METH_O | METH_CLASS,
		"Shallow copy the dict." },
	{ "__deepcopy__", (PyCFunction)dict_DeepCopy, METH_VARARGS,
		"Deep copy the dict." },
	{ "clear", (PyCFunction)dict_Clear, METH_NOARGS,
		"Removes all elements from the dict." },
	{ "get", (PyCFunction)dict_Get, METH_VARARGS,
		"Get value or None." },
	{ "pop", (PyCFunction)dict_Pop, METH_VARARGS,
		"Remove specified key and return the corresponding value." },
	{ "keys", (PyCFunction)dict_Keys, METH_NOARGS,
		"Get key list of the dict." },
	{ "values", (PyCFunction)dict_Values, METH_NOARGS,
		"Get value list of the dict." },
	{ "items", (PyCFunction)dict_Items, METH_NOARGS,
		"Get item list of the dict." },
	{ "itervalues", (PyCFunction)dict_IterValue, METH_NOARGS,
		"Iterator over values of the dict." },
	{ "iteritems", (PyCFunction)dict_IterItem, METH_NOARGS,
		"Iterator over the (key, value) items of the dict." },
	{ "setdefault", (PyCFunction)dict_SetDefault, METH_VARARGS,
		"Get value of the key, also set the default value if key not in the dict." },
	{ "update", (PyCFunction)dict_Update, METH_O,
		"Update items from another dict." },
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,  /* tp_flags          */
	"A Typy Dict",                            /* tp_doc            */
	(traverseproc)dict_Traverse,              /* tp_traverse       */
	(inquiry)dict_GcClear,                    /* tp_clear          */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,         /* tp_flags          */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,         /* tp_flags          */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,         /* tp_flags          */
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
