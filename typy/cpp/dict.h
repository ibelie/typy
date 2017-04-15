// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_DICT_H__
#define TYPY_DICT_H__

#include "typy.h"

namespace typy {

template <typename K, typename V>
Dict<K, V>::Entry::Entry() : Message() {
	::typy::Initialize(key);
	::typy::Initialize(value);
}

template <typename K, typename V>
void Dict<K, V>::Entry::Clear() {
	::typy::Clear(key);
	::typy::Clear(value);
}

template <typename K, typename V>
void Dict<K, V>::Entry::MergeFrom(const typename Dict<K, V>::Entry& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) {
		GOOGLE_CHECK(false) << __FILE__ << ":" << __LINE__;
	}
	::typy::MergeFrom(key, from.key);
	::typy::MergeFrom(value, from.value);
}

template <typename K, typename V>
void Dict<K, V>::Entry::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, key, output);
	::typy::Write(2, value, output);
}

template <typename K, typename V>
int Dict<K, V>::Entry::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, key);
	::typy::ByteSize(total_size, 1, value);
	return total_size;
}

template <typename K, typename V>
int Dict<K, V>::Entry::GetCachedSize() const {
	int total_size = 0;
	::typy::GetCachedSize(total_size, 1, key);
	::typy::GetCachedSize(total_size, 1, value);
	return total_size;
}

template <typename K, typename V>
bool Dict<K, V>::Entry::MergePartialFromCodedStream(CodedInputStream* input) {
	uint32 tag;
	for (;;) {
		tag = input->ReadTagWithCutoff(0x7F).first;
		if (tag == KeyTag) {
			if (!::typy::Read(key, input)) { return false; }
			if (input->ExpectTag(ValueTag)) { goto parse_value; }
		} else if (tag == ValueTag) {
		parse_value:
			if (!::typy::Read(value, input)) { return false; }
			if (input->ExpectAtEnd()) { return true; }
		} else if (tag == ValuePackedTag) {
			if (!::typy::ReadPacked(value, input)) { return false; }
			if (input->ExpectAtEnd()) { return true; }
		} else {
			if (tag == 0 || WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
				return true;
			}
			if (!WireFormatLite::SkipField(input, tag)) { return false; }
		}
	}
}

namespace dict {

template <typename K, typename V>
static bool SetItem(PyObject* self, PyObject* key, PyObject* v) {
	typename Type<K>::KeyType k;
	if (!::typy::CheckAndSet(key, k, "Dict key type error: ")) {
		return false;
	}
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	typename Dict<K, V>::iterator it = dict->find(k);
	if (v == NULL) {
		if (it != dict->end()) {
			::typy::Clear(it->second);
			dict->erase(k);
		}
		return true;
	}
	if (!::typy::CheckAndSet(v, (*dict)[k], "Dict value type error: ")) {
		dict->erase(k);
		return false;
	}
	return true;
}

} // namespace dict

template <typename K, typename V>
bool MergeDict(PyObject* arg, Dict<K, V>& value) {
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	while (PyDict_Next(arg, &pos, &k, &v)) {
		if (!::typy::dict::SetItem<K, V>(&value, k, v)) {
			return false;
		}
	}
	return true;
}

template <typename K, typename V>
bool MergeIter(PyObject* iter, Py_ssize_t size, Dict<K, V>& value) {
	for (Py_ssize_t i = 0; i < size; i++) {
		ScopedPyObjectPtr item(PyIter_Next(iter));
		if (!::typy::dict::SetItem<K, V>(&value, PyTuple_GET_ITEM(item.get(), 0),
			PyTuple_GET_ITEM(item.get(), 1))) {
			return false;
		}
	}
	return true;
}

