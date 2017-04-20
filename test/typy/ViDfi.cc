// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "ViDfi.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(ViDfi);

void ViDfi::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	case 2: ::typy::Clear(_value2); break;
	}
	_tag = 0;
}

void ViDfi::CopyFrom(const ViDfi& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::CopyFrom(_value1, from._value1); break;
	case 2: ::typy::CopyFrom(_value2, from._value2); break;
	}
	_tag = from._tag;
}

void ViDfi::MergeFrom(const ViDfi& from) {
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

void ViDfi::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	case 2: ::typy::Write(_tag, _value2, output); break;
	}
}

int ViDfi::ByteSize() const {
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

bool ViDfi::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(2)
	FIRST_READ_VARIANT_CASE(1, _value1, SINGLE_ARG(Dict<int32, float >))
	NEXT_READ_VARIANT_CASE(2, _value2, int32)
	END_READ_CASE()
}

PyObject* ViDfi::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	case 2: return ::typy::GetPyObject(_value2);
	default: Py_RETURN_NONE;
	}
}

bool ViDfi::fromPyObject(PyObject* value) {
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
	if (PyObject_TypeCheck(value, &SINGLE_ARG(Dict<int32, float >)::_Type) || PyDict_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		if (::typy::CheckAndSet(value, _value1, "Dict of Variant expect Dict(int32 -> float), but ")) {
			_tag = 1;
			return true;
		}
		return false;
	}
	return false;
}

PyObject* ViDfi::Json(bool slim) {
	if (!slim || _tag != 0) {
		switch (_tag) {
		case 1: return ::typy::Json(_value1, slim);
		case 2: return ::typy::Json(_value2, slim);
		default: Py_RETURN_NONE;
		}
	} else {
		return NULL;
	}
}

ViDfi* ViDfi::FromJson(PyObject* json) {
	ViDfi* object = new ViDfi;
	if (PyObject_HasAttrString(json, "iteritems")) {
		ScopedPyObjectPtr _t(PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("_t")).get()));
		PyErr_Clear();
		if (PyBytes_Check(_t.get())) {
			
		}
		if (::typy::FromJson(object->_value1, json)) { object->_tag = 1; return object; }
		PyErr_Clear();
	} else if (PyObject_HasAttrString(json, "__iter__")) {
	} else if (object->fromPyObject(json)) { return object; }
	delete object;
	FormatTypeError(json, "Variant.FromJson error, ");
	return NULL;
}

bool CheckAndSet(PyObject* arg, ViDfi*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new ViDfi;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(ViDfi);
