// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "onInitRuntime.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(onInitRuntime);

onInitRuntime::onInitRuntime() : Message() {
	PyObject_INIT(this, &Object<onInitRuntime>::_Type);
	ZERO(onInitRuntime, p_buckID, _cached_size);
}

void onInitRuntime::Clear() {
	::typy::Clear(p_buckID);
	::typy::Clear(p_events);
	::typy::Clear(p_flowID);
	::typy::Clear(p_ownerID);
	::typy::Clear(p_params);
	::typy::Clear(p_passBuckEvents);
	::typy::Clear(p_prev);
	::typy::Clear(p_skillID);
	::typy::Clear(p_skillIdx);
	::typy::Clear(p_specificEvents);
	::typy::Clear(p_uiid);
	::typy::Clear(p_unitIndex);
}

void onInitRuntime::MergeFrom(const onInitRuntime& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	::typy::MergeFrom(p_buckID, from.p_buckID);
	::typy::MergeFrom(p_events, from.p_events);
	::typy::MergeFrom(p_flowID, from.p_flowID);
	::typy::MergeFrom(p_ownerID, from.p_ownerID);
	::typy::MergeFrom(p_params, from.p_params);
	::typy::MergeFrom(p_passBuckEvents, from.p_passBuckEvents);
	::typy::MergeFrom(p_prev, from.p_prev);
	::typy::MergeFrom(p_skillID, from.p_skillID);
	::typy::MergeFrom(p_skillIdx, from.p_skillIdx);
	::typy::MergeFrom(p_specificEvents, from.p_specificEvents);
	::typy::MergeFrom(p_uiid, from.p_uiid);
	::typy::MergeFrom(p_unitIndex, from.p_unitIndex);
}

void onInitRuntime::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_buckID, output);
	::typy::Write(2, p_events, output);
	::typy::Write(3, p_flowID, output);
	::typy::Write(4, p_ownerID, output);
	::typy::Write(5, p_params, output);
	::typy::Write(6, p_passBuckEvents, output);
	::typy::Write(7, p_prev, output);
	::typy::Write(8, p_skillID, output);
	::typy::Write(9, p_skillIdx, output);
	::typy::Write(10, p_specificEvents, output);
	::typy::Write(11, p_uiid, output);
	::typy::Write(12, p_unitIndex, output);
}

