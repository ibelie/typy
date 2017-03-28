
#ifndef TYPY_Corpus_H__
#define TYPY_Corpus_H__

#include "typy.h"

#define ENUM Corpus

namespace typy {

enum ENUM {
	UNIVERSAL = 0,
	WEB = 1,
	IMAGES = 2,
	LOCAL = 3,
	NEWS = 4,
	PRODUCTS = 5,
	VIDEO = 6,
};

PyObject* _InitCorpus(PyObject* m, PyObject* arg);

bool InitCorpus(PyObject* m);

template <> struct Type<ENUM> {
	typedef ENUM ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_ENUM,
		WireType = WireFormatLite::WIRETYPE_VARINT,
	};
};

PyObject* GetPyObject(const ENUM& value);
bool CheckAndSet(PyObject* arg, ENUM& value, const char* err);
inline void CopyFrom(ENUM& lvalue, const ENUM& rvalue) { lvalue = rvalue; }
inline void Clear(ENUM& value) { value = static_cast<ENUM>(0); }
inline void MergeFrom(ENUM& lvalue, const ENUM& rvalue) {
	if (rvalue != 0) { CopyFrom(lvalue, rvalue); } }
inline void ByteSize(int& total, int tagsize, const ENUM& value) {
	if (value != 0) { total += tagsize + WireFormatLite::EnumSize(value); } }
inline void Write(int field_number, const ENUM& value, CodedOutputStream* output) {
	if (value != 0) { WireFormatLite::WriteEnum(field_number, value, output); } }

inline bool Read(ENUM& value, CodedInputStream* input) {
	int i;
	if (WireFormatLite::ReadPrimitive<int,
		WireFormatLite::FieldType(Type<ENUM>::FieldType)>(input, &i)) {
		value = static_cast<ENUM>(i);
		return true;
	}
	return false;
}

inline void ByteSize(int& total, int tagsize, List<ENUM>* value) {
	if (value == NULL) { return; }
	int data_size = 0;
	for (int i = 0; i < value->size(); i++) {
		data_size += WireFormatLite::EnumSize(value->Get(i));
	}
	if (data_size > 0) {
		total += tagsize + WireFormatLite::Int32Size(data_size);
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	value->_cached_size = data_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	total += data_size;
}

inline void Write(int field_number, List<ENUM>* value, CodedOutputStream* output) {
	if (value == NULL) { return; }
	if (value->_cached_size > 0) {
		WireFormatLite::WriteTag(field_number, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
		output->WriteVarint32(value->_cached_size);
	}
	for (int i = 0; i < value->size(); i++) {
		WireFormatLite::WriteEnumNoTag(value->Get(i), output);
	}
}

inline bool Read(List<ENUM>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<ENUM>; }
	ENUM item;
	if (!Read(item, input)) { return false; }
	value->Add(item);
	return true;
}

inline bool ReadPacked(List<ENUM>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<ENUM>; }
	uint32 length;
	if (!input->ReadVarint32(&length)) {
		return false;
	}
	CodedInputStream::Limit limit = input->PushLimit(length);
	while (input->BytesUntilLimit() > 0) {
		ENUM item;
		if (!Read(item, input)) { return false; }
		value->Add(item);
	}
	input->PopLimit(limit);
	return true;
}

inline bool ReadRepeated(int tagsize, uint32 tag, List<ENUM>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<ENUM>; }
	ENUM item;
	if (!Read(item, input)) { return false; }
	value->Add(item);
	int elements_already_reserved = value->Capacity() - value->size();
	while (elements_already_reserved > 0 && input->ExpectTag(tag)) {
		if (!Read(item, input)) { return false; }
		value->AddAlreadyReserved(item);
		elements_already_reserved--;
	}
	return true;
}

} /* namespace typy */

#undef ENUM

#endif // TYPY_Corpus_H__
