// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_OBJECT_H__
#define TYPY_OBJECT_H__

#include "typy.h"

#define TypyHeaderBegin(OBJECT) \
namespace typy {                                                               \
                                                                               \
class OBJECT : public PyObject, public Message {                               \
public:                                                                        \
	typedef OBJECT* ValueType;                                                 \
	enum {                                                                     \
		FieldType = WireFormatLite::TYPE_MESSAGE,                              \
		WireType = WireFormatLite::WIRETYPE_LENGTH_DELIMITED,                  \
	};                                                                         \
                                                                               \
	static const char* FullName;                                               \
	static const char* Name;                                                   \
                                                                               \
	static const int PropertyCount;                                            \
	static char* Properties[];                                                 \
                                                                               \
	void* operator new(size_t size) {                                          \
		OBJECT* object = reinterpret_cast<OBJECT*>(malloc(size));              \
		if (object != NULL) {                                                  \
			(void)PyObject_INIT(object, &Object<OBJECT>::_Type);               \
		}                                                                      \
		return object;                                                         \
	}                                                                          \
                                                                               \
	void operator delete(void* ptr) {                                          \
		free(ptr);                                                             \
	}                                                                          \
                                                                               \
	OBJECT();                                                                  \
	~OBJECT() { Clear(); }                                                     \
                                                                               \
	OBJECT* New() const;                                                       \
	void Clear();                                                              \
	void CheckTypeAndMergeFrom(const Message&);                                \
	bool IsInitialized() const { return true; }                                \
	void CopyFrom(const OBJECT&);                                              \
	void MergeFrom(const OBJECT&);                                             \
	int GetCachedSize() const { return _cached_size; }                         \
                                                                               \
	::std::string GetTypeName() const;                                         \
                                                                               \
	int ByteSize() const;                                                      \
	bool MergePartialFromCodedStream(CodedInputStream*);                       \
	void SerializeWithCachedSizes(CodedOutputStream*) const;                   \
	bool SetPropertySequence(PyObject*);                                       \
	PyObject* GetPropertySequence();                                           \
	PyObject* Json(bool);                                                      \
	static OBJECT* FromJson(PyObject*);                                        \
	int Visit(visitproc, void*);                                               \
                                                                               \
	char* PropertyName(int);                                                   \
	int PropertyTag(char*);                                                    \
	int PropertyByteSize(int) const;                                           \
	int DeserializeProperty(CodedInputStream*);                                \
	void SerializeProperty(CodedOutputStream*, int) const;

#define TypyHeaderEnd(OBJECT) \
	mutable int _cached_size; \
}; /* class OBJECT */         \
                              \
} /* namespace typy */

#define TypyObjectBegin(OBJECT) \
namespace typy {                                                               \
                                                                               \
const char* OBJECT::FullName = FULL_MODULE_NAME "." #OBJECT;                   \
const char* OBJECT::Name = OBJECT::FullName + FULL_NAME_LEN;                   \
                                                                               \
char* OBJECT::PropertyName(int tag) {                                          \
	if (tag < 1 || tag > OBJECT::PropertyCount) {                              \
		return NULL;                                                           \
	} else {                                                                   \
		return OBJECT::Properties[tag - 1];                                    \
	}                                                                          \
}                                                                              \
                                                                               \
int OBJECT::PropertyTag(char* name) {                                          \
	int tag;                                                                   \
	for (tag = 0; tag < OBJECT::PropertyCount; tag ++) {                       \
		if (strcmp(name, OBJECT::Properties[tag]) == 0) {                      \
			return tag + 1;                                                    \
		}                                                                      \
	}                                                                          \
	return 0;                                                                  \
}                                                                              \
                                                                               \
namespace {                                                                    \
                                                                               \
static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;                     \
static void MergeFromFail(int line) {                                          \
	GOOGLE_CHECK(false) << __FILE__ << ":" << line;                            \
}                                                                              \
                                                                               \
} /* namespace */                                                              \
                                                                               \
::std::string OBJECT::GetTypeName() const {                                    \
	static std::string str(FullName);                                          \
	return str;                                                                \
}                                                                              \
                                                                               \
OBJECT* OBJECT::New() const {                                                  \
	return new OBJECT;                                                         \
}                                                                              \
                                                                               \
void OBJECT::CheckTypeAndMergeFrom(const MessageLite& from) {                  \
	MergeFrom(*::google::protobuf::down_cast<const OBJECT*>(&from));           \
}                                                                              \
                                                                               \
void OBJECT::CopyFrom(const OBJECT& from) {                                    \
	if (&from == this) { return; }                                             \
	Clear();                                                                   \
	MergeFrom(from);                                                           \
}

#define TypyObjectEnd(OBJECT) \
} /* namespace typy */

