// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_ABSTRACT_H__
#define TYPY_ABSTRACT_H__

namespace typy {

#define GetPyObjectPy(TYPE) \
GetPyObject(TYPE value) {                      \
	if (value == NULL) { Py_RETURN_NONE; }     \
	Py_INCREF(value);                          \
	return reinterpret_cast<PyObject*>(value); \
}

inline PyObject* GetPyObject(const int32& value) { return PyInt_FromLong(value); }
inline PyObject* GetPyObject(const int64& value) { return PyLong_FromLongLong(value); }
inline PyObject* GetPyObject(const uint32& value) { return PyInt_FromSize_t(value); }
inline PyObject* GetPyObject(const uint64& value) { return PyLong_FromUnsignedLongLong(value); }
inline PyObject* GetPyObject(const double& value) { return PyFloat_FromDouble(value); }
inline PyObject* GetPyObject(const float& value) { return PyFloat_FromDouble(value); }
inline PyObject* GetPyObject(const bool& value) { return PyBool_FromLong(value); }
inline PyObject* GetPyObjectPy(const string&);
inline PyObject* GetPyObjectPy(const bytes&);
PyObject* GetPyObject(const ::std::string& value);

template <typename T>
inline PyObject* GetPyObject(List<T>*& value) {
	if (value == NULL) {
		value = new List<T>;
	}
	Py_INCREF(value);
	return value;
}

template <typename K, typename V>
inline PyObject* GetPyObject(Dict<K, V>*& value) {
	if (value == NULL) {
		value = new Dict<K, V>;
	}
	Py_INCREF(value);
	return value;
}

template <typename T>
inline PyObject* GetPyObject(T* value) {
	if (value == NULL) {
		Py_RETURN_NONE;
	}
	Py_INCREF(value);
	return value;
}

#undef GetPyObjectPy

#define PRIMITIVE_VALUE_TYPE(TYPE, Name, NAME, WIRETYPE, SIZE) \
template <> struct Type<TYPE> {                                                           \
	typedef TYPE KeyType;                                                                 \
	typedef TYPE ValueType;                                                               \
	enum {                                                                                \
		FieldType = WireFormatLite::TYPE_##NAME,                                          \
		WireType = WireFormatLite::WIRETYPE_##WIRETYPE,                                   \
	};                                                                                    \
};                                                                                        \
bool CheckAndSet(PyObject* arg, TYPE& value, const char* err);                            \
inline void CopyFrom(TYPE& lvalue, const TYPE& rvalue) { lvalue = rvalue; }               \
inline void Clear(TYPE& value) { value = 0; }                                             \
inline void MergeFrom(TYPE& lvalue, const TYPE& rvalue) {                                 \
	if (rvalue != 0) { CopyFrom(lvalue, rvalue); } }                                      \
inline void ByteSize(int& total, int tagsize, const TYPE& value) {                        \
	if (value != 0) { total += tagsize + WireFormatLite::SIZE; } }                        \
inline void Write(int field_number, const TYPE& value, CodedOutputStream* output) {       \
	if (value != 0) { WireFormatLite::Write##Name(field_number, value, output); } }       \
inline void WriteTag(int tag, const TYPE& value, CodedOutputStream* output) {             \
	WireFormatLite::WriteTag(tag, WireFormatLite::WIRETYPE_##WIRETYPE, output); }         \
inline bool Read(TYPE& value, CodedInputStream* input) {                                  \
	return WireFormatLite::ReadPrimitive<TYPE,                                            \
		WireFormatLite::FieldType(Type<TYPE>::FieldType)>(input, &value); }

PRIMITIVE_VALUE_TYPE(int32, Int32, INT32, VARINT, Int32Size(value));
PRIMITIVE_VALUE_TYPE(int64, Int64, INT64, VARINT, Int64Size(value));
PRIMITIVE_VALUE_TYPE(uint32, UInt32, UINT32, VARINT, UInt32Size(value));
PRIMITIVE_VALUE_TYPE(uint64, UInt64, UINT64, VARINT, UInt64Size(value));
PRIMITIVE_VALUE_TYPE(double, Double, DOUBLE, FIXED64, kDoubleSize);
PRIMITIVE_VALUE_TYPE(float, Float, FLOAT, FIXED32, kFloatSize);
PRIMITIVE_VALUE_TYPE(bool, Bool, BOOL, VARINT, kBoolSize);

#undef PRIMITIVE_VALUE_TYPE

//=============================================================================

template <> struct Type< ::std::string> {
	typedef ::std::string KeyType;
	typedef ::std::string ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_STRING,
		WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
	};
};

template <> struct Type<bytes> {
	typedef ::std::string KeyType;
	typedef bytes ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_BYTES,
		WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
	};
};

template <> struct Type<string> {
	typedef ::std::string KeyType;
	typedef string ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_STRING,
		WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
	};
};

inline void CopyFrom(::std::string& value, const char* s, size_t n) { value.assign(s, n); }                      \
inline void CopyFrom(::std::string& lvalue, const ::std::string& rvalue) { lvalue = rvalue; }

template <typename T>
inline void CopyFrom(T*& lvalue, T* rvalue) {
	Py_XDECREF(lvalue);
	if (rvalue == NULL) { lvalue = NULL; return; }
	Py_INCREF(rvalue);
	lvalue = rvalue;
}

inline void Clear(::std::string& value) { value.clear(); }

template <typename T>
inline void Clear(T*& value) {
	Py_XDECREF(value);
	value = NULL;
}

//=============================================================================

bool CheckAndSet(PyObject* arg, ::std::string& value, const char* err);
bool CheckAndSet(PyObject* arg, bytes& value, const char* err);
bool CheckAndSet(PyObject* arg, string& value, const char* err);

template <typename T>
bool CheckAndSet(PyObject* arg, List<T>*& value, const char* err) {
	if (arg == Py_None) {
		Clear(value);
		return true;
	} else if (PyObject_TypeCheck(arg, &List<T>::_Type)) {
		CopyFrom(value, static_cast<List<T>*>(arg));
		return true;
	} else if (PySequence_Check(arg)) {
		if (value == NULL) { value = new List<T>; }
		return CheckAndSetList(arg, *value);
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

template <typename K, typename V>
bool CheckAndSet(PyObject* arg, Dict<K, V>*& value, const char* err) {
	if (arg == Py_None) {
		Clear(value);
		return true;
	} else if (PyObject_TypeCheck(arg, SINGLE_ARG(&Dict<K, V>::_Type))) {
		CopyFrom(value, static_cast<Dict<K, V>*>(arg));
		return true;
	} else if (PyDict_Check(arg)) {
		if (value == NULL) { value = new Dict<K, V>; }
		return CheckAndSetDict(arg, *value);
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

template <typename T>
bool CheckAndSet(PyObject* arg, T*& value, const char* err) {
	if (arg == Py_None) {
		Clear(value);
		return true;
	}
	if (!PyObject_TypeCheck(arg, &Object<T>::_Type)) {
		FormatTypeError(arg, err);
		return false;
	}
	CopyFrom(value, static_cast<T*>(arg));
	return true;
}

//=============================================================================

inline void MergeFrom(::std::string& lvalue, const ::std::string& rvalue) {
	if (rvalue.size() > 0) { CopyFrom(lvalue, rvalue); }
}

inline void MergeFrom(bytes& lvalue, const bytes& rvalue) {
	if (rvalue != NULL) { CopyFrom(lvalue, rvalue); }
}

inline void MergeFrom(string& lvalue, const string& rvalue) {
	if (rvalue != NULL) { CopyFrom(lvalue, rvalue); }
}

template <typename T>
inline void MergeFrom(List<T>*& lvalue, List<T>* rvalue) {
	if (rvalue == NULL) { return; }
	if (lvalue == NULL) { lvalue = new List<T>; }
	for (int i = 0; i < rvalue->size(); i++) {
		CopyFrom(*lvalue->Add(), rvalue->Get(i));
	}
}

template <typename K, typename V>
inline void MergeFrom(Dict<K, V>*& lvalue, Dict<K, V>* rvalue) {
	if (rvalue == NULL) { return; }
	if (lvalue == NULL) { lvalue = new Dict<K, V>; }
	for (typename Dict<K, V>::const_iterator it = rvalue->begin(); it != rvalue->end(); ++it) {
		CopyFrom((*lvalue)[it->first], it->second);
	}
}

template <typename T>
inline void MergeFrom(T*& lvalue, T* rvalue) {
	if (rvalue == NULL) { return; }
	if (lvalue == NULL) { lvalue = new T; }
	lvalue->T::MergeFrom(*rvalue);
}

//=============================================================================

inline PyObject* _DecodeString(const string& value) {
	PyObject* result = NULL;
	if (isDefaultEncodingUTF8) {
		result = _PyUnicode_AsDefaultEncodedString(reinterpret_cast<PyObject*>(value), NULL);
		Py_INCREF(result);
	} else {
		result = PyUnicode_AsEncodedObject(reinterpret_cast<PyObject*>(value), "utf-8", NULL);
	}
	return result;
}

inline void ByteSize(int& total, int tagsize, const ::std::string& value) {
	size_t size = value.size();
	if (size > 0) {
		total += tagsize + CodedOutputStream::VarintSize32(static_cast<uint32>(size)) + size;
	}
}

inline void ByteSize(int& total, int tagsize, const bytes& value) {
	if (value != NULL) {
		Py_ssize_t size = PyBytes_GET_SIZE(value);
		if (size > 0) {
			total += tagsize + CodedOutputStream::VarintSize32(static_cast<uint32>(size)) + size;
		}
	}
}

inline void ByteSize(int& total, int tagsize, const string& value) {
	if (value != NULL) {
		ScopedPyObjectPtr encoded_string(_DecodeString(value));
		ByteSize(total, tagsize, reinterpret_cast<bytes>(encoded_string.get()));
	}
}

template <typename T>
inline void ByteSize(int& total, int tagsize, List<T>* value) {
	if (value != NULL) {
		for (int i = 0; i < value->size(); i++) {
			ByteSize(total, tagsize, value->Get(i));
		}
	}
}

template <typename K, typename V>
inline void ByteSize(int& total, int tagsize, Dict<K, V>* value) {
	if (value != NULL) {
		total += tagsize * value->size();
		typename Dict<K, V>::Entry entry;
		for (typename Dict<K, V>::const_iterator it = value->begin(); it != value->end(); ++it) {
			entry.key = it->first;
			entry.value = it->second;
			total += WireFormatLite::MessageSizeNoVirtual(entry);
		}
	}
}

template <typename T>
inline void ByteSize(int& total, int tagsize, T* value) {
	if (value != NULL) {
		total += tagsize + WireFormatLite::MessageSizeNoVirtual(*value);
	}
}

//=============================================================================

inline void GetCachedSize(int& total, int tagsize, const ::std::string& value) {
	ByteSize(total, tagsize, value);
}

inline void GetCachedSize(int& total, int tagsize, const bytes& value) {
	ByteSize(total, tagsize, value);
}

inline void GetCachedSize(int& total, int tagsize, const string& value) {
	ByteSize(total, tagsize, value);
}

template <typename T>
inline void GetCachedSize(int& total, int tagsize, List<T>* value) {
	if (value != NULL) { ByteSize(total, tagsize, value); }
}

template <typename K, typename V>
inline void GetCachedSize(int& total, int tagsize, Dict<K, V>* value) {
	if (value != NULL) { ByteSize(total, tagsize, value); }
}

template <typename T>
inline void GetCachedSize(int& total, int tagsize, const T& value) {
	ByteSize(total, tagsize, value);
}

template <typename T>
inline void GetCachedSize(int& total, int tagsize, T* value) {
	if (value != NULL) {
		total += tagsize + WireFormatLite::LengthDelimitedSize(value->GetCachedSize());
	}
}

//=============================================================================

inline void Write(int field_number, const ::std::string& value, CodedOutputStream* output) {
	size_t size = value.size();
	if (size > 0) {
		WireFormatLite::WriteTag(field_number, WireFormatLite::WireType(Type< ::std::string>::WireType), output);
		GOOGLE_CHECK_LE(size, ::google::protobuf::kint32max);
		output->WriteVarint32(size);
		output->WriteRaw(value.data(), size);
	}
}

inline void Write(int field_number, const bytes& value, CodedOutputStream* output) {
	if (value != NULL) {
		Py_ssize_t size = PyBytes_GET_SIZE(value);
		if (size > 0) {
			WireFormatLite::WriteTag(field_number, WireFormatLite::WireType(Type<bytes>::WireType), output);
			GOOGLE_CHECK_LE(size, ::google::protobuf::kint32max);
			output->WriteVarint32(size);
			output->WriteRaw(PyBytes_AS_STRING(value), size);
		}
	}
}

inline void Write(int field_number, const string& value, CodedOutputStream* output) {
	if (value != NULL) {
		ScopedPyObjectPtr encoded_string(_DecodeString(value));
		Write(field_number, reinterpret_cast<bytes>(encoded_string.get()), output);
	}
}

template <typename T>
inline void Write(int field_number, List<T>* value, CodedOutputStream* output) {
	if (value != NULL) {
		for (unsigned int i = 0, n = value->size(); i < n; i++) {
			Write(field_number, value->Get(i), output);
		}
	}
}

template <typename K, typename V>
inline void Write(int field_number, Dict<K, V>* value, CodedOutputStream* output) {
	if (value != NULL) {
		typename Dict<K, V>::Entry entry;
		for (typename Dict<K, V>::const_iterator it = value->begin(); it != value->end(); ++it) {
			entry.key = it->first;
			entry.value = it->second;
			WireFormatLite::WriteMessage(field_number, entry, output);
		}
	}
}

template <typename T>
inline void Write(int field_number, T* value, CodedOutputStream* output) {
	if (value != NULL) {
		WireFormatLite::WriteMessage(field_number, *value, output);
	}
}

//=============================================================================

inline void WriteTag(int tag, const ::std::string& value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WireType(Type< ::std::string>::WireType), output);
}

inline void WriteTag(int tag, const bytes& value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WireType(Type<bytes>::WireType), output);
}

inline void WriteTag(int tag, const string& value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WireType(Type<string>::WireType), output);
}

template <typename T>
inline void WriteTag(int tag, List<T>* value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WireType(Type<T>::WireType), output);
}

template <typename K, typename V>
inline void WriteTag(int tag, Dict<K, V>* value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
}

template <typename T>
inline void WriteTag(int tag, T* value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
}

//=============================================================================

inline bool Read(::std::string& value, CodedInputStream* input) {
	uint32 size;
	input->ReadVarint32(&size);
	value.resize(size);
	if (!input->ReadRaw(&*value.begin(), size)) {
		return false;
	}
	return true;
}

inline bool Read(bytes& value, CodedInputStream* input) {
	uint32 size;
	input->ReadVarint32(&size);
	value = reinterpret_cast<bytes>(PyBytes_FromStringAndSize(NULL, size));
	if (value == NULL) {
		return false;
	}
	if (!input->ReadRaw(PyBytes_AS_STRING(value), size)) {
		Clear(value);
		return false;
	}
	return true;
}

inline bool Read(string& value, CodedInputStream* input) {
	bytes o = NULL;
	bool success = Read(o, input);
	if (success) {
		value = reinterpret_cast<string>(PyUnicode_FromEncodedObject(
			reinterpret_cast<PyObject*>(o), "utf-8", NULL));
	}
	Clear(o);
	return success;
}

template <typename T>
inline bool Read(T*& value, CodedInputStream* input) {
	if (value == NULL) { value = new T; }
	if (!WireFormatLite::ReadMessageNoVirtual(input, value)) {
		Clear(value);
		return false;
	}
	return true;
}

template <typename K, typename V>
inline bool Read(Dict<K, V>*& value, CodedInputStream* input) {
	typename Dict<K, V>::Entry* entry = NULL;
	bool success = Read(entry, input);
	if (success) {
		if (value == NULL) { value = new Dict<K, V>; }
		CopyFrom((*value)[entry->key], entry->value);
	}
	if (entry != NULL) {
		entry->Clear();
		delete entry;
	}
	return success;
}

template <typename T>
inline bool Read(List<T>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<T>; }
	T** item = value->Add();
	*item = new T;
	if (!WireFormatLite::ReadMessageNoVirtualNoRecursionDepth(input, *item)) {
		Clear(item);
		value->RemoveLast();
		return false;
	}
	return true;
}

//=============================================================================

inline void Initialize(::std::string& value) { value.clear(); }

template <typename T>
inline void Initialize(T& value) { value = static_cast<T>(0); }

} // namespace typy

#endif // TYPY_ABSTRACT_H__
