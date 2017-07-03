// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TYPY_PROPERTY_HANDLER

PyObject* Typy_GET(TypyObject* self, size_t index) {
	register PyObject* result = _Typy_GET(self, index);
	if ((Typy_FIELDTYPE(self, index) == FIELD_TYPE_LIST ||
		Typy_FIELDTYPE(self, index) == FIELD_TYPE_DICT) && Typy_FIELD(self, index)) {
		TypyComposite_AddOwner((TypyComposite*)Typy_FIELD(self, index),
			(TypyComposite*)self, FIELD_TYPE_OBJECT, Typy_PROPFLAG(self, index));
	}
	return result;
}

bool Typy_READ(TypyObject* self, size_t index, byte** input, size_t* length) {
	Typy_RECORD(self, index);
	register bool result = _Typy_READ(self, index, input, length);
	if (result) { Typy_NOTIFY(self, index); }
	return result;
}

bool Typy_CHECKSET(TypyObject* self, size_t index, PyObject* arg, const char* err) {
	Typy_RECORD(self, index);
	register bool result = _Typy_CHECKSET(self, index, arg, err);
	if (result) { Typy_NOTIFY(self, index); }
	return result;
}

bool Typy_FROMJSON(TypyObject* self, size_t index, PyObject* json) {
	Typy_RECORD(self, index);
	register bool result = _Typy_FROMJSON(self, index, json);
	if (result) { Typy_NOTIFY(self, index); }
	return result;
}

IblBitmap TypyProperty_Register(TypyMetaObject* type, TypyHandlerData data, TypyHandlerFunc func) {
	if (type->handlers_length) {
		register size_t i;
		for (i = 0; i < type->handlers_length && (type->handlers_list[i].handler_data != data ||
			type->handlers_list[i].handler_func != func); i++);
		if (i < type->handlers_length) {
			return type->handlers_list[i].handler_flag;
		}
	}
	if (type->handlers_length + 1 > type->handlers_capacity) {
		register size_t capacity = Ibl_Max(2 * type->handlers_capacity + 1, MIN_HANDLER_CAPACITY);
		register TypyPropertyHandler buffer = (TypyPropertyHandler)calloc(capacity, sizeof(struct _TypyPropertyHandler));
		if (!buffer) {
			return NULL;
		}
		type->handlers_capacity = capacity;
		if (type->handlers_list) {
			memcpy(buffer, type->handlers_list, type->handlers_length * sizeof(struct _TypyPropertyHandler));
			free(type->handlers_list);
		}
		type->handlers_list = buffer;
	}
	register TypyPropertyHandler handler = &type->handlers_list[type->handlers_length];
	handler->handler_flag = IblBitmap_New(type->prop_flagmax);
	handler->handler_data = data;
	handler->handler_func = func;
	type->handlers_length++;
	return handler->handler_flag;
}

void TypyProperty_Unregister(TypyMetaObject* type, TypyHandlerData data, TypyHandlerFunc func) {
	if (type->handlers_length) {
		register size_t i;
		for (i = 0; i < type->handlers_length && (type->handlers_list[i].handler_data != data ||
			type->handlers_list[i].handler_func != func); i++);
		if (i < type->handlers_length) {
			free(type->handlers_list[i].handler_flag);
		}
		for (; i < type->handlers_length - 1; i++) {
			memcpy(&type->handlers_list[i], &type->handlers_list[i + 1], sizeof(struct _TypyPropertyHandler));
		}
		if (i < type->handlers_length) {
			memset(&type->handlers_list[i], 0, sizeof(struct _TypyPropertyHandler));
			type->handlers_length--;
		}
	}
}

void TypyProperty_Changed(TypyObject* self, PropertyFlag flag, FieldType field_type, TypyType typy_type, TypyField old, TypyField new) {
	register size_t i;
	for (i = 0; i < Typy_TYPE(self)->handlers_length; i++) {
		register TypyPropertyHandler handler = &Typy_TYPE(self)->handlers_list[i];
		if (!IblBitmap_Get(handler->handler_flag, flag)) { continue; }
		handler->handler_func(self, flag, handler->handler_data, field_type, typy_type, old, new);
		if (PyErr_Occurred()) {
			PyErr_Print();
			if (Py_FlushLine()) {
				PyErr_Clear();
			}
		}
	}
}

bool Meta_HandleProperty(TypyMetaObject* type, size_t index, TypyHandlerData data, TypyHandlerFunc func) {
	register IblBitmap flag = TypyProperty_Register(type, data, func);
	if (!flag) { return false; }
	register size_t i = Meta_PROPFLAG(type, index);
	register size_t end = type->prop_flagmax;
	if (index + 1 < type->meta_size) {
		end = Meta_PROPFLAG(type, index + 1);
	}
	for (; i < end; i++) {
		if (!IblBitmap_Set(flag, i, true)) {
			TypyProperty_Unregister(type, data, func);
			return false;
		}
	}
	return true;
}