int onInitRuntime::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, p_buckID);
	::typy::ByteSize(total_size, 1, p_events);
	::typy::ByteSize(total_size, 1, p_flowID);
	::typy::ByteSize(total_size, 1, p_ownerID);
	::typy::ByteSize(total_size, 1, p_params);
	::typy::ByteSize(total_size, 1, p_passBuckEvents);
	::typy::ByteSize(total_size, 1, p_prev);
	::typy::ByteSize(total_size, 1, p_skillID);
	::typy::ByteSize(total_size, 1, p_skillIdx);
	::typy::ByteSize(total_size, 1, p_specificEvents);
	::typy::ByteSize(total_size, 1, p_uiid);
	::typy::ByteSize(total_size, 1, p_unitIndex);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool onInitRuntime::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(12)
	FIRST_READ_NORMAL_CASE(1, p_buckID, bytes)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(2, p_events, int32)
	NEXT_READ_NORMAL_CASE(3, p_flowID, int32)
	NEXT_READ_NORMAL_CASE(4, p_ownerID, bytes)
	NEXT_READ_REPEATED_OBJECT_CASE(5, p_params, SkillParam, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(6, p_passBuckEvents, int32)
	NEXT_READ_NORMAL_CASE(7, p_prev, int32)
	NEXT_READ_NORMAL_CASE(8, p_skillID, int32)
	NEXT_READ_NORMAL_CASE(9, p_skillIdx, int32)
	NEXT_READ_REPEATED_OBJECT_CASE(10, p_specificEvents, SINGLE_ARG(Dict<bytes, List< int32 > >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(11, p_uiid, int32)
	NEXT_READ_NORMAL_CASE(12, p_unitIndex, int32)
	END_READ_CASE()
}

PyObject* onInitRuntime::Json(bool slim) {
	PyObject* json = PyDict_New();
	if (json == NULL) { return NULL; }
	PyObject* value = PyString_FromString(onInitRuntime::Name);
	if (value == NULL) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	value = ::typy::Json(p_buckID, slim);
	if (value != NULL) { PyDict_SetItemString(json, "buckID", value); Py_DECREF(value); }
	value = ::typy::Json(p_events, slim);
	if (value != NULL) { PyDict_SetItemString(json, "events", value); Py_DECREF(value); }
	value = ::typy::Json(p_flowID, slim);
	if (value != NULL) { PyDict_SetItemString(json, "flowID", value); Py_DECREF(value); }
	value = ::typy::Json(p_ownerID, slim);
	if (value != NULL) { PyDict_SetItemString(json, "ownerID", value); Py_DECREF(value); }
	value = ::typy::Json(p_params, slim);
	if (value != NULL) { PyDict_SetItemString(json, "params", value); Py_DECREF(value); }
	value = ::typy::Json(p_passBuckEvents, slim);
	if (value != NULL) { PyDict_SetItemString(json, "passBuckEvents", value); Py_DECREF(value); }
	value = ::typy::Json(p_prev, slim);
	if (value != NULL) { PyDict_SetItemString(json, "prev", value); Py_DECREF(value); }
	value = ::typy::Json(p_skillID, slim);
	if (value != NULL) { PyDict_SetItemString(json, "skillID", value); Py_DECREF(value); }
	value = ::typy::Json(p_skillIdx, slim);
	if (value != NULL) { PyDict_SetItemString(json, "skillIdx", value); Py_DECREF(value); }
	value = ::typy::Json(p_specificEvents, slim);
	if (value != NULL) { PyDict_SetItemString(json, "specificEvents", value); Py_DECREF(value); }
	value = ::typy::Json(p_uiid, slim);
	if (value != NULL) { PyDict_SetItemString(json, "uiid", value); Py_DECREF(value); }
	value = ::typy::Json(p_unitIndex, slim);
	if (value != NULL) { PyDict_SetItemString(json, "unitIndex", value); Py_DECREF(value); }
	return json;
}

onInitRuntime* onInitRuntime::FromJson(PyObject* json) {
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
	} else if (strcmp(PyBytes_AS_STRING(value), onInitRuntime::Name)) {
		PyErr_Format(PyExc_TypeError, "Object expect '%.100s', but Json has type %.100s",
			onInitRuntime::Name, PyBytes_AS_STRING(value));
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);
	onInitRuntime* object = new onInitRuntime();
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("buckID")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_buckID, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("events")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_events, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("flowID")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_flowID, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("ownerID")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_ownerID, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("params")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_params, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("passBuckEvents")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_passBuckEvents, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("prev")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_prev, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("skillID")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_skillID, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("skillIdx")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_skillIdx, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("specificEvents")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_specificEvents, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("uiid")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_uiid, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("unitIndex")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_unitIndex, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	return object;
}

// ===================================================================

const int onInitRuntime::PropertyCount = 12;
char* onInitRuntime::Properties[] = {
	"buckID",
	"events",
	"flowID",
	"ownerID",
	"params",
	"passBuckEvents",
	"prev",
	"skillID",
	"skillIdx",
	"specificEvents",
	"uiid",
	"unitIndex"
};

int onInitRuntime::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
	case 1: ::typy::ByteSize(size, 1, p_buckID); if (size == 0) { size = 1; } break;
	case 2: ::typy::ByteSize(size, 1, p_events); if (size == 0) { size = 1; } break;
	case 3: ::typy::ByteSize(size, 1, p_flowID); if (size == 0) { size = 1; } break;
	case 4: ::typy::ByteSize(size, 1, p_ownerID); if (size == 0) { size = 1; } break;
	case 5: ::typy::ByteSize(size, 1, p_params); if (size == 0) { size = 1; } break;
	case 6: ::typy::ByteSize(size, 1, p_passBuckEvents); if (size == 0) { size = 1; } break;
	case 7: ::typy::ByteSize(size, 1, p_prev); if (size == 0) { size = 1; } break;
	case 8: ::typy::ByteSize(size, 1, p_skillID); if (size == 0) { size = 1; } break;
	case 9: ::typy::ByteSize(size, 1, p_skillIdx); if (size == 0) { size = 1; } break;
	case 10: ::typy::ByteSize(size, 1, p_specificEvents); if (size == 0) { size = 1; } break;
	case 11: ::typy::ByteSize(size, 1, p_uiid); if (size == 0) { size = 1; } break;
	case 12: ::typy::ByteSize(size, 1, p_unitIndex); if (size == 0) { size = 1; } break;
	}
	return size;
}

