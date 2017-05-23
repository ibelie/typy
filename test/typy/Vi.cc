// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "Vi.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(Vi);

void Vi::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	}
	_tag = 0;
}

int Vi::Visit(visitproc visit, void* arg) {
	register int result = 0;
	switch (_tag) {
	case 1: result = ::typy::Visit(_value1, visit, arg); break;
	}
	return result;
}

void Vi::CopyFrom(const Vi& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::CopyFrom(_value1, from._value1); break;
	}
	_tag = from._tag;
}

void Vi::MergeFrom(const Vi& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) {
		GOOGLE_CHECK(false) << __FILE__ << ":" << __LINE__;
	}
	if (from._tag == 0) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::MergeFrom(_value1, from._value1); break;
	}
	_tag = from._tag;
}

void Vi::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	}
}

int Vi::ByteSize() const {
	int total_size = 0;
	switch (_tag) {
	case 1: ::typy::ByteSize(total_size, 1, _value1); break;
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Vi::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(1)
	FIRST_READ_VARIANT_CASE(1, _value1, int32)
	END_READ_CASE()
}

PyObject* Vi::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	default: Py_RETURN_NONE;
	}
}

bool Vi::fromPyObject(PyObject* value) {
	if (value == NULL || value == Py_None) {
		Clear();
		return true;
	}
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
	return false;
}

PyObject* Vi::Json(bool slim) {
	if (!slim || _tag != 0) {
		switch (_tag) {
		case 1: return ::typy::Json(_value1, slim);
		default: Py_RETURN_NONE;
		}
	} else {
		return NULL;
	}
}

Vi* Vi::FromJson(PyObject* json) {
	Vi* object = new Vi;
	if (PyObject_HasAttrString(json, "iteritems")) {
		ScopedPyObjectPtr _t(PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("_t")).get()));
		PyErr_Clear();
		ScopedPyObjectPtr _b;
		if (_t == NULL) {
		} else if (PyBytes_Check(_t.get())) {
			Py_INCREF(_t.get());
			_b.reset(_t.get());
		} else if (PyUnicode_Check(_t.get())) {
			_b.reset(PyUnicode_AsEncodedObject(_t.get(), "utf-8", NULL));
		}
		if (_b != NULL) {
			
		}
		PyErr_Clear();
	} else if (PyObject_HasAttrString(json, "__iter__")) {
	} else if (object->fromPyObject(json)) { return object; }
	delete object;
	FormatTypeError(json, "Variant.FromJson error, ");
	return NULL;
}

bool CheckAndSet(PyObject* arg, Vi*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new Vi;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(Vi);
