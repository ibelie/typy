// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "Fighter_Part2.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Fighter_Part2);

Fighter_Part2::Fighter_Part2() : Message() {
	ZERO(Fighter_Part2, p_bd, _cached_size);
}

void Fighter_Part2::Clear() {
	::typy::Clear(p_bd);
	::typy::Clear(p_bl);
	::typy::Clear(p_ed);
	::typy::Clear(p_el);
	::typy::Clear(p_fl);
	::typy::Clear(p_hp);
	::typy::Clear(p_isAwake);
	::typy::Clear(p_ll);
	::typy::Clear(p_pos);
	::typy::Clear(p_posi);
	::typy::Clear(p_posl);
	::typy::Clear(p_posll);
	::typy::Clear(p_poss);
	::typy::Clear(p_pyd);
	::typy::Clear(p_pyl);
	::typy::Clear(p_pyv1);
	::typy::Clear(p_pyv2);
	::typy::Clear(p_sd);
	::typy::Clear(p_sl);
}

int Fighter_Part2::Visit(visitproc visit, void* arg) {
	register int result = 0;
	if((result = ::typy::Visit(p_bd, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_bl, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_ed, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_el, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_fl, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_hp, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_isAwake, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_ll, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_pos, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_posi, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_posl, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_posll, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_poss, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_pyd, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_pyl, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_pyv1, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_pyv2, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_sd, visit, arg)) != 0) { return result; }
	if((result = ::typy::Visit(p_sl, visit, arg)) != 0) { return result; }
	return result;
}

void Fighter_Part2::MergeFrom(const Fighter_Part2& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	::typy::MergeFrom(p_bd, from.p_bd);
	::typy::MergeFrom(p_bl, from.p_bl);
	::typy::MergeFrom(p_ed, from.p_ed);
	::typy::MergeFrom(p_el, from.p_el);
	::typy::MergeFrom(p_fl, from.p_fl);
	::typy::MergeFrom(p_hp, from.p_hp);
	::typy::MergeFrom(p_isAwake, from.p_isAwake);
	::typy::MergeFrom(p_ll, from.p_ll);
	::typy::MergeFrom(p_pos, from.p_pos);
	::typy::MergeFrom(p_posi, from.p_posi);
	::typy::MergeFrom(p_posl, from.p_posl);
	::typy::MergeFrom(p_posll, from.p_posll);
	::typy::MergeFrom(p_poss, from.p_poss);
	::typy::MergeFrom(p_pyd, from.p_pyd);
	::typy::MergeFrom(p_pyl, from.p_pyl);
	::typy::MergeFrom(p_pyv1, from.p_pyv1);
	::typy::MergeFrom(p_pyv2, from.p_pyv2);
	::typy::MergeFrom(p_sd, from.p_sd);
	::typy::MergeFrom(p_sl, from.p_sl);
}

void Fighter_Part2::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_bd, output);
	::typy::Write(2, p_bl, output);
	::typy::Write(3, p_ed, output);
	::typy::Write(4, p_el, output);
	::typy::Write(5, p_fl, output);
	::typy::Write(6, p_hp, output);
	::typy::Write(7, p_isAwake, output);
	::typy::Write(8, p_ll, output);
	::typy::Write(9, p_pos, output);
	::typy::Write(10, p_posi, output);
	::typy::Write(11, p_posl, output);
	::typy::Write(12, p_posll, output);
	::typy::Write(13, p_poss, output);
	::typy::Write(14, p_pyd, output);
	::typy::Write(15, p_pyl, output);
	::typy::Write(16, p_pyv1, output);
	::typy::Write(17, p_pyv2, output);
	::typy::Write(18, p_sd, output);
	::typy::Write(19, p_sl, output);
}

