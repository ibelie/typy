
#include "Vector2.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Vector2);

Vector2::Vector2() : Message() {
	PyObject_INIT(this, &Object<Vector2>::_Type);
	ZERO(Vector2, p_b, _cached_size);
}

void Vector2::Clear() {
	::typy::Clear(p_b);
	::typy::Clear(p_e);
	::typy::Clear(p_i);
	::typy::Clear(p_p);
	::typy::Clear(p_s);
	::typy::Clear(p_t);
	::typy::Clear(p_x);
	::typy::Clear(p_y);
}

void Vector2::MergeFrom(const Vector2& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	::typy::MergeFrom(p_b, from.p_b);
	::typy::MergeFrom(p_e, from.p_e);
	::typy::MergeFrom(p_i, from.p_i);
	::typy::MergeFrom(p_p, from.p_p);
	::typy::MergeFrom(p_s, from.p_s);
	::typy::MergeFrom(p_t, from.p_t);
	::typy::MergeFrom(p_x, from.p_x);
	::typy::MergeFrom(p_y, from.p_y);
}

void Vector2::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_b, output);
	::typy::Write(2, p_e, output);
	::typy::Write(3, p_i, output);
	::typy::Write(4, p_p, output);
	::typy::Write(5, p_s, output);
	::typy::Write(6, p_x, output);
	::typy::Write(7, p_y, output);
}

int Vector2::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, p_b);
	::typy::ByteSize(total_size, 1, p_e);
	::typy::ByteSize(total_size, 1, p_i);
	::typy::ByteSize(total_size, 1, p_p);
	::typy::ByteSize(total_size, 1, p_s);
	::typy::ByteSize(total_size, 1, p_x);
	::typy::ByteSize(total_size, 1, p_y);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Vector2::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(7)
	FIRST_READ_NORMAL_CASE(1, p_b, bytes)
	NEXT_READ_NORMAL_CASE(2, p_e, Corpus)
	NEXT_READ_NORMAL_CASE(3, p_i, Empty)
	NEXT_READ_NORMAL_CASE(4, p_p, Python<Shadow_PyType>)
	NEXT_READ_NORMAL_CASE(5, p_s, string)
	NEXT_READ_NORMAL_CASE(6, p_x, float)
	NEXT_READ_NORMAL_CASE(7, p_y, SINGLE_ARG(FixedPoint<1, -10>))
	END_READ_CASE()
}

// ===================================================================

const int Vector2::PropertyCount = 7;
char* Vector2::Properties[] = {
	"b",
	"e",
	"i",
	"p",
	"s",
	"x",
	"y"
};

int Vector2::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
		case 1: ::typy::ByteSize(size, 1, p_b); if (size == 0) { size = 1; } break;
		case 2: ::typy::ByteSize(size, 1, p_e); if (size == 0) { size = 1; } break;
		case 3: ::typy::ByteSize(size, 1, p_i); if (size == 0) { size = 1; } break;
		case 4: ::typy::ByteSize(size, 1, p_p); if (size == 0) { size = 1; } break;
		case 5: ::typy::ByteSize(size, 1, p_s); if (size == 0) { size = 1; } break;
		case 6: ::typy::ByteSize(size, 1, p_x); if (size == 0) { size = 1; } break;
		case 7: ::typy::ByteSize(size, 1, p_y); if (size == 0) { size = 1; } break;
	}
	return size;
}

void Vector2::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	case 1:
		::typy::Write(1, p_b, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(1, p_b, output);
		}
		break;
	case 2:
		::typy::Write(2, p_e, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(2, p_e, output);
		}
		break;
	case 3:
		::typy::Write(3, p_i, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(3, p_i, output);
		}
		break;
	case 4:
		::typy::Write(4, p_p, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(4, p_p, output);
		}
		break;
	case 5:
		::typy::Write(5, p_s, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(5, p_s, output);
		}
		break;
	case 6:
		::typy::Write(6, p_x, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(6, p_x, output);
		}
		break;
	case 7:
		::typy::Write(7, p_y, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(7, p_y, output);
		}
		break;
	}
}

int Vector2::DeserializeProperty(CodedInputStream* input) {
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
	case 1: ::typy::Clear(p_b); break;
		case 2: ::typy::Clear(p_e); break;
		case 3: ::typy::Clear(p_i); break;
		case 4: ::typy::Clear(p_p); break;
		case 5: ::typy::Clear(p_s); break;
		case 6: ::typy::Clear(p_x); break;
		case 7: ::typy::Clear(p_y); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

// ===================================================================

TYPY_GETSET(Vector2, p_b, bytes);
TYPY_GETSET(Vector2, p_e, Corpus);
TYPY_GETSET(Vector2, p_i, Empty);
TYPY_GETSET(Vector2, p_p, Python<Shadow_PyType>);
TYPY_GETSET(Vector2, p_s, string);
TYPY_GETSET(Vector2, p_t, Python<PyObject>);
TYPY_GETSET(Vector2, p_x, float);
TYPY_GETSET(Vector2, p_y, SINGLE_ARG(FixedPoint<1, -10>));

template <> PyGetSetDef Object<Vector2>::GetSet[] = {
	{"b", (getter)Get_p_b, (setter)Set_p_b, "Property b"},
	{"e", (getter)Get_p_e, (setter)Set_p_e, "Property e"},
	{"i", (getter)Get_p_i, (setter)Set_p_i, "Property i"},
	{"p", (getter)Get_p_p, (setter)Set_p_p, "Property p"},
	{"s", (getter)Get_p_s, (setter)Set_p_s, "Property s"},
	{"t", (getter)Get_p_t, (setter)Set_p_t, "Property t"},
	{"x", (getter)Get_p_x, (setter)Set_p_x, "Property x"},
	{"y", (getter)Get_p_y, (setter)Set_p_y, "Property y"},
	{NULL}
};

TypyObjectEnd(Vector2);