namespace dict {

template <typename K, typename V>
static int tp_AssSubscript(PyObject* self, PyObject* key, PyObject* v) {
	return SetItem<K, V>(self, key, v) ? 0 : -1;
}

static void SetKeyError(PyObject *arg) {
	ScopedPyObjectPtr tup(PyTuple_Pack(1, arg));
	if (tup == NULL) { return; }
	PyErr_Clear();
	PyErr_SetObject(PyExc_KeyError, tup.get());
}

template <typename K, typename V>
static void tp_Dealloc(PyObject* self) {
	delete static_cast<Dict<K, V>*>(self);
}

template <typename K, typename V>
static Py_ssize_t tp_Len(PyObject* self) {
	return static_cast<Dict<K, V>*>(self)->size();
}

template <typename K, typename V>
static PyObject* tp_Clear(PyObject* self) {
	static_cast<Dict<K, V>*>(self)->Clear();
	Py_RETURN_NONE;
}

template <typename K, typename V>
static PyObject* tp_Contains(PyObject* self, PyObject* key) {
	typename Type<K>::KeyType k;
	if (!::typy::CheckAndSet(key, k, "")) {
		PyErr_Clear();
		Py_RETURN_FALSE;
	}
	if (static_cast<Dict<K, V>*>(self)->count(k) == 0) {
		Py_RETURN_FALSE;
	} else {
		Py_RETURN_TRUE;
	}
}

template <typename K, typename V>
static PyObject* tp_Get(PyObject* self, PyObject* args) {
	PyObject* key;
	PyObject* failobj = Py_None;
	if (!PyArg_UnpackTuple(args, "get", 1, 2, &key, &failobj)) {
		return NULL;
	}
	typename Type<K>::KeyType k;
	if (!::typy::CheckAndSet(key, k, "")) {
		PyErr_Clear();
		Py_INCREF(failobj);
		return failobj;
	}
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	typename Dict<K, V>::iterator it = dict->find(k);
	if (it == dict->end()) {
		Py_INCREF(failobj);
		return failobj;
	} else {
		return ::typy::GetPyObject(it->second);
	}
}

template <typename K, typename V>
static PyObject* tp_Pop(PyObject* self, PyObject* args) {
	PyObject* key;
	PyObject* failobj = Py_None;
	if (!PyArg_UnpackTuple(args, "pop", 1, 2, &key, &failobj)) {
		return NULL;
	}
	typename Type<K>::KeyType k;
	if (!::typy::CheckAndSet(key, k, "")) {
		PyErr_Clear();
		Py_INCREF(failobj);
		return failobj;
	}
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	typename Dict<K, V>::iterator it = dict->find(k);
	if (it == dict->end()) {
		Py_INCREF(failobj);
		return failobj;
	} else {
		PyObject* value = ::typy::GetPyObject(it->second);
		::typy::Clear(it->second);
		dict->erase(k);
		return value;
	}
}

template <typename K, typename V>
static PyObject* tp_Keys(PyObject* self) {
	PyObject* keys = PyList_New(0);
	if (keys == NULL) { return NULL; }
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	for (typename Dict<K, V>::const_iterator it = dict->begin(); it != dict->end(); ++it) {
		PyList_Append(keys, ::typy::GetPyObject(it->first));
	}
	return keys;
}

template <typename K, typename V>
static PyObject* tp_Items(PyObject* self) {
	PyObject* items = PyList_New(0);
	if (items == NULL) { return NULL; }
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	for (typename Dict<K, V>::const_iterator it = dict->begin(); it != dict->end(); ++it) {
		PyList_Append(items, PyTuple_Pack(2, ::typy::GetPyObject(it->first), ::typy::GetPyObject(it->second)));
	}
	return items;
}

template <typename K, typename V>
static PyObject* tp_Update(PyObject* self, PyObject* arg) {
	PyObject* items;
	if (!arg || arg == Py_None) {
		Py_RETURN_NONE;
	} else if (PyDict_Check(arg)) {
		::typy::MergeDict(arg, *static_cast<Dict<K, V>*>(self));
		Py_RETURN_NONE;
	} else if ((items = PyObject_CallMethod(arg, "iteritems", NULL)) != NULL) {
		::typy::MergeIter(items, _PyObject_LengthHint(arg, 0), *static_cast<Dict<K, V>*>(self));
		Py_DECREF(items);
		Py_RETURN_NONE;
	} else {
		FormatTypeError(arg, "update expect dict, but");
		return NULL;
	}
}

template <typename K, typename V>
static PyObject* tp_Subscript(PyObject* self, PyObject* key) {
	typename Type<K>::KeyType k;
	if (!::typy::CheckAndSet(key, k, "")) {
		SetKeyError(key);
		return NULL;
	}
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	typename Dict<K, V>::iterator it = dict->find(k);
	if (it == dict->end()) {
		SetKeyError(key);
		return NULL;
	} else {
		return ::typy::GetPyObject(it->second);
	}
}

template <typename K, typename V>
static PyObject* tp_IterKey(PyObject* self) {
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	typename Dict<K, V>::Iterator* it = reinterpret_cast<typename Dict<K, V>::Iterator*>(
		PyType_GenericAlloc(&Dict<K, V>::IterKey_Type, 0));
	if (it == NULL) { return NULL; }
	it->it_result = NULL;
	it->it_index = 0;
	it->it = dict->begin();
	Py_INCREF(self);
	it->it_dict = dict;
	return reinterpret_cast<PyObject*>(it);
}

template <typename K, typename V>
static PyObject* tp_IterValue(PyObject* self) {
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	typename Dict<K, V>::Iterator* it = reinterpret_cast<typename Dict<K, V>::Iterator*>(
		PyType_GenericAlloc(&Dict<K, V>::IterValue_Type, 0));
	if (it == NULL) { return NULL; }
	it->it_result = NULL;
	it->it_index = 0;
	it->it = dict->begin();
	Py_INCREF(self);
	it->it_dict = dict;
	return reinterpret_cast<PyObject*>(it);
}

template <typename K, typename V>
static PyObject* tp_IterItem(PyObject* self) {
	Dict<K, V>* dict = static_cast<Dict<K, V>*>(self);
	typename Dict<K, V>::Iterator* it = reinterpret_cast<typename Dict<K, V>::Iterator*>(
		PyType_GenericAlloc(&Dict<K, V>::IterItem_Type, 0));
	if (it == NULL) { return NULL; }
    it->it_result = PyTuple_Pack(2, Py_None, Py_None);
    if (it->it_result == NULL) {
        Py_DECREF(it);
        return NULL;
    }
	it->it_index = 0;
	it->it = dict->begin();
	Py_INCREF(self);
	it->it_dict = dict;
	return reinterpret_cast<PyObject*>(it);
}

template <typename K, typename V>
static void iter_Dealloc(typename Dict<K, V>::Iterator* it)
{
	Py_XDECREF(it->it_dict);
	Py_XDECREF(it->it_result);
	Py_TYPE(it)->tp_free(it);
}

template <typename K, typename V>
static PyObject* iter_Len(typename Dict<K, V>::Iterator* it)
{
	Py_ssize_t len;
	if (it->it_dict) {
		len = it->it_dict->size() - it->it_index;
		if (len >= 0) { return PyInt_FromSsize_t(len); }
	}
	return PyInt_FromLong(0);
}

template <typename K, typename V>
static int iter_Traverse(typename Dict<K, V>::Iterator* it, visitproc visit, void* arg)
{
	Py_VISIT(it->it_dict);
	Py_VISIT(it->it_result);
	return 0;
}

template <typename K, typename V>
static PyObject* iter_NextKey(typename Dict<K, V>::Iterator* it)
{
	assert(it != NULL);
	Dict<K, V>* dict = it->it_dict;
	if (dict == NULL) { return NULL; }
	if (it->it != dict->end()) {
		it->it_index++;
		return ::typy::GetPyObject((it->it++)->first);
	}
	it->it_dict = NULL;
	Py_DECREF(dict);
	return NULL;
}

template <typename K, typename V>
static PyObject* iter_NextValue(typename Dict<K, V>::Iterator* it)
{
	assert(it != NULL);
	Dict<K, V>* dict = it->it_dict;
	if (dict == NULL) { return NULL; }
	if (it->it != dict->end()) {
		it->it_index++;
		return ::typy::GetPyObject((it->it++)->second);
	}
	it->it_dict = NULL;
	Py_DECREF(dict);
	return NULL;
}

template <typename K, typename V>
static PyObject* iter_NextItem(typename Dict<K, V>::Iterator* it)
{
	assert(it != NULL);
	Dict<K, V>* dict = it->it_dict;
	if (dict == NULL) { return NULL; }
	if (it->it != dict->end()) {
		PyObject* key = ::typy::GetPyObject(it->it->first);
		if (key == NULL) { return NULL; }
		PyObject* value = ::typy::GetPyObject(it->it->second);
		if (value == NULL) { Py_DECREF(key); return NULL; }
		PyObject* result = it->it_result;
		if (result->ob_refcnt == 1) {
			Py_INCREF(result);
			Py_DECREF(PyTuple_GET_ITEM(result, 0));
			Py_DECREF(PyTuple_GET_ITEM(result, 1));
		} else {
			result = PyTuple_New(2);
			if (result == NULL) { return NULL; }
		}
		PyTuple_SET_ITEM(result, 0, key);
		PyTuple_SET_ITEM(result, 1, value);
		it->it_index++;
		it->it++;
		return result;
	}
	it->it_dict = NULL;
	Py_DECREF(dict);
	return NULL;
}

} // namespace dict

