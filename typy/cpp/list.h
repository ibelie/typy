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
	if (index < 0 || index >= o->size()) {
		PyErr_Format(PyExc_IndexError, "List index (%zd) out of range (%d).\n", index, o->size());
		return -1;
	}
	if (arg == NULL) {
		::typy::Clear(*o->Mutable(index));
		for (; index < o->size() - 1; index++) {
			*o->Mutable(index) = o->Get(index + 1);
		}
		*o->Mutable(index) = static_cast<typename Type<T>::ValueType>(NULL);
		o->RemoveLast();
		return 0;
	}
	return ::typy::CheckAndSet(arg, *o->Mutable(index), "List item type error: ") ? 0 : -1;
}

template <typename T>
static PyObject* tp_Extend(PyObject* self, PyObject* other) {
	if (!PyObject_Not(other) && Py_TYPE(other)->tp_as_sequence != NULL) {
		if (!::typy::ExtendList(other, *static_cast<List<T>*>(self))) {
			return NULL;
		}
	}
	Py_RETURN_NONE;
}

template <typename T>
static PyObject* tp_PyList(PyObject* self) {
	List<T>* o = static_cast<List<T>*>(self);
	PyObject* list = PyList_New(o->size());
	if (list == NULL) { return NULL; }
	for (int i = 0; i < o->size(); i++) {
		PyList_SET_ITEM(list, i, ::typy::GetPyObject(o->Get(i)));
	}
	return list;
}

template <typename T>
static PyObject* tp_Concat(PyObject* self, PyObject* other) {
	if (PyObject_TypeCheck(other, Py_TYPE(self))) {
		List<T>* list = new List<T>;
		if (list == NULL) { return NULL; }
		List<T>* o1 = static_cast<List<T>*>(self);
		List<T>* o2 = static_cast<List<T>*>(self);
		for (int i = 0; i < o1->size(); i++) {
			::typy::CopyFrom(*list->Add(), o1->Get(i));
		}
		for (int i = 0; i < o2->size(); i++) {
			::typy::CopyFrom(*list->Add(), o2->Get(i));
		}
		return list;
	} else {
		PyObject* list = tp_PyList<T>(self);
		if (list == NULL) { return NULL; }
		ScopedPyObjectPtr(PySequence_InPlaceConcat(list, other));
		return list;
	}
}

template <typename T>
static PyObject* tp_InplaceConcat(PyObject* self, PyObject* other) {
	if (!PyObject_Not(other) && Py_TYPE(other)->tp_as_sequence != NULL) {
		if (!::typy::ExtendList(other, *static_cast<List<T>*>(self))) {
			return NULL;
		}
	}
	Py_INCREF(self);
	return self;
}

template <typename T>
static PyObject* tp_Repeat(PyObject* self, Py_ssize_t n) {
	List<T>* o = static_cast<List<T>*>(self);
	if (n <= 0) {
		return new List<T>;
	} else if (n > 0 && o->size() > PY_SSIZE_T_MAX / n) {
		return PyErr_NoMemory();
	}
	List<T>* list = new List<T>;
	if (list == NULL) { return NULL; }
	for (Py_ssize_t i = 0; i < n; i++) {
		for (int i = 0; i < o->size(); i++) {
			::typy::CopyFrom(*list->Add(), o->Get(i));
		}
	}
	return list;
}

template <typename T>
static PyObject* tp_InplaceRepeat(PyObject* self, Py_ssize_t n) {
	List<T>* list = static_cast<List<T>*>(self);
	int size = list->size();
	if (n < 1) { list->Clear(); }
	if (size == 0 || n <= 1) {
		Py_INCREF(self);
		return self;
	} else if (size > PY_SSIZE_T_MAX / n) {
		return PyErr_NoMemory();
	}
	for (Py_ssize_t i = 1; i < n; i++) {
		for (int j = 0; j < size; j++) {
			::typy::CopyFrom(*list->Add(), list->Get(j));
		}
	}
	Py_INCREF(self);
	return self;
}