namespace typy {

PyObject* CallObject(PyObject* self, const char *name);

template<typename... Args>
PyObject* CallObject(PyObject* self, const char *name, Args... args) {
	PyObject* method = PyDict_GetItemString(Py_TYPE(self)->tp_dict, name);
	if (method != NULL && PyMethod_Check(method)) {
		descrgetfunc f = Py_TYPE(method)->tp_descr_get;
		if (f != NULL) {
			ScopedPyObjectPtr m(f(method, self, reinterpret_cast<PyObject*>(Py_TYPE(self))));
			ScopedPyObjectPtr a(PyTuple_Pack(sizeof...(Args), args...));
			if (a != NULL) {
				return PyEval_CallObject(m.get(), a.get());
			}
		}
	}
	return NULL;
}

template <typename T>
class Object {
public:
	static PyGetSetDef GetSet[];
	static PyMethodDef Methods[];
	static PyNumberMethods NbMethods;
	static PyMappingMethods MpMethods;
	static PySequenceMethods SqMethods;
	static PyMethodDef _InitDef;
	static PyTypeObject _Type;

	static void tp_Dealloc(PyObject* self) {
		delete static_cast<T*>(self);
	}

	static PyObject* tp_New(PyTypeObject* cls, PyObject* args, PyObject* kwargs) {
		T* object = new T;
		PyObject *k, *v;
		Py_ssize_t pos = 0;
		if (args) {
			if (!object->SetPropertySequence(args)) {
				delete object;
				return NULL;
			}
		}
		if (kwargs != NULL) {
			while (PyDict_Next(kwargs, &pos, &k, &v)) {
				if (PyObject_SetAttr(object, k, v) == -1) {
					delete object;
					return NULL;
				}
			}
		}
		return object;
	}

	static PyObject* tp_Clear(PyObject* self) {
		static_cast<T*>(self)->Clear();
		Py_RETURN_NONE;
	}

	static PyObject* tp_CopyFrom(PyObject* self, PyObject* arg) {
		if (self == arg) {
			Py_RETURN_NONE;
		}
		if (!PyObject_TypeCheck(arg, Py_TYPE(self))) {
			PyErr_Format(PyExc_TypeError,
				"Parameter to CopyFrom() must be instance of same class: "
				"expected %.100s got %.100s.",
				Py_TYPE(self)->tp_name, Py_TYPE(arg)->tp_name);
			return NULL;
		}
		static_cast<T*>(self)->CopyFrom(*(static_cast<T*>(arg)));
		Py_RETURN_NONE;
	}

	static PyObject* tp_MergeFrom(PyObject* self, PyObject* arg) {
		if (self == arg) {
			Py_RETURN_NONE;
		}
		if (!PyObject_TypeCheck(arg, Py_TYPE(self))) {
			PyErr_Format(PyExc_TypeError,
				"Parameter to MergeFrom() must be instance of same class: "
				"expected %.100s got %.100s.",
				Py_TYPE(self)->tp_name, Py_TYPE(arg)->tp_name);
			return NULL;
		}
		static_cast<T*>(self)->MergeFrom(*(static_cast<T*>(arg)));
		Py_RETURN_NONE;
	}

	static PyObject* tp_Serialize(PyObject* self) {
		T* object = static_cast<T*>(self);
		int size = object->ByteSize();
		if (size <= 0) {
			return PyBytes_FromString("");
		}
		PyObject* result = PyBytes_FromStringAndSize(NULL, size);
		if (result == NULL) {
			return NULL;
		}
		char* buffer = PyBytes_AS_STRING(result);
		object->SerializeWithCachedSizesToArray(reinterpret_cast<uint8*>(buffer));
		return result;
	}

