// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_LIST_H__
#define TYPY_LIST_H__

#include "typy.h"

namespace typy {

template <typename T>
void List<T>::Clear() {
	for (int i = 0; i < this->size(); i++) {
		::typy::Clear(*this->Mutable(i));
	}
	this->RepeatedField::Clear();
}

namespace list {

template <typename T>
static bool Append(PyObject* self, PyObject* item) {
	if (!::typy::CheckAndSet(item, *static_cast<List<T>*>(self)->Add(), "List item type error: ")) {
		static_cast<List<T>*>(self)->RemoveLast();
		return false;
	}
	return true;
}

} // namespace list

template <typename T>
bool ExtendList(PyObject* arg, List<T>& value) {
	if (PyList_CheckExact(arg) || PyTuple_CheckExact(arg) || (PyObject*)&value == arg) {
		ScopedPyObjectPtr list(PySequence_Fast(arg, "argument must be iterable"));
		if (list == NULL) { return false; }
		register Py_ssize_t i, size = PySequence_Fast_GET_SIZE(list.get());
		register PyObject** src = PySequence_Fast_ITEMS(list.get());
		for (i = 0; i < size; i++) {
			if (!::typy::list::Append<T>(&value, src[i])) {
				return false;
			}
		}
		return true;
	}
	register Py_ssize_t i, size = _PyObject_LengthHint(arg, 0);
	if (size < 0) { return false; } else if (!size) { return true; }
	ScopedPyObjectPtr it(PyObject_GetIter(arg));
	if (it == NULL) { return false; }
	register iternextfunc iternext = *it.get()->ob_type->tp_iternext;
	for (i = 0; i < size; i++) {
		if (!::typy::list::Append<T>(&value, ScopedPyObjectPtr(iternext(it.get())).get())) {
			return false;
		}
	}
	return true;
}

inline bool Read(List<string>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<string>; }
	if (!Read(*value->Add(), input)) {
		value->RemoveLast();
		return false;
	}
	return true;
}

inline bool Read(List<bytes>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<bytes>; }
	if (!Read(*value->Add(), input)) {
		value->RemoveLast();
		return false;
	}
	return true;
}

template <typename T>
inline bool ReadPacked(T& value, CodedInputStream* input) { return false; }

template <typename T>
inline bool ReadRepeated(int tagsize, uint32 tag, List<T>*& value, CodedInputStream* input) {
	return false;
}

#define INTEGER_LIST_SIZE(Name) 0; \
	for (int i = 0; i < value->size(); i++) {                   \
		data_size += WireFormatLite::Name##Size(value->Get(i)); \
	}

#define FIXED_LIST_SIZE(Name) \
	WireFormatLite::k##Name##Size * value->size();

#define PRIMITIVE_LIST_TYPE(TYPE, Name, SIZE) \
inline void ByteSize(int& total, int tagsize, List<TYPE>* value) {                         \
	if (value == NULL) { return; }                                                         \
	int data_size = SIZE(Name);                                                            \
	if (data_size > 0) {                                                                   \
		total += tagsize + WireFormatLite::Int32Size(data_size);                           \
	}                                                                                      \
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();                                                 \
	value->_cached_size = data_size;                                                       \
	GOOGLE_SAFE_CONCURRENT_WRITES_END();                                                   \
	total += data_size;                                                                    \
}                                                                                          \
                                                                                           \
inline void Write(int field_number, List<TYPE>* value, CodedOutputStream* output) {        \
	if (value == NULL) { return; }                                                         \
	if (value->_cached_size > 0) {                                                         \
		WireFormatLite::WriteTag(field_number,                                             \
			WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);                            \
		output->WriteVarint32(value->_cached_size);                                        \
	}                                                                                      \
	for (int i = 0; i < value->size(); i++) {                                              \
		WireFormatLite::Write##Name##NoTag(value->Get(i), output);                         \
	}                                                                                      \
}                                                                                          \
                                                                                           \
inline void WriteTag(int tag, List<TYPE>* value, CodedOutputStream* output) {              \
	WireFormatLite::WriteTag(tag, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);      \
}                                                                                          \
                                                                                           \
inline bool Read(List<TYPE>*& value, CodedInputStream* input) {                            \
	if (value == NULL) { value = new List<TYPE>; }                                         \
	TYPE item;                                                                             \
	if (!Read(item, input)) return false;                                                  \
	value->Add(item);                                                                      \
	return true;                                                                           \
}                                                                                          \
                                                                                           \
inline bool ReadPacked(List<TYPE>*& value, CodedInputStream* input) {                      \
	if (value == NULL) { value = new List<TYPE>; }                                         \
	return WireFormatLite::ReadPackedPrimitive<TYPE,                                       \
		WireFormatLite::FieldType(Type<TYPE>::FieldType)>(input, value);                   \
}                                                                                          \
                                                                                           \
