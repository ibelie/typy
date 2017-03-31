// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_VARIANT_H__
#define TYPY_VARIANT_H__

#include "typy.h"

#define TypyVariantHeader(VARIANT, FIELDS) \
namespace typy {                                                         \
                                                                         \
class VARIANT : public Message {                                         \
public:                                                                  \
	typedef VARIANT* ValueType;                                          \
	enum {                                                               \
		FieldType = WireFormatLite::TYPE_MESSAGE,                        \
		WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,            \
	};                                                                   \
                                                                         \
	VARIANT();                                                           \
	virtual ~VARIANT();                                                  \
                                                                         \
	VARIANT* New() const;                                                \
	void Clear();                                                        \
	void CheckTypeAndMergeFrom(const Message&);                          \
	bool IsInitialized() const { return true; }                          \
	void CopyFrom(const VARIANT&);                                       \
	void MergeFrom(const VARIANT&);                                      \
	int GetCachedSize() const { return _cached_size; }                   \
                                                                         \
	::std::string GetTypeName() const;                                   \
                                                                         \
	int ByteSize() const;                                                \
	bool MergePartialFromCodedStream(CodedInputStream*);                 \
	void SerializeWithCachedSizes(CodedOutputStream*) const;             \
                                                                         \
	PyObject* toPyObject();                                              \
	bool fromPyObject(PyObject* value);                                  \
                                                                         \
private:                                                                 \
	union {                                                              \
		FIELDS                                                           \
	};                                                                   \
	int _tag;                                                            \
	mutable int _cached_size;                                            \
}; /* class VARIANT */                                                   \
                                                                         \
inline PyObject* GetPyObject(VARIANT* value) {                           \
	if (value == NULL) { Py_RETURN_NONE; }                               \
	return value->toPyObject();                                          \
}                                                                        \
                                                                         \
inline void CopyFrom(VARIANT*& lvalue, VARIANT* rvalue) {                \
	if (rvalue == NULL) { delete lvalue; lvalue = NULL; return; }        \
	if (lvalue == NULL) { lvalue = new VARIANT; }                        \
	lvalue->VARIANT::CopyFrom(*rvalue);                                  \
}                                                                        \
                                                                         \
inline void Clear(VARIANT*& value) {                                     \
	delete value; value = NULL;                                          \
}                                                                        \
                                                                         \
inline void MergeFrom(VARIANT*& lvalue, VARIANT* rvalue) {               \
	if (rvalue == NULL) { return; }                                      \
	if (lvalue == NULL) { lvalue = new VARIANT; }                        \
	lvalue->VARIANT::MergeFrom(*rvalue);                                 \
}                                                                        \
                                                                         \
bool CheckAndSet(PyObject* arg, VARIANT*& value, const char* err);       \
                                                                         \
} /* namespace typy */

#define TypyVariantBegin(VARIANT) \
namespace typy {                                                         \
                                                                         \
VARIANT::VARIANT() : Message() {                                         \
	ZERO(VARIANT, _value1, _tag);                                        \
}                                                                        \
VARIANT::~VARIANT() { Clear(); }                                         \
                                                                         \
::std::string VARIANT::GetTypeName() const {                             \
	return #FULL_MODULE_NAME "." #VARIANT;                               \
}                                                                        \
                                                                         \
VARIANT* VARIANT::New() const { return new VARIANT; }                    \
void VARIANT::CheckTypeAndMergeFrom(const MessageLite& from) {           \
	MergeFrom(*::google::protobuf::down_cast<const VARIANT*>(&from));    \
}

#define TypyVariantEnd(VARIANT) \
} /* namespace typy */

#define FIRST_READ_VARIANT_CASE(TAG, NAME, TYPE) \
	case TAG: {                                                       \
		if (tag == _MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType)) { \
			if (_tag != 0 && _tag != TAG) { Clear(); }                \
			if (!GOOGLE_PREDICT_TRUE(Read(NAME, input))) {            \
				return false;                                         \
			}                                                         \
			_tag = TAG;                                               \
		} else if (tag == _MAKETAG(TAG,                               \
			WireFormatLite::WIRETYPE_LENGTH_DELIMITED)) {             \
			if (_tag != 0 && _tag != TAG) { Clear(); }                \
			if (!GOOGLE_PREDICT_TRUE(ReadPacked(NAME, input))) {      \
				return false;                                         \
			}                                                         \
			_tag = TAG;                                               \
		} else {                                                      \
			goto handle_unusual;                                      \
		}

#define NEXT_READ_VARIANT_CASE(TAG, NAME, TYPE) \
		if (input->ExpectAtEnd()) { return true; }                    \
		break;                                                        \
	}                                                                 \
	FIRST_READ_VARIANT_CASE(TAG, NAME, SINGLE_ARG(TYPE))

#endif // TYPY_VARIANT_H__