	static PyObject* tp_MergeFromString(PyObject* self, PyObject* arg) {
		const void* data;
		Py_ssize_t size;
		if (PyObject_AsReadBuffer(arg, &data, &size) < 0) {
			return NULL;
		}
		CodedInputStream input(reinterpret_cast<const uint8*>(data), size);
		bool success = static_cast<T*>(self)->MergePartialFromCodedStream(&input);
		if (success) {
			return PyInt_FromLong(input.CurrentPosition());
		} else {
			PyErr_Format(PyExc_RuntimeError, "Error parsing object");
			return NULL;
		}
	}

	static PyObject* tp_Copy(PyTypeObject* cls, PyObject* arg) {
		if (!PyObject_TypeCheck(arg, cls)) {
			PyErr_Format(PyExc_TypeError,
				"Parameter to __copy__() must be instance of same class: "
				"expected %.100s got %.100s.",
				cls->tp_name, Py_TYPE(arg)->tp_name);
			return NULL;
		}
		ScopedPyObjectPtr args(static_cast<T*>(arg)->GetPropertySequence());
		return tp_New(cls, args.get(), NULL);
	}

	static PyObject* tp_DeepCopy(PyObject* self, PyObject* args) {
		T* object = new T;
		ScopedPyObjectPtr data(tp_Serialize(self));
		ScopedPyObjectPtr result(tp_MergeFromString(object, data.get()));
		return object;
	}

	static PyObject* tp_ParseFromString(PyObject* self, PyObject* arg) {
		static_cast<T*>(self)->Clear();
		return tp_MergeFromString(self, arg);
	}

	static PyObject* tp_SerializeProperty(PyObject* self, PyObject* arg) {
		T* object = static_cast<T*>(self);
		if (arg == NULL || arg == Py_None) {
			FormatTypeError(arg, "SerializeProperty expect property name, but ");
			return NULL;
		} else if (PyUnicode_Check(arg)) {
			arg = PyUnicode_AsEncodedObject(arg, "utf-8", NULL);
			if (arg == NULL) { return NULL; }
		} else if (PyBytes_Check(arg)) {
			Py_INCREF(arg);
		} else {
			FormatTypeError(arg, "SerializeProperty expect property name, but ");
			return NULL;
		}

		int tag = object->PropertyTag(PyBytes_AS_STRING(arg));
		if (tag <= 0) {
			FormatTypeError(arg, "SerializeProperty expect property name, but ");
			Py_DECREF(arg);
			return NULL;
		}
		int size = object->PropertyByteSize(tag);
		if (size <= 0) {
			PyErr_Format(PyExc_RuntimeError, "Error serializing object");
			Py_DECREF(arg);
			return NULL;
		}
		PyObject* result = PyBytes_FromStringAndSize(NULL, size);
		if (result == NULL) {
			Py_DECREF(arg);
			return NULL;
		}
		char* buffer = PyBytes_AS_STRING(result);
		ArrayOutputStream out(reinterpret_cast<uint8*>(buffer), size);
		CodedOutputStream coded_out(&out);
		object->SerializeProperty(&coded_out, tag);
		Py_DECREF(arg);
		return result;
	}

	static PyObject* tp_DeserializeProperty(PyObject* self, PyObject* arg) {
		const void* data;
		Py_ssize_t size;
		if (PyObject_AsReadBuffer(arg, &data, &size) < 0) {
			return NULL;
		}
		T* object = static_cast<T*>(self);
		CodedInputStream input(reinterpret_cast<const uint8*>(data), size);
		int tag = object->DeserializeProperty(&input);
		if (tag <= 0) {
			PyErr_Format(PyExc_RuntimeError, "Error deserializing object");
			return NULL;
		} else {
			return PyBytes_FromString(object->PropertyName(tag));
		}
	}

	static PyObject* tp_Args(PyObject* self) {
		return static_cast<T*>(self)->GetPropertySequence();
	}

	static PyObject* tp_Json(PyObject* self, PyObject* args) {
		PyObject* slim = Py_False;
		if (!PyArg_ParseTuple(args, "|O", &slim)) {
			return NULL;
		}
		return static_cast<T*>(self)->Json(PyObject_IsTrue(slim) == 1);
	}

