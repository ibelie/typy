
#include "VfisVector2.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(VfisVector2);

void VfisVector2::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	case 2: ::typy::Clear(_value2); break;
	case 3: ::typy::Clear(_value3); break;
	case 4: ::typy::Clear(_value4); break;
	}
	_tag = 0;
}

void VfisVector2::CopyFrom(const VfisVector2& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::CopyFrom(_value1, from._value1); break;
	case 2: ::typy::CopyFrom(_value2, from._value2); break;
	case 3: ::typy::CopyFrom(_value3, from._value3); break;
	case 4: ::typy::CopyFrom(_value4, from._value4); break;
	}
	_tag = from._tag;
}

void VfisVector2::MergeFrom(const VfisVector2& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) {
		GOOGLE_CHECK(false) << __FILE__ << ":" << __LINE__;
	}
	if (from._tag == 0) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::MergeFrom(_value1, from._value1); break;
	case 2: ::typy::MergeFrom(_value2, from._value2); break;
	case 3: ::typy::MergeFrom(_value3, from._value3); break;
	case 4: ::typy::MergeFrom(_value4, from._value4); break;
	}
	_tag = from._tag;
}

void VfisVector2::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	case 2: ::typy::Write(_tag, _value2, output); break;
	case 3: ::typy::Write(_tag, _value3, output); break;
	case 4: ::typy::Write(_tag, _value4, output); break;
	}
}

int VfisVector2::ByteSize() const {
	int total_size = 0;
	switch (_tag) {
	case 1: ::typy::ByteSize(total_size, 1, _value1); break;
	case 2: ::typy::ByteSize(total_size, 1, _value2); break;
	case 3: ::typy::ByteSize(total_size, 1, _value3); break;
	case 4: ::typy::ByteSize(total_size, 1, _value4); break;
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool VfisVector2::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(4)
	FIRST_READ_VARIANT_CASE(1, _value1, float)
	NEXT_READ_VARIANT_CASE(2, _value2, int32)
	NEXT_READ_VARIANT_CASE(3, _value3, string)
	NEXT_READ_VARIANT_CASE(4, _value4, Vector2)
	END_READ_CASE()
}

PyObject* VfisVector2::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	case 2: return ::typy::GetPyObject(_value2);
	case 3: return ::typy::GetPyObject(_value3);
	case 4: return ::typy::GetPyObject(_value4);
	default: Py_RETURN_NONE;
	}
}

bool VfisVector2::fromPyObject(PyObject* value) {
	if (PyBool_Check(value)) {
		if (_tag != 0 && _tag != 2) { Clear(); }
		::typy::CopyFrom(_value2, PyInt_AsLong(value));
		_tag = 2;
		return true;
	}
	if (PyInt_Check(value) || PyLong_Check(value)) {
		if (_tag != 0 && _tag != 2) { Clear(); }
		::typy::CopyFrom(_value2, PyInt_AsLong(value));
		_tag = 2;
		return true;
	}
	if (PyFloat_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		::typy::CopyFrom(_value1, static_cast<float>(PyFloat_AsDouble(value)));
		_tag = 1;
		return true;
	}
	if (PyUnicode_Check(value)) {
		if (_tag != 0 && _tag != 3) { Clear(); }
		::typy::CopyFrom(_value3, reinterpret_cast<string>(value));
		_tag = 3;
		return true;
	}
	if (PyBytes_Check(value)) {
		if (_tag != 0 && _tag != 3) { Clear(); }
		
		ScopedPyObjectPtr s(PyUnicode_FromEncodedObject(value, "utf-8", NULL));
		if (s == NULL) { return false; }
		::typy::CopyFrom(_value3, reinterpret_cast<string>(s.get()));
		_tag = 3;
		return true;
	}
	if (PyObject_TypeCheck(value, &Object<Vector2>::_Type)) {
		if (_tag != 0 && _tag != 4) { Clear(); }
		::typy::CopyFrom(_value4, static_cast<Vector2*>(value));
		_tag = 4;
		return true;
	}
	return false;
}

bool CheckAndSet(PyObject* arg, VfisVector2*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new VfisVector2;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(VfisVector2);