static void onPropertyChanged(TypyObject* object, PropertyFlag flag, PyObject* handler, FieldType field_type, TypyType typy_type, TypyField old, TypyField new) {
	if (flag >= Typy_TYPE(object)->prop_flagmax) {
		PyErr_Format(PyExc_RuntimeError, "'%s' type onPropertyChanged flag larger than flagmax(%d).", Typy_NAME(object), flag);
		return;
	}
	register size_t i;
	for (i = Typy_SIZE(object) - 1; i >= 0 && flag < Typy_PROPFLAG(object, i); i--);
	register TypyField old_origin = old;
	register TypyField new_origin = new;
	register PyObject* old_object = abstract_GetPyObject[field_type](typy_type, &old);
	register PyObject* new_object = abstract_GetPyObject[field_type](typy_type, &new);
	register PyObject* result = PyObject_CallFunction(handler, "OsOO", object, Typy_PropertyName(object, i), old_object, new_object);
	Py_XDECREF(old_object);
	Py_XDECREF(new_object);
	Py_XDECREF(result);
	if (!old_origin && old) {
		TypyField_Clr(field_type, old);
	}
	if (!new_origin && new) {
		TypyField_Clr(field_type, new);
	}
}

PyObject* Py_RegisterHandler(TypyMetaObject* type, PyObject* args) {
	char *name = NULL;
	PyObject* handler = NULL;
	if (!PyArg_ParseTuple(args, "O|s", &handler, &name)) {
		return NULL;
	} else if (!PyCallable_Check(handler)) {
		FormatTypeError(handler, "RegisterHandler expect callable, but ");
		return NULL;
	} else if (name) {
		register int index = Meta_PropertyIndex(type, name);
		if (index < 0) {
			PyErr_Format(PyExc_AttributeError, "'%s' type has no attribute handler '%s'.", Meta_NAME(type), name);
			return NULL;
		} else if (!Meta_HandleProperty(type, index, (TypyHandlerData)handler, (TypyHandlerFunc)onPropertyChanged)) {
			PyErr_Format(PyExc_RuntimeError, "Register attribute '%s' handler for '%s' type failed.", name, Meta_NAME(type));
			return NULL;
		}
		Py_RETURN_NONE;
	}

	register size_t i;
	for (i = 0; i < type->meta_size; i++) {
		if (!Meta_HandleProperty(type, i, (TypyHandlerData)handler, (TypyHandlerFunc)onPropertyChanged)) {
			PyErr_Format(PyExc_RuntimeError, "Register attribute '%s' handler for '%s' type failed.", Meta_PropertyName(type, i), Meta_NAME(type));
			return NULL;
		}
	}
	Py_RETURN_NONE;
}

PyObject* Py_UnregisterHandler(TypyMetaObject* type, PyObject* handler) {
	TypyProperty_Unregister(type, (TypyHandlerData)handler, (TypyHandlerFunc)onPropertyChanged);
	Py_RETURN_NONE;
}

#endif

//=============================================================================

IblMap_KEY_STRING(TypyFieldMap,
	int index;
);

PyTypeObject* _InheritTypyObjectType(void) {
	register PyTypeObject* type = (PyTypeObject*)malloc(sizeof(PyTypeObject));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Inherit TypyObjectType out of memory %zu.", sizeof(PyTypeObject));
		return NULL;
	}
	memcpy(type, &BaseTypyObjectType, sizeof(PyTypeObject));
	type->tp_base = TypyObjectType;
	if (PyType_Ready(type) < 0) {
		free(type);
		return NULL;
	}
	return type;
}

TypyObject* Typy_New(TypyMetaObject* type, PyObject* args, PyObject* kwargs) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	TypyObject* object = (TypyObject*)_PyObject_GC_Malloc(IBL_ALIGNED_SIZE(sizeof(TypyObject) + sizeof(TypyField) * type->meta_size));
	if (!object) { return NULL; }
	(void)PyObject_INIT(object, type->py_type);
	memset(object->object_fields, 0, sizeof(TypyField) * type->meta_size);
	PyObject_GC_Track(object);
	Py_INCREF(type);
	Typy_TYPE(object) = type;
	TypyComposite_INIT(object);

	if (args) {
		register Py_ssize_t i;
		for (i = 0; i < PyTuple_GET_SIZE(args); i++) {
			if (!Typy_CHECKSET(object, i, PyTuple_GET_ITEM(args, i), "InitAttr ")) {
				Py_DECREF(object);
				return NULL;
			}
		}
	}
	if (kwargs) {
		while (PyDict_Next(kwargs, &pos, &k, &v)) {
			if (PyObject_SetAttr((PyObject*)object, k, v) == -1) {
				Py_DECREF(object);
				return NULL;
			}
		}
	}

	return object;
}