template <typename K, typename V>
void Dict<K, V>::Clear() {
	for (typename Dict<K, V>::iterator it = this->begin(); it != this->end(); ++it) {
		::typy::Clear(it->second);
	}
	this->Map::clear();
}

template <typename K, typename V>
PyMappingMethods Dict<K, V>::MpMethods = {
	(lenfunc)::typy::dict::tp_Len<K, V>,                /* mp_length        */
	(binaryfunc)::typy::dict::tp_Subscript<K, V>,       /* mp_subscript     */
	(objobjargproc)::typy::dict::tp_AssSubscript<K, V>, /* mp_ass_subscript */
};

template <typename K, typename V>
PyMethodDef Dict<K, V>::Methods[] = {
	{ "__contains__", ::typy::dict::tp_Contains<K, V>, METH_O,
		"Tests whether a key is a member of the map." },
	{ "clear", (PyCFunction)::typy::dict::tp_Clear<K, V>, METH_NOARGS,
		"Removes all elements from the map." },
	{ "get", (PyCFunction)::typy::dict::tp_Get<K, V>, METH_VARARGS,
		"Get value or None." },
	{ "pop", (PyCFunction)::typy::dict::tp_Pop<K, V>, METH_VARARGS,
		"Remove specified key and return the corresponding value." },
	{ "keys", (PyCFunction)::typy::dict::tp_Keys<K, V>, METH_NOARGS,
		"Get key list of the map." },
	{ "items", (PyCFunction)::typy::dict::tp_Items<K, V>, METH_NOARGS,
		"Get item list of the map." },
	{ "itervalues", (PyCFunction)::typy::dict::tp_IterValue<K, V>, METH_NOARGS,
		"Iterator over values of the map." },
	{ "iteritems", (PyCFunction)::typy::dict::tp_IterItem<K, V>, METH_NOARGS,
		"Iterator over the (key, value) items of the map." },
	{ "update", (PyCFunction)::typy::dict::tp_Update<K, V>, METH_O,
		"Update items from another map." },
	{ NULL, NULL }
};