	static PyObject* tp_FromJson(PyTypeObject* cls, PyObject* data) {
		return T::FromJson(data);
	}

	static int half_cmp(PyObject* v, PyObject* w) {
		ScopedPyObjectPtr result(CallObject(v, "__cmp__", w));
		if (result == NULL || result.get() == Py_NotImplemented) {
			return -2;
		} else {
			long l = PyInt_AsLong(result.get());
			return l < 0 ? -1 : l > 0 ? 1 : 0;
		}
	}

	static int tp_Compare(PyObject* v, PyObject* w) {
		int c = PyNumber_CoerceEx(&v, &w);
		if (c < 0) {
			return -2;
		} else if (c == 0) {
			if (!PyObject_TypeCheck(v, &_Type) && !PyObject_TypeCheck(w, &_Type)) {
				c = PyObject_Compare(v, w);
				Py_DECREF(v);
				Py_DECREF(w);
				if (PyErr_Occurred()) { return -2; }
				return c < 0 ? -1 : c > 0 ? 1 : 0;
			}
		} else {
			Py_INCREF(v);
			Py_INCREF(w);
		}

		if (PyObject_TypeCheck(v, &_Type)) {
			c = half_cmp(v, w);
			if (c <= 1) {
				Py_DECREF(v);
				Py_DECREF(w);
				return c;
			}
		}
		if (PyObject_TypeCheck(w, &_Type)) {
			c = half_cmp(w, v);
			if (c <= 1) {
				Py_DECREF(v);
				Py_DECREF(w);
				if (c >= -1) { c = -c; }
				return c;
			}
		}
		Py_DECREF(v);
		Py_DECREF(w);
		return 2;
	}

	static PyObject* half_richcompare(PyObject* v, PyObject* w, int op) {
		static char* name_op[] = {
			"__lt__",
			"__le__",
			"__eq__",
			"__ne__",
			"__gt__",
			"__ge__",
		};
		PyObject* result = CallObject(v, name_op[op], w);
		if (result == NULL) {
			Py_INCREF(Py_NotImplemented);
			return Py_NotImplemented;
		}
		return result;
	}

	static PyObject* tp_Richcompare(PyObject* v, PyObject* w, int op) {
		static bool cmp_op[3][6] = {
			{ true,  true, false,  true, false, false},
			{false,  true,  true, false, false,  true},
			{false, false, false,  true,  true, false},
		};
		register PyObject* result;
		static int swapped_op[] = {Py_GT, Py_GE, Py_EQ, Py_NE, Py_LT, Py_LE};
		if (PyObject_TypeCheck(v, &_Type)) {
			result = half_richcompare(v, w, op);
		} else if (PyObject_TypeCheck(w, &_Type)) {
			result = half_richcompare(w, v, swapped_op[op]);
		} else {
			Py_INCREF(Py_NotImplemented);
			result = Py_NotImplemented;
		}
		if (result == Py_NotImplemented) {
			Py_DECREF(Py_NotImplemented);
			register int cmp = tp_Compare(v, w);
			if (cmp >= -1 && cmp <= 1) {
				if (cmp_op[cmp + 1][op]) {
					Py_RETURN_TRUE;
				} else {
					Py_RETURN_FALSE;
				}
			} else if (op == 2) {
				if (v == w) {
					Py_RETURN_TRUE;
				} else {
					Py_RETURN_FALSE;
				}
			} else if (op == 3) {
				if (v == w) {
					Py_RETURN_FALSE;
				} else {
					Py_RETURN_TRUE;
				}
			}
			Py_INCREF(Py_NotImplemented);
		}
		return result;
	}

	static PyObject* tp_Repr(PyObject* self) {
		PyObject* result = CallObject(self, "__repr__");
		if (result != NULL) { return result; }
		ScopedPyObjectPtr json(static_cast<T*>(self)->T::Json(false));
		if (json == NULL) { return NULL; }
		return PyObject_Repr(json.get());
	}