int Meta_PropertyIndex(TypyMetaObject* type, char* key) {
	register TypyFieldMap field = (TypyFieldMap)IblMap_Get(type->meta_field2index, &key);
	return field ? field->index : -1;
}

PyObject* Meta_ToJson(TypyMetaObject* type, TypyObject* self, bool slim) {
	register PyObject* json = PyDict_New();
	if (!json) { return NULL; }
	register PyObject* value = PyString_FromString(Meta_NAME(type));
	if (!value) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	register size_t i;
	for (i = 0; i < type->meta_size; i++) {
		if (!Typy_TAG(self, i)) { continue; }
		value = Typy_TOJSON(self, i, slim);
		if (value) {
			PyDict_SetItemString(json, Meta_PropertyName(type, i), value);
			Py_DECREF(value);
		}
	}
	return json;
}

TypyObject* Meta_FromJson(TypyMetaObject* type, PyObject* json) {
	if (!PyObject_HasAttrString(json, "iteritems")) {
		FormatTypeError(json, "FromJson expect dict, but ");
		return NULL;
	}
	register PyObject* value = PyObject_GetItem(json, k_t);
	if (!value) {
		FormatTypeError(json, "Json expect _t, ");
		return NULL;
	} else if (PyUnicode_Check(value)) {
		register PyObject* _value = PyUnicode_AsEncodedObject(value, "utf-8", NULL);
		Py_DECREF(value);
		value = _value;
	} else if (!PyBytes_Check(value)) {
		FormatTypeError(value, "Json _t expect String, but ");
		Py_DECREF(value);
		return NULL;
	}
	if (strcmp(PyBytes_AS_STRING(value), Meta_NAME(type))) {
		PyErr_Format(PyExc_TypeError, "Object expect '%.100s', but Json has type %.100s",
			Meta_NAME(type), PyBytes_AS_STRING(value));
		Py_DECREF(value);
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);

	register TypyObject* object = Typy_New(type, NULL, NULL);
	if (!object) { return NULL; }
	register size_t i;
	for (i = 0; i < type->meta_size; i++) {
		if (!Meta_TAG(type, i)) { continue; }
		register PyObject* p = PyString_FromString(Meta_PropertyName(type, i));
		value = PyObject_GetItem(json, p);
		PyErr_Clear();
		Py_XDECREF(p);
		if (value) {
			register bool success = Typy_FROMJSON(object, i, value);
			Py_DECREF(value);
			if (!success) {
				Py_DECREF(object);
				return NULL;
			}
		}
	}
	return object;
}

//=============================================================================

TypyMetaObject* _Typy_RegisterMeta(PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	PyObject* descriptors;
	size_t i, meta_size;
	PyObject* typy_type;
	uint8 tagsize, wire_type, field_type;
	uint32 tag;

	if (!PyArg_ParseTuple(args, "s#O", &name, &nameLen, &descriptors)) {
		return NULL;
	} else if (!PySequence_Check(descriptors)) {
		FormatTypeError(descriptors, "RegisterMeta descriptors expect sequence type, but ");
		return NULL;
	} else if ((meta_size = PySequence_Size(descriptors)) < 0) {
		FormatTypeError(descriptors, "RegisterMeta descriptors cannot get sequence size, and ");
		return NULL;
	}

	register size_t size = IBL_ALIGNED_SIZE(sizeof(TypyMetaObject) + sizeof(TypyDescriptor) * meta_size + sizeof(char) * nameLen);
	register TypyMetaObject* type = (TypyMetaObject*)malloc(size);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Register Meta out of memory %zu.", size);
		return NULL;
	}

	type->meta_size = meta_size;
	Meta_NAME(type)[nameLen] = 0;
	memcpy(Meta_NAME(type), name, nameLen);
	(void)PyObject_INIT(type, &TypyMetaObjectType);
	type->meta_index2field = (char**)calloc(meta_size, sizeof(char*));
	if (!type->meta_index2field) {
		PyErr_Format(PyExc_RuntimeError, "Register Meta index2field out of memory %zu.", meta_size * sizeof(char*));
		Py_DECREF(type); return NULL;
	}
	type->meta_field2index = TypyFieldMap_New();
	if (!type->meta_field2index) {
		PyErr_Format(PyExc_RuntimeError, "Register Meta field2index out of memory.");
		Py_DECREF(type); return NULL;
	}

#ifdef TYPY_PROPERTY_HANDLER
	register PropertyFlag prop_flag = 1;