template <typename K, typename V>
PyTypeObject Dict<K, V>::_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	FULL_MODULE_NAME ".Dict",                    /* tp_name           */
	sizeof(Dict<K, V>),                          /* tp_basicsize      */
	0,                                           /* tp_itemsize       */
	(destructor)::typy::dict::tp_Dealloc<K, V>,  /* tp_dealloc        */
	0,                                           /* tp_print          */
	0,                                           /* tp_getattr        */
	0,                                           /* tp_setattr        */
	0,                                           /* tp_compare        */
	0,                                           /* tp_repr           */
	0,                                           /* tp_as_number      */
	0,                                           /* tp_as_sequence    */
	&MpMethods,                                  /* tp_as_mapping     */
	PyObject_HashNotImplemented,                 /* tp_hash           */
	0,                                           /* tp_call           */
	0,                                           /* tp_str            */
	0,                                           /* tp_getattro       */
	0,                                           /* tp_setattro       */
	0,                                           /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                          /* tp_flags          */
	"A Typy Dict",                               /* tp_doc            */
	0,                                           /* tp_traverse       */
	0,                                           /* tp_clear          */
	0,                                           /* tp_richcompare    */
	0,                                           /* tp_weaklistoffset */
	(getiterfunc)::typy::dict::tp_IterKey<K, V>, /* tp_iter           */
	0,                                           /* tp_iternext       */
	Methods,                                     /* tp_methods        */
	0,                                           /* tp_members        */
	0,                                           /* tp_getset         */
	0,                                           /* tp_base           */
	0,                                           /* tp_dict           */
	0,                                           /* tp_descr_get      */
	0,                                           /* tp_descr_set      */
	0,                                           /* tp_dictoffset     */
	0,                                           /* tp_init           */
};