	static PyObject* tp_Str(PyObject* self) {
		PyObject* result = CallObject(self, "__str__");
		if (result == NULL) {
			PyErr_Clear();
			return tp_Repr(self);
		}
		return result;
	}

	static int tp_Traverse(PyObject* self, visitproc visit, void* arg) {
		static_cast<T*>(self)->T::Visit(visit, arg);
		return 0;
	}

	static int tp_GcClear(PyObject* self) {
		static_cast<T*>(self)->Clear();
		return 0;
	}

	static PyObject* _Init(PyObject* m, PyObject* args) {
		PyObject* attrs = Py_None;
		PyObject* type = reinterpret_cast<PyObject*>(&_Type);
		if (PyArg_ParseTuple(args, "|O", &attrs)) {
			if (PyDict_Check(attrs)) {
				PyObject *k, *v;
				Py_ssize_t pos = 0;
				while (PyDict_Next(attrs, &pos, &k, &v)) {
					if (PyFunction_Check(v) || PyUnbound_Check(v)) {
						v = PyMethod_New(v, NULL, type);
					}
					PyDict_SetItem(_Type.tp_dict, k, v);
				}
				PyObject* metaclass = PyDict_GetItemString(attrs, "__metaclass__");
				if (metaclass != NULL) {
					type->ob_type = reinterpret_cast<PyTypeObject*>(metaclass);
				}
			}
		}
		return type;
	}

