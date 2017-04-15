// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_PYTHON_H__
#define TYPY_PYTHON_H__

#include "typy.h"

namespace typy {

template <typename T>
class Python : public Message {
public:
	typedef Python* ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_MESSAGE,
		WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
	};
	static PyMethodDef _InitDef;
	static PyTypeObject* _Type;

	Python() : Message() {
		object = NULL;
		_cached_size = 0;
		_cached_data = NULL;
	}
	~Python() { Clear(); }

	inline Python* New() const { return new Python; }
	inline ::std::string GetTypeName() const { return FULL_MODULE_NAME ".Python"; }
	inline bool IsInitialized() const { return true; }
	inline int GetCachedSize() const { return _cached_size; }

	inline void Clear() {
		Py_XDECREF(object);
		object = NULL;
		Py_XDECREF(_cached_data);
		_cached_data = NULL;
	}

	inline void CopyFrom(const Python& from) {
		if (&from == this) { return; }
		Py_XDECREF(object);
		Py_XINCREF(from.object);
		object = from.object;
	}

	inline void MergeFrom(const Python& from) {
		if (&from == this || !from.object) { return; }
		Py_XDECREF(object);
		Py_INCREF(from.object);
		object = from.object;
	}

	inline void CheckTypeAndMergeFrom(const Message& from) {
		MergeFrom(*::google::protobuf::down_cast<const Python*>(&from));
	}

	inline int ByteSize() const {
		int total_size = 0;
		PyObject* data = NULL;
		if (object != NULL) {
			data = PyObject_CallMethod(object, "Serialize", NULL);
			if (data == NULL) {
				return 0;
			}
			total_size = PyBytes_GET_SIZE(data);
		}
		GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
		_cached_size = total_size;
		Py_XDECREF(_cached_data);
		_cached_data = data;
		GOOGLE_SAFE_CONCURRENT_WRITES_END();
		return total_size;
	}

	inline void SerializeWithCachedSizes(CodedOutputStream* output) const {
		if (_cached_data != NULL) {
			output->WriteRaw(PyBytes_AS_STRING(_cached_data), PyBytes_GET_SIZE(_cached_data));
		}
	}

	inline bool MergePartialFromCodedStream(CodedInputStream* input) {
		const void* data;
		int size;
		input->GetDirectBufferPointerInline(&data, &size);
		input->Skip(size);
		input->ExpectAtEnd();
		ScopedPyObjectPtr str(PyBytes_FromStringAndSize(static_cast<const char*>(data), size));
		if (object == NULL) {
			object = PyObject_CallObject((PyObject*)_Type, NULL);
		}
		if (object != NULL) {
			ScopedPyObjectPtr result(PyObject_CallMethod(object, "Deserialize", "O", str.get()));
		}
		return true;
	}

	inline PyObject* Json(bool slim) {
		if (!slim || object != NULL) {
			PyObject* json = object == NULL ? PyDict_New() : PyObject_CallMethod(object, "Json", NULL);
			PyDict_SetItemString(json, "_t", PyString_FromString(_Type->tp_name));
			return json;
		} else {
			return NULL;
		}
	}

	inline static Python* FromJson(PyObject* json) {
		ScopedPyObjectPtr iter(PyObject_CallMethod(json, "iteritems", NULL));
		if (iter == NULL) {
			FormatTypeError(json, "FromJson expect dict, but ");
			return false;
		}
		ScopedPyObjectPtr dict(PyDict_New());
		if (dict == NULL) { return NULL; }
		bool type_check = false;
		Py_ssize_t size = _PyObject_LengthHint(iter.get(), 0);
		for (Py_ssize_t i = 0; i < size; i++) {
			ScopedPyObjectPtr item(PyIter_Next(iter.get()));
			PyObject* k = PyTuple_GET_ITEM(item.get(), 0);
			PyObject* v = PyTuple_GET_ITEM(item.get(), 1);
			if (PyBytes_Check(k) && !strcmp(PyBytes_AS_STRING(k), "_t")) {
				if (!PyBytes_Check(v)) {
					FormatTypeError(v, "Json _t expect String, but ");
					return NULL;
				} else if (strcmp(PyBytes_AS_STRING(v), _Type->tp_name)) {
					PyErr_Format(PyExc_TypeError, "Python expect '%.100s', but Json has type %.100s",
						_Type->tp_name, PyBytes_AS_STRING(v));
					return NULL;
				}
				type_check = true;
				continue;
			}
			PyDict_SetItem(dict.get(), k, v);
		}
		if (!type_check) {
			FormatTypeError(json, "Json expect _t, ");
			return NULL;
		}
		PyObject* o = PyObject_CallMethod((PyObject*)_Type, "FromJson", "O", dict.get());
		if (o == NULL) { return NULL; }
		Python* self = new Python;
		self->object = o;
		return self;
	}

	PyObject* object;

	static PyObject* _Init(PyObject* m, PyObject* arg) {
		if (PyType_Check(arg)) {
			_Type = reinterpret_cast<PyTypeObject*>(arg);
			Py_RETURN_NONE;
		} else {
			FormatTypeError(arg, "Python expect a type, but ");
			return NULL;
		}
	}

	static bool Init(PyObject* m, const char* name) {
		PyCFunctionObject* method = reinterpret_cast<PyCFunctionObject*>(
			PyType_GenericAlloc(&PyCFunction_Type, 0));
		method->m_ml = &_InitDef;
		method->m_self = m;
		method->m_module = NULL;
		PyModule_AddObject(m, name, reinterpret_cast<PyObject*>(method));
		return true;
	}

private:
	mutable int _cached_size;
	mutable PyObject* _cached_data;
}; /* class Python */

template <typename T>
PyTypeObject* Python<T>::_Type = &PyBaseObject_Type;

template <typename T>
PyMethodDef Python<T>::_InitDef = { "InitPython", (PyCFunction)_Init, METH_O,
	"Initialize Python Type." };

template <typename T>
inline PyObject* GetPyObject(Python<T>* value) {
	if (value == NULL) { Py_RETURN_NONE; }
	Py_INCREF(value->object);
	return value->object;
}

template <typename T>
inline void CopyFrom(Python<T>*& lvalue, Python<T>* rvalue) {
	if (rvalue == NULL) { delete lvalue; lvalue = NULL; return; }
	if (lvalue == NULL) { lvalue = new Python<T>; }
	lvalue->Python<T>::CopyFrom(*rvalue);
}

template <typename T>
inline void Clear(Python<T>*& value) {
	delete value; value = NULL;
}

template <typename T>
inline bool CheckAndSet(PyObject* arg, Python<T>*& value, const char* err) {
	if (arg == Py_None) {
		Clear(value);
		return true;
	} else if (PyObject_TypeCheck(arg, Python<T>::_Type)) {
		if (value == NULL) { value = new Python<T>; }
		else { value->Clear(); }
		Py_INCREF(arg);
		value->object = arg;
		return true;
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

} // namespace typy
#endif // TYPY_PYTHON_H__
