// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "RPG.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(RPG);

RPG::RPG() : Message() {
	ZERO(RPG, p_fighter, _cached_size);
}

void RPG::Clear() {
	::typy::Clear(p_fighter);
}

int RPG::Visit(visitproc visit, void* arg) {
	register int result = 0;
	if((result = ::typy::Visit(p_fighter, visit, arg)) != 0) { return result; }
	return result;
}

void RPG::MergeFrom(const RPG& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	::typy::MergeFrom(p_fighter, from.p_fighter);
}

void RPG::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_fighter, output);
}

int RPG::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, p_fighter);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool RPG::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(1)
	FIRST_READ_NORMAL_CASE(1, p_fighter, Fighter)
	END_READ_CASE()
}

PyObject* RPG::Json(bool slim) {
	PyObject* json = PyDict_New();
	if (json == NULL) { return NULL; }
	PyObject* value = PyString_FromString(RPG::Name);
	if (value == NULL) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	value = ::typy::Json(p_fighter, slim);
	if (value != NULL) { PyDict_SetItemString(json, "fighter", value); Py_DECREF(value); }
	return json;
}

RPG* RPG::FromJson(PyObject* json) {
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
	} else if (!PyBytes_Check(value)) {
		FormatTypeError(value, "Json _t expect String, but ");
		return NULL;
	}
	if (strcmp(PyBytes_AS_STRING(value), RPG::Name)) {
		PyErr_Format(PyExc_TypeError, "Object expect '%.100s', but Json has type %.100s",
			RPG::Name, PyBytes_AS_STRING(value));
		Py_DECREF(value);
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);
	RPG* object = new RPG();
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("fighter")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_fighter, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	return object;
}

// ===================================================================

const int RPG::PropertyCount = 1;
char* RPG::Properties[] = {
	"fighter"
};

int RPG::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
	case 1: ::typy::ByteSize(size, 1, p_fighter); if (size == 0) { size = 1; } break;
	}
	return size;
}

void RPG::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 1:
		::typy::Write(1, p_fighter, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(1, p_fighter, output);
		}
		break;
	}
}

int RPG::DeserializeProperty(CodedInputStream* input) {
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
	case 1: ::typy::Clear(p_fighter); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

bool RPG::SetPropertySequence(PyObject* args) {
	for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++) {
		switch(i) {
		case 0: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 0), p_fighter, "Property 'fighter' expect Fighter, but ")) { return false; } break;
		default: PyErr_Format(PyExc_TypeError, "Unsurported property number %lu.", i); return false;
		}
	}
	return true;
}

PyObject* RPG::GetPropertySequence() {
	PyObject* result = PyTuple_New(1);
	if (result == NULL) { return result; }
	PyTuple_SET_ITEM(result, 0, ::typy::GetPyObject(p_fighter));
	return result;
}

// ===================================================================

TYPY_GETSET(RPG, p_fighter, Fighter);

template <> PyGetSetDef Object<RPG>::GetSet[] = {
	{"fighter", (getter)Get_p_fighter, (setter)Set_p_fighter, "Property fighter"},
	{NULL}
};

TypyObjectEnd(RPG);
