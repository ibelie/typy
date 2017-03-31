// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_TYPE_H__
#define TYPY_TYPE_H__

#define HAVE_ROUND

#include <string>
#include <string.h>
#include <Python.h>
#include <google/protobuf/map_field_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#define SINGLE_ARG(...) __VA_ARGS__
#define FULL_MODULE_NAME _typy
#define FULL_NAME_LEN 6

#ifndef PyVarObject_HEAD_INIT
#define PyVarObject_HEAD_INIT(type, size) PyObject_HEAD_INIT(type) size,
#endif
#ifndef Py_TYPE
#define Py_TYPE(ob) (((PyObject*)(ob))->ob_type)
#endif

#if PY_MAJOR_VERSION >= 3
	#define PyInt_Check PyLong_Check
	#define PyInt_AsLong PyLong_AsLong
	#define PyInt_FromLong PyLong_FromLong
	#define PyInt_FromSize_t PyLong_FromSize_t
	#define PyString_Check PyUnicode_Check
	#define PyString_FromString PyUnicode_FromString
	#define PyString_FromStringAndSize PyUnicode_FromStringAndSize
	#if PY_VERSION_HEX < 0x03030000
		#error "Python 3.0 - 3.2 are not supported."
	#else
	#define PyString_AsString(ob) \
		(PyUnicode_Check(ob)? PyUnicode_AsUTF8(ob): PyBytes_AsString(ob))
	#define PyString_AsStringAndSize(ob, charpp, sizep) \
		(PyUnicode_Check(ob)? \
			((*(charpp) = PyUnicode_AsUTF8AndSize(ob, (sizep))) == NULL? -1: 0): \
			PyBytes_AsStringAndSize(ob, (charpp), (sizep)))
	#endif
#endif

#if defined(__clang__)
#define ZR_HELPER(TYPE, FIELD) \
	_Pragma("clang diagnostic push")                           \
	_Pragma("clang diagnostic ignored \"-Winvalid-offsetof\"") \
	__builtin_offsetof(TYPE, FIELD)                            \
	_Pragma("clang diagnostic pop")
#else
#define ZR_HELPER(TYPE, FIELD) reinterpret_cast<char*>(&reinterpret_cast<TYPE*>(16)->FIELD)
#endif

#define ZERO(TYPE, FIRST, LAST) do {\
	::memset(&FIRST, 0, ZR_HELPER(TYPE, LAST) - ZR_HELPER(TYPE, FIRST) + sizeof(LAST)); \
} while (0)

// ===================================================================

