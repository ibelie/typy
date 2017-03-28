
#include "Fighter_Part1.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Fighter_Part1);

Fighter_Part1::Fighter_Part1() : Message() {
	PyObject_INIT(this, &Object<Fighter_Part1>::_Type);
	ZERO(Fighter_Part1, p_hp, _cached_size);
}

void Fighter_Part1::Clear() {
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
}

void Fighter_Part1::MergeFrom(const Fighter_Part1& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
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
}

void Fighter_Part1::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_hp, output);
	::typy::Write(2, p_isAwake, output);
	::typy::Write(3, p_pos, output);
	::typy::Write(4, p_posi, output);
	::typy::Write(5, p_posl, output);
	::typy::Write(6, p_poss, output);
	::typy::Write(7, p_pyd, output);
	::typy::Write(8, p_pyl, output);
	::typy::Write(9, p_pyv1, output);
	::typy::Write(10, p_pyv2, output);
}

int Fighter_Part1::ByteSize() const {
	int total_size = 0;
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
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Fighter_Part1::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(10)
	FIRST_READ_NORMAL_CASE(1, p_hp, float)
	NEXT_READ_NORMAL_CASE(2, p_isAwake, bool)
	NEXT_READ_NORMAL_CASE(3, p_pos, Vector2)
	NEXT_READ_REPEATED_OBJECT_CASE(4, p_posi, SINGLE_ARG(Dict<int32, float >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(5, p_posl, Vector2, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(6, p_poss, SINGLE_ARG(Dict<int32, Vector2 >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(7, p_pyd, SINGLE_ARG(Dict<int32, Python<Shadow_PyType> >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(8, p_pyl, Python<Shadow_PyType>, PREV_IS_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(9, p_pyv1, ViPyType)
	NEXT_READ_NORMAL_CASE(10, p_pyv2, ViPyType)
	END_READ_CASE()
}

// ===================================================================

const int Fighter_Part1::PropertyCount = 10;
char* Fighter_Part1::Properties[] = {
	"hp",
	"isAwake",
	"pos",
	"posi",
	"posl",
	"poss",
	"pyd",
	"pyl",
	"pyv1",
	"pyv2"
};

int Fighter_Part1::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
		case 1: ::typy::ByteSize(size, 1, p_hp); if (size == 0) { size = 1; } break;
		case 2: ::typy::ByteSize(size, 1, p_isAwake); if (size == 0) { size = 1; } break;
		case 3: ::typy::ByteSize(size, 1, p_pos); if (size == 0) { size = 1; } break;
		case 4: ::typy::ByteSize(size, 1, p_posi); if (size == 0) { size = 1; } break;
		case 5: ::typy::ByteSize(size, 1, p_posl); if (size == 0) { size = 1; } break;
		case 6: ::typy::ByteSize(size, 1, p_poss); if (size == 0) { size = 1; } break;
		case 7: ::typy::ByteSize(size, 1, p_pyd); if (size == 0) { size = 1; } break;
		case 8: ::typy::ByteSize(size, 1, p_pyl); if (size == 0) { size = 1; } break;
		case 9: ::typy::ByteSize(size, 1, p_pyv1); if (size == 0) { size = 1; } break;
		case 10: ::typy::ByteSize(size, 1, p_pyv2); if (size == 0) { size = 1; } break;
	}
	return size;
}

void Fighter_Part1::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 1:
		::typy::Write(1, p_hp, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(1, p_hp, output);
		}
		break;
	case 2:
		::typy::Write(2, p_isAwake, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(2, p_isAwake, output);
		}
		break;
	case 3:
		::typy::Write(3, p_pos, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(3, p_pos, output);
		}
		break;
	case 4:
		::typy::Write(4, p_posi, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(4, p_posi, output);
		}
		break;
	case 5:
		::typy::Write(5, p_posl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(5, p_posl, output);
		}
		break;
	case 6:
		::typy::Write(6, p_poss, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(6, p_poss, output);
		}
		break;
	case 7:
		::typy::Write(7, p_pyd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(7, p_pyd, output);
		}
		break;
	case 8:
		::typy::Write(8, p_pyl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(8, p_pyl, output);
		}
		break;
	case 9:
		::typy::Write(9, p_pyv1, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(9, p_pyv1, output);
		}
		break;
	case 10:
		::typy::Write(10, p_pyv2, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(10, p_pyv2, output);
		}
		break;
	}
}

int Fighter_Part1::DeserializeProperty(CodedInputStream* input) {
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
	case 1: ::typy::Clear(p_hp); break;
		case 2: ::typy::Clear(p_isAwake); break;
		case 3: ::typy::Clear(p_pos); break;
		case 4: ::typy::Clear(p_posi); break;
		case 5: ::typy::Clear(p_posl); break;
		case 6: ::typy::Clear(p_poss); break;
		case 7: ::typy::Clear(p_pyd); break;
		case 8: ::typy::Clear(p_pyl); break;
		case 9: ::typy::Clear(p_pyv1); break;
		case 10: ::typy::Clear(p_pyv2); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

// ===================================================================

TYPY_GETSET(Fighter_Part1, p_hp, float);
TYPY_GETSET(Fighter_Part1, p_isAwake, bool);
TYPY_GETSET(Fighter_Part1, p_pos, Vector2);
TYPY_GETSET(Fighter_Part1, p_posi, Dict(int32 -> float));
TYPY_GETSET(Fighter_Part1, p_posl, List(Vector2));
TYPY_GETSET(Fighter_Part1, p_poss, Dict(int32 -> Vector2));
TYPY_GETSET(Fighter_Part1, p_pyd, Dict(int32 -> Python<Shadow_PyType>));
TYPY_GETSET(Fighter_Part1, p_pyl, List(Python<Shadow_PyType>));
TYPY_GETSET(Fighter_Part1, p_pyv1, Variant(Integer, PyType));
TYPY_GETSET(Fighter_Part1, p_pyv2, Variant(Integer, PyType));

template <> PyGetSetDef Object<Fighter_Part1>::GetSet[] = {
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
	{NULL}
};

TypyObjectEnd(Fighter_Part1);