int Fighter_Part2::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, p_bd);
	::typy::ByteSize(total_size, 1, p_bl);
	::typy::ByteSize(total_size, 1, p_ed);
	::typy::ByteSize(total_size, 1, p_el);
	::typy::ByteSize(total_size, 1, p_fl);
	::typy::ByteSize(total_size, 1, p_hp);
	::typy::ByteSize(total_size, 1, p_isAwake);
	::typy::ByteSize(total_size, 1, p_ll);
	::typy::ByteSize(total_size, 1, p_pos);
	::typy::ByteSize(total_size, 1, p_posi);
	::typy::ByteSize(total_size, 1, p_posl);
	::typy::ByteSize(total_size, 1, p_posll);
	::typy::ByteSize(total_size, 1, p_poss);
	::typy::ByteSize(total_size, 1, p_pyd);
	::typy::ByteSize(total_size, 1, p_pyl);
	::typy::ByteSize(total_size, 2, p_pyv1);
	::typy::ByteSize(total_size, 2, p_pyv2);
	::typy::ByteSize(total_size, 2, p_sd);
	::typy::ByteSize(total_size, 2, p_sl);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Fighter_Part2::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(19)
	FIRST_READ_REPEATED_OBJECT_CASE(1, p_bd, SINGLE_ARG(Dict<string, bytes >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(2, p_bl, bytes)
	NEXT_READ_REPEATED_OBJECT_CASE(3, p_ed, SINGLE_ARG(Dict<int64, Corpus >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(4, p_el, Corpus)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(5, p_fl, float)
	NEXT_READ_NORMAL_CASE(6, p_hp, float)
	NEXT_READ_NORMAL_CASE(7, p_isAwake, bool)
	NEXT_READ_REPEATED_OBJECT_CASE(8, p_ll, VLf, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(9, p_pos, Vector2)
	NEXT_READ_REPEATED_OBJECT_CASE(10, p_posi, SINGLE_ARG(Dict<int32, float >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(11, p_posl, Vector2, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(12, p_posll, VLVector2, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(13, p_poss, SINGLE_ARG(Dict<int64, Vector2 >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(14, p_pyd, SINGLE_ARG(Dict<int32, Python<Shadow_PyType> >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(15, p_pyl, Python<Shadow_PyType>, PREV_IS_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(16, p_pyv1, ViPyType)
	NEXT_READ_NORMAL_CASE(17, p_pyv2, ViPyType)
	NEXT_READ_REPEATED_OBJECT_CASE(18, p_sd, SINGLE_ARG(Dict<int32, string >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(19, p_sl, string)
	END_READ_CASE()
}

PyObject* Fighter_Part2::Json(bool slim) {
	PyObject* json = PyDict_New();
	if (json == NULL) { return NULL; }
	PyObject* value = PyString_FromString(Fighter_Part2::Name);
	if (value == NULL) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	value = ::typy::Json(p_bd, slim);
	if (value != NULL) { PyDict_SetItemString(json, "bd", value); Py_DECREF(value); }
	value = ::typy::Json(p_bl, slim);
	if (value != NULL) { PyDict_SetItemString(json, "bl", value); Py_DECREF(value); }
	value = ::typy::Json(p_ed, slim);
	if (value != NULL) { PyDict_SetItemString(json, "ed", value); Py_DECREF(value); }
	value = ::typy::Json(p_el, slim);
	if (value != NULL) { PyDict_SetItemString(json, "el", value); Py_DECREF(value); }
	value = ::typy::Json(p_fl, slim);
	if (value != NULL) { PyDict_SetItemString(json, "fl", value); Py_DECREF(value); }
	value = ::typy::Json(p_hp, slim);
	if (value != NULL) { PyDict_SetItemString(json, "hp", value); Py_DECREF(value); }
	value = ::typy::Json(p_isAwake, slim);
	if (value != NULL) { PyDict_SetItemString(json, "isAwake", value); Py_DECREF(value); }
	value = ::typy::Json(p_ll, slim);
	if (value != NULL) { PyDict_SetItemString(json, "ll", value); Py_DECREF(value); }
	value = ::typy::Json(p_pos, slim);
	if (value != NULL) { PyDict_SetItemString(json, "pos", value); Py_DECREF(value); }
	value = ::typy::Json(p_posi, slim);
	if (value != NULL) { PyDict_SetItemString(json, "posi", value); Py_DECREF(value); }
	value = ::typy::Json(p_posl, slim);
	if (value != NULL) { PyDict_SetItemString(json, "posl", value); Py_DECREF(value); }
	value = ::typy::Json(p_posll, slim);
	if (value != NULL) { PyDict_SetItemString(json, "posll", value); Py_DECREF(value); }
	value = ::typy::Json(p_poss, slim);
	if (value != NULL) { PyDict_SetItemString(json, "poss", value); Py_DECREF(value); }
	value = ::typy::Json(p_pyd, slim);
	if (value != NULL) { PyDict_SetItemString(json, "pyd", value); Py_DECREF(value); }
	value = ::typy::Json(p_pyl, slim);
	if (value != NULL) { PyDict_SetItemString(json, "pyl", value); Py_DECREF(value); }
	value = ::typy::Json(p_pyv1, slim);
	if (value != NULL) { PyDict_SetItemString(json, "pyv1", value); Py_DECREF(value); }
	value = ::typy::Json(p_pyv2, slim);
	if (value != NULL) { PyDict_SetItemString(json, "pyv2", value); Py_DECREF(value); }
	value = ::typy::Json(p_sd, slim);
	if (value != NULL) { PyDict_SetItemString(json, "sd", value); Py_DECREF(value); }
	value = ::typy::Json(p_sl, slim);
	if (value != NULL) { PyDict_SetItemString(json, "sl", value); Py_DECREF(value); }
	return json;
}

Fighter_Part2* Fighter_Part2::FromJson(PyObject* json) {
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
	if (strcmp(PyBytes_AS_STRING(value), Fighter_Part2::Name)) {
		PyErr_Format(PyExc_TypeError, "Object expect '%.100s', but Json has type %.100s",
			Fighter_Part2::Name, PyBytes_AS_STRING(value));
		Py_DECREF(value);
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);
	Fighter_Part2* object = new Fighter_Part2();
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("bd")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_bd, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("bl")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_bl, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("ed")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_ed, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("el")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_el, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("fl")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_fl, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("hp")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_hp, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("isAwake")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_isAwake, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("ll")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_ll, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("pos")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_pos, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("posi")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_posi, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("posl")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_posl, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("posll")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_posll, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("poss")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_poss, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("pyd")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_pyd, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("pyl")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_pyl, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("pyv1")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_pyv1, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("pyv2")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_pyv2, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("sd")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_sd, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("sl")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_sl, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }
	return object;
}

// ===================================================================

const int Fighter_Part2::PropertyCount = 19;
char* Fighter_Part2::Properties[] = {
	"bd",
	"bl",
	"ed",
	"el",
	"fl",
	"hp",
	"isAwake",
	"ll",
	"pos",
	"posi",
	"posl",
	"posll",
	"poss",
	"pyd",
	"pyl",
	"pyv1",
	"pyv2",
	"sd",
	"sl"
};

int Fighter_Part2::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
	case 1: ::typy::ByteSize(size, 1, p_bd); if (size == 0) { size = 1; } break;
	case 2: ::typy::ByteSize(size, 1, p_bl); if (size == 0) { size = 1; } break;
	case 3: ::typy::ByteSize(size, 1, p_ed); if (size == 0) { size = 1; } break;
	case 4: ::typy::ByteSize(size, 1, p_el); if (size == 0) { size = 1; } break;
	case 5: ::typy::ByteSize(size, 1, p_fl); if (size == 0) { size = 1; } break;
	case 6: ::typy::ByteSize(size, 1, p_hp); if (size == 0) { size = 1; } break;
	case 7: ::typy::ByteSize(size, 1, p_isAwake); if (size == 0) { size = 1; } break;
	case 8: ::typy::ByteSize(size, 1, p_ll); if (size == 0) { size = 1; } break;
	case 9: ::typy::ByteSize(size, 1, p_pos); if (size == 0) { size = 1; } break;
	case 10: ::typy::ByteSize(size, 1, p_posi); if (size == 0) { size = 1; } break;
	case 11: ::typy::ByteSize(size, 1, p_posl); if (size == 0) { size = 1; } break;
	case 12: ::typy::ByteSize(size, 1, p_posll); if (size == 0) { size = 1; } break;
	case 13: ::typy::ByteSize(size, 1, p_poss); if (size == 0) { size = 1; } break;
	case 14: ::typy::ByteSize(size, 1, p_pyd); if (size == 0) { size = 1; } break;
	case 15: ::typy::ByteSize(size, 1, p_pyl); if (size == 0) { size = 1; } break;
	case 16: ::typy::ByteSize(size, 2, p_pyv1); if (size == 0) { size = 2; } break;
	case 17: ::typy::ByteSize(size, 2, p_pyv2); if (size == 0) { size = 2; } break;
	case 18: ::typy::ByteSize(size, 2, p_sd); if (size == 0) { size = 2; } break;
	case 19: ::typy::ByteSize(size, 2, p_sl); if (size == 0) { size = 2; } break;
	}
	return size;
}

void Fighter_Part2::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 1:
		::typy::Write(1, p_bd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(1, p_bd, output);
		}
		break;
	case 2:
		::typy::Write(2, p_bl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(2, p_bl, output);
		}
		break;
	case 3:
		::typy::Write(3, p_ed, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(3, p_ed, output);
		}
		break;
	case 4:
		::typy::Write(4, p_el, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(4, p_el, output);
		}
		break;
	case 5:
		::typy::Write(5, p_fl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(5, p_fl, output);
		}
		break;
	case 6:
		::typy::Write(6, p_hp, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(6, p_hp, output);
		}
		break;
	case 7:
		::typy::Write(7, p_isAwake, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(7, p_isAwake, output);
		}
		break;
	case 8:
		::typy::Write(8, p_ll, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(8, p_ll, output);
		}
		break;
	case 9:
		::typy::Write(9, p_pos, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(9, p_pos, output);
		}
		break;
	case 10:
		::typy::Write(10, p_posi, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(10, p_posi, output);
		}
		break;
	case 11:
		::typy::Write(11, p_posl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(11, p_posl, output);
		}
		break;
	case 12:
		::typy::Write(12, p_posll, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(12, p_posll, output);
		}
		break;
	case 13:
		::typy::Write(13, p_poss, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(13, p_poss, output);
		}
		break;
	case 14:
		::typy::Write(14, p_pyd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(14, p_pyd, output);
		}
		break;
	case 15:
		::typy::Write(15, p_pyl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(15, p_pyl, output);
		}
		break;
	case 16:
		::typy::Write(16, p_pyv1, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(16, p_pyv1, output);
		}
		break;
	case 17:
		::typy::Write(17, p_pyv2, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(17, p_pyv2, output);
		}
		break;
	case 18:
		::typy::Write(18, p_sd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(18, p_sd, output);
		}
		break;
	case 19:
		::typy::Write(19, p_sl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(19, p_sl, output);
		}
		break;
	}
}

int Fighter_Part2::DeserializeProperty(CodedInputStream* input) {
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
	case 1: ::typy::Clear(p_bd); break;
	case 2: ::typy::Clear(p_bl); break;
	case 3: ::typy::Clear(p_ed); break;
	case 4: ::typy::Clear(p_el); break;
	case 5: ::typy::Clear(p_fl); break;
	case 6: ::typy::Clear(p_hp); break;
	case 7: ::typy::Clear(p_isAwake); break;
	case 8: ::typy::Clear(p_ll); break;
	case 9: ::typy::Clear(p_pos); break;
	case 10: ::typy::Clear(p_posi); break;
	case 11: ::typy::Clear(p_posl); break;
	case 12: ::typy::Clear(p_posll); break;
	case 13: ::typy::Clear(p_poss); break;
	case 14: ::typy::Clear(p_pyd); break;
	case 15: ::typy::Clear(p_pyl); break;
	case 16: ::typy::Clear(p_pyv1); break;
	case 17: ::typy::Clear(p_pyv2); break;
	case 18: ::typy::Clear(p_sd); break;
	case 19: ::typy::Clear(p_sl); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

bool Fighter_Part2::SetPropertySequence(PyObject* args) {
	for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++) {
		switch(i) {
		case 0: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 0), p_bd, "Property 'bd' expect Dict(string -> bytes), but ")) { return false; } break;
		case 1: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 1), p_bl, "Property 'bl' expect List(bytes), but ")) { return false; } break;
		case 2: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 2), p_ed, "Property 'ed' expect Dict(int64 -> Corpus), but ")) { return false; } break;
		case 3: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 3), p_el, "Property 'el' expect List(Corpus), but ")) { return false; } break;
		case 4: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 4), p_fl, "Property 'fl' expect List(float), but ")) { return false; } break;
		case 5: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 5), p_hp, "Property 'hp' expect float, but ")) { return false; } break;
		case 6: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 6), p_isAwake, "Property 'isAwake' expect bool, but ")) { return false; } break;
		case 7: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 7), p_ll, "Property 'll' expect List(VLf), but ")) { return false; } break;
		case 8: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 8), p_pos, "Property 'pos' expect Vector2, but ")) { return false; } break;
		case 9: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 9), p_posi, "Property 'posi' expect Dict(int32 -> float), but ")) { return false; } break;
		case 10: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 10), p_posl, "Property 'posl' expect List(Vector2), but ")) { return false; } break;
		case 11: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 11), p_posll, "Property 'posll' expect List(VLVector2), but ")) { return false; } break;
		case 12: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 12), p_poss, "Property 'poss' expect Dict(int64 -> Vector2), but ")) { return false; } break;
		case 13: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 13), p_pyd, "Property 'pyd' expect Dict(int32 -> Python<Shadow_PyType>), but ")) { return false; } break;
		case 14: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 14), p_pyl, "Property 'pyl' expect List(Python<Shadow_PyType>), but ")) { return false; } break;
		case 15: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 15), p_pyv1, "Property 'pyv1' expect Variant(Integer, PyType), but ")) { return false; } break;
		case 16: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 16), p_pyv2, "Property 'pyv2' expect Variant(Integer, PyType), but ")) { return false; } break;
		case 17: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 17), p_sd, "Property 'sd' expect Dict(int32 -> string), but ")) { return false; } break;
		case 18: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, 18), p_sl, "Property 'sl' expect List(string), but ")) { return false; } break;
		default: PyErr_Format(PyExc_TypeError, "Unsurported property number %lu.", i); return false;
		}
	}
	return true;
}

