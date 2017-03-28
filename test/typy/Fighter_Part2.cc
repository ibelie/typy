
#include "Fighter_Part2.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Fighter_Part2);

Fighter_Part2::Fighter_Part2() : Message() {
	PyObject_INIT(this, &Object<Fighter_Part2>::_Type);
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
	::typy::Clear(p_pos);
	::typy::Clear(p_posi);
	::typy::Clear(p_posl);
	::typy::Clear(p_poss);
	::typy::Clear(p_pyd);
	::typy::Clear(p_pyl);
	::typy::Clear(p_pyv1);
	::typy::Clear(p_pyv2);
	::typy::Clear(p_sd);
	::typy::Clear(p_sl);
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
	::typy::MergeFrom(p_pos, from.p_pos);
	::typy::MergeFrom(p_posi, from.p_posi);
	::typy::MergeFrom(p_posl, from.p_posl);
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
	::typy::Write(8, p_pos, output);
	::typy::Write(9, p_posi, output);
	::typy::Write(10, p_posl, output);
	::typy::Write(11, p_poss, output);
	::typy::Write(12, p_pyd, output);
	::typy::Write(13, p_pyl, output);
	::typy::Write(14, p_pyv1, output);
	::typy::Write(15, p_pyv2, output);
	::typy::Write(16, p_sd, output);
	::typy::Write(17, p_sl, output);
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
	::typy::ByteSize(total_size, 1, p_pos);
	::typy::ByteSize(total_size, 1, p_posi);
	::typy::ByteSize(total_size, 1, p_posl);
	::typy::ByteSize(total_size, 1, p_poss);
	::typy::ByteSize(total_size, 1, p_pyd);
	::typy::ByteSize(total_size, 1, p_pyl);
	::typy::ByteSize(total_size, 1, p_pyv1);
	::typy::ByteSize(total_size, 1, p_pyv2);
	::typy::ByteSize(total_size, 2, p_sd);
	::typy::ByteSize(total_size, 2, p_sl);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Fighter_Part2::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(17)
	FIRST_READ_REPEATED_OBJECT_CASE(1, p_bd, SINGLE_ARG(Dict<string, bytes >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(2, p_bl, bytes)
	NEXT_READ_REPEATED_OBJECT_CASE(3, p_ed, SINGLE_ARG(Dict<int32, Corpus >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(4, p_el, Corpus)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(5, p_fl, float)
	NEXT_READ_NORMAL_CASE(6, p_hp, float)
	NEXT_READ_NORMAL_CASE(7, p_isAwake, bool)
	NEXT_READ_NORMAL_CASE(8, p_pos, Vector2)
	NEXT_READ_REPEATED_OBJECT_CASE(9, p_posi, SINGLE_ARG(Dict<int32, float >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(10, p_posl, Vector2, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(11, p_poss, SINGLE_ARG(Dict<int32, Vector2 >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(12, p_pyd, SINGLE_ARG(Dict<int32, Python<Shadow_PyType> >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(13, p_pyl, Python<Shadow_PyType>, PREV_IS_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(14, p_pyv1, ViPyType)
	NEXT_READ_NORMAL_CASE(15, p_pyv2, ViPyType)
	NEXT_READ_REPEATED_OBJECT_CASE(16, p_sd, SINGLE_ARG(Dict<int32, string >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(17, p_sl, string)
	END_READ_CASE()
}

// ===================================================================

const int Fighter_Part2::PropertyCount = 17;
char* Fighter_Part2::Properties[] = {
	"bd",
	"bl",
	"ed",
	"el",
	"fl",
	"hp",
	"isAwake",
	"pos",
	"posi",
	"posl",
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
		case 8: ::typy::ByteSize(size, 1, p_pos); if (size == 0) { size = 1; } break;
		case 9: ::typy::ByteSize(size, 1, p_posi); if (size == 0) { size = 1; } break;
		case 10: ::typy::ByteSize(size, 1, p_posl); if (size == 0) { size = 1; } break;
		case 11: ::typy::ByteSize(size, 1, p_poss); if (size == 0) { size = 1; } break;
		case 12: ::typy::ByteSize(size, 1, p_pyd); if (size == 0) { size = 1; } break;
		case 13: ::typy::ByteSize(size, 1, p_pyl); if (size == 0) { size = 1; } break;
		case 14: ::typy::ByteSize(size, 1, p_pyv1); if (size == 0) { size = 1; } break;
		case 15: ::typy::ByteSize(size, 1, p_pyv2); if (size == 0) { size = 1; } break;
		case 16: ::typy::ByteSize(size, 2, p_sd); if (size == 0) { size = 2; } break;
		case 17: ::typy::ByteSize(size, 2, p_sl); if (size == 0) { size = 2; } break;
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
		::typy::Write(8, p_pos, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(8, p_pos, output);
		}
		break;
	case 9:
		::typy::Write(9, p_posi, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(9, p_posi, output);
		}
		break;
	case 10:
		::typy::Write(10, p_posl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(10, p_posl, output);
		}
		break;
	case 11:
		::typy::Write(11, p_poss, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(11, p_poss, output);
		}
		break;
	case 12:
		::typy::Write(12, p_pyd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(12, p_pyd, output);
		}
		break;
	case 13:
		::typy::Write(13, p_pyl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(13, p_pyl, output);
		}
		break;
	case 14:
		::typy::Write(14, p_pyv1, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(14, p_pyv1, output);
		}
		break;
	case 15:
		::typy::Write(15, p_pyv2, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(15, p_pyv2, output);
		}
		break;
	case 16:
		::typy::Write(16, p_sd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(16, p_sd, output);
		}
		break;
	case 17:
		::typy::Write(17, p_sl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(17, p_sl, output);
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
		case 8: ::typy::Clear(p_pos); break;
		case 9: ::typy::Clear(p_posi); break;
		case 10: ::typy::Clear(p_posl); break;
		case 11: ::typy::Clear(p_poss); break;
		case 12: ::typy::Clear(p_pyd); break;
		case 13: ::typy::Clear(p_pyl); break;
		case 14: ::typy::Clear(p_pyv1); break;
		case 15: ::typy::Clear(p_pyv2); break;
		case 16: ::typy::Clear(p_sd); break;
		case 17: ::typy::Clear(p_sl); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

// ===================================================================

TYPY_GETSET(Fighter_Part2, p_bd, Dict(string -> bytes));
TYPY_GETSET(Fighter_Part2, p_bl, List(bytes));
TYPY_GETSET(Fighter_Part2, p_ed, Dict(int32 -> Corpus));
TYPY_GETSET(Fighter_Part2, p_el, List(Corpus));
TYPY_GETSET(Fighter_Part2, p_fl, List(float));
TYPY_GETSET(Fighter_Part2, p_hp, float);
TYPY_GETSET(Fighter_Part2, p_isAwake, bool);
TYPY_GETSET(Fighter_Part2, p_pos, Vector2);
TYPY_GETSET(Fighter_Part2, p_posi, Dict(int32 -> float));
TYPY_GETSET(Fighter_Part2, p_posl, List(Vector2));
TYPY_GETSET(Fighter_Part2, p_poss, Dict(int32 -> Vector2));
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
	{"pos", (getter)Get_p_pos, (setter)Set_p_pos, "Property pos"},
	{"posi", (getter)Get_p_posi, (setter)Set_p_posi, "Property posi"},
	{"posl", (getter)Get_p_posl, (setter)Set_p_posl, "Property posl"},
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
