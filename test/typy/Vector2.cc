// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "Vector2.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Vector2);

Vector2::Vector2() : Message() {
	PyObject_INIT(this, &Object<Vector2>::_Type);
	ZERO(Vector2, p_x, _cached_size);
}

void Vector2::Clear() {
	::typy::Clear(p_x);
	::typy::Clear(p_y);
	::typy::Clear(p_b);
	::typy::Clear(p_e);
	::typy::Clear(p_i);
	::typy::Clear(p_p);
	::typy::Clear(p_s);
	::typy::Clear(p_t);
}

void Vector2::MergeFrom(const Vector2& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	::typy::MergeFrom(p_x, from.p_x);
	::typy::MergeFrom(p_y, from.p_y);
	::typy::MergeFrom(p_b, from.p_b);
	::typy::MergeFrom(p_e, from.p_e);
	::typy::MergeFrom(p_i, from.p_i);
	::typy::MergeFrom(p_p, from.p_p);
	::typy::MergeFrom(p_s, from.p_s);
	::typy::MergeFrom(p_t, from.p_t);
}

void Vector2::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_x, output);
	::typy::Write(2, p_y, output);
	::typy::Write(3, p_b, output);
	::typy::Write(4, p_e, output);
	::typy::Write(5, p_i, output);
	::typy::Write(6, p_p, output);
	::typy::Write(7, p_s, output);
}

int Vector2::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, p_x);
	::typy::ByteSize(total_size, 1, p_y);
	::typy::ByteSize(total_size, 1, p_b);
	::typy::ByteSize(total_size, 1, p_e);
	::typy::ByteSize(total_size, 1, p_i);
	::typy::ByteSize(total_size, 1, p_p);
	::typy::ByteSize(total_size, 1, p_s);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Vector2::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(7)
	FIRST_READ_NORMAL_CASE(1, p_x, float)
	NEXT_READ_NORMAL_CASE(2, p_y, SINGLE_ARG(FixedPoint<1, -10>))
	NEXT_READ_NORMAL_CASE(3, p_b, bytes)
	NEXT_READ_NORMAL_CASE(4, p_e, Corpus)
	NEXT_READ_NORMAL_CASE(5, p_i, Empty)
	NEXT_READ_NORMAL_CASE(6, p_p, Python<Shadow_PyType>)
	NEXT_READ_NORMAL_CASE(7, p_s, string)
	END_READ_CASE()
}

PyObject* Vector2::Json(bool slim) {
	PyObject* json = PyDict_New();
	if (json == NULL) { return NULL; }
	PyObject* value = PyString_FromString(Vector2::Name);
	if (value == NULL) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	value = ::typy::Json(p_x, slim);
	if (value != NULL) { PyDict_SetItemString(json, "x", value); Py_DECREF(value); }
	value = ::typy::Json(p_y, slim);
	if (value != NULL) { PyDict_SetItemString(json, "y", value); Py_DECREF(value); }
	value = ::typy::Json(p_b, slim);
	if (value != NULL) { PyDict_SetItemString(json, "b", value); Py_DECREF(value); }
	value = ::typy::Json(p_e, slim);
	if (value != NULL) { PyDict_SetItemString(json, "e", value); Py_DECREF(value); }
	value = ::typy::Json(p_i, slim);
	if (value != NULL) { PyDict_SetItemString(json, "i", value); Py_DECREF(value); }
	value = ::typy::Json(p_p, slim);
	if (value != NULL) { PyDict_SetItemString(json, "p", value); Py_DECREF(value); }
	value = ::typy::Json(p_s, slim);
	if (value != NULL) { PyDict_SetItemString(json, "s", value); Py_DECREF(value); }
	return json;
}

Vector2* Vector2::FromJson(PyObject* json) {
	if (!PyObject_HasAttrString(json, "iteritems")) {
		FormatTypeError(json, "FromJson expect dict, but ");
		return NULL;
	}
	PyObject* value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("_t")).get());
	if (value == NULL) {
		FormatTypeError(json, "Json expect _t, ");
		return NULL;
	} else if (PyUnicode_Check(value)) {
		PyObject* _value = PyUnicode_AsEncodedObject(value, "utf-8", NULL);
		Py_DECREF(value);
		value = _value;
	} else if (PyBytes_Check(value)) {
		Py_INCREF(value);
	} else {
		FormatTypeError(value, "Json _t expect String, but ");
		return NULL;
	}
	if (strcmp(PyBytes_AS_STRING(value), Vector2::Name)) {
		PyErr_Format(PyExc_TypeError, "Object expect '%.100s', but Json has type %.100s",
			Vector2::Name, PyBytes_AS_STRING(value));
		Py_DECREF(value);
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);
	Vector2* object = new Vector2();
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("x")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_x, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("y")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_y, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("b")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_b, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("e")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_e, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("i")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_i, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("p")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_p, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("s")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_s, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	return object;
}

// ===================================================================

const int Vector2::PropertyCount = 7;
char* Vector2::Properties[] = {
	"x",
	"y",
	"b",
	"e",
	"i",
	"p",
	"s"
};