PyObject* Fighter_Part2::GetPropertySequence() {
	PyObject* result = PyTuple_New(19);
	if (result == NULL) { return result; }
	PyTuple_SET_ITEM(result, 0, ::typy::GetPyObject(p_bd));
	PyTuple_SET_ITEM(result, 1, ::typy::GetPyObject(p_bl));
	PyTuple_SET_ITEM(result, 2, ::typy::GetPyObject(p_ed));
	PyTuple_SET_ITEM(result, 3, ::typy::GetPyObject(p_el));
	PyTuple_SET_ITEM(result, 4, ::typy::GetPyObject(p_fl));
	PyTuple_SET_ITEM(result, 5, ::typy::GetPyObject(p_hp));
	PyTuple_SET_ITEM(result, 6, ::typy::GetPyObject(p_isAwake));
	PyTuple_SET_ITEM(result, 7, ::typy::GetPyObject(p_ll));
	PyTuple_SET_ITEM(result, 8, ::typy::GetPyObject(p_pos));
	PyTuple_SET_ITEM(result, 9, ::typy::GetPyObject(p_posi));
	PyTuple_SET_ITEM(result, 10, ::typy::GetPyObject(p_posl));
	PyTuple_SET_ITEM(result, 11, ::typy::GetPyObject(p_posll));
	PyTuple_SET_ITEM(result, 12, ::typy::GetPyObject(p_poss));
	PyTuple_SET_ITEM(result, 13, ::typy::GetPyObject(p_pyd));
	PyTuple_SET_ITEM(result, 14, ::typy::GetPyObject(p_pyl));
	PyTuple_SET_ITEM(result, 15, ::typy::GetPyObject(p_pyv1));
	PyTuple_SET_ITEM(result, 16, ::typy::GetPyObject(p_pyv2));
	PyTuple_SET_ITEM(result, 17, ::typy::GetPyObject(p_sd));
	PyTuple_SET_ITEM(result, 18, ::typy::GetPyObject(p_sl));
	return result;
}

