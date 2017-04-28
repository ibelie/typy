// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "VLf.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(VLf);

void VLf::Clear() {
	switch (_tag) {
	case 1: ::typy::Clear(_value1); break;
	}
	_tag = 0;
}

void VLf::CopyFrom(const VLf& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	case 1: ::typy::CopyFrom(_value1, from._value1); break;
	}
	_tag = from._tag;
}

void VLf::MergeFrom(const VLf& from) {
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

void VLf::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	case 1: ::typy::Write(_tag, _value1, output); break;
	}
}

int VLf::ByteSize() const {
	int total_size = 0;
	switch (_tag) {
	case 1: ::typy::ByteSize(total_size, 1, _value1); break;
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool VLf::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(1)
	FIRST_READ_VARIANT_CASE(1, _value1, List< float >)
	END_READ_CASE()
}

PyObject* VLf::toPyObject() {
	switch (_tag) {
	case 1: return ::typy::GetPyObject(_value1);
	default: Py_RETURN_NONE;
	}
}

bool VLf::fromPyObject(PyObject* value) {
	if (value == NULL || value == Py_None) {
		Clear();
		return true;
	}
	if (PyObject_TypeCheck(value, &List< float >::_Type) || PySequence_Check(value)) {
		if (_tag != 0 && _tag != 1) { Clear(); }
		if (::typy::CheckAndSet(value, _value1, "List of Variant expect List(float), but ")) {
			_tag = 1;
			return true;
		}
		return false;
	}
	return false;
}

PyObject* VLf::Json(bool slim) {
	if (!slim || _tag != 0) {
		switch (_tag) {
		case 1: return ::typy::Json(_value1, slim);
		default: Py_RETURN_NONE;
		}
	} else {
		return NULL;
	}
}

VLf* VLf::FromJson(PyObject* json) {
	VLf* object = new VLf;
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
		if (::typy::FromJson(object->_value1, json)) { object->_tag = 1; return object; }
	} else if (object->fromPyObject(json)) { return object; }
	delete object;
	FormatTypeError(json, "Variant.FromJson error, ");
	return NULL;
}

bool CheckAndSet(PyObject* arg, VLf*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new VLf;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(VLf);
