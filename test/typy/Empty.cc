// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "Empty.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Empty);

Empty::Empty() : Message() {
	PyObject_INIT(this, &Object<Empty>::_Type);
	ZERO(Empty, _cached_size, _cached_size);
}

void Empty::Clear() {
	
}

void Empty::MergeFrom(const Empty& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	
}

void Empty::SerializeWithCachedSizes(CodedOutputStream* output) const {
	
}

int Empty::ByteSize() const {
	int total_size = 0;
	
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Empty::MergePartialFromCodedStream(CodedInputStream* input) {
	input->ExpectAtEnd();
	return true;
}

PyObject* Empty::Json(bool slim) {
	PyObject* json = PyDict_New();
	if (json == NULL) { return NULL; }
	PyObject* value = PyString_FromString(Empty::Name);
	if (value == NULL) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	
	return json;
}

Empty* Empty::FromJson(PyObject* json) {
	if (!PyObject_HasAttrString(json, "iteritems")) {
		FormatTypeError(json, "FromJson expect dict, but ");
		return NULL;
	}
	PyObject* value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("_t")).get());
	if (value == NULL) {
		FormatTypeError(json, "Json expect _t, ");
		return NULL;
	} else if (!PyBytes_Check(value)) {
		FormatTypeError(value, "Json _t expect String, but ");
		return NULL;
	} else if (strcmp(PyBytes_AS_STRING(value), Empty::Name)) {
		PyErr_Format(PyExc_TypeError, "Object expect '%.100s', but Json has type %.100s",
			Empty::Name, PyBytes_AS_STRING(value));
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);
	Empty* object = new Empty();
	
	return object;
}

// ===================================================================

const int Empty::PropertyCount = 0;
char* Empty::Properties[] = {
	""
};

int Empty::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
	case 0: break;
	}
	return size;
}

void Empty::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 0: break;
	}
}

int Empty::DeserializeProperty(CodedInputStream* input) {
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
	case 0: break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

bool Empty::SetPropertySequence(PyObject* args) {
	for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++) {
		switch(i) {
		case 0: break;
		default: PyErr_Format(PyExc_TypeError, "Unsurported property number %lu.", i); return false;
		}
	}
	return true;
}

PyObject* Empty::GetPropertySequence() {
	PyObject* result = PyTuple_New(0);
	if (result == NULL) { return result; }
	
	return result;
}

// ===================================================================



template <> PyGetSetDef Object<Empty>::GetSet[] = {
	
	{NULL}
};

TypyObjectEnd(Empty);