template <typename K, typename V>
PyMethodDef Dict<K, V>::IteratorMethods[] = {
	{ "__length_hint__", (PyCFunction)::typy::dict::iter_Len<K, V>, METH_NOARGS,
		"Private method returning an estimate of len(list(it))." },
	{ NULL, NULL }
};

template <typename K, typename V>
PyTypeObject Dict<K, V>::IterKey_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	FULL_MODULE_NAME ".Dict.KeyIterator",            /* tp_name           */
	sizeof(Dict<K, V>::Iterator),                    /* tp_basicsize      */
	0,                                               /* tp_itemsize       */
	(destructor)::typy::dict::iter_Dealloc<K, V>,    /* tp_dealloc        */
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
	(traverseproc)::typy::dict::iter_Traverse<K, V>, /* tp_traverse       */
	0,                                               /* tp_clear          */
	0,                                               /* tp_richcompare    */
	0,                                               /* tp_weaklistoffset */
	PyObject_SelfIter,                               /* tp_iter           */
	(iternextfunc)::typy::dict::iter_NextKey<K, V>,  /* tp_iternext       */
	IteratorMethods,                                 /* tp_methods        */
	0,                                               /* tp_members        */
};

template <typename K, typename V>
PyTypeObject Dict<K, V>::IterValue_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	FULL_MODULE_NAME ".Dict.ValueIterator",          /* tp_name           */
	sizeof(Dict<K, V>::Iterator),                    /* tp_basicsize      */
	0,                                               /* tp_itemsize       */
	(destructor)::typy::dict::iter_Dealloc<K, V>,    /* tp_dealloc        */
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
	(traverseproc)::typy::dict::iter_Traverse<K, V>, /* tp_traverse       */
	0,                                               /* tp_clear          */
	0,                                               /* tp_richcompare    */
	0,                                               /* tp_weaklistoffset */
	PyObject_SelfIter,                               /* tp_iter           */
	(iternextfunc)::typy::dict::iter_NextValue<K, V>,/* tp_iternext       */
	IteratorMethods,                                 /* tp_methods        */
	0,                                               /* tp_members        */
};

template <typename K, typename V>
PyTypeObject Dict<K, V>::IterItem_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	FULL_MODULE_NAME ".Dict.ItemIterator",           /* tp_name           */
	sizeof(Dict<K, V>::Iterator),                    /* tp_basicsize      */
	0,                                               /* tp_itemsize       */
	(destructor)::typy::dict::iter_Dealloc<K, V>,    /* tp_dealloc        */
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
	(traverseproc)::typy::dict::iter_Traverse<K, V>, /* tp_traverse       */
	0,                                               /* tp_clear          */
	0,                                               /* tp_richcompare    */
	0,                                               /* tp_weaklistoffset */
	PyObject_SelfIter,                               /* tp_iter           */
	(iternextfunc)::typy::dict::iter_NextItem<K, V>, /* tp_iternext       */
	IteratorMethods,                                 /* tp_methods        */
	0,                                               /* tp_members        */
};

} // namespace typy
#endif // TYPY_DICT_H__