inline bool ReadRepeated(int tagsize, uint32 tag,                                          \
	List<TYPE>*& value, CodedInputStream* input) {                                         \
	if (value == NULL) { value = new List<TYPE>; }                                         \
	return WireFormatLite::ReadRepeatedPrimitiveNoInline<TYPE,                             \
		WireFormatLite::FieldType(Type<TYPE>::FieldType)>(tagsize, tag, input, value);     \
}

PRIMITIVE_LIST_TYPE(int32, Int32, INTEGER_LIST_SIZE);
PRIMITIVE_LIST_TYPE(int64, Int64, INTEGER_LIST_SIZE);
PRIMITIVE_LIST_TYPE(uint32, UInt32, INTEGER_LIST_SIZE);
PRIMITIVE_LIST_TYPE(uint64, UInt64, INTEGER_LIST_SIZE);
PRIMITIVE_LIST_TYPE(double, Double, FIXED_LIST_SIZE);
PRIMITIVE_LIST_TYPE(float, Float, FIXED_LIST_SIZE);
PRIMITIVE_LIST_TYPE(bool, Bool, FIXED_LIST_SIZE);

#undef PRIMITIVE_LIST_TYPE
#undef INTEGER_LIST_SIZE
#undef FIXED_LIST_SIZE

namespace list {

template <typename T>
static PyObject* tp_Append(PyObject* self, PyObject* item) {
	if (!Append<T>(self, item)) { return NULL; }
	Py_RETURN_NONE;
}

template <typename T>
static void tp_Dealloc(PyObject* self) {
	delete static_cast<List<T>*>(self);
}

template <typename T>
static Py_ssize_t tp_Len(PyObject* self) {
	return static_cast<List<T>*>(self)->size();
}

template <typename T>
static PyObject* tp_Item(PyObject* self, Py_ssize_t index) {
	List<T>* o = static_cast<List<T>*>(self);
	if (index < 0) {
		index = o->size() + index;
	}
	return ::typy::GetPyObject(o->Get(index));
}

template <typename T>
static int tp_AssignItem(PyObject* self, Py_ssize_t index, PyObject* arg) {
	List<T>* o = static_cast<List<T>*>(self);
	if (index < 0) {
		index = o->size() + index;
	}
	return ::typy::CheckAndSet(arg, *o->Mutable(index), "List item type error: ") ? 0 : -1;
}

template <typename T>
static PyObject* tp_Extend(PyObject* self, PyObject* value) {
	if (!PyObject_Not(value) && Py_TYPE(value)->tp_as_sequence != NULL) {
		::typy::ExtendList(value, *static_cast<List<T>*>(self));
	}
	Py_RETURN_NONE;
}

template <typename T>
static PyObject* tp_Subscript(PyObject* self, PyObject* slice) {
	Py_ssize_t from;
	Py_ssize_t to;
	Py_ssize_t step;
	Py_ssize_t length;
	Py_ssize_t slicelength;
	bool return_list = false;
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		from = to = PyInt_AsLong(slice);
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		from = to = PyLong_AsLong(slice);
	} else if (PySlice_Check(slice)) {
		length = tp_Len<T>(self);
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx(reinterpret_cast<PySliceObject*>(slice),
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return NULL;
		}
		return_list = true;
	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return NULL;
	}

	if (!return_list) {
		return tp_Item<T>(self, from);
	}

	PyObject* list = PyList_New(0);
	if (list == NULL) {
		return NULL;
	}
	if (from <= to) {
		if (step < 0) {
			return list;
		}
		for (Py_ssize_t index = from; index < to; index += step) {
			if (index < 0 || index >= length) {
				break;
			}
			ScopedPyObjectPtr s(tp_Item<T>(self, index));
			PyList_Append(list, s.get());
		}
	} else {
		if (step > 0) {
			return list;
		}
		for (Py_ssize_t index = from; index > to; index += step) {
			if (index < 0 || index >= length) {
				break;
			}
			ScopedPyObjectPtr s(tp_Item<T>(self, index));
			PyList_Append(list, s.get());
		}
	}
	return list;
}

