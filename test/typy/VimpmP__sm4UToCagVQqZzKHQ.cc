
#include "VimpmP__sm4UToCagVQqZzKHQ.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(VimpmP__sm4UToCagVQqZzKHQ);

void VimpmP__sm4UToCagVQqZzKHQ::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	case 2: ::typy::Clear(_value2); break;
	}
	_tag = 0;
}

void VimpmP__sm4UToCagVQqZzKHQ::CopyFrom(const VimpmP__sm4UToCagVQqZzKHQ& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::CopyFrom(_value1, from._value1); break;
	case 2: ::typy::CopyFrom(_value2, from._value2); break;
	}
	_tag = from._tag;
}

void VimpmP__sm4UToCagVQqZzKHQ::MergeFrom(const VimpmP__sm4UToCagVQqZzKHQ& from) {
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

void VimpmP__sm4UToCagVQqZzKHQ::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	case 2: ::typy::Write(_tag, _value2, output); break;
	}
}

int VimpmP__sm4UToCagVQqZzKHQ::ByteSize() const {
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

bool VimpmP__sm4UToCagVQqZzKHQ::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(2)
	FIRST_READ_VARIANT_CASE(1, _value1, SINGLE_ARG(Dict<int32, Vfs >))
	NEXT_READ_VARIANT_CASE(2, _value2, int32)
	END_READ_CASE()
}

PyObject* VimpmP__sm4UToCagVQqZzKHQ::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	case 2: return ::typy::GetPyObject(_value2);
	default: Py_RETURN_NONE;
	}
}

bool VimpmP__sm4UToCagVQqZzKHQ::fromPyObject(PyObject* value) {
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
		if (_tag != 0 && _tag != 2) { Clear(); }
		::typy::CopyFrom(_value2, PyInt_AsLong(value));
		_tag = 2;
		return true;
	}
	if (PyObject_TypeCheck(value, &SINGLE_ARG(Dict<int32, Vfs >)::_Type) || PyDict_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		if (::typy::CheckAndSet(value, _value1, "Dict of Variant expect Dict(int32 -> Vfs), but ")) {
			_tag = 1;
			return true;
		}
		return false;
	}
	return false;
}

bool CheckAndSet(PyObject* arg, VimpmP__sm4UToCagVQqZzKHQ*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new VimpmP__sm4UToCagVQqZzKHQ;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(VimpmP__sm4UToCagVQqZzKHQ);
