// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "ViLVfs.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(ViLVfs);

void ViLVfs::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	case 2: ::typy::Clear(_value2); break;
	}
	_tag = 0;
}

void ViLVfs::CopyFrom(const ViLVfs& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::CopyFrom(_value1, from._value1); break;
	case 2: ::typy::CopyFrom(_value2, from._value2); break;
	}
	_tag = from._tag;
}

void ViLVfs::MergeFrom(const ViLVfs& from) {
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

void ViLVfs::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	case 2: ::typy::Write(_tag, _value2, output); break;
	}
}

int ViLVfs::ByteSize() const {
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

bool ViLVfs::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(2)
	FIRST_READ_VARIANT_CASE(1, _value1, int32)
	NEXT_READ_VARIANT_CASE(2, _value2, List< Vfs >)
	END_READ_CASE()
}

PyObject* ViLVfs::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	case 2: return ::typy::GetPyObject(_value2);
	default: Py_RETURN_NONE;
	}
}

bool ViLVfs::fromPyObject(PyObject* value) {
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

PyObject* ViLVfs::Json(bool slim) {
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

ViLVfs* ViLVfs::FromJson(PyObject* json) {
	ViLVfs* object = new ViLVfs;
	if (PyObject_HasAttrString(json, "iteritems")) {
		ScopedPyObjectPtr _t(PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("_t")).get()));
		if (PyBytes_Check(_t.get())) {
			
		}
		PyErr_Clear();
	} else if (PyObject_HasAttrString(json, "__iter__")) {
		if (::typy::FromJson(object->_value2, json)) { return object; }
	} else if (object->fromPyObject(json)) { return object; }
	delete object;
	FormatTypeError(json, "Variant.FromJson error, ");
	return NULL;
}

bool CheckAndSet(PyObject* arg, ViLVfs*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new ViLVfs;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(ViLVfs);
