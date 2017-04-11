// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_FIXEDPOINT_H__
#define TYPY_FIXEDPOINT_H__

#include "typy.h"

#define FIXEDPOINT FixedPoint<precision, floor>

namespace typy {

template <int precision, int floor> struct FixedPoint {
	typedef FixedPoint ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_INT32,
		WireType = WireFormatLite::WIRETYPE_VARINT,
	};
	enum { Precision = 10 * FixedPoint<precision - 1, 0>::Precision };

	int32 value;

	inline operator double() const { return (double)value / FIXEDPOINT::Precision + floor; }
	inline FixedPoint& operator=(const double& v) { value = (int32)((v - floor) * FIXEDPOINT::Precision); return *this; }
};

template <> struct FixedPoint<0, 0> {
	enum { Precision = 1 };
};

template <int precision, int floor>
inline PyObject* GetPyObject(const FIXEDPOINT& value) {
	return PyFloat_FromDouble(value);
}

template <int precision, int floor>
bool CheckAndSet(PyObject* arg, FIXEDPOINT& value, const char* err) {
	if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	CopyFrom(value, PyFloat_AsDouble(arg));
	return true;
}

template <int precision, int floor>
inline void CopyFrom(FIXEDPOINT& lvalue, const double& rvalue) {
	lvalue = rvalue;
}

template <int precision, int floor>
inline void Clear(FIXEDPOINT& value) { value.value = 0; }

template <int precision, int floor>
inline void MergeFrom(FIXEDPOINT& lvalue, const double& rvalue) {
	if (rvalue != floor) { CopyFrom(lvalue, rvalue); }
}

template <int precision, int floor>
inline void ByteSize(int& total, int tagsize, const FIXEDPOINT& value) {
	if (value.value != 0) { total += tagsize + WireFormatLite::Int32Size(value.value); }
}

template <int precision, int floor>
inline void Write(int field_number, const FIXEDPOINT& value, CodedOutputStream* output) {
	if (value.value != 0) { WireFormatLite::WriteInt32(field_number, value.value, output); }
}

template <int precision, int floor>
inline void WriteTag(int tag, const FIXEDPOINT& value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WIRETYPE_VARINT, output);
}

template <int precision, int floor>
inline bool Read(FIXEDPOINT& value, CodedInputStream* input) {
	return WireFormatLite::ReadPrimitive<int32,
		WireFormatLite::FieldType(Type<int32>::FieldType)>(input, &value.value);
}

template <int precision, int floor>
inline void ByteSize(int& total, int tagsize, List< FIXEDPOINT >* value) {
	if (value == NULL) { return; }
	int data_size = 0;
	for (int i = 0; i < value->size(); i++) {
		data_size += WireFormatLite::Int32Size(value->Get(i).value);
	}
	if (data_size > 0) {
		total += tagsize + WireFormatLite::Int32Size(data_size);
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	value->_cached_size = data_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	total += data_size;
}

template <int precision, int floor>
inline void Write(int field_number, List< FIXEDPOINT >* value, CodedOutputStream* output) {
	if (value->_cached_size > 0) {
		WireFormatLite::WriteTag(field_number, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
		output->WriteVarint32(value->_cached_size);
	}
	for (int i = 0; i < value->size(); i++) {
		WireFormatLite::WriteInt32NoTag(value->Get(i).value, output);
	}
}

template <int precision, int floor>
inline void WriteTag(int tag, List< FIXEDPOINT >* value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
}

template <int precision, int floor>
inline bool Read(List< FIXEDPOINT >*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List< FIXEDPOINT >; }
	FIXEDPOINT item;
	if (!Read(item, input)) { return false; }
	value->Add(item);
	return true;
}

template <int precision, int floor>
inline bool ReadPacked(List< FIXEDPOINT >*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List< FIXEDPOINT >; }
	uint32 length;
	if (!input->ReadVarint32(&length)) return false;
	CodedInputStream::Limit limit = input->PushLimit(length);
	while (input->BytesUntilLimit() > 0) {
		FIXEDPOINT item;
		if (!Read(item, input)) { return false; }
		value->Add(item);
	}
	input->PopLimit(limit);
	return true;
}

template <int precision, int floor>
inline bool ReadRepeated(int tagsize, uint32 tag, List< FIXEDPOINT >*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List< FIXEDPOINT >; }
	FIXEDPOINT item;
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

} // namespace typy
#endif // TYPY_FIXEDPOINT_H__
