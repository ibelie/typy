
#include "Fighter.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(Fighter);

Fighter::Fighter() : Message() {
	PyObject_INIT(this, &Object<Fighter>::_Type);
	ZERO(Fighter, p_bd, _cached_size);
}

void Fighter::Clear() {
	::typy::Clear(p_bd);
	::typy::Clear(p_bl);
	::typy::Clear(p_dd);
	::typy::Clear(p_dv);
	::typy::Clear(p_ed);
	::typy::Clear(p_el);
	::typy::Clear(p_fdd);
	::typy::Clear(p_fdv);
	::typy::Clear(p_fl);
	::typy::Clear(p_fld);
	::typy::Clear(p_flv);
	::typy::Clear(p_hp);
	::typy::Clear(p_isAwake);
	::typy::Clear(p_ld);
	::typy::Clear(p_lv);
	::typy::Clear(p_nv);
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
	::typy::Clear(p_v1);
	::typy::Clear(p_v2);
	::typy::Clear(p_v3);
	::typy::Clear(p_v4);
	::typy::Clear(p_vd);
	::typy::Clear(p_vl);
}

void Fighter::MergeFrom(const Fighter& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	::typy::MergeFrom(p_bd, from.p_bd);
	::typy::MergeFrom(p_bl, from.p_bl);
	::typy::MergeFrom(p_dd, from.p_dd);
	::typy::MergeFrom(p_dv, from.p_dv);
	::typy::MergeFrom(p_ed, from.p_ed);
	::typy::MergeFrom(p_el, from.p_el);
	::typy::MergeFrom(p_fdd, from.p_fdd);
	::typy::MergeFrom(p_fdv, from.p_fdv);
	::typy::MergeFrom(p_fl, from.p_fl);
	::typy::MergeFrom(p_fld, from.p_fld);
	::typy::MergeFrom(p_flv, from.p_flv);
	::typy::MergeFrom(p_hp, from.p_hp);
	::typy::MergeFrom(p_isAwake, from.p_isAwake);
	::typy::MergeFrom(p_ld, from.p_ld);
	::typy::MergeFrom(p_lv, from.p_lv);
	::typy::MergeFrom(p_nv, from.p_nv);
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
	::typy::MergeFrom(p_v1, from.p_v1);
	::typy::MergeFrom(p_v2, from.p_v2);
	::typy::MergeFrom(p_v3, from.p_v3);
	::typy::MergeFrom(p_v4, from.p_v4);
	::typy::MergeFrom(p_vd, from.p_vd);
	::typy::MergeFrom(p_vl, from.p_vl);
}

void Fighter::SerializeWithCachedSizes(CodedOutputStream* output) const {
	::typy::Write(1, p_bd, output);
	::typy::Write(2, p_bl, output);
	::typy::Write(3, p_dd, output);
	::typy::Write(4, p_dv, output);
	::typy::Write(5, p_ed, output);
	::typy::Write(6, p_el, output);
	::typy::Write(7, p_fdd, output);
	::typy::Write(8, p_fdv, output);
	::typy::Write(9, p_fl, output);
	::typy::Write(10, p_fld, output);
	::typy::Write(11, p_flv, output);
	::typy::Write(12, p_hp, output);
	::typy::Write(13, p_isAwake, output);
	::typy::Write(14, p_ld, output);
	::typy::Write(15, p_lv, output);
	::typy::Write(16, p_nv, output);
	::typy::Write(17, p_pos, output);
	::typy::Write(18, p_posi, output);
	::typy::Write(19, p_posl, output);
	::typy::Write(20, p_poss, output);
	::typy::Write(21, p_pyd, output);
	::typy::Write(22, p_pyl, output);
	::typy::Write(23, p_pyv1, output);
	::typy::Write(24, p_pyv2, output);
	::typy::Write(25, p_sd, output);
	::typy::Write(26, p_sl, output);
	::typy::Write(27, p_v1, output);
	::typy::Write(28, p_v2, output);
	::typy::Write(29, p_v3, output);
	::typy::Write(30, p_v4, output);
	::typy::Write(31, p_vd, output);
	::typy::Write(32, p_vl, output);
}