void onInitRuntime::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 1:
		::typy::Write(1, p_buckID, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(1, p_buckID, output);
		}
		break;
	case 2:
		::typy::Write(2, p_events, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(2, p_events, output);
		}
		break;
	case 3:
		::typy::Write(3, p_flowID, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(3, p_flowID, output);
		}
		break;
	case 4:
		::typy::Write(4, p_ownerID, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(4, p_ownerID, output);
		}
		break;
	case 5:
		::typy::Write(5, p_params, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(5, p_params, output);
		}
		break;
	case 6:
		::typy::Write(6, p_passBuckEvents, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(6, p_passBuckEvents, output);
		}
		break;
	case 7:
		::typy::Write(7, p_prev, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(7, p_prev, output);
		}
		break;
	case 8:
		::typy::Write(8, p_skillID, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(8, p_skillID, output);
		}
		break;
	case 9:
		::typy::Write(9, p_skillIdx, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(9, p_skillIdx, output);
		}
		break;
	case 10:
		::typy::Write(10, p_specificEvents, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(10, p_specificEvents, output);
		}
		break;
	case 11:
		::typy::Write(11, p_uiid, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(11, p_uiid, output);
		}
		break;
	case 12:
		::typy::Write(12, p_unitIndex, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(12, p_unitIndex, output);
		}
		break;
	}
}

int onInitRuntime::DeserializeProperty(CodedInputStream* input) {
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
	case 1: ::typy::Clear(p_buckID); break;
	case 2: ::typy::Clear(p_events); break;
	case 3: ::typy::Clear(p_flowID); break;
	case 4: ::typy::Clear(p_ownerID); break;
	case 5: ::typy::Clear(p_params); break;
	case 6: ::typy::Clear(p_passBuckEvents); break;
	case 7: ::typy::Clear(p_prev); break;
	case 8: ::typy::Clear(p_skillID); break;
	case 9: ::typy::Clear(p_skillIdx); break;
	case 10: ::typy::Clear(p_specificEvents); break;
	case 11: ::typy::Clear(p_uiid); break;
	case 12: ::typy::Clear(p_unitIndex); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

bool onInitRuntime::SetPropertySequence(PyObject* args) {
	for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++) {
		switch(i) {
		case 0: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 0), p_buckID, "Property 'buckID' expect bytes, but ")) { return false; } break;
		case 1: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 1), p_events, "Property 'events' expect List(int32), but ")) { return false; } break;
		case 2: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 2), p_flowID, "Property 'flowID' expect int32, but ")) { return false; } break;
		case 3: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 3), p_ownerID, "Property 'ownerID' expect bytes, but ")) { return false; } break;
		case 4: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 4), p_params, "Property 'params' expect List(SkillParam), but ")) { return false; } break;
		case 5: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 5), p_passBuckEvents, "Property 'passBuckEvents' expect List(int32), but ")) { return false; } break;
		case 6: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 6), p_prev, "Property 'prev' expect int32, but ")) { return false; } break;
		case 7: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 7), p_skillID, "Property 'skillID' expect int32, but ")) { return false; } break;
		case 8: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 8), p_skillIdx, "Property 'skillIdx' expect int32, but ")) { return false; } break;
		case 9: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 9), p_specificEvents, "Property 'specificEvents' expect Dict(bytes -> List< int32 >), but ")) { return false; } break;
		case 10: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 10), p_uiid, "Property 'uiid' expect int32, but ")) { return false; } break;
		case 11: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 11), p_unitIndex, "Property 'unitIndex' expect int32, but ")) { return false; } break;
		default: PyErr_Format(PyExc_TypeError, "Unsurported property number %lu.", i); return false;
		}
	}
	return true;
}

