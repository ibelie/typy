
#include "ViSlI3tNgu2aFVB8BXYUxf9w.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(ViSlI3tNgu2aFVB8BXYUxf9w);

void ViSlI3tNgu2aFVB8BXYUxf9w::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	case 2: ::typy::Clear(_value2); break;
	}
	_tag = 0;
}

void ViSlI3tNgu2aFVB8BXYUxf9w::CopyFrom(const ViSlI3tNgu2aFVB8BXYUxf9w& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::CopyFrom(_value1, from._value1); break;
	case 2: ::typy::CopyFrom(_value2, from._value2); break;
	}
	_tag = from._tag;
}

void ViSlI3tNgu2aFVB8BXYUxf9w::MergeFrom(const ViSlI3tNgu2aFVB8BXYUxf9w& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) {
		GOOGLE_CHECK(false) << __FILE__ << ":" << __LINE__;
	}
	if (from._tag == 0) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::MergeFrom(_value1, from._value1); break;
	case 2: ::typy::MergeFrom(_value2, from._value2); break;
	}
	_tag = from._tag;
}

void ViSlI3tNgu2aFVB8BXYUxf9w::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	case 2: ::typy::Write(_tag, _value2, output); break;
	}
}

int ViSlI3tNgu2aFVB8BXYUxf9w::ByteSize() const {
	int total_size = 0;
	switch (_tag) {
	case 1: ::typy::ByteSize(total_size, 1, _value1); break;
	case 2: ::typy::ByteSize(total_size, 1, _value2); break;
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool ViSlI3tNgu2aFVB8BXYUxf9w::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(2)
	FIRST_READ_VARIANT_CASE(1, _value1, int32)
	NEXT_READ_VARIANT_CASE(2, _value2, List< Vfs >)
	END_READ_CASE()
}

PyObject* ViSlI3tNgu2aFVB8BXYUxf9w::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	case 2: return ::typy::GetPyObject(_value2);
	default: Py_RETURN_NONE;
	}
}

bool ViSlI3tNgu2aFVB8BXYUxf9w::fromPyObject(PyObject* value) {
	if (PyBool_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		::typy::CopyFrom(_value1, PyInt_AsLong(value));
		_tag = 1;
		return true;
	}
	if (PyInt_Check(value) || PyLong_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		::typy::CopyFrom(_value1, PyInt_AsLong(value));
		_tag = 1;
		return true;
	}
	if (PyFloat_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		::typy::CopyFrom(_value1, PyInt_AsLong(value));
		_tag = 1;
		return true;
	}
	if (PyObject_TypeCheck(value, &List< Vfs >::_Type) || PySequence_Check(value)) {
		if (_tag != 0 && _tag != 2) { Clear(); }
		if (::typy::CheckAndSet(value, _value2, "List of Variant expect List(Vfs), but ")) {
			_tag = 2;
			return true;
		}
		return false;
	}
	return false;
}

bool CheckAndSet(PyObject* arg, ViSlI3tNgu2aFVB8BXYUxf9w*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new ViSlI3tNgu2aFVB8BXYUxf9w;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(ViSlI3tNgu2aFVB8BXYUxf9w);