template <typename T>
static int tp_AssSubscript(PyObject* self, PyObject* slice, PyObject* value) {
	Py_ssize_t from;
	Py_ssize_t to;
	Py_ssize_t step;
	Py_ssize_t length;
	Py_ssize_t slicelength;
	bool create_list = false;

#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		from = to = PyInt_AsLong(slice);
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		from = to = PyLong_AsLong(slice);
	} else if (PySlice_Check(slice)) {
		length = tp_Len<T>(self);
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx(reinterpret_cast<PySliceObject*>(slice),
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return -1;
		}
		create_list = true;
	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return -1;
	}

	if (!create_list) {
		return tp_AssignItem<T>(self, from, value);
	}

	ScopedPyObjectPtr full_slice(PySlice_New(NULL, NULL, NULL));
	if (full_slice == NULL) {
		return -1;
	}
	ScopedPyObjectPtr new_list(tp_Subscript<T>(self, full_slice.get()));
	if (new_list == NULL) {
		return -1;
	}
	if (PySequence_SetSlice(new_list.get(), from, to, value) < 0) {
		return -1;
	}

	static_cast<List<T>*>(self)->Clear();
	return ::typy::ExtendList(new_list.get(), *static_cast<List<T>*>(self)) ? 0 : -1;
}

template <typename T>
static PyObject* tp_Insert(PyObject* self, PyObject* args) {
	Py_ssize_t index;
	PyObject* value;
	if (!PyArg_ParseTuple(args, "lO", &index, &value)) {
		return NULL;
	}
	ScopedPyObjectPtr full_slice(PySlice_New(NULL, NULL, NULL));
	ScopedPyObjectPtr new_list(tp_Subscript<T>(self, full_slice.get()));
	if (PyList_Insert(new_list.get(), index, value) < 0) {
		return NULL;
	}
	static_cast<List<T>*>(self)->Clear();
	if (!::typy::ExtendList(new_list.get(), *static_cast<List<T>*>(self))) {
		return NULL;
	}
	Py_RETURN_NONE;
}

template <typename T>
static PyObject* tp_Remove(PyObject* self, PyObject* value) {
	List<T>* o = static_cast<List<T>*>(self);
	typename List<T>::iterator it = o->begin();
	while (it != o->end()) {
		ScopedPyObjectPtr elem(::typy::GetPyObject(*it));
		if (PyObject_RichCompareBool(elem.get(), value, Py_EQ)) {
			break;
		}
		it++;
	}
	if (it == o->end()) {
		PyErr_SetString(PyExc_ValueError, "remove(x): x not in container");
		return NULL;
	}
	::typy::Clear(*it);
	o->erase(it);
	Py_RETURN_NONE;
}

template <typename T>
static PyObject* tp_Pop(PyObject* self, PyObject* args) {
	Py_ssize_t index = -1;
	if (!PyArg_ParseTuple(args, "|n", &index)) {
		return NULL;
	}
	List<T>* o = static_cast<List<T>*>(self);
	typename List<T>::iterator it = o->begin();
	for (Py_ssize_t i = 0; it != o->end(); ++it, ++i) {
		if (i == index) { break; }
	}
	if (it == o->end()) {
		PyErr_SetString(PyExc_ValueError, "pop(i): i not in container");
		return NULL;
	}
	PyObject* item = ::typy::GetPyObject(*it);
	::typy::Clear(*it);
	o->erase(it);
	return item;
}

template <typename T>
static PyObject* tp_Iter(PyObject* self) {
	typename List<T>::Iterator* it = reinterpret_cast<typename List<T>::Iterator*>(
		PyType_GenericAlloc(&List<T>::Iterator_Type, 0));
	if (it == NULL) { return NULL; }
	it->it_index = 0;
	Py_INCREF(self);
	it->it_seq = static_cast<List<T>*>(self);
	return reinterpret_cast<PyObject*>(it);
}

template <typename T>
static void iter_Dealloc(typename List<T>::Iterator* it)
{
	Py_XDECREF(it->it_seq);
	Py_TYPE(it)->tp_free(it);
}

template <typename T>
static PyObject* iter_Len(typename List<T>::Iterator* it)
{
	Py_ssize_t len;
	if (it->it_seq) {
		len = it->it_seq->size() - it->it_index;
		if (len >= 0) { return PyInt_FromSsize_t(len); }
	}
	return PyInt_FromLong(0);
}

template <typename T>
static int iter_Traverse(typename List<T>::Iterator* it, visitproc visit, void* arg)
{
	Py_VISIT(it->it_seq);
	return 0;
}

template <typename T>
static PyObject* iter_Next(typename List<T>::Iterator* it)
{
	assert(it != NULL);
	List<T>* seq = it->it_seq;
	if (seq == NULL) { return NULL; }
	if (it->it_index < seq->size()) {
		return tp_Item<T>(seq, it->it_index++);
	}
	it->it_seq = NULL;
	Py_DECREF(seq);
	return NULL;
}

} // namespace list

template <typename T>
PySequenceMethods List<T>::SqMethods = {
	(lenfunc)::typy::list::tp_Len<T>,                /* sq_length   */
	0,                                               /* sq_concat   */
	0,                                               /* sq_repeat   */
	(ssizeargfunc)::typy::list::tp_Item<T>,          /* sq_item     */
	0,                                               /* sq_slice    */
	(ssizeobjargproc)::typy::list::tp_AssignItem<T>, /* sq_ass_item */
};