#endif

	register uint32 max_tag = 0;
	for (i = 0; i < meta_size; i++) {
		register PyObject* item = PySequence_GetItem(descriptors, i);
		typy_type = NULL;
		if (!PyArg_ParseTuple(item, "sIBBB|O", &name, &tag, &tagsize, &wire_type, &field_type, &typy_type)) {
			Py_XDECREF(item);
			Py_DECREF(type);
			return NULL;
		}
		max_tag = Ibl_Max(max_tag, tag);
		type->meta_descriptor[i].desc_tag       = tag;
		type->meta_descriptor[i].desc_tagsize   = tagsize;
		type->meta_descriptor[i].desc_WireType  = wire_type;
		type->meta_descriptor[i].desc_FieldType = field_type;
		type->meta_descriptor[i].desc_type      = Meta_FromInitializer;

#ifdef TYPY_PROPERTY_HANDLER
		type->meta_descriptor[i].desc_PropFlag = prop_flag;
		if (field_type == FIELD_TYPE_OBJECT || field_type == FIELD_TYPE_VARIANT) {
			prop_flag += ((TypyMetaObject*)(type->meta_descriptor[i].desc_type))->prop_flagmax;
		} else {
			prop_flag += 1;
		}
#endif
		register TypyFieldMap field = (TypyFieldMap)IblMap_Set(type->meta_field2index, &name);
		if (!field) {
			PyErr_Format(PyExc_RuntimeError, "Register Meta cannot set field2index.");
			Py_XDECREF(item);
			Py_DECREF(type);
			return NULL;
		}
		field->index = i;
		type->meta_index2field[i] = field->key;
		Py_XDECREF(item);
	}
	type->meta_cutoff = max_tag <= 0x7F ? 0x7F : (max_tag <= 0x3FFF ? 0x3FFF : max_tag);

#ifdef TYPY_PROPERTY_HANDLER
	type->prop_flagmax      = prop_flag;
	type->handlers_list     = NULL;
	type->handlers_length   = 0;
	type->handlers_capacity = 0;
#endif

	return type;
}

void Typy_Clear(TypyObject* self) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (Typy_FIELD(self, i)) {
			Typy_CLEAR(self, i);
		}
	}
}

void Typy_Dealloc(TypyObject* self) {
	PyObject_GC_UnTrack(self);
	TypyComposite_FREE(self);
	Typy_Clear(self);
	Py_DECREF(Typy_TYPE(self));
	PyObject_GC_Del(self);
}

void Typy_MergeFrom(TypyObject* self, TypyObject* from) {
	if (from == self) { return; }
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_MERGEFROM(self, i, Typy_FIELD(from, i));
	}
}

void Typy_CopyFrom(TypyObject* self, TypyObject* from) {
	if (from == self) { return; }
	Typy_Clear(self);
	Typy_MergeFrom(self, from);
}

size_t Typy_ByteSize(TypyObject* self) {
	register size_t size = 0, i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (!Typy_TAG(self, i) || !Typy_FIELD(self, i)) { continue; }
		size += Typy_BYTESIZE(self, i);
	}
	return size;
}

void Typy_SerializeString(TypyObject* self, byte* output) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		if (!Typy_TAG(self, i) || !Typy_FIELD(self, i)) { continue; }
		output += Typy_WRITE(self, i, output);
	}
}

size_t Typy_MergeFromString(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	while (remain) {
		if (!Typy_ReadTag(&input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
			goto handle_unusual;
		}
		register int index = TAG_INDEX(tag);
		if (index < 0 || (size_t)index >= Typy_SIZE(self)) { goto handle_unusual; }
		if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
			if (!Typy_READ(self, index, &input, &remain)) {
				return 0;
			}
			continue;
		} else if (Typy_FIELDTYPE(self, index) == FIELD_TYPE_LIST &&
			TAG_WIRETYPE(tag) == MetaList_WIRETYPE(Typy_TYPYTYPE(self, index))) {
			if (!TypyList_ReadRepeated(Typy_TYPYTYPE(self, index), (TypyList**)&Typy_FIELD(self, index), &input, &remain)) {
				return 0;
			}
			continue;
		}

	handle_unusual:
		if (tag == 0) { break; }
		if (!Typy_SkipField(&input, &remain, tag)) { return 0; }
	}
	return length - remain;
}

size_t Typy_PropertyByteSize(TypyObject* self, int index) {
	if (!Typy_TAG(self, index)) { return 0; }
	register size_t size = 0;
	if (Typy_FIELD(self, index)) {
		size = Typy_BYTESIZE(self, index);
	}
	return size ? size : Typy_TAGSIZE(self, index);
}

void Typy_SerializeProperty(TypyObject* self, byte* output, int index) {
	if (!Typy_TAG(self, index)) { return; }
	register size_t size = 0;
	if (Typy_FIELD(self, index)) {
		size = Typy_WRITE(self, index, output);
	}
	if (!size) {
		Typy_WriteTag(output, Typy_TAG(self, index));
	}
}