namespace typy {

extern bool isDefaultEncodingUTF8;

typedef PyBytesObject* bytes;
typedef PyUnicodeObject* string;
typedef ::google::protobuf::uint8 uint8;
typedef ::google::protobuf::int32 int32;
typedef ::google::protobuf::int64 int64;
typedef ::google::protobuf::uint32 uint32;
typedef ::google::protobuf::uint64 uint64;
typedef ::google::protobuf::io::CodedInputStream CodedInputStream;
typedef ::google::protobuf::io::CodedOutputStream CodedOutputStream;
typedef ::google::protobuf::io::ArrayOutputStream ArrayOutputStream;
typedef ::google::protobuf::internal::WireFormatLite WireFormatLite;
typedef ::google::protobuf::MessageLite Message;

void FormatTypeError(PyObject* arg, const char* err);

#define _MAKETAG(TAG, WIRETYPE) GOOGLE_PROTOBUF_WIRE_FORMAT_MAKE_TAG(TAG, SINGLE_ARG(WIRETYPE))
#define _MAXTAG(TAG) _MAKETAG(TAG, WireFormatLite::kTagTypeMask)

#define BEGINE_READ_CASE(TAG) \
	uint32 tag;                                                                    \
	for (;;) {                                                                     \
		::std::pair<uint32, bool> p = input->ReadTagWithCutoff(                    \
			_MAXTAG(TAG) <= 0x7F ? 0x7F : (_MAXTAG(TAG) <= 0x3FFF ?                \
				0x3FFF : _MAXTAG(TAG)));                                           \
		tag = p.first;                                                             \
		if (!p.second) { goto handle_unusual; }                                    \
		switch (WireFormatLite::GetTagFieldNumber(tag)) {

#define _FIRST_READ_NORMAL_CASE(TAG, NAME, TYPE, PARSE_TAG) \
	case TAG: {                                                                    \
		if (tag == _MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType)) { PARSE_TAG    \
			if (!GOOGLE_PREDICT_TRUE(Read(NAME, input))) { return false; }         \
		} else { goto handle_unusual; }

#define FIRST_READ_NORMAL_CASE(TAG, NAME, TYPE) \
	_FIRST_READ_NORMAL_CASE(TAG, NAME, TYPE, )

#define NEXT_READ_NORMAL_CASE(TAG, NAME, TYPE) \
		if (input->ExpectTag(_MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType))) {   \
			goto parse_##TAG;                                                      \
		}                                                                          \
		break;                                                                     \
	}                                                                              \
	_FIRST_READ_NORMAL_CASE(TAG, NAME, SINGLE_ARG(TYPE), parse_##TAG:)

#define _FIRST_READ_REPEATED_PRIMITIVE_CASE(TAG, NAME, TYPE, PARSE_TAG) \
	case TAG: {                                                                    \
		if (tag == _MAKETAG(TAG, WireFormatLite::WIRETYPE_LENGTH_DELIMITED)) {     \
		PARSE_TAG                                                                  \
			if (!GOOGLE_PREDICT_TRUE(ReadPacked(NAME, input))) {                   \
				return false;                                                      \
			}                                                                      \
		} else if (tag == _MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType)) {       \
			if (!GOOGLE_PREDICT_TRUE(ReadRepeated(TAG, tag, NAME, input))) {       \
				return false;                                                      \
			}                                                                      \
		} else { goto handle_unusual; }

#define FIRST_READ_REPEATED_PRIMITIVE_CASE(TAG, NAME, TYPE) \
	_FIRST_READ_REPEATED_PRIMITIVE_CASE(TAG, NAME, TYPE, )

#define NEXT_READ_REPEATED_PRIMITIVE_CASE(TAG, NAME, TYPE) \
		if (input->ExpectTag(_MAKETAG(TAG,                                         \
			WireFormatLite::WIRETYPE_LENGTH_DELIMITED))) {                         \
			goto parse_##TAG;                                                      \
		}                                                                          \
		break;                                                                     \
	}                                                                              \
	_FIRST_READ_REPEATED_PRIMITIVE_CASE(TAG, NAME, SINGLE_ARG(TYPE), parse_##TAG:)

#define _FIRST_READ_REPEATED_OBJECT_CASE(TAG, NAME, TYPE, NEXT, PARSE_TAG) \
	case TAG: {                                                                    \
		if (tag == _MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType)) {              \
		PARSE_TAG                                                                  \
			if (!GOOGLE_PREDICT_TRUE(input->IncrementRecursionDepth())) {          \
				return false;                                                      \
			}                                                                      \
		parse_##TAG##_loop:                                                        \
			if (!GOOGLE_PREDICT_TRUE(Read(NAME, input))) { return false; }         \
		} else {                                                                   \
			goto handle_unusual;                                                   \
		}                                                                          \
		if (input->ExpectTag(_MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType))) {   \
			goto parse_##TAG##_loop;                                               \
		}                                                                          \
		NEXT()

#define PREV_IS_REPEATED_OBJECT(TAG, NAME, TYPE, NEXT) \
		if (input->ExpectTag(_MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType))) {   \
			goto parse_##TAG##_loop;                                               \
		}                                                                          \
		input->UnsafeDecrementRecursionDepth();                                    \
		break;                                                                     \
	}                                                                              \
	_FIRST_READ_REPEATED_OBJECT_CASE(TAG, NAME, SINGLE_ARG(TYPE), NEXT, )

#define PREV_NO_REPEATED_OBJECT(TAG, NAME, TYPE, NEXT) \
		if (input->ExpectTag(_MAKETAG(TAG, Type<SINGLE_ARG(TYPE)>::WireType))) {   \
			goto parse_##TAG;                                                      \
		}                                                                          \
		break;                                                                     \
	}                                                                              \
	_FIRST_READ_REPEATED_OBJECT_CASE(TAG, NAME, SINGLE_ARG(TYPE), NEXT, parse_##TAG:)

#define NEXT_IS_REPEATED_OBJECT()

#define NEXT_NO_REPEATED_OBJECT() \
	input->UnsafeDecrementRecursionDepth();

#define FIRST_READ_REPEATED_OBJECT_CASE(TAG, NAME, TYPE, PREV, NEXT) \
	_FIRST_READ_REPEATED_OBJECT_CASE(TAG, NAME, SINGLE_ARG(TYPE), NEXT, )

#define NEXT_READ_REPEATED_OBJECT_CASE(TAG, NAME, TYPE, PREV, NEXT) \
	PREV(TAG, NAME, SINGLE_ARG(TYPE), NEXT)

#define END_READ_CASE() \
		if (input->ExpectAtEnd()) { return true; }                                 \
		break;                                                                     \
	}                                                                              \
                                                                                   \
	default: {                                                                     \
	handle_unusual:                                                                \
		if (tag == 0 || WireFormatLite::GetTagWireType(tag) ==                     \
			WireFormatLite::WIRETYPE_END_GROUP) {                                  \
			return true;                                                           \
		}                                                                          \
		if (!GOOGLE_PREDICT_TRUE(WireFormatLite::SkipField(input, tag))) {         \
			return false;                                                          \
		}                                                                          \
		break;                                                                     \
	}}} return true;