int Vector2::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
	case 1: ::typy::ByteSize(size, 1, p_x); if (size == 0) { size = 1; } break;
	case 2: ::typy::ByteSize(size, 1, p_y); if (size == 0) { size = 1; } break;
	case 3: ::typy::ByteSize(size, 1, p_b); if (size == 0) { size = 1; } break;
	case 4: ::typy::ByteSize(size, 1, p_e); if (size == 0) { size = 1; } break;
	case 5: ::typy::ByteSize(size, 1, p_i); if (size == 0) { size = 1; } break;
	case 6: ::typy::ByteSize(size, 1, p_p); if (size == 0) { size = 1; } break;
	case 7: ::typy::ByteSize(size, 1, p_s); if (size == 0) { size = 1; } break;
	}
	return size;
}

void Vector2::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 1:
		::typy::Write(1, p_x, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(1, p_x, output);
		}
		break;
	case 2:
		::typy::Write(2, p_y, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(2, p_y, output);
		}
		break;
	case 3:
		::typy::Write(3, p_b, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(3, p_b, output);
		}
		break;
	case 4:
		::typy::Write(4, p_e, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(4, p_e, output);
		}
		break;
	case 5:
		::typy::Write(5, p_i, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(5, p_i, output);
		}
		break;
	case 6:
		::typy::Write(6, p_p, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(6, p_p, output);
		}
		break;
	case 7:
		::typy::Write(7, p_s, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(7, p_s, output);
		}
		break;
	}
}

int Vector2::DeserializeProperty(CodedInputStream* input) {
	const void* data;
	int size;
	input->GetDirectBufferPointerInline(&data, &size);
	CodedInputStream tagInput(reinterpret_cast<const uint8*>(data), size);
	::std::pair<uint32, bool> p = tagInput.ReadTagWithCutoff(_MAXTAG(25)
		<= 0x7F ? 0x7F : (_MAXTAG(25) <= 0x3FFF ? 0x3FFF : _MAXTAG(25)));
	uint32 tag = p.first;
	if (!p.second) { return 0; }

	int index = WireFormatLite::GetTagFieldNumber(tag);
	switch(index) {
	case 1: ::typy::Clear(p_x); break;
	case 2: ::typy::Clear(p_y); break;
	case 3: ::typy::Clear(p_b); break;
	case 4: ::typy::Clear(p_e); break;
	case 5: ::typy::Clear(p_i); break;
	case 6: ::typy::Clear(p_p); break;
	case 7: ::typy::Clear(p_s); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

bool Vector2::SetPropertySequence(PyObject* args) {
	for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++) {
		switch(i) {
		case 0: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 0), p_x, "Property 'x' expect float, but ")) { return false; } break;
		case 1: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 1), p_y, "Property 'y' expect SINGLE_ARG(FixedPoint<1, -10>), but ")) { return false; } break;
		case 2: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 2), p_b, "Property 'b' expect bytes, but ")) { return false; } break;
		case 3: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 3), p_e, "Property 'e' expect Corpus, but ")) { return false; } break;
		case 4: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 4), p_i, "Property 'i' expect Empty, but ")) { return false; } break;
		case 5: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 5), p_p, "Property 'p' expect Python<Shadow_PyType>, but ")) { return false; } break;
		case 6: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 6), p_s, "Property 's' expect string, but ")) { return false; } break;
		case 7: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 7), p_t, "Property 't' expect Python<PyObject>, but ")) { return false; } break;
		default: PyErr_Format(PyExc_TypeError, "Unsurported property number %lu.", i); return false;
		}
	}
	return true;
}

PyObject* Vector2::GetPropertySequence() {
	PyObject* result = PyTuple_New(8);
	if (result == NULL) { return result; }
	PyTuple_SET_ITEM(result, 0, ::typy::GetPyObject(p_x));
	PyTuple_SET_ITEM(result, 1, ::typy::GetPyObject(p_y));
	PyTuple_SET_ITEM(result, 2, ::typy::GetPyObject(p_b));
	PyTuple_SET_ITEM(result, 3, ::typy::GetPyObject(p_e));
	PyTuple_SET_ITEM(result, 4, ::typy::GetPyObject(p_i));
	PyTuple_SET_ITEM(result, 5, ::typy::GetPyObject(p_p));
	PyTuple_SET_ITEM(result, 6, ::typy::GetPyObject(p_s));
	PyTuple_SET_ITEM(result, 7, ::typy::GetPyObject(p_t));
	return result;
}

// ===================================================================

TYPY_GETSET(Vector2, p_x, float);
TYPY_GETSET(Vector2, p_y, SINGLE_ARG(FixedPoint<1, -10>));
TYPY_GETSET(Vector2, p_b, bytes);
TYPY_GETSET(Vector2, p_e, Corpus);
TYPY_GETSET(Vector2, p_i, Empty);
TYPY_GETSET(Vector2, p_p, Python<Shadow_PyType>);
TYPY_GETSET(Vector2, p_s, string);
TYPY_GETSET(Vector2, p_t, Python<PyObject>);

template <> PyGetSetDef Object<Vector2>::GetSet[] = {
	{"x", (getter)Get_p_x, (setter)Set_p_x, "Property x"},
	{"y", (getter)Get_p_y, (setter)Set_p_y, "Property y"},
	{"b", (getter)Get_p_b, (setter)Set_p_b, "Property b"},
	{"e", (getter)Get_p_e, (setter)Set_p_e, "Property e"},
	{"i", (getter)Get_p_i, (setter)Set_p_i, "Property i"},
	{"p", (getter)Get_p_p, (setter)Set_p_p, "Property p"},
	{"s", (getter)Get_p_s, (setter)Set_p_s, "Property s"},
	{"t", (getter)Get_p_t, (setter)Set_p_t, "Property t"},
	{NULL}
};

TypyObjectEnd(Vector2);