PyObject* onInitRuntime::GetPropertySequence() {
	PyObject* result = PyTuple_New(12);
	if (result == NULL) { return result; }
	PyTuple_SET_ITEM(result, 0, ::typy::GetPyObject(p_buckID));
	PyTuple_SET_ITEM(result, 1, ::typy::GetPyObject(p_events));
	PyTuple_SET_ITEM(result, 2, ::typy::GetPyObject(p_flowID));
	PyTuple_SET_ITEM(result, 3, ::typy::GetPyObject(p_ownerID));
	PyTuple_SET_ITEM(result, 4, ::typy::GetPyObject(p_params));
	PyTuple_SET_ITEM(result, 5, ::typy::GetPyObject(p_passBuckEvents));
	PyTuple_SET_ITEM(result, 6, ::typy::GetPyObject(p_prev));
	PyTuple_SET_ITEM(result, 7, ::typy::GetPyObject(p_skillID));
	PyTuple_SET_ITEM(result, 8, ::typy::GetPyObject(p_skillIdx));
	PyTuple_SET_ITEM(result, 9, ::typy::GetPyObject(p_specificEvents));
	PyTuple_SET_ITEM(result, 10, ::typy::GetPyObject(p_uiid));
	PyTuple_SET_ITEM(result, 11, ::typy::GetPyObject(p_unitIndex));
	return result;
}

// ===================================================================

TYPY_GETSET(onInitRuntime, p_buckID, bytes);
TYPY_GETSET(onInitRuntime, p_events, List(int32));
TYPY_GETSET(onInitRuntime, p_flowID, int32);
TYPY_GETSET(onInitRuntime, p_ownerID, bytes);
TYPY_GETSET(onInitRuntime, p_params, List(SkillParam));
TYPY_GETSET(onInitRuntime, p_passBuckEvents, List(int32));
TYPY_GETSET(onInitRuntime, p_prev, int32);
TYPY_GETSET(onInitRuntime, p_skillID, int32);
TYPY_GETSET(onInitRuntime, p_skillIdx, int32);
TYPY_GETSET(onInitRuntime, p_specificEvents, Dict(bytes -> List< int32 >));
TYPY_GETSET(onInitRuntime, p_uiid, int32);
TYPY_GETSET(onInitRuntime, p_unitIndex, int32);

template <> PyGetSetDef Object<onInitRuntime>::GetSet[] = {
	{"buckID", (getter)Get_p_buckID, (setter)Set_p_buckID, "Property buckID"},
	{"events", (getter)Get_p_events, (setter)Set_p_events, "Property events"},
	{"flowID", (getter)Get_p_flowID, (setter)Set_p_flowID, "Property flowID"},
	{"ownerID", (getter)Get_p_ownerID, (setter)Set_p_ownerID, "Property ownerID"},
	{"params", (getter)Get_p_params, (setter)Set_p_params, "Property params"},
	{"passBuckEvents", (getter)Get_p_passBuckEvents, (setter)Set_p_passBuckEvents, "Property passBuckEvents"},
	{"prev", (getter)Get_p_prev, (setter)Set_p_prev, "Property prev"},
	{"skillID", (getter)Get_p_skillID, (setter)Set_p_skillID, "Property skillID"},
	{"skillIdx", (getter)Get_p_skillIdx, (setter)Set_p_skillIdx, "Property skillIdx"},
	{"specificEvents", (getter)Get_p_specificEvents, (setter)Set_p_specificEvents, "Property specificEvents"},
	{"uiid", (getter)Get_p_uiid, (setter)Set_p_uiid, "Property uiid"},
	{"unitIndex", (getter)Get_p_unitIndex, (setter)Set_p_unitIndex, "Property unitIndex"},
	{NULL}
};

TypyObjectEnd(onInitRuntime);