int Fighter::ByteSize() const {
	int total_size = 0;
	::typy::ByteSize(total_size, 1, p_bd);
	::typy::ByteSize(total_size, 1, p_bl);
	::typy::ByteSize(total_size, 1, p_dd);
	::typy::ByteSize(total_size, 1, p_dv);
	::typy::ByteSize(total_size, 1, p_ed);
	::typy::ByteSize(total_size, 1, p_el);
	::typy::ByteSize(total_size, 1, p_fdd);
	::typy::ByteSize(total_size, 1, p_fdv);
	::typy::ByteSize(total_size, 1, p_fl);
	::typy::ByteSize(total_size, 1, p_fld);
	::typy::ByteSize(total_size, 1, p_flv);
	::typy::ByteSize(total_size, 1, p_hp);
	::typy::ByteSize(total_size, 1, p_isAwake);
	::typy::ByteSize(total_size, 1, p_ld);
	::typy::ByteSize(total_size, 1, p_lv);
	::typy::ByteSize(total_size, 2, p_nv);
	::typy::ByteSize(total_size, 2, p_pos);
	::typy::ByteSize(total_size, 2, p_posi);
	::typy::ByteSize(total_size, 2, p_posl);
	::typy::ByteSize(total_size, 2, p_poss);
	::typy::ByteSize(total_size, 2, p_pyd);
	::typy::ByteSize(total_size, 2, p_pyl);
	::typy::ByteSize(total_size, 2, p_pyv1);
	::typy::ByteSize(total_size, 2, p_pyv2);
	::typy::ByteSize(total_size, 2, p_sd);
	::typy::ByteSize(total_size, 2, p_sl);
	::typy::ByteSize(total_size, 2, p_v1);
	::typy::ByteSize(total_size, 2, p_v2);
	::typy::ByteSize(total_size, 2, p_v3);
	::typy::ByteSize(total_size, 2, p_v4);
	::typy::ByteSize(total_size, 2, p_vd);
	::typy::ByteSize(total_size, 2, p_vl);
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool Fighter::MergePartialFromCodedStream(CodedInputStream* input) {
	BEGINE_READ_CASE(32)
	FIRST_READ_REPEATED_OBJECT_CASE(1, p_bd, SINGLE_ARG(Dict<string, bytes >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(2, p_bl, bytes)
	NEXT_READ_REPEATED_OBJECT_CASE(3, p_dd, SINGLE_ARG(Dict<int32, SINGLE_ARG(Dict<int32, VdesVector2 >) >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(4, p_dv, VimpmP__sm4UToCagVQqZzKHQ)
	NEXT_READ_REPEATED_OBJECT_CASE(5, p_ed, SINGLE_ARG(Dict<int32, Corpus >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(6, p_el, Corpus)
	NEXT_READ_REPEATED_OBJECT_CASE(7, p_fdd, SINGLE_ARG(Dict<int32, SINGLE_ARG(Dict<int32, float >) >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(8, p_fdv, VipfCS1hEiUnKAH2vnYWM13w)
	NEXT_READ_REPEATED_PRIMITIVE_CASE(9, p_fl, float)
	NEXT_READ_REPEATED_OBJECT_CASE(10, p_fld, SINGLE_ARG(Dict<int32, List< float > >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(11, p_flv, sq0t2631Zu6lArqg9rFtFlQ)
	NEXT_READ_NORMAL_CASE(12, p_hp, float)
	NEXT_READ_NORMAL_CASE(13, p_isAwake, bool)
	NEXT_READ_REPEATED_OBJECT_CASE(14, p_ld, SINGLE_ARG(Dict<int32, List< VdesVector2 > >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(15, p_lv, ViSlI3tNgu2aFVB8BXYUxf9w)
	NEXT_READ_NORMAL_CASE(16, p_nv, Vi)
	NEXT_READ_NORMAL_CASE(17, p_pos, Vector2)
	NEXT_READ_REPEATED_OBJECT_CASE(18, p_posi, SINGLE_ARG(Dict<int32, float >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(19, p_posl, Vector2, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(20, p_poss, SINGLE_ARG(Dict<int32, Vector2 >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(21, p_pyd, SINGLE_ARG(Dict<int32, Python<Shadow_PyType> >)::Entry, PREV_IS_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(22, p_pyl, Python<Shadow_PyType>, PREV_IS_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(23, p_pyv1, ViPyType)
	NEXT_READ_NORMAL_CASE(24, p_pyv2, ViPyType)
	NEXT_READ_REPEATED_OBJECT_CASE(25, p_sd, SINGLE_ARG(Dict<int32, string >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	NEXT_READ_NORMAL_CASE(26, p_sl, string)
	NEXT_READ_NORMAL_CASE(27, p_v1, VbyfiVector2)
	NEXT_READ_NORMAL_CASE(28, p_v2, VfisVector2)
	NEXT_READ_NORMAL_CASE(29, p_v3, VfisVector2)
	NEXT_READ_NORMAL_CASE(30, p_v4, VfisVector2)
	NEXT_READ_REPEATED_OBJECT_CASE(31, p_vd, SINGLE_ARG(Dict<int32, VdesVector2 >)::Entry, PREV_NO_REPEATED_OBJECT, NEXT_IS_REPEATED_OBJECT)
	NEXT_READ_REPEATED_OBJECT_CASE(32, p_vl, VfpisVector2, PREV_IS_REPEATED_OBJECT, NEXT_NO_REPEATED_OBJECT)
	END_READ_CASE()
}

// ===================================================================

const int Fighter::PropertyCount = 32;
char* Fighter::Properties[] = {
	"bd",
	"bl",
	"dd",
	"dv",
	"ed",
	"el",
	"fdd",
	"fdv",
	"fl",
	"fld",
	"flv",
	"hp",
	"isAwake",
	"ld",
	"lv",
	"nv",
	"pos",
	"posi",
	"posl",
	"poss",
	"pyd",
	"pyl",
	"pyv1",
	"pyv2",
	"sd",
	"sl",
	"v1",
	"v2",
	"v3",
	"v4",
	"vd",
	"vl"
};

int Fighter::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
		case 1: ::typy::ByteSize(size, 1, p_bd); if (size == 0) { size = 1; } break;
		case 2: ::typy::ByteSize(size, 1, p_bl); if (size == 0) { size = 1; } break;
		case 3: ::typy::ByteSize(size, 1, p_dd); if (size == 0) { size = 1; } break;
		case 4: ::typy::ByteSize(size, 1, p_dv); if (size == 0) { size = 1; } break;
		case 5: ::typy::ByteSize(size, 1, p_ed); if (size == 0) { size = 1; } break;
		case 6: ::typy::ByteSize(size, 1, p_el); if (size == 0) { size = 1; } break;
		case 7: ::typy::ByteSize(size, 1, p_fdd); if (size == 0) { size = 1; } break;
		case 8: ::typy::ByteSize(size, 1, p_fdv); if (size == 0) { size = 1; } break;
		case 9: ::typy::ByteSize(size, 1, p_fl); if (size == 0) { size = 1; } break;
		case 10: ::typy::ByteSize(size, 1, p_fld); if (size == 0) { size = 1; } break;
		case 11: ::typy::ByteSize(size, 1, p_flv); if (size == 0) { size = 1; } break;
		case 12: ::typy::ByteSize(size, 1, p_hp); if (size == 0) { size = 1; } break;
		case 13: ::typy::ByteSize(size, 1, p_isAwake); if (size == 0) { size = 1; } break;
		case 14: ::typy::ByteSize(size, 1, p_ld); if (size == 0) { size = 1; } break;
		case 15: ::typy::ByteSize(size, 1, p_lv); if (size == 0) { size = 1; } break;
		case 16: ::typy::ByteSize(size, 2, p_nv); if (size == 0) { size = 2; } break;
		case 17: ::typy::ByteSize(size, 2, p_pos); if (size == 0) { size = 2; } break;
		case 18: ::typy::ByteSize(size, 2, p_posi); if (size == 0) { size = 2; } break;
		case 19: ::typy::ByteSize(size, 2, p_posl); if (size == 0) { size = 2; } break;
		case 20: ::typy::ByteSize(size, 2, p_poss); if (size == 0) { size = 2; } break;
		case 21: ::typy::ByteSize(size, 2, p_pyd); if (size == 0) { size = 2; } break;
		case 22: ::typy::ByteSize(size, 2, p_pyl); if (size == 0) { size = 2; } break;
		case 23: ::typy::ByteSize(size, 2, p_pyv1); if (size == 0) { size = 2; } break;
		case 24: ::typy::ByteSize(size, 2, p_pyv2); if (size == 0) { size = 2; } break;
		case 25: ::typy::ByteSize(size, 2, p_sd); if (size == 0) { size = 2; } break;
		case 26: ::typy::ByteSize(size, 2, p_sl); if (size == 0) { size = 2; } break;
		case 27: ::typy::ByteSize(size, 2, p_v1); if (size == 0) { size = 2; } break;
		case 28: ::typy::ByteSize(size, 2, p_v2); if (size == 0) { size = 2; } break;
		case 29: ::typy::ByteSize(size, 2, p_v3); if (size == 0) { size = 2; } break;
		case 30: ::typy::ByteSize(size, 2, p_v4); if (size == 0) { size = 2; } break;
		case 31: ::typy::ByteSize(size, 2, p_vd); if (size == 0) { size = 2; } break;
		case 32: ::typy::ByteSize(size, 2, p_vl); if (size == 0) { size = 2; } break;
	}
	return size;
}

void Fighter::SerializeProperty(CodedOutputStream* output, int tag) const {
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
		::typy::Write(3, p_dd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(3, p_dd, output);
		}
		break;
	case 4:
		::typy::Write(4, p_dv, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(4, p_dv, output);
		}
		break;
	case 5:
		::typy::Write(5, p_ed, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(5, p_ed, output);
		}
		break;
	case 6:
		::typy::Write(6, p_el, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(6, p_el, output);
		}
		break;
	case 7:
		::typy::Write(7, p_fdd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(7, p_fdd, output);
		}
		break;
	case 8:
		::typy::Write(8, p_fdv, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(8, p_fdv, output);
		}
		break;
	case 9:
		::typy::Write(9, p_fl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(9, p_fl, output);
		}
		break;
	case 10:
		::typy::Write(10, p_fld, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(10, p_fld, output);
		}
		break;
	case 11:
		::typy::Write(11, p_flv, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(11, p_flv, output);
		}
		break;
	case 12:
		::typy::Write(12, p_hp, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(12, p_hp, output);
		}
		break;
	case 13:
		::typy::Write(13, p_isAwake, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(13, p_isAwake, output);
		}
		break;
	case 14:
		::typy::Write(14, p_ld, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(14, p_ld, output);
		}
		break;
	case 15:
		::typy::Write(15, p_lv, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(15, p_lv, output);
		}
		break;
	case 16:
		::typy::Write(16, p_nv, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(16, p_nv, output);
		}
		break;
	case 17:
		::typy::Write(17, p_pos, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(17, p_pos, output);
		}
		break;
	case 18:
		::typy::Write(18, p_posi, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(18, p_posi, output);
		}
		break;
	case 19:
		::typy::Write(19, p_posl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(19, p_posl, output);
		}
		break;
	case 20:
		::typy::Write(20, p_poss, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(20, p_poss, output);
		}
		break;
	case 21:
		::typy::Write(21, p_pyd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(21, p_pyd, output);
		}
		break;
	case 22:
		::typy::Write(22, p_pyl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(22, p_pyl, output);
		}
		break;
	case 23:
		::typy::Write(23, p_pyv1, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(23, p_pyv1, output);
		}
		break;
	case 24:
		::typy::Write(24, p_pyv2, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(24, p_pyv2, output);
		}
		break;
	case 25:
		::typy::Write(25, p_sd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(25, p_sd, output);
		}
		break;
	case 26:
		::typy::Write(26, p_sl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(26, p_sl, output);
		}
		break;
	case 27:
		::typy::Write(27, p_v1, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(27, p_v1, output);
		}
		break;
	case 28:
		::typy::Write(28, p_v2, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(28, p_v2, output);
		}
		break;
	case 29:
		::typy::Write(29, p_v3, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(29, p_v3, output);
		}
		break;
	case 30:
		::typy::Write(30, p_v4, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(30, p_v4, output);
		}
		break;
	case 31:
		::typy::Write(31, p_vd, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(31, p_vd, output);
		}
		break;
	case 32:
		::typy::Write(32, p_vl, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(32, p_vl, output);
		}
		break;
	}
}

int Fighter::DeserializeProperty(CodedInputStream* input) {
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
		case 3: ::typy::Clear(p_dd); break;
		case 4: ::typy::Clear(p_dv); break;
		case 5: ::typy::Clear(p_ed); break;
		case 6: ::typy::Clear(p_el); break;
		case 7: ::typy::Clear(p_fdd); break;
		case 8: ::typy::Clear(p_fdv); break;
		case 9: ::typy::Clear(p_fl); break;
		case 10: ::typy::Clear(p_fld); break;
		case 11: ::typy::Clear(p_flv); break;
		case 12: ::typy::Clear(p_hp); break;
		case 13: ::typy::Clear(p_isAwake); break;
		case 14: ::typy::Clear(p_ld); break;
		case 15: ::typy::Clear(p_lv); break;
		case 16: ::typy::Clear(p_nv); break;
		case 17: ::typy::Clear(p_pos); break;
		case 18: ::typy::Clear(p_posi); break;
		case 19: ::typy::Clear(p_posl); break;
		case 20: ::typy::Clear(p_poss); break;
		case 21: ::typy::Clear(p_pyd); break;
		case 22: ::typy::Clear(p_pyl); break;
		case 23: ::typy::Clear(p_pyv1); break;
		case 24: ::typy::Clear(p_pyv2); break;
		case 25: ::typy::Clear(p_sd); break;
		case 26: ::typy::Clear(p_sl); break;
		case 27: ::typy::Clear(p_v1); break;
		case 28: ::typy::Clear(p_v2); break;
		case 29: ::typy::Clear(p_v3); break;
		case 30: ::typy::Clear(p_v4); break;
		case 31: ::typy::Clear(p_vd); break;
		case 32: ::typy::Clear(p_vl); break;
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

// ===================================================================

TYPY_GETSET(Fighter, p_bd, Dict(string -> bytes));
TYPY_GETSET(Fighter, p_bl, List(bytes));
TYPY_GETSET(Fighter, p_dd, Dict(int32 -> SINGLE_ARG(Dict<int32, VdesVector2 >)));
TYPY_GETSET(Fighter, p_dv, Variant(Integer, Dict));
TYPY_GETSET(Fighter, p_ed, Dict(int32 -> Corpus));
TYPY_GETSET(Fighter, p_el, List(Corpus));
TYPY_GETSET(Fighter, p_fdd, Dict(int32 -> SINGLE_ARG(Dict<int32, float >)));
TYPY_GETSET(Fighter, p_fdv, Variant(Integer, Dict));
TYPY_GETSET(Fighter, p_fl, List(float));
TYPY_GETSET(Fighter, p_fld, Dict(int32 -> List< float >));
TYPY_GETSET(Fighter, p_flv, Variant(Integer, List));
TYPY_GETSET(Fighter, p_hp, float);
TYPY_GETSET(Fighter, p_isAwake, bool);
TYPY_GETSET(Fighter, p_ld, Dict(int32 -> List< VdesVector2 >));
TYPY_GETSET(Fighter, p_lv, Variant(Integer, List));
TYPY_GETSET(Fighter, p_nv, Variant(Integer));
TYPY_GETSET(Fighter, p_pos, Vector2);
TYPY_GETSET(Fighter, p_posi, Dict(int32 -> float));
TYPY_GETSET(Fighter, p_posl, List(Vector2));
TYPY_GETSET(Fighter, p_poss, Dict(int32 -> Vector2));
TYPY_GETSET(Fighter, p_pyd, Dict(int32 -> Python<Shadow_PyType>));
TYPY_GETSET(Fighter, p_pyl, List(Python<Shadow_PyType>));
TYPY_GETSET(Fighter, p_pyv1, Variant(Integer, PyType));
TYPY_GETSET(Fighter, p_pyv2, Variant(Integer, PyType));
TYPY_GETSET(Fighter, p_sd, Dict(int32 -> string));
TYPY_GETSET(Fighter, p_sl, List(string));
TYPY_GETSET(Fighter, p_v1, Variant(Integer, Vector2, Float, Bytes));
TYPY_GETSET(Fighter, p_v2, Variant(Integer, Vector2, Float, String));
TYPY_GETSET(Fighter, p_v3, Variant(Integer, Vector2, Float, String));
TYPY_GETSET(Fighter, p_v4, Variant(Integer, Vector2, Float, String));
TYPY_GETSET(Fighter, p_vd, Dict(int32 -> VdesVector2));
TYPY_GETSET(Fighter, p_vl, List(VfpisVector2));

template <> PyGetSetDef Object<Fighter>::GetSet[] = {
	{"bd", (getter)Get_p_bd, (setter)Set_p_bd, "Property bd"},
	{"bl", (getter)Get_p_bl, (setter)Set_p_bl, "Property bl"},
	{"dd", (getter)Get_p_dd, (setter)Set_p_dd, "Property dd"},
	{"dv", (getter)Get_p_dv, (setter)Set_p_dv, "Property dv"},
	{"ed", (getter)Get_p_ed, (setter)Set_p_ed, "Property ed"},
	{"el", (getter)Get_p_el, (setter)Set_p_el, "Property el"},
	{"fdd", (getter)Get_p_fdd, (setter)Set_p_fdd, "Property fdd"},
	{"fdv", (getter)Get_p_fdv, (setter)Set_p_fdv, "Property fdv"},
	{"fl", (getter)Get_p_fl, (setter)Set_p_fl, "Property fl"},
	{"fld", (getter)Get_p_fld, (setter)Set_p_fld, "Property fld"},
	{"flv", (getter)Get_p_flv, (setter)Set_p_flv, "Property flv"},
	{"hp", (getter)Get_p_hp, (setter)Set_p_hp, "Property hp"},
	{"isAwake", (getter)Get_p_isAwake, (setter)Set_p_isAwake, "Property isAwake"},
	{"ld", (getter)Get_p_ld, (setter)Set_p_ld, "Property ld"},
	{"lv", (getter)Get_p_lv, (setter)Set_p_lv, "Property lv"},
	{"nv", (getter)Get_p_nv, (setter)Set_p_nv, "Property nv"},
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
	{"v1", (getter)Get_p_v1, (setter)Set_p_v1, "Property v1"},
	{"v2", (getter)Get_p_v2, (setter)Set_p_v2, "Property v2"},
	{"v3", (getter)Get_p_v3, (setter)Set_p_v3, "Property v3"},
	{"v4", (getter)Get_p_v4, (setter)Set_p_v4, "Property v4"},
	{"vd", (getter)Get_p_vd, (setter)Set_p_vd, "Property vd"},
	{"vl", (getter)Get_p_vl, (setter)Set_p_vl, "Property vl"},
	{NULL}
};

TypyObjectEnd(Fighter);