int Typy_DeserializeProperty(TypyObject* self, byte* input, size_t length) {
	uint32 tag;
	size_t remain = length;
	register int index = -1;
	bool* clearFlags = (bool*)calloc(Typy_SIZE(self), sizeof(bool));
	while (remain) {
		if (!Typy_ReadTag(&input, &remain, &tag, Typy_TYPE(self)->meta_cutoff)) {
			goto handle_unusual;
		} else if (index >= 0 && index != TAG_INDEX(tag)) {
			goto handle_unusual;
		}
		index = TAG_INDEX(tag);
		if (index < 0 || (size_t)index >= Typy_SIZE(self)) { goto handle_unusual; }
		if (clearFlags && !clearFlags[index]) {
			Typy_CLEAR(self, index);
			clearFlags[index] = true;
		}
		if (!remain) { break; }
		if (TAG_WIRETYPE(tag) == Typy_WIRETYPE(self, index)) {
			if (!Typy_READ(self, index, &input, &remain)) {
				break;
			}
			continue;
		} else if (Typy_FIELDTYPE(self, index) == FIELD_TYPE_LIST &&
			TAG_WIRETYPE(tag) == MetaList_WIRETYPE(Typy_TYPYTYPE(self, index))) {
			if (!TypyList_ReadRepeated(Typy_TYPYTYPE(self, index), (TypyList**)&Typy_FIELD(self, index), &input, &remain)) {
				break;
			}
			continue;
		}

	handle_unusual:
		if (tag == 0 || !Typy_SkipField(&input, &remain, tag)) { break; }
	}
	if (clearFlags) { free(clearFlags); }
	return (size_t)index >= Typy_SIZE(self) ? -1 : index;
}

PyObject* Typy_GetAttr(TypyObject* self, PyObject* arg) {
	register PyBytes name = Typy_CheckBytes(arg, "GetAttr expect property name, but ");
	if (name) {
		if (!strcmp(PyBytes_AS_STRING(name), "__class__")) {
			Py_DECREF(name);
			Py_INCREF(Typy_TYPE(self));
			return (PyObject*)Typy_TYPE(self);
		}
		register int index = Typy_PropertyIndex(self, PyBytes_AS_STRING(name));
		Py_DECREF(name);
		if (index >= 0) {
			return Typy_GET(self, index);
		}
	}
	register PyObject* attr = PyObject_GenericGetAttr((PyObject*)self, arg);
	if (attr && PyMethod_Check(attr) && PyMethod_GET_SELF(attr) == (PyObject*)Typy_TYPE(self)->py_type) {
		Py_DECREF(PyMethod_GET_SELF(attr));
		Py_INCREF(Typy_TYPE(self));
		PyMethod_GET_SELF(attr) = (PyObject*)Typy_TYPE(self);
	} else if (attr && PyCFunction_Check(attr) && PyCFunction_GET_SELF(attr) == (PyObject*)Typy_TYPE(self)->py_type) {
		Py_DECREF(PyCFunction_GET_SELF(attr));
		Py_INCREF(Typy_TYPE(self));
		PyCFunction_GET_SELF(attr) = (PyObject*)Typy_TYPE(self);
	}
	return attr;
}

int Typy_SetAttr(TypyObject* self, PyObject* arg, PyObject* value) {
	register PyBytes name = Typy_CheckBytes(arg, "SetAttr expect property name, but ");
	if (name) {
		register int index = Typy_PropertyIndex(self, PyBytes_AS_STRING(name));
		Py_DECREF(name);
		if (index >= 0) {
			return Typy_CHECKSET(self, index, value, "SetAttr ") ? 0 : -1;
		}
	}
	return PyObject_GenericSetAttr((PyObject*)self, arg, value);
}

PyObject* Typy_Args(TypyObject* self) {
	register PyObject* result = PyTuple_New(Typy_SIZE(self));
	if (!result) { return result; }
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		PyTuple_SET_ITEM(result, i, Typy_GET(self, i));
	}
	return result;
}

//=============================================================================

PyObject* TypyObject_GetPyObject(TypyMetaObject* type, TypyObject** value) {
	if (*value) {
		Py_INCREF(*value);
		return (PyObject*)(*value);
	}
	Py_RETURN_NONE;
}

bool TypyObject_CheckAndSet(TypyMetaObject* type, TypyObject** value, PyObject* arg, const char* err) {
	if (arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	}
	if (!Typy_TypeCheckExact(arg, type)) {
		FormatTypeError(arg, err);
		return false;
	}
	Py_XDECREF(*value);
	Py_INCREF(arg);
	*value = (TypyObject*)arg;
	return true;
}