	static bool Init(PyObject* m) {
		if (PyType_Ready(&_Type) < 0) {
			return false;
		}
		ScopedPyObjectPtr method(PyCFunction_NewEx(&_InitDef, m, NULL));
		if (method == NULL) { return false; }
		return PyDict_SetItemString(PyModule_GetDict(m), T::Name, method.get()) == 0;
	}
};

template <typename T>
PyMethodDef Object<T>::Methods[] = {
	{ "__copy__", (PyCFunction)tp_Copy, METH_O | METH_CLASS,
		"Shallow copy the object." },
	{ "__deepcopy__", (PyCFunction)tp_DeepCopy, METH_VARARGS,
		"Deep copy the object." },
	{ "Clear", (PyCFunction)tp_Clear, METH_NOARGS,
		"Clears the object." },
	{ "CopyFrom", (PyCFunction)tp_CopyFrom, METH_O,
		"Copies a protocol object into the current object." },
	{ "MergeFrom", (PyCFunction)tp_MergeFrom, METH_O,
		"Merges a protocol object into the current object." },
	{ "MergeFromString", (PyCFunction)tp_MergeFromString, METH_O,
		"Merges a serialized object into the current object." },
	{ "ParseFromString", (PyCFunction)tp_ParseFromString, METH_O,
		"Parses a serialized object into the current object." },
	{ "SerializeToString", (PyCFunction)tp_Serialize, METH_NOARGS,
		"Serializes the object to a string, only for initialized objects." },
	{ "SerializeProperty", (PyCFunction)tp_SerializeProperty, METH_O,
		"Serializes property to a string." },
	{ "DeserializeProperty", (PyCFunction)tp_DeserializeProperty, METH_O,
		"Deserialize property from a string and return name of the property." },
	{ "Args", (PyCFunction)tp_Args, METH_NOARGS,
		"Get a property tuple can be used as args." },
	{ "Json", (PyCFunction)tp_Json, METH_VARARGS,
		"Get Json of the object." },
	{ "FromJson", (PyCFunction)tp_FromJson, METH_O | METH_CLASS,
		"Get object from Json." },
	{ NULL, NULL}
};

namespace object {

Py_ssize_t tp_Len(PyObject* self);
PyObject* tp_Item(PyObject* self, PyObject* key);
PyObject* tp_Call(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* tp_nb_index(PyObject* self);
PyObject* tp_Richcompare(PyObject* v, PyObject* w, int op);
PyObject* tp_Getiter(PyObject* self);
PyObject* tp_nb_long(PyObject* self);
PyObject* tp_nb_pow(PyObject* v, PyObject* w, PyObject* z);
PyObject* tp_nb_ipow(PyObject* v, PyObject* w, PyObject* z);
int tp_AssignItem(PyObject* self, PyObject* key, PyObject* value);
int tp_Contains(PyObject* self, PyObject* member);
int tp_Compare(PyObject* v, PyObject* w);
int tp_nb_nonzero(PyObject* self);
int tp_nb_coerce(PyObject** pv, PyObject** pw);
long tp_Hash(PyObject* self);

PyObject* tp_nb_neg(PyObject* self);
PyObject* tp_nb_pos(PyObject* self);
PyObject* tp_nb_abs(PyObject* self);
PyObject* tp_nb_invert(PyObject* self);
PyObject* tp_nb_float(PyObject* self);
PyObject* tp_nb_int(PyObject* self);
PyObject* tp_nb_oct(PyObject* self);
PyObject* tp_nb_hex(PyObject* self);

PyObject* tp_nb_or(PyObject* v, PyObject* w);
PyObject* tp_nb_and(PyObject* v, PyObject* w);
PyObject* tp_nb_xor(PyObject* v, PyObject* w);
PyObject* tp_nb_lshift(PyObject* v, PyObject* w);
PyObject* tp_nb_rshift(PyObject* v, PyObject* w);
PyObject* tp_nb_add(PyObject* v, PyObject* w);
PyObject* tp_nb_sub(PyObject* v, PyObject* w);
PyObject* tp_nb_mul(PyObject* v, PyObject* w);
PyObject* tp_nb_div(PyObject* v, PyObject* w);
PyObject* tp_nb_mod(PyObject* v, PyObject* w);
PyObject* tp_nb_divmod(PyObject* v, PyObject* w);
PyObject* tp_nb_floordiv(PyObject* v, PyObject* w);
PyObject* tp_nb_truediv(PyObject* v, PyObject* w);

PyObject* tp_nb_ior(PyObject* v, PyObject* w);
PyObject* tp_nb_ixor(PyObject* v, PyObject* w);
PyObject* tp_nb_iand(PyObject* v, PyObject* w);
PyObject* tp_nb_ilshift(PyObject* v, PyObject* w);
PyObject* tp_nb_irshift(PyObject* v, PyObject* w);
PyObject* tp_nb_iadd(PyObject* v, PyObject* w);
PyObject* tp_nb_isub(PyObject* v, PyObject* w);
PyObject* tp_nb_imul(PyObject* v, PyObject* w);
PyObject* tp_nb_idiv(PyObject* v, PyObject* w);
PyObject* tp_nb_imod(PyObject* v, PyObject* w);
PyObject* tp_nb_ifloordiv(PyObject* v, PyObject* w);
PyObject* tp_nb_itruediv(PyObject* v, PyObject* w);

} // namespace object

template <typename T>
PyMethodDef Object<T>::_InitDef = { "InitObject", (PyCFunction)_Init, METH_VARARGS,
	"Initialize Object Type." };

template <typename T>
PyNumberMethods Object<T>::NbMethods = {
	::typy::object::tp_nb_add,               /* nb_add                  */
	::typy::object::tp_nb_sub,               /* nb_subtract             */
	::typy::object::tp_nb_mul,               /* nb_multiply             */
	::typy::object::tp_nb_div,               /* nb_divide               */
	::typy::object::tp_nb_mod,               /* nb_remainder            */
	::typy::object::tp_nb_divmod,            /* nb_divmod               */
	::typy::object::tp_nb_pow,               /* nb_power                */
	(unaryfunc)::typy::object::tp_nb_neg,    /* nb_negative             */
	(unaryfunc)::typy::object::tp_nb_pos,    /* nb_positive             */
	(unaryfunc)::typy::object::tp_nb_abs,    /* nb_absolute             */
	(inquiry)::typy::object::tp_nb_nonzero,  /* nb_nonzero              */
	(unaryfunc)::typy::object::tp_nb_invert, /* nb_invert               */
	::typy::object::tp_nb_lshift,            /* nb_lshift               */
	::typy::object::tp_nb_rshift,            /* nb_rshift               */
	::typy::object::tp_nb_and,               /* nb_and                  */
	::typy::object::tp_nb_xor,               /* nb_xor                  */
	::typy::object::tp_nb_or,                /* nb_or                   */
	::typy::object::tp_nb_coerce,            /* nb_coerce               */
	(unaryfunc)::typy::object::tp_nb_int,    /* nb_int                  */
	(unaryfunc)::typy::object::tp_nb_long,   /* nb_long                 */
	(unaryfunc)::typy::object::tp_nb_float,  /* nb_float                */
	(unaryfunc)::typy::object::tp_nb_oct,    /* nb_oct                  */
	(unaryfunc)::typy::object::tp_nb_hex,    /* nb_hex                  */
	::typy::object::tp_nb_iadd,              /* nb_inplace_add          */
	::typy::object::tp_nb_isub,              /* nb_inplace_subtract     */
	::typy::object::tp_nb_imul,              /* nb_inplace_multiply     */
	::typy::object::tp_nb_idiv,              /* nb_inplace_divide       */
	::typy::object::tp_nb_imod,              /* nb_inplace_remainder    */
	::typy::object::tp_nb_ipow,              /* nb_inplace_power        */
	::typy::object::tp_nb_ilshift,           /* nb_inplace_lshift       */
	::typy::object::tp_nb_irshift,           /* nb_inplace_rshift       */
	::typy::object::tp_nb_iand,              /* nb_inplace_and          */
	::typy::object::tp_nb_ixor,              /* nb_inplace_xor          */
	::typy::object::tp_nb_ior,               /* nb_inplace_or           */
	::typy::object::tp_nb_floordiv,          /* nb_floor_divide         */
	::typy::object::tp_nb_truediv,           /* nb_true_divide          */
	::typy::object::tp_nb_ifloordiv,         /* nb_inplace_floor_divide */
	::typy::object::tp_nb_itruediv,          /* nb_inplace_true_divide  */
	(unaryfunc)::typy::object::tp_nb_index,  /* nb_index                */
};

template <typename T>
PyMappingMethods Object<T>::MpMethods = {
	0,                                            /* mp_length        */
	(binaryfunc)::typy::object::tp_Item,          /* mp_subscript     */
	(objobjargproc)::typy::object::tp_AssignItem, /* mp_ass_subscript */
};

template <typename T>
PySequenceMethods Object<T>::SqMethods = {
	(lenfunc)::typy::object::tp_Len,         /* sq_length    */
	0,                                       /* sq_concat    */
	0,                                       /* sq_repeat    */
	0,                                       /* sq_item      */
	0,                                       /* sq_slice     */
	0,                                       /* sq_ass_item  */
	0,                                       /* sq_ass_slice */
	(objobjproc)::typy::object::tp_Contains, /* sq_contains  */
};

template <typename T>
PyTypeObject Object<T>::_Type = {
	PyVarObject_HEAD_INIT(&PyType_Type, 0)
	T::FullName,                              /* tp_name           */
	sizeof(T),                                /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)tp_Dealloc,                   /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	tp_Compare,                               /* tp_compare        */
	(reprfunc)tp_Repr,                        /* tp_repr           */
	&NbMethods,                               /* tp_as_number      */
	&SqMethods,                               /* tp_as_sequence    */
	&MpMethods,                               /* tp_as_mapping     */
	(hashfunc)::typy::object::tp_Hash,        /* tp_hash           */
	::typy::object::tp_Call,                  /* tp_call           */
	(reprfunc)tp_Str,                         /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags          */
	"A Typy Object",                          /* tp_doc            */
	(traverseproc)tp_Traverse,                /* tp_traverse       */
	(inquiry)tp_GcClear,                      /* tp_clear          */
	tp_Richcompare,                           /* tp_richcompare    */
	0,                                        /* tp_weaklistoffset */
	::typy::object::tp_Getiter,               /* tp_iter           */
	0,                                        /* tp_iternext       */
	Methods,                                  /* tp_methods        */
	0,                                        /* tp_members        */
	GetSet,                                   /* tp_getset         */
	0,                                        /* tp_base           */
	0,                                        /* tp_dict           */
	0,                                        /* tp_descr_get      */
	0,                                        /* tp_descr_set      */
	0,                                        /* tp_dictoffset     */
	0,                                        /* tp_init           */
	0,                                        /* tp_alloc          */
	tp_New,                                   /* tp_new            */
};

} // namespace typy
#endif // TYPY_OBJECT_H__