#define TYPY_GETSET(OBJECT, NAME, TYPE) \
static PyObject* Get_##NAME(PyObject* self, void* closure) {                       \
	return ::typy::GetPyObject(static_cast<OBJECT*>(self)->NAME);                  \
}                                                                                  \
                                                                                   \
static int Set_##NAME(PyObject* self, PyObject* value, void* closure) {            \
	return ::typy::CheckAndSet(value, static_cast<OBJECT*>(self)->NAME,            \
		"Property '" #NAME "' expect " #TYPE ", but ") ? 0 : -1;                   \
}

template <typename T>
struct Type {
	typedef typename T::ValueType ValueType;
	enum { FieldType = T::FieldType, WireType = T::WireType };
};

//=============================================================================

template <typename T>
class List : public PyObject, public ::google::protobuf::RepeatedField<typename Type<T>::ValueType> {
public:
	typedef ::google::protobuf::RepeatedField<typename Type<T>::ValueType> RepeatedField;
	typedef List* ValueType;
	enum {
		FieldType = Type<T>::FieldType,
		WireType = Type<T>::WireType,
	};

	static PySequenceMethods SqMethods;
	static PyMappingMethods MpMethods;
	static PyMethodDef Methods[];
	static PyTypeObject _Type;

	mutable int _cached_size;

	List() : RepeatedField() {
		PyObject_INIT(this, &_Type);
		_cached_size = 0;
	}
	~List() { Clear(); }

	void Clear();

	typedef struct {
		PyObject_HEAD
		long it_index;
		List* it_seq; /* Set to NULL when iterator is exhausted */
	} Iterator;

	static PyMethodDef IteratorMethods[];
	static PyTypeObject Iterator_Type;
}; /* class List */

//=============================================================================

template <typename K, typename V>
class Dict : public PyObject, public ::google::protobuf::Map<typename Type<K>::KeyType, typename Type<V>::ValueType> {
public:
	typedef ::google::protobuf::Map<typename Type<K>::KeyType, typename Type<V>::ValueType> Map;
	typedef Dict* ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_MESSAGE,
		WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
	};

	static PyMappingMethods MpMethods;
	static PyMethodDef Methods[];
	static PyTypeObject _Type;

	Dict() : Map() {
		PyObject_INIT(this, &_Type);
	}
	~Dict() { Clear(); }

	void Clear();

	class Entry : public Message {
	public:
		typedef Entry* ValueType;
		enum {
			FieldType = WireFormatLite::TYPE_MESSAGE,
			WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
			KeyTag = GOOGLE_PROTOBUF_WIRE_FORMAT_MAKE_TAG(1, Type<K>::WireType),
			ValueTag = GOOGLE_PROTOBUF_WIRE_FORMAT_MAKE_TAG(2, Type<V>::WireType),
			ValuePackedTag = GOOGLE_PROTOBUF_WIRE_FORMAT_MAKE_TAG(2, WireFormatLite::WIRETYPE_LENGTH_DELIMITED),
		};

		typename Type<K>::KeyType key;
		typename Type<V>::ValueType value;

		::std::string GetTypeName() const { return #FULL_MODULE_NAME ".Dict.Entry"; }
		Entry* New() const { return new Entry; }
		bool IsInitialized() const { return true; }

		void CheckTypeAndMergeFrom(const MessageLite& from) {
			MergeFrom(*::google::protobuf::down_cast<const Entry*>(&from));
		}

		void CopyFrom(const Entry& from) {
			if (&from == this) { return; }
			Clear();
			MergeFrom(from);
		}

		Entry();
		void Clear();
		void MergeFrom(const Entry& from);
		void SerializeWithCachedSizes(CodedOutputStream* output) const;
		int ByteSize() const;
		int GetCachedSize() const;
		bool MergePartialFromCodedStream(CodedInputStream* input);
	}; /* class Entry */

	typedef struct {
		PyObject_HEAD
		PyObject* it_result; /* reusable result tuple for iteritems */
		typename Dict<K, V>::size_type it_index;
		typename Dict<K, V>::const_iterator it;
		Dict* it_dict; /* Set to NULL when iterator is exhausted */
	} Iterator;

	static PyMethodDef IteratorMethods[];
	static PyTypeObject IterKey_Type;
	static PyTypeObject IterItem_Type;
}; /* class Dict */

bool InitModule(PyObject* m);

} // namespace typy

#include "utils.h"
#include "object.h"
#include "shadow.h"
#include "abstract.h"
#include "variant.h"
#include "fixedpoint.h"

#endif // TYPY_TYPE_H__