size_t TypyObject_ByteSize(TypyMetaObject* type, TypyObject** value, int tagsize) {
	register TypyObject* self = *value;
	register size_t size = 0;
	if (self) {
		size = Typy_ByteSize(self);
		self->cached_size = size;
	}
	return tagsize + IblSizeVarint(size) + size;
}

size_t TypyObject_Write(TypyMetaObject* type, TypyObject** value, int tag, byte* output) {
	register TypyObject* self = *value;
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	size += IblPutUvarint(output + size, self->cached_size);
	if (self->cached_size) {
		Typy_SerializeString(self, output + size);
	}
	return size + self->cached_size;
}

#define TypyObject_FromValueOrNew(s, v, t, r) \
	register TypyObject* s = *(v);                \
	if (!s) {                                     \
		s = Typy_New(t, NULL, NULL);              \
		if (!s) { return r; }                     \
		*(v) = s;                                 \
	}

bool TypyObject_Read(TypyMetaObject* type, TypyObject** value, byte** input, size_t* length) {
	uint32 limit;
	if (!Typy_ReadVarint32(input, length, &limit)) {
		return false;
	} else if (limit > *length) {
		return false;
	}
	TypyObject_FromValueOrNew(self, value, type, false);
	if (!limit) { return true; }
	register size_t size = Typy_MergeFromString(self, *input, limit);
	*input  += limit;
	*length -= limit;
	return size == limit;
}

void TypyObject_MergeFrom(TypyMetaObject* type, TypyObject** lvalue, TypyObject* rvalue) {
	if (!rvalue) { return; }
	TypyObject_FromValueOrNew(self, lvalue, type, )
	Typy_MergeFrom(self, rvalue);
}

PyObject* TypyObject_ToJson(TypyMetaObject* type, TypyObject** value, bool slim) {
	if (!slim && !(*value)) {
		Py_RETURN_NONE;
	} else if (*value) {
		return Meta_ToJson(type, *value, slim);
	} else {
		return NULL;
	}
}

bool TypyObject_FromJson(TypyMetaObject* type, TypyObject** value, PyObject* json) {
	if (!json || json == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	}
	register TypyObject* object = Meta_FromJson(type, json);
	if (!object) { return false; }
	Py_XDECREF(*value);
	*value = object;
	return true;
}

//=============================================================================

PyObject* Py_Copy(PyTypeObject* cls, TypyObject* self) {
	if (!PyObject_TypeCheck(self, TypyObjectType)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to __copy__() must be instance of Typy Object, bug got %.100s.",
			Py_TYPE(self)->tp_name);
		return NULL;
	}
	register TypyObject* object = Typy_New(Typy_TYPE(self), NULL, NULL);
	if (!object) { return NULL; }
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		Typy_SET(object, i, Typy_FIELD(self, i));
	}
	return (PyObject*)object;
}

PyObject* Py_DeepCopy(TypyObject* self, PyObject* args) {
	register TypyObject* object = Typy_New(Typy_TYPE(self), NULL, NULL);
	if (!object) { return NULL; }
	size_t size = Typy_ByteSize(self);
	if (size <= 0) { return (PyObject*)object; }
	register byte* data = (byte*)calloc(size, sizeof(byte));
	if (!data) { return (PyObject*)object; }
	Typy_SerializeString(self, data);
	Typy_MergeFromString(object, data, size);
	return (PyObject*)object;
}

PyObject* Py_CopyFrom(TypyObject* self, TypyObject* from) {
	if (self == from) {
		Py_RETURN_NONE;
	}
	if (Typy_TYPE(self) != Typy_TYPE(from)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to CopyFrom() must be instance of same class: "
			"expected %.100s got %.100s(%.100s).",
			Typy_NAME(self), Py_TYPE(from)->tp_name, Typy_NAME(from));
		return NULL;
	}
	Typy_CopyFrom(self, from);
	Py_RETURN_NONE;
}

PyObject* Py_MergeFrom(TypyObject* self, TypyObject* from) {
	if (self == from) {
		Py_RETURN_NONE;
	}
	if (Typy_TYPE(self) != Typy_TYPE(from)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to CopyFrom() must be instance of same class: "
			"expected %.100s got %.100s(%.100s).",
			Typy_NAME(self), Py_TYPE(from)->tp_name, Typy_NAME(from));
		return NULL;
	}
	Typy_MergeFrom(self, from);
	Py_RETURN_NONE;
}

PyObject* Py_SerializeString(TypyObject* self) {
	size_t size = Typy_ByteSize(self);
	if (size <= 0) {
		return PyBytes_FromString("");
	}
	PyObject* result = PyBytes_FromStringAndSize(NULL, size);
	if (!result) { return NULL; }
	Typy_SerializeString(self, (byte*)PyBytes_AS_STRING(result));
	return result;
}