template <typename T>
PyMappingMethods List<T>::MpMethods = {
	(lenfunc)::typy::list::tp_Len<T>,                /* mp_length        */
	(binaryfunc)::typy::list::tp_Subscript<T>,       /* mp_subscript     */
	(objobjargproc)::typy::list::tp_AssSubscript<T>, /* mp_ass_subscript */
};

template <typename T>
PyMethodDef List<T>::Methods[] = {
	{ "append", (PyCFunction)::typy::list::tp_Append<T>, METH_O,
		"Appends an object to the list." },
	{ "extend", (PyCFunction)::typy::list::tp_Extend<T>, METH_O,
		"Appends objects to the list." },
	{ "insert", (PyCFunction)::typy::list::tp_Insert<T>, METH_VARARGS,
		"Appends objects to the list." },
	{ "pop", (PyCFunction)::typy::list::tp_Pop<T>, METH_VARARGS,
		"Removes an object from the list and returns it." },
	{ "remove", (PyCFunction)::typy::list::tp_Remove<T>, METH_O,
		"Removes an object from the list." },
	{ NULL, NULL }
};

template <typename T>
PyTypeObject List<T>::_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	FULL_MODULE_NAME ".List",                /* tp_name           */
	sizeof(List<T>),                         /* tp_basicsize      */
	0,                                       /* tp_itemsize       */
	(destructor)::typy::list::tp_Dealloc<T>, /* tp_dealloc        */
	0,                                       /* tp_print          */
	0,                                       /* tp_getattr        */
	0,                                       /* tp_setattr        */
	0,                                       /* tp_compare        */
	0,                                       /* tp_repr           */
	0,                                       /* tp_as_number      */
	&SqMethods,                              /* tp_as_sequence    */
	&MpMethods,                              /* tp_as_mapping     */
	PyObject_HashNotImplemented,             /* tp_hash           */
	0,                                       /* tp_call           */
	0,                                       /* tp_str            */
	0,                                       /* tp_getattro       */
	0,                                       /* tp_setattro       */
	0,                                       /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                      /* tp_flags          */
	"A Typy List",                           /* tp_doc            */
	0,                                       /* tp_traverse       */
	0,                                       /* tp_clear          */
	0,                                       /* tp_richcompare    */
	0,                                       /* tp_weaklistoffset */
	(getiterfunc)::typy::list::tp_Iter<T>,   /* tp_iter           */
	0,                                       /* tp_iternext       */
	Methods,                                 /* tp_methods        */
	0,                                       /* tp_members        */
	0,                                       /* tp_getset         */
	0,                                       /* tp_base           */
	0,                                       /* tp_dict           */
	0,                                       /* tp_descr_get      */
	0,                                       /* tp_descr_set      */
	0,                                       /* tp_dictoffset     */
	0,                                       /* tp_init           */
};

template <typename T>
PyMethodDef List<T>::IteratorMethods[] = {
	{ "__length_hint__", (PyCFunction)::typy::list::iter_Len<T>, METH_NOARGS,
		"Private method returning an estimate of len(list(it))." },
	{ NULL, NULL }
};

template <typename T>
PyTypeObject List<T>::Iterator_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	FULL_MODULE_NAME ".List.Iterator",            /* tp_name           */
	sizeof(List<T>::Iterator),                    /* tp_basicsize      */
	0,                                            /* tp_itemsize       */
	(destructor)::typy::list::iter_Dealloc<T>,    /* tp_dealloc        */
	0,                                            /* tp_print          */
	0,                                            /* tp_getattr        */
	0,                                            /* tp_setattr        */
	0,                                            /* tp_compare        */
	0,                                            /* tp_repr           */
	0,                                            /* tp_as_number      */
	0,                                            /* tp_as_sequence    */
	0,                                            /* tp_as_mapping     */
	0,                                            /* tp_hash           */
	0,                                            /* tp_call           */
	0,                                            /* tp_str            */
	PyObject_GenericGetAttr,                      /* tp_getattro       */
	0,                                            /* tp_setattro       */
	0,                                            /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                           /* tp_flags          */
	"A Typy List Iterator",                       /* tp_doc            */
	(traverseproc)::typy::list::iter_Traverse<T>, /* tp_traverse       */
	0,                                            /* tp_clear          */
	0,                                            /* tp_richcompare    */
	0,                                            /* tp_weaklistoffset */
	PyObject_SelfIter,                            /* tp_iter           */
	(iternextfunc)::typy::list::iter_Next<T>,     /* tp_iternext       */
	IteratorMethods,                              /* tp_methods        */
	0,                                            /* tp_members        */
};

} // namespace typy
#endif // TYPY_LIST_H__