// ===================================================================

TYPY_GETSET(Fighter_Part2, p_bd, Dict(string -> bytes));
TYPY_GETSET(Fighter_Part2, p_bl, List(bytes));
TYPY_GETSET(Fighter_Part2, p_ed, Dict(int64 -> Corpus));
TYPY_GETSET(Fighter_Part2, p_el, List(Corpus));
TYPY_GETSET(Fighter_Part2, p_fl, List(float));
TYPY_GETSET(Fighter_Part2, p_hp, float);
TYPY_GETSET(Fighter_Part2, p_isAwake, bool);
TYPY_GETSET(Fighter_Part2, p_ll, List(VLf));
TYPY_GETSET(Fighter_Part2, p_pos, Vector2);
TYPY_GETSET(Fighter_Part2, p_posi, Dict(int32 -> float));
TYPY_GETSET(Fighter_Part2, p_posl, List(Vector2));
TYPY_GETSET(Fighter_Part2, p_posll, List(VLVector2));
TYPY_GETSET(Fighter_Part2, p_poss, Dict(int64 -> Vector2));
TYPY_GETSET(Fighter_Part2, p_pyd, Dict(int32 -> Python<Shadow_PyType>));
TYPY_GETSET(Fighter_Part2, p_pyl, List(Python<Shadow_PyType>));
TYPY_GETSET(Fighter_Part2, p_pyv1, Variant(Integer, PyType));
TYPY_GETSET(Fighter_Part2, p_pyv2, Variant(Integer, PyType));
TYPY_GETSET(Fighter_Part2, p_sd, Dict(int32 -> string));
TYPY_GETSET(Fighter_Part2, p_sl, List(string));