PyObject* Py_MergeFromString(TypyObject* self, PyObject* arg) {
	const void* data;
	Py_ssize_t size;
	if (PyObject_AsReadBuffer(arg, &data, &size) < 0) {
		return NULL;
	} else if (size <= 0) {
		Py_RETURN_NONE;
	} else if ((size = Typy_MergeFromString(self, (byte*)data, size)) > 0) {
		return PyInt_FromLong(size);
	} else {
		PyErr_Format(PyExc_RuntimeError, "Error parsing object");
		return NULL;
	}
}

PyObject* Py_SerializeProperty(TypyObject* self, PyObject* arg) {
	register PyBytes name = Typy_CheckBytes(arg, "SerializeProperty expect property name, but ");
	if (!name) { return NULL; }
	register int index = Typy_PropertyIndex(self, PyBytes_AS_STRING(name));
	if (index < 0) {
		FormatTypeError(arg, "SerializeProperty expect property name, but ");
		Py_DECREF(name);
		return NULL;
	}
	register size_t size = Typy_PropertyByteSize(self, index);
	if (size <= 0) {
		PyErr_Format(PyExc_RuntimeError, "Error serializing object");
		Py_DECREF(name);
		return NULL;
	}
	register PyObject* result = PyBytes_FromStringAndSize(NULL, size);
	if (!result) {
		Py_DECREF(name);
		return NULL;
	}
	Typy_SerializeProperty(self, (byte*)PyBytes_AS_STRING(result), index);
	Py_DECREF(name);
	return result;
}

PyObject* Py_DeserializeProperty(TypyObject* self, PyObject* arg) {
	const void* data;
	Py_ssize_t size;
	if (PyObject_AsReadBuffer(arg, &data, &size) < 0) {
		return NULL;
	} else if (size <= 0) {
		Py_RETURN_NONE;
	}
	register int index = Typy_DeserializeProperty(self, (byte*)data, size);
	if (index < 0) {
		PyErr_Format(PyExc_RuntimeError, "Error deserializing object");
		return NULL;
	} else {
		return PyBytes_FromString(Typy_PropertyName(self, index));
	}
}

PyObject* Py_Clear(TypyObject* self) {
	Typy_Clear(self);
	Py_RETURN_NONE;
}

PyObject* Py_ParseFromPyString(TypyObject* self, PyObject* arg) {
	Typy_Clear(self);
	return Py_MergeFromString(self, arg);
}

PyObject* Py_ToJson(TypyObject* self, PyObject* args) {
	PyObject* slim = Py_False;
	if (!PyArg_ParseTuple(args, "|O", &slim)) {
		return NULL;
	}
	return Meta_ToJson(Typy_TYPE(self), self, PyObject_IsTrue(slim) == 1);
}

PyObject* Py_FromJson(TypyMetaObject* type, PyObject* json) {
	if (!json || json == Py_None) {
		Py_RETURN_NONE;
	} else {
		return (PyObject*)Meta_FromJson(type, json);
	}
}

int Py_Traverse(TypyObject* self, visitproc visit, void* arg) {
	register size_t i;
	for (i = 0; i < Typy_SIZE(self); i++) {
		TypyField_Vst(Typy_FIELDTYPE(self, i), Typy_FIELD(self, i));
	}
	return 0;
}

int Py_GcClear(TypyObject* self) {
	Typy_Clear(self);
	return 0;
}

// ===================================================================

static PyTypeObject _TypyObjectType;
PyTypeObject* TypyObjectType = &_TypyObjectType;

void TypyMeta_Dealloc(TypyMetaObject* type) {
	if (type->py_type) {
		if (type->py_type != TypyObjectType) {
			free(type->py_type);
		}
	}
	if (type->meta_index2field) {
		free(type->meta_index2field);
	}
	if (type->meta_field2index) {
		IblMap_Free(type->meta_field2index);
	}

#ifdef TYPY_PROPERTY_HANDLER
	if (type->handlers_length) {
		register size_t i;
		for (i = 0; i < type->handlers_length; i++) {
			free(type->handlers_list[i].handler_flag);
		}
	}
	if (type->handlers_list) {
		free(type->handlers_list);
	}
#endif

	free(type);
}

static TypyMetaObject* MetaObject_Initialize(TypyMetaObject* type, PyObject* args) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	PyObject* attrs = Py_None;
	if (PyArg_ParseTuple(args, "|O", &attrs)) {
		if (PyDict_Check(attrs)) {
			while (PyDict_Next(attrs, &pos, &k, &v)) {
				if (type->py_type == TypyObjectType) {
					type->py_type = _InheritTypyObjectType();
					if (!type->py_type) {
						type->py_type = TypyObjectType;
						return NULL;
					}
				}
				if (PyFunction_Check(v) || PyUnbound_Check(v)) {
					v = PyMethod_New(v, NULL, (PyObject*)type->py_type);
				}
				PyDict_SetItem(type->py_type->tp_dict, k, v);
			}
			register PyObject* metaclass = PyDict_GetItemString(attrs, "__metaclass__");
			if (metaclass) { type->py_type->ob_type = (PyTypeObject*)metaclass; }
		}
	}
	Py_INCREF(type);
	return type;
}

