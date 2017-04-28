// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "VdesVector2.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(VdesVector2);

void VdesVector2::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	case 2: ::typy::Clear(_value2); break;
	case 3: ::typy::Clear(_value3); break;
	case 4: ::typy::Clear(_value4); break;
	}
	_tag = 0;
}

void VdesVector2::CopyFrom(const VdesVector2& from) {
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

void VdesVector2::MergeFrom(const VdesVector2& from) {
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

void VdesVector2::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	case 2: ::typy::Write(_tag, _value2, output); break;
	case 3: ::typy::Write(_tag, _value3, output); break;
	case 4: ::typy::Write(_tag, _value4, output); break;
	}
}

int VdesVector2::ByteSize() const {
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

bool VdesVector2::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(4)
	FIRST_READ_VARIANT_CASE(1, _value1, double)
	NEXT_READ_VARIANT_CASE(2, _value2, Corpus)
	NEXT_READ_VARIANT_CASE(3, _value3, string)
	NEXT_READ_VARIANT_CASE(4, _value4, Vector2)
	END_READ_CASE()
}

PyObject* VdesVector2::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	case 2: return ::typy::GetPyObject(_value2);
	case 3: return ::typy::GetPyObject(_value3);
	case 4: return ::typy::GetPyObject(_value4);
	default: Py_RETURN_NONE;
	}
}

bool VdesVector2::fromPyObject(PyObject* value) {
	if (value == NULL || value == Py_None) {
		Clear();
		return true;
	}
	if (PyBool_Check(value)) {
		if (_tag != 0 && _tag != 2) { Clear(); }
		::typy::CopyFrom(_value2, static_cast<Corpus>(PyInt_AsLong(value)));
		_tag = 2;
		return true;
	}
	if (PyInt_Check(value) || PyLong_Check(value)) {
		if (_tag != 0 && _tag != 2) { Clear(); }
		::typy::CopyFrom(_value2, static_cast<Corpus>(PyInt_AsLong(value)));
		_tag = 2;
		return true;
	}
	if (PyFloat_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		::typy::CopyFrom(_value1, PyFloat_AsDouble(value));
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

PyObject* VdesVector2::Json(bool slim) {
	if (!slim || _tag != 0) {
		switch (_tag) {
		case 1: return ::typy::Json(_value1, slim);
		case 2: return ::typy::Json(_value2, slim);
		case 3: return ::typy::Json(_value3, slim);
		case 4: return ::typy::Json(_value4, slim);
		default: Py_RETURN_NONE;
		}
	} else {
		return NULL;
	}
}

VdesVector2* VdesVector2::FromJson(PyObject* json) {
	VdesVector2* object = new VdesVector2;
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
			if (!strcmp(PyBytes_AS_STRING(_b.get()), "Vector2")) {
				if (::typy::FromJson(object->_value4, json)) { object->_tag = 4; return object; }
			}
		}
		PyErr_Clear();
	} else if (PyObject_HasAttrString(json, "__iter__")) {
	} else if (object->fromPyObject(json)) { return object; }
	delete object;
	FormatTypeError(json, "Variant.FromJson error, ");
	return NULL;
}

bool CheckAndSet(PyObject* arg, VdesVector2*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new VdesVector2;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(VdesVector2);