template <typename T>
static int tp_Contains(PyObject* self, PyObject* value) {
	int cmp = 0;
	List<T>* list = static_cast<List<T>*>(self);
	for (int i = 0; cmp == 0 && i < list->size(); i++) {
		ScopedPyObjectPtr item(::typy::GetPyObject(list->Get(i)));
		cmp = PyObject_RichCompareBool(value, item.get(), Py_EQ);
	}
	return cmp;
}

template <typename T>
static PyObject* tp_Slice(PyObject* self, Py_ssize_t ilow, Py_ssize_t ihigh) {
	List<T>* list = static_cast<List<T>*>(self);
	if (ilow < 0) {
		ilow = 0;
	} else if (ilow > list->size()) {
		ilow = list->size();
	}
	if (ihigh < ilow) {
		ihigh = ilow;
	} else if (ihigh > list->size()) {
		ihigh = list->size();
	}

	List<T>* slice = new List<T>;
	if (slice == NULL) { return NULL; }
	for (Py_ssize_t i = ilow; i < ihigh; i++) {
		::typy::CopyFrom(*slice->Add(), list->Get(i));
	}
	return slice;
}

template <typename T>
static int tp_AssignSlice(PyObject* self, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject* value) {
	ScopedPyObjectPtr list(tp_PyList<T>(self));
	if (list == NULL) { return -1; }
	if (PySequence_SetSlice(list.get(), ilow, ihigh, value) < 0) {
		return -1;
	}

	static_cast<List<T>*>(self)->Clear();
	return ::typy::ExtendList(list.get(), *static_cast<List<T>*>(self)) ? 0 : -1;
}

template <typename T>
static PyObject* tp_Subscript(PyObject* self, PyObject* slice) {
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		return tp_Item<T>(self, PyInt_AsLong(slice));
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		return tp_Item<T>(self, PyLong_AsLong(slice));
	} else if (PySlice_Check(slice)) {
		Py_ssize_t from, to, step, slicelength;
		Py_ssize_t length = tp_Len<T>(self);
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx(reinterpret_cast<PySliceObject*>(slice),
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return NULL;
		}

		List<T>* slice = new List<T>;
		if (slice == NULL) { return NULL; }
		List<T>* list = static_cast<List<T>*>(self);

		if (from <= to) {
			if (step < 0) { return slice; }
			for (Py_ssize_t i = from; i < to; i += step) {
				if (i < 0 || i >= length) { break; }
				::typy::CopyFrom(*slice->Add(), list->Get(i));
			}
		} else {
			if (step > 0) { return slice; }
			for (Py_ssize_t i = from; i > to; i += step) {
				if (i < 0 || i >= length) { break; }
				::typy::CopyFrom(*slice->Add(), list->Get(i));
			}
		}

		return slice;

	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return NULL;
	}
}

template <typename T>
static int tp_AssSubscript(PyObject* self, PyObject* slice, PyObject* value) {
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(slice)) {
		return tp_AssignItem<T>(self, PyInt_AsLong(slice), value);
	} else  /* NOLINT */