static PyObject* MetaObject_Repr(TypyMetaObject* type) {
	return PyString_FromFormat("<Object '" FULL_MODULE_NAME ".%.100s'>", Meta_NAME(type));
}

static PyObject* MetaObject_GetAttr(TypyMetaObject* type, PyObject* arg) {
	if (PyBytes_Check(arg) && !strcmp(PyBytes_AS_STRING(arg), "__name__")) {
		return PyBytes_FromString(Meta_NAME(type));
	} else if (!type->py_type || !arg) {
		Py_RETURN_NONE;
	}
	register PyObject* attr = PyObject_GetAttr((PyObject*)type->py_type, arg);
	if (attr && PyMethod_Check(attr) && PyMethod_GET_SELF(attr) == (PyObject*)type->py_type) {
		Py_DECREF(PyMethod_GET_SELF(attr));
		Py_INCREF(type);
		PyMethod_GET_SELF(attr) = (PyObject*)type;
	} else if (attr && PyCFunction_Check(attr) && PyCFunction_GET_SELF(attr) == (PyObject*)type->py_type) {
		Py_DECREF(PyCFunction_GET_SELF(attr));
		Py_INCREF(type);
		PyCFunction_GET_SELF(attr) = (PyObject*)type;
	}
	return attr;
}

static int MetaObject_SetAttr(TypyMetaObject* type, PyObject* arg, PyObject* value) {
	if (type->py_type == TypyObjectType) {
		type->py_type = _InheritTypyObjectType();
		if (!type->py_type) {
			type->py_type = TypyObjectType;
			return -1;
		}
	} else if (!type->py_type || !arg) {
		register PyObject* repr = PyObject_Repr(arg);
		if (!repr) { return -1; }
		PyErr_Format(PyExc_AttributeError, "'%.100s' type has no attribute %.100s",
			Meta_NAME(type), PyString_AsString(repr));
		Py_DECREF(repr);
		return -1;
	}
	return PyObject_SetAttr((PyObject*)type->py_type, arg, value);
}

static PyObject* MetaObject_New(PyTypeObject* cls, PyObject* args, PyObject* kwargs) {
	register TypyMetaObject* type = (TypyMetaObject*)PyTuple_GET_ITEM(PyTuple_GET_ITEM(args, 1), 0);
	return PyObject_Call((PyObject*)type->py_type->ob_type, args, kwargs);
}

static PyMethodDef _InitDef = { "InitObject", (PyCFunction)MetaObject_Initialize, METH_VARARGS,
	"Initialize Object Type." };

PyObject* Typy_RegisterObject(PyObject* m, PyObject* args) {
	register TypyMetaObject* type = _Typy_RegisterMeta(args);
	register PyObject* method = PyCFunction_NewEx(&_InitDef, (PyObject*)type, NULL);
	Py_XDECREF(type);
	if (!type || !method) { return NULL; }
	type->py_type = TypyObjectType;
	return method;
}

PyTypeObject TypyMetaObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".MetaType",            /* tp_name           */
	sizeof(TypyMetaObject),                  /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)TypyMeta_Dealloc,            /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	(reprfunc)MetaObject_Repr,               /* tp_repr           */
	0,                                       /* tp_as_number      */
	0,                                       /* tp_as_sequence    */
	0,                                       /* tp_as_mapping     */
	0,                                       /* tp_hash           */
	(ternaryfunc)Typy_New,                   /* tp_call           */
	(reprfunc)MetaObject_Repr,               /* tp_str            */
	(getattrofunc)MetaObject_GetAttr,        /* tp_getattro       */
	(setattrofunc)MetaObject_SetAttr,        /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"The Typy Object Metaclass",             /* tp_doc            */
	0,                                       /* tp_traverse       */
	0,                                       /* tp_clear          */
	0,                                       /* tp_richcompare    */
	0,                                       /* tp_weaklistoffset */
	0,                                       /* tp_iter           */
	0,                                       /* tp_iternext       */
	0,                                       /* tp_methods        */
	0,                                       /* tp_members        */
	0,                                       /* tp_getset         */
	0,                                       /* tp_base           */
	0,                                       /* tp_dict           */
	0,                                       /* tp_descr_get      */
	0,                                       /* tp_descr_set      */
	0,                                       /* tp_dictoffset     */
	0,                                       /* tp_init           */
	0,                                       /* tp_alloc          */
	MetaObject_New,                          /* tp_new            */
};

#ifdef __cplusplus
}
#endif