template <> PyGetSetDef Object<Fighter_Part2>::GetSet[] = {
	{"bd", (getter)Get_p_bd, (setter)Set_p_bd, "Property bd"},
	{"bl", (getter)Get_p_bl, (setter)Set_p_bl, "Property bl"},
	{"ed", (getter)Get_p_ed, (setter)Set_p_ed, "Property ed"},
	{"el", (getter)Get_p_el, (setter)Set_p_el, "Property el"},
	{"fl", (getter)Get_p_fl, (setter)Set_p_fl, "Property fl"},
	{"hp", (getter)Get_p_hp, (setter)Set_p_hp, "Property hp"},
	{"isAwake", (getter)Get_p_isAwake, (setter)Set_p_isAwake, "Property isAwake"},
	{"ll", (getter)Get_p_ll, (setter)Set_p_ll, "Property ll"},
	{"pos", (getter)Get_p_pos, (setter)Set_p_pos, "Property pos"},
	{"posi", (getter)Get_p_posi, (setter)Set_p_posi, "Property posi"},
	{"posl", (getter)Get_p_posl, (setter)Set_p_posl, "Property posl"},
	{"posll", (getter)Get_p_posll, (setter)Set_p_posll, "Property posll"},
	{"poss", (getter)Get_p_poss, (setter)Set_p_poss, "Property poss"},
	{"pyd", (getter)Get_p_pyd, (setter)Set_p_pyd, "Property pyd"},
	{"pyl", (getter)Get_p_pyl, (setter)Set_p_pyl, "Property pyl"},
	{"pyv1", (getter)Get_p_pyv1, (setter)Set_p_pyv1, "Property pyv1"},
	{"pyv2", (getter)Get_p_pyv2, (setter)Set_p_pyv2, "Property pyv2"},
	{"sd", (getter)Get_p_sd, (setter)Set_p_sd, "Property sd"},
	{"sl", (getter)Get_p_sl, (setter)Set_p_sl, "Property sl"},
	{NULL}
};

TypyObjectEnd(Fighter_Part2);