#endif
	if (PyLong_Check(slice)) {
		return tp_AssignItem<T>(self, PyLong_AsLong(slice), value);
	} else if (PySlice_Check(slice)) {
		Py_ssize_t from, to, step, slicelength;
		Py_ssize_t length = tp_Len<T>(self);
#if PY_MAJOR_VERSION >= 3
		if (PySlice_GetIndicesEx(slice,
#else
		if (PySlice_GetIndicesEx(reinterpret_cast<PySliceObject*>(slice),
#endif
				length, &from, &to, &step, &slicelength) == -1) {
			return -1;
		}
		return tp_AssignSlice<T>(self, from, to, value);
	} else {
		PyErr_SetString(PyExc_TypeError, "list indices must be integers");
		return -1;
	}
}

template <typename T>
static PyObject* tp_Insert(PyObject* self, PyObject* args) {
	Py_ssize_t index;
	PyObject* value;
	if (!PyArg_ParseTuple(args, "lO", &index, &value)) {
		return NULL;
	}
	ScopedPyObjectPtr list(tp_PyList<T>(self));
	if (PyList_Insert(list.get(), index, value) < 0) {
		return NULL;
	}
	static_cast<List<T>*>(self)->Clear();
	if (!::typy::ExtendList(list.get(), *static_cast<List<T>*>(self))) {
		return NULL;
	}
	Py_RETURN_NONE;
}

template <typename T>
static PyObject* tp_Remove(PyObject* self, PyObject* value) {
	List<T>* o = static_cast<List<T>*>(self);
	int i;
	for (i = 0; i < o->size(); i++) {
		ScopedPyObjectPtr elem(::typy::GetPyObject(o->Get(i)));
		if (PyObject_RichCompareBool(elem.get(), value, Py_EQ)) {
			break;
		}
	}
	if (i == o->size()) {
		PyErr_SetString(PyExc_ValueError, "remove(x): x not in container");
		return NULL;
	}
	::typy::Clear(*o->Mutable(i));
	for (; i < o->size() - 1; i++) {
		*o->Mutable(i) = o->Get(i + 1);
	}
	*o->Mutable(i) = static_cast<typename Type<T>::ValueType>(NULL);
	o->RemoveLast();
	Py_RETURN_NONE;
}

template <typename T>
static PyObject* tp_Pop(PyObject* self, PyObject* args) {
	Py_ssize_t index = -1;
	List<T>* o = static_cast<List<T>*>(self);
	if (!PyArg_ParseTuple(args, "|n", &index)) {
		return NULL;
	} else if (index < 0 || index >= o->size()) {
		PyErr_SetString(PyExc_ValueError, "pop(i) - i not in container");
		return NULL;
	}
	PyObject* item = ::typy::GetPyObject(o->Get(index));
	::typy::Clear(*o->Mutable(index));
	for (; index < o->size() - 1; index++) {
		*o->Mutable(index) = o->Get(index + 1);
	}
	*o->Mutable(index) = static_cast<typename Type<T>::ValueType>(NULL);
	o->RemoveLast();
	return item;
}

template <typename T>
static PyObject* tp_Repr(PyObject* self) {
	ScopedPyObjectPtr json(::typy::Json(static_cast<List<T>*>(self), false));
	if (json == NULL) { return NULL; }
	return PyObject_Repr(json.get());
}

template <typename T>
static PyObject* tp_Copy(PyTypeObject* cls, PyObject* arg) {
	if (!PyObject_TypeCheck(arg, cls)) {
		PyErr_Format(PyExc_TypeError,
			"Parameter to __copy__() must be instance of same class: "
			"expected %.100s got %.100s.",
			cls->tp_name, Py_TYPE(arg)->tp_name);
		return NULL;
	}
	List<T>* list = new List<T>;
	if (list == NULL) { return NULL; }
	List<T>* self = static_cast<List<T>*>(arg);
	for (int i = 0; i < self->size(); i++) {
		::typy::CopyFrom(*list->Add(), self->Get(i));
	}
	return list;
}

template <typename T>
static PyObject* tp_DeepCopy(PyObject* self, PyObject* args) {
	List<T>* list = NULL;
	ScopedPyObjectPtr json(::typy::Json(static_cast<List<T>*>(self), false));
	if (json == NULL) { return NULL; }
	if (!::typy::FromJson(list, json.get())) {
		if (list != NULL) {
			delete list;
			list = NULL;
		}
	}
	return list;
}

template <typename T>
static int tp_Traverse(PyObject* self, visitproc visit, void* arg) {
	register int result;
	List<T>* list = static_cast<List<T>*>(self);
	for (int i = 0; i < list->size(); i++) {
		result = ::typy::Visit(*list->Mutable(i), visit, arg);
		if (result) { return result; }
	}
	return 0;
}

template <typename T>
static int tp_Clear(PyObject* self) {
	static_cast<List<T>*>(self)->Clear();
	return 0;
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
	(lenfunc)::typy::list::tp_Len<T>,                      /* sq_length         */
	(binaryfunc)::typy::list::tp_Concat<T>,                /* sq_concat         */
	(ssizeargfunc)::typy::list::tp_Repeat<T>,              /* sq_repeat         */
	(ssizeargfunc)::typy::list::tp_Item<T>,                /* sq_item           */
	(ssizessizeargfunc)::typy::list::tp_Slice<T>,          /* sq_slice          */
	(ssizeobjargproc)::typy::list::tp_AssignItem<T>,       /* sq_ass_item       */
	(ssizessizeobjargproc)::typy::list::tp_AssignSlice<T>, /* sq_ass_slice      */
	(objobjproc)::typy::list::tp_Contains<T>,              /* sq_contains       */
	(binaryfunc)::typy::list::tp_InplaceConcat<T>,         /* sq_inplace_concat */
	(ssizeargfunc)::typy::list::tp_InplaceRepeat<T>,       /* sq_inplace_repeat */
};

template <typename T>
PyMappingMethods List<T>::MpMethods = {
	(lenfunc)::typy::list::tp_Len<T>,                /* mp_length        */
	(binaryfunc)::typy::list::tp_Subscript<T>,       /* mp_subscript     */
	(objobjargproc)::typy::list::tp_AssSubscript<T>, /* mp_ass_subscript */
};

template <typename T>
PyMethodDef List<T>::Methods[] = {
	{ "__copy__", (PyCFunction)::typy::list::tp_Copy<T>, METH_O | METH_CLASS,
		"Shallow copy the list." },
	{ "__deepcopy__", (PyCFunction)::typy::list::tp_DeepCopy<T>, METH_VARARGS,
		"Deep copy the list." },
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
	FULL_MODULE_NAME ".List",                   /* tp_name           */
	sizeof(List<T>),                            /* tp_basicsize      */
	0,                                          /* tp_itemsize       */
	(destructor)::typy::list::tp_Dealloc<T>,    /* tp_dealloc        */
	0,                                          /* tp_print          */
	0,                                          /* tp_getattr        */
	0,                                          /* tp_setattr        */
	0,                                          /* tp_compare        */
	(reprfunc)::typy::list::tp_Repr<T>,         /* tp_repr           */
	0,                                          /* tp_as_number      */
	&SqMethods,                                 /* tp_as_sequence    */
	&MpMethods,                                 /* tp_as_mapping     */
	PyObject_HashNotImplemented,                /* tp_hash           */
	0,                                          /* tp_call           */
	(reprfunc)::typy::list::tp_Repr<T>,         /* tp_str            */
	0,                                          /* tp_getattro       */
	0,                                          /* tp_setattro       */
	0,                                          /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags          */
	"A Typy List",                              /* tp_doc            */
	(traverseproc)::typy::list::tp_Traverse<T>, /* tp_traverse       */
	(inquiry)::typy::list::tp_Clear<T>,         /* tp_clear          */
	0,                                          /* tp_richcompare    */
	0,                                          /* tp_weaklistoffset */
	(getiterfunc)::typy::list::tp_Iter<T>,      /* tp_iter           */
	0,                                          /* tp_iternext       */
	Methods,                                    /* tp_methods        */
	0,                                          /* tp_members        */
	0,                                          /* tp_getset         */
	0,                                          /* tp_base           */
	0,                                          /* tp_dict           */
	0,                                          /* tp_descr_get      */
	0,                                          /* tp_descr_set      */
	0,                                          /* tp_dictoffset     */
	0,                                          /* tp_init           */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,      /* tp_flags          */
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
