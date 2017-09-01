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
inline PyObject* GetPyObjectPy(const symbol&);
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
inline int  Visit(const TYPE& value, visitproc visit, void* arg) { return 0; }            \
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
		WireFormatLite::FieldType(Type<TYPE>::FieldType)>(input, &value); }               \
inline PyObject* Json(const TYPE& value, bool slim) {                                     \
	return (!slim || value != 0) ? GetPyObject(value) : NULL; }                           \
inline bool FromJson(TYPE& value, PyObject* json) {                                       \
	return CheckAndSet(json, value, "FromJson expect '" #TYPE "', but "); }

PRIMITIVE_VALUE_TYPE(int32, Int32, INT32, VARINT, Int32Size(value));
PRIMITIVE_VALUE_TYPE(int64, Int64, INT64, VARINT, Int64Size(value));
PRIMITIVE_VALUE_TYPE(uint32, UInt32, UINT32, VARINT, UInt32Size(value));
PRIMITIVE_VALUE_TYPE(uint64, UInt64, UINT64, VARINT, UInt64Size(value));
PRIMITIVE_VALUE_TYPE(double, Double, DOUBLE, FIXED64, kDoubleSize);
PRIMITIVE_VALUE_TYPE(float, Float, FLOAT, FIXED32, kFloatSize);
PRIMITIVE_VALUE_TYPE(bool, Bool, BOOL, VARINT, kBoolSize);

#undef PRIMITIVE_VALUE_TYPE

//=============================================================================

#define FROM_JSON_KEY_NUMBER(TYPE, PYTHON) \
inline bool FromJsonKey(TYPE& value, PyObject* json) {                                        \
	ScopedPyObjectPtr arg(PyNumber_##PYTHON(json));                                                \
	if (arg == NULL) { return false; }                                                        \
	return CheckAndSet(arg.get(), value, "FromJsonKey expect a string of '" #TYPE "', but "); \
}

FROM_JSON_KEY_NUMBER(int32, Int);
FROM_JSON_KEY_NUMBER(int64, Long);
FROM_JSON_KEY_NUMBER(uint32, Int);
FROM_JSON_KEY_NUMBER(uint64, Long);
FROM_JSON_KEY_NUMBER(float, Float);
FROM_JSON_KEY_NUMBER(double, Float);
FROM_JSON_KEY_NUMBER(bool, Int);

#undef FROM_JSON_KEY_NUMBER

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

template <> struct Type<symbol> {
	typedef ::std::string KeyType;
	typedef symbol ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_BYTES,
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

inline int Visit(const ::std::string& value, visitproc visit, void* arg) { return 0; }

inline int Visit(bytes& value, visitproc visit, void* arg) {
	Py_VISIT(value);
	return 0;
}

inline int Visit(string& value, visitproc visit, void* arg) {
	Py_VISIT(value);
	return 0;
}

inline int Visit(symbol& value, visitproc visit, void* arg) {
	Py_VISIT(value);
	return 0;
}

template <typename T>
inline int Visit(T*& value, visitproc visit, void* arg) {
	Py_VISIT(value);
	return 0;
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
bool CheckAndSet(PyObject* arg, symbol& value, const char* err);

template <typename T>
bool CheckAndSet(PyObject* arg, List<T>*& value, const char* err) {
	if (arg == Py_None) {
		Clear(value);
		return true;
	} else if (PyObject_TypeCheck(arg, &List<T>::_Type)) {
		CopyFrom(value, static_cast<List<T>*>(arg));
		return true;
	} else if (PySequence_Check(arg)) {
		Clear(value);
		value = new List<T>;
		return ExtendList(arg, *value);
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

template <typename K, typename V>
bool CheckAndSet(PyObject* arg, Dict<K, V>*& value, const char* err) {
	PyObject* items;
	if (arg == Py_None) {
		Clear(value);
		return true;
	} else if (PyObject_TypeCheck(arg, SINGLE_ARG(&Dict<K, V>::_Type))) {
		CopyFrom(value, static_cast<Dict<K, V>*>(arg));
		return true;
	} else if (PyDict_Check(arg)) {
		Clear(value);
		value = new Dict<K, V>;
		return MergeDict(arg, *value);
	} else if ((items = PyObject_CallMethod(arg, "iteritems", NULL)) != NULL) {
		Clear(value);
		value = new Dict<K, V>;
		register bool success = MergeIter(items, _PyObject_LengthHint(arg, 0), *value);
		Py_DECREF(items);
		return success;
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

inline void MergeFrom(symbol& lvalue, const symbol& rvalue) {
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

inline void ByteSize(int& total, int tagsize, const symbol& value) {
	if (value != NULL) {
		Py_ssize_t size = PyBytes_GET_SIZE(value);
		if (size > 0) {
			size = (size * 6 + 7) / 8;
			total += tagsize + CodedOutputStream::VarintSize32(static_cast<uint32>(size)) + size;
		}
	}
}

template <typename T>
inline void ByteSize(int& total, int tagsize, List<T>* value) {
	if (value != NULL) {
		for (int i = 0; i < value->size(); i++) {
			typename Type<T>::ValueType item = value->Get(i);
			if (item != NULL) {
				ByteSize(total, tagsize, item);
			} else {
				total += tagsize + CodedOutputStream::VarintSize32(0);
			}
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

inline void GetCachedSize(int& total, int tagsize, const symbol& value) {
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

static const char SymbolEncodeMap[256] = {
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF' /*-*/, '\xFF',       '\xFF',
	'\x35' /*0*/, '\x36' /*1*/, '\x37' /*2*/, '\x38' /*3*/, '\x39' /*4*/, '\x3A' /*5*/, '\x3B' /*6*/, '\x3C', /*7*/
	'\x3D' /*8*/, '\x3E' /*9*/, '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\x01' /*A*/, '\x02' /*B*/, '\x03' /*C*/, '\x04' /*D*/, '\x05' /*E*/, '\x06' /*F*/, '\x07' /*G*/,
	'\x08' /*H*/, '\x09' /*I*/, '\x0A' /*J*/, '\x0B' /*K*/, '\x0C' /*L*/, '\x0D' /*M*/, '\x0E' /*N*/, '\x0F' /*O*/,
	'\x10' /*P*/, '\x11' /*Q*/, '\x12' /*R*/, '\x13' /*S*/, '\x14' /*T*/, '\x15' /*U*/, '\x16' /*V*/, '\x17' /*W*/,
	'\x18' /*X*/, '\x19' /*Y*/, '\x1A' /*Z*/, '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\x3F' /*_*/,
	'\xFF',       '\x1B' /*a*/, '\x1C' /*b*/, '\x1D' /*c*/, '\x1E' /*d*/, '\x1F' /*e*/, '\x20' /*f*/, '\x21' /*g*/,
	'\x22' /*h*/, '\x23' /*i*/, '\x24' /*j*/, '\x25' /*k*/, '\x26' /*l*/, '\x27' /*m*/, '\x28' /*n*/, '\x29' /*o*/,
	'\x2A' /*p*/, '\x2B' /*q*/, '\x2C' /*r*/, '\x2D' /*s*/, '\x2E' /*t*/, '\x2F' /*u*/, '\x30' /*v*/, '\x31' /*w*/,
	'\x32' /*x*/, '\x33' /*y*/, '\x34' /*z*/, '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
	'\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',       '\xFF',
};

inline void Write(int field_number, const symbol& value, CodedOutputStream* output) {
	if (value != NULL) {
		Py_ssize_t size = PyBytes_GET_SIZE(value);
		if (size > 0) {
			int dstSize = (size * 6 + 7) / 8;
			WireFormatLite::WriteTag(field_number, WireFormatLite::WireType(Type<bytes>::WireType), output);
			GOOGLE_CHECK_LE(size, ::google::protobuf::kint32max);
			output->WriteVarint32(dstSize);

			unsigned int v;
			char* dst = new char[dstSize];
			char* src = PyBytes_AS_STRING(value);
			int di = 0, n = size / 4 * 4;
			for (int si = 0; si < n; si += 4) {
				// Convert 4x 6bit source bytes into 3 bytes
				v = (SymbolEncodeMap[src[si+0]] << 18) |
					(SymbolEncodeMap[src[si+1]] << 12) |
					(SymbolEncodeMap[src[si+2]] << 6) |
					(SymbolEncodeMap[src[si+3]] << 0);

				dst[di++] = (char)(0xFF & (v >> 16));
				dst[di++] = (char)(0xFF & (v >> 8));
				dst[di++] = (char)(0xFF & (v >> 0));
			}

			switch (size - n) {
			case 1:
				dst[di++] = (char)(0xFF & (SymbolEncodeMap[src[n]] << 2));
				break;
			case 2:
				dst[di++] = (char)(0xFF & ((SymbolEncodeMap[src[n]] << 2) |
										(SymbolEncodeMap[src[n + 1]] >> 4)));
				dst[di++] = (char)(0xFF & (SymbolEncodeMap[src[n + 1]] << 4));
				break;
			case 3:
				v = (SymbolEncodeMap[src[n+0]] << 18) |
					(SymbolEncodeMap[src[n+1]] << 12) |
					(SymbolEncodeMap[src[n+2]] << 6);
				dst[di++] = (char)(0xFF & (v >> 16));
				dst[di++] = (char)(0xFF & (v >> 8));
				dst[di++] = (char)(0xFF & (v >> 0));
				break;
			}

			output->WriteRaw(dst, dstSize);
			delete []dst;
		}
	}
}

template <typename T>
inline void Write(int field_number, List<T>* value, CodedOutputStream* output) {
	if (value != NULL) {
		for (unsigned int i = 0, n = value->size(); i < n; i++) {
			typename Type<T>::ValueType item = value->Get(i);
			if (item != NULL) {
				Write(field_number, item, output);
			} else {
				WireFormatLite::WriteTag(field_number, WireFormatLite::WireType(Type<T>::WireType), output);
				output->WriteVarint32(0);
			}
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

inline void WriteTag(int tag, const symbol& value, CodedOutputStream* output) {
	WireFormatLite::WriteTag(tag, WireFormatLite::WireType(Type<symbol>::WireType), output);
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

inline PyObject* Json(const ::std::string& value, bool slim) {
	return (!slim || value.size() > 0) ? GetPyObject(value) : NULL;
}

inline PyObject* Json(const bytes& value, bool slim) {
	return (!slim || (value != NULL && PyBytes_GET_SIZE(value) > 0)) ? GetPyObject(value) : NULL;
}

inline PyObject* Json(const string& value, bool slim) {
	return (!slim || (value != NULL && PyUnicode_GET_SIZE(value) > 0)) ? GetPyObject(value) : NULL;
}

inline PyObject* Json(const symbol& value, bool slim) {
	return (!slim || (value != NULL && PyBytes_GET_SIZE(value) > 0)) ? GetPyObject(value) : NULL;
}

template <typename T>
inline PyObject* Json(T* value, bool slim) {
	if (!slim && value == NULL) {
		Py_RETURN_NONE;
	} else if (value != NULL) {
		return value->T::Json(slim);
	} else {
		return NULL;
	}
}

template <typename T>
inline PyObject* Json(List<T>* value, bool slim) {
	if (!slim && value == NULL) {
		return PyList_New(0);
	} else if (value != NULL) {
		PyObject* list = PyList_New(value->size());
		if (list == NULL) { return NULL; }
		for (int i = 0; i < value->size(); i++) {
			PyObject* item = ::typy::Json(value->Get(i), slim);
			if (item == NULL) {
				Py_INCREF(Py_None);
				item = Py_None;
			}
			PyList_SetItem(list, i, item);
		}
		return list;
	} else {
		return NULL;
	}
}

template <typename K, typename V>
inline PyObject* Json(Dict<K, V>* value, bool slim) {
	if (!slim && value == NULL) {
		return PyDict_New();
	} else if (value != NULL) {
		PyObject* dict = PyDict_New();
		for (typename Dict<K, V>::const_iterator it = value->begin(); it != value->end(); ++it) {
			ScopedPyObjectPtr k(::typy::GetPyObject(it->first));
			ScopedPyObjectPtr key(PyObject_Str(k.get()));
			ScopedPyObjectPtr value(::typy::Json(it->second, slim));
			if (value == NULL) {
				Py_INCREF(Py_None);
				value.reset(Py_None);
			}
			PyDict_SetItem(dict, key.get(), value.get());
		}
		return dict;
	} else {
		return NULL;
	}
}

//=============================================================================

inline bool FromJsonKey(::std::string& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

inline bool FromJsonKey(bytes& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

inline bool FromJsonKey(string& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

inline bool FromJsonKey(symbol& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

inline bool FromJson(::std::string& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

inline bool FromJson(bytes& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

inline bool FromJson(string& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

inline bool FromJson(symbol& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect String, but ");
}

template <typename T>
inline bool FromJson(T*& value, PyObject* json) {
	if (json == NULL || json == Py_None) {
		Clear(value);
		return true;
	}
	T* object = T::FromJson(json);
	if (object == NULL) { return false; }
	CopyFrom(value, object);
	return true;
}

template <typename T>
inline bool FromJson(List<T>*& value, PyObject* json) {
	if (value == NULL) { value = new List<T>; }
	if (PyList_CheckExact(json) || PyTuple_CheckExact(json)) {
		ScopedPyObjectPtr list(PySequence_Fast(json, "argument must be iterable"));
		if (list == NULL) { return false; }
		register Py_ssize_t i, size = PySequence_Fast_GET_SIZE(list.get());
		register PyObject** src = PySequence_Fast_ITEMS(list.get());
		for (i = 0; i < size; i++) {
			if (!FromJson(*value->Add(), src[i])) {
				value->RemoveLast();
				return false;
			}
		}
		return true;
	}
	register Py_ssize_t i, size = _PyObject_LengthHint(json, 0);
	if (size < 0) { return false; } else if (!size) { return true; }
	ScopedPyObjectPtr it(PyObject_GetIter(json));
	if (it == NULL) { return false; }
	register iternextfunc iternext = *it.get()->ob_type->tp_iternext;
	for (i = 0; i < size; i++) {
		if (!FromJson(*value->Add(), ScopedPyObjectPtr(iternext(it.get())).get())) {
			value->RemoveLast();
			return false;
		}
	}
	return true;
}

template <typename K, typename V>
inline bool FromJson(Dict<K, V>*& dict, PyObject* json) {
	ScopedPyObjectPtr iter(PyObject_CallMethod(json, "iteritems", NULL));
	if (iter == NULL) {
		FormatTypeError(json, "FromJson expect dict, but ");
		return false;
	}
	if (dict == NULL) { dict = new Dict<K, V>; }
	Py_ssize_t size = _PyObject_LengthHint(json, 0);
	for (Py_ssize_t i = 0; i < size; i++) {
		ScopedPyObjectPtr item(PyIter_Next(iter.get()));
		typename Type<K>::KeyType key;
		if (!FromJsonKey(key, PyTuple_GET_ITEM(item.get(), 0))) { return false; }
		if (!FromJson((*dict)[key], PyTuple_GET_ITEM(item.get(), 1))) {
			dict->erase(key);
			return false;
		}
	}
	return true;
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

static const char SymbolDecodeMap[] = "-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

inline bool Read(symbol& value, CodedInputStream* input) {
	uint32 size;
	input->ReadVarint32(&size);
	value = reinterpret_cast<symbol>(PyBytes_FromStringAndSize(NULL, size * 4 / 3));
	if (value == NULL) {
		return false;
	}
	char* src = new char[size];
	if (!input->ReadRaw(src, size)) {
		delete []src;
		return false;
	}

	unsigned int v;
	int di = 0, n = size / 3 * 3;
	char* dst = PyBytes_AS_STRING(value);
	for (int si = 0; si < n; si += 3) {
		// Convert 3x 8bit source bytes into 4 bytes
		v = ((unsigned char)(src[si+0]) << 16) |
			((unsigned char)(src[si+1]) << 8) |
			((unsigned char)(src[si+2]) << 0);

		dst[di++] = SymbolDecodeMap[0x3F & (v >> 18)];
		dst[di++] = SymbolDecodeMap[0x3F & (v >> 12)];
		dst[di++] = SymbolDecodeMap[0x3F & (v >> 6)];
		dst[di++] = SymbolDecodeMap[0x3F & (v >> 0)];
	}

	switch (size - n) {
	case 1:
		dst[di++] = SymbolDecodeMap[0x3F & ((unsigned char)(src[n]) >> 2)];
		break;
	case 2:
		v = ((unsigned char)(src[n + 0]) << 8) |
			((unsigned char)(src[n + 1]) << 0);
		dst[di++] = SymbolDecodeMap[0x3F & (v >> 10)];
		dst[di++] = SymbolDecodeMap[0x3F & (v >> 4)];
		break;
	}

	if (dst[di - 1] == SymbolDecodeMap[0]) {
		dst[di - 1] = 0;
		PyString_GET_SIZE(value)--;
	}
	delete []src;
	return true;
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
