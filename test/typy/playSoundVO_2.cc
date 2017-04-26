// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "playSoundVO_2.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(playSoundVO_2);

playSoundVO_2::playSoundVO_2() : Message() {
	PyObject_INIT(this, &Object<playSoundVO_2>::_Type);
	ZERO(playSoundVO_2, p_path, _cached_size);
}

void playSoundVO_2::Clear() {
	::typy::Clear(p_path);
}

void playSoundVO_2::MergeFrom(const playSoundVO_2& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	::typy::MergeFrom(p_path, from.p_path);
}

void playSoundVO_2::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_path, output);
}

int playSoundVO_2::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, p_path);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool playSoundVO_2::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(1)
	FIRST_READ_NORMAL_CASE(1, p_path, string)
	END_READ_CASE()
}

PyObject* playSoundVO_2::Json(bool slim) {
	PyObject* json = PyDict_New();
	if (json == NULL) { return NULL; }
	PyObject* value = PyString_FromString(playSoundVO_2::Name);
	if (value == NULL) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	value = ::typy::Json(p_path, slim);
	if (value != NULL) { PyDict_SetItemString(json, "path", value); Py_DECREF(value); }
	return json;
}

playSoundVO_2* playSoundVO_2::FromJson(PyObject* json) {
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
	if (strcmp(PyBytes_AS_STRING(value), playSoundVO_2::Name)) {
		PyErr_Format(PyExc_TypeError, "Object expect '%.100s', but Json has type %.100s",
			playSoundVO_2::Name, PyBytes_AS_STRING(value));
		Py_DECREF(value);
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);
	playSoundVO_2* object = new playSoundVO_2();
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("path")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_path, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	return object;
}

// ===================================================================

const int playSoundVO_2::PropertyCount = 1;
char* playSoundVO_2::Properties[] = {
	"path"
};

int playSoundVO_2::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
	case 1: ::typy::ByteSize(size, 1, p_path); if (size == 0) { size = 1; } break;
	}
	return size;
}

void playSoundVO_2::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 1:
		::typy::Write(1, p_path, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(1, p_path, output);
		}
		break;
	}
}

int playSoundVO_2::DeserializeProperty(CodedInputStream* input) {
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
	case 1: ::typy::Clear(p_path); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

bool playSoundVO_2::SetPropertySequence(PyObject* args) {
	for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++) {
		switch(i) {
		case 0: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 0), p_path, "Property 'path' expect string, but ")) { return false; } break;
		default: PyErr_Format(PyExc_TypeError, "Unsurported property number %lu.", i); return false;
		}
	}
	return true;
}

PyObject* playSoundVO_2::GetPropertySequence() {
	PyObject* result = PyTuple_New(1);
	if (result == NULL) { return result; }
	PyTuple_SET_ITEM(result, 0, ::typy::GetPyObject(p_path));
	return result;
}

// ===================================================================

TYPY_GETSET(playSoundVO_2, p_path, string);

template <> PyGetSetDef Object<playSoundVO_2>::GetSet[] = {
	{"path", (getter)Get_p_path, (setter)Set_p_path, "Property path"},
	{NULL}
};

TypyObjectEnd(playSoundVO_2);
