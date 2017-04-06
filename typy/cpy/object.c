// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

static PyObject* CallObject(PyObject* self, const char *name) {
	PyObject* method = PyDict_GetItemString(Py_TYPE(self)->tp_dict, name);
	if (method && Py_TYPE(method) == &PyMethod_Type) {
		register descrgetfunc f = Py_TYPE(method)->tp_descr_get;
		if (f) {
			register PyObject* m = f(method, self, (PyObject*)Py_TYPE(self));
			register PyObject* result = PyEval_CallObject(m, NULL);
			Py_XDECREF(m);
			return result;
		}
	}
	return NULL;
}

static PyObject* CallObject1(PyObject* self, const char *name, PyObject* arg1) {
	PyObject* method = PyDict_GetItemString(Py_TYPE(self)->tp_dict, name);
	if (method && Py_TYPE(method) == &PyMethod_Type) {
		register descrgetfunc f = Py_TYPE(method)->tp_descr_get;
		if (f) {
			register PyObject* m = f(method, self, (PyObject*)Py_TYPE(self));
			register PyObject* a = PyTuple_Pack(1, arg1);
			register PyObject* result = PyEval_CallObject(m, a);
			Py_XDECREF(m);
			Py_XDECREF(a);
			return result;
		}
	}
	return NULL;
}

static PyObject* CallObject2(PyObject* self, const char *name, PyObject* arg1, PyObject* arg2) {
	PyObject* method = PyDict_GetItemString(Py_TYPE(self)->tp_dict, name);
	if (method && Py_TYPE(method) == &PyMethod_Type) {
		descrgetfunc f = Py_TYPE(method)->tp_descr_get;
		if (f) {
			register PyObject* m = f(method, self, (PyObject*)Py_TYPE(self));
			register PyObject* a = PyTuple_Pack(2, arg1, arg2);
			register PyObject* result = PyEval_CallObject(m, a);
			Py_XDECREF(m);
			Py_XDECREF(a);
			return result;
		}
	}
	return NULL;
}

static int half_cmp(PyObject* v, PyObject* w) {
	PyObject* result = CallObject1(v, "__cmp__", w);
	if (!result || result == Py_NotImplemented) {
		Py_XDECREF(result);
		return -2;
	} else {
		long l = PyInt_AsLong(result);
		Py_DECREF(result);
		return l < 0 ? -1 : l > 0 ? 1 : 0;
	}
}

static int object_Compare(PyObject* v, PyObject* w) {
	int c = PyNumber_CoerceEx(&v, &w);
	if (c < 0) {
		return -2;
	} else if (c == 0) {
		if (!PyObject_TypeCheck(v, TypyObjectType) && !PyObject_TypeCheck(w, TypyObjectType)) {
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

	if (PyObject_TypeCheck(v, TypyObjectType)) {
		c = half_cmp(v, w);
		if (c <= 1) {
			Py_DECREF(v);
			Py_DECREF(w);
			return c;
		}
	}
	if (PyObject_TypeCheck(w, TypyObjectType)) {
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
	PyObject* result = CallObject1(v, name_op[op], w);
	if (!result) {
		if (op == 2) {
			return v == w ? Py_True : Py_False;
		} else if (op == 3) {
			return v == w ? Py_True : Py_False;
		} else {
			Py_INCREF(Py_NotImplemented);
			return Py_NotImplemented;
		}
	}
	return result;
}

static PyObject* object_Richcompare(PyObject* v, PyObject* w, int op) {
	static int swapped_op[] = {Py_GT, Py_GE, Py_EQ, Py_NE, Py_LT, Py_LE};
	if (PyObject_TypeCheck(v, TypyObjectType)) {
		return half_richcompare(v, w, op);
	} else if (PyObject_TypeCheck(w, TypyObjectType)) {
		return half_richcompare(w, v, swapped_op[op]);
	}
	Py_INCREF(Py_NotImplemented);
	return Py_NotImplemented;
}

static Py_ssize_t object_Len(PyObject* self) {
	PyObject* result = CallObject(self, "__len__");
	if (result) {
		register Py_ssize_t len = PyInt_AsSsize_t(result);
		Py_DECREF(result);
		return len;
	}
	return -1;
}

static PyObject* object_Item(PyObject* self, PyObject* key) {
	return CallObject1(self, "__getitem__", key);
}

static int object_AssignItem(PyObject* self, PyObject* key, PyObject* value) {
	PyObject* result;
	if (!value) {
		result = CallObject(self, "__delitem__");
	} else {
		result = CallObject2(self, "__setitem__", key, value);
	}
	register success = result ? 0 : -1;
	Py_XDECREF(result);
	return success;
}

static int object_Contains(PyObject* self, PyObject* member) {
	PyObject* result = CallObject1(self, "__contains__", member);
	if (!result) {
		register PyObject* items = CallObject(self, "__iter__");
		if (!items) { return -1; }
		register PyObject* iter = PyObject_GetIter(items);
		if (!iter) { Py_DECREF(items); return -1; }
		PyErr_Clear();
		while (true) {
			register PyObject* item = PyIter_Next(iter);
			if (!item) { break; }
			int r = PyObject_Compare(member, item);
			Py_DECREF(item);
			if (PyErr_Occurred()) {
				Py_DECREF(items);
				Py_DECREF(iter);
				return -1;
			} else if (r == 0) {
				Py_DECREF(items);
				Py_DECREF(iter);
				return true;
			}
		}
		Py_DECREF(items);
		Py_DECREF(iter);
		return false;
	}
	register contains = PyObject_IsTrue(result);
	Py_DECREF(result);
	return contains;
}

static PyObject* object_Call(PyObject* self, PyObject* args, PyObject* kwargs) {
	PyObject* result;
	PyObject* call = PyObject_GetAttrString(self, "__call__");
	if (!call || Py_EnterRecursiveCall(" in __call__")) {
		Py_XDECREF(call);
		return NULL;
	}
	result = PyObject_Call(call, args, kwargs);
	Py_LeaveRecursiveCall();
	return result;
}

static PyObject* object_Repr(PyObject* self) {
	PyObject* result = CallObject(self, "__repr__");
	if (result) { return result; }
	return PyString_FromFormat("<" FULL_MODULE_NAME ".%s instance at %p>", Typy_NAME(self), self);
}

static PyObject* object_Str(PyObject* self) {
	PyObject* result = CallObject(self, "__str__");
	if (!result) {
		PyErr_Clear();
		return object_Repr(self);
	}
	return result;
}

static long object_Hash(PyObject* self) {
	PyObject* result = CallObject(self, "__hash__");
	if (!result) {
		PyErr_Clear();
		long x;
		size_t y = (size_t)self;
		/* bottom 3 or 4 bits are likely to be 0; rotate y by 4 to avoid
		   excessive hash collisions for dicts and sets */
		y = (y >> 4) | (y << (8 * SIZEOF_VOID_P - 4));
		x = (long)y;
		if (x == -1) { x = -2; }
		return x;
	} else if (PyInt_Check(result) || PyLong_Check(result)) {
		/* This already converts a -1 result to -2. */
		register long hash = result->ob_type->tp_hash(result);
		Py_DECREF(result);
		return hash;
	} else {
		Py_DECREF(result);
		return -1;
	}
}

static PyObject* object_Getiter(PyObject* self) {
	return CallObject(self, "__iter__");
}

static int object_nb_nonzero(PyObject* self) {
	return self != NULL;
}

static PyObject* object_nb_index(PyObject* self) {
	return CallObject(self, "__index__");
}

/* Try one half of a binary operator involving a class instance. */
static PyObject* half_binop(PyObject* v, PyObject* w, char* opname, binaryfunc thisfunc, int swapped) {
	PyObject* coerced = CallObject1(v, "__coerce__", w);
	if (!coerced || coerced == Py_None || coerced == Py_NotImplemented) {
		PyErr_Clear();
		Py_XDECREF(coerced);
		return CallObject1(v, opname, w);
	}
	PyObject* v1 = PyTuple_GetItem(coerced, 0);
	w = PyTuple_GetItem(coerced, 1);
	if (v1->ob_type == v->ob_type) {
		register PyObject* result = CallObject1(v1, opname, w);
		Py_DECREF(coerced);
		return result;
	} else {
		if (Py_EnterRecursiveCall(" after coercion")) { Py_DECREF(coerced); return NULL; }
		register PyObject* result;
		if (swapped) {
			result = (thisfunc)(w, v1);
		} else {
			result = (thisfunc)(v1, w);
		}
		Py_LeaveRecursiveCall();
		Py_DECREF(coerced);
		return result;
	}
}

static int object_nb_coerce(PyObject** pv, PyObject** pw) {
	PyObject* result = CallObject1(*pv, "__coerce__", *pw);
	if (!result) { return -1; }
	if (result == Py_None || result == Py_NotImplemented) { Py_DECREF(result); return 1; }
	*pv = PyTuple_GetItem(result, 0);
	*pw = PyTuple_GetItem(result, 1);
	Py_INCREF(*pv);
	Py_INCREF(*pw);
	Py_DECREF(result);
	return 0;
}

#define UNARY(funcname, methodname) \
static PyObject* funcname(PyObject* self) { \
	return CallObject(self, methodname);    \
}

#define BINARY(f, m, n) \
static PyObject* f(PyObject* v, PyObject* w) {               \
	PyObject* result = half_binop(v, w, "__" m "__", n, 0);  \
	if (!result || result == Py_NotImplemented) {            \
		PyErr_Clear();                                       \
		Py_XDECREF(result);                                  \
		result = half_binop(w, v, "__r" m "__", n, 1);       \
	}                                                        \
    return result;                                           \
}

#define BINARY_INPLACE(f, m, n) \
static PyObject* f(PyObject* v, PyObject* w) {               \
	PyObject* result = half_binop(v, w, "__i" m "__", n, 0); \
	if (!result || result == Py_NotImplemented) {            \
		PyErr_Clear();                                       \
		Py_XDECREF(result);                                  \
		result = half_binop(v, w, "__" m "__", n, 0);        \
		if (!result || result == Py_NotImplemented) {        \
			PyErr_Clear();                                   \
			Py_XDECREF(result);                              \
			result = half_binop(w, v, "__r" m "__", n, 1);   \
		}                                                    \
	}                                                        \
	return result;                                           \
}

UNARY(object_nb_neg, "__neg__")
UNARY(object_nb_pos, "__pos__")
UNARY(object_nb_abs, "__abs__")
UNARY(object_nb_invert, "__invert__")
UNARY(object_nb_float, "__float__")
UNARY(object_nb_int, "__int__")
UNARY(object_nb_oct, "__oct__")
UNARY(object_nb_hex, "__hex__")

BINARY(object_nb_or, "or", PyNumber_Or)
BINARY(object_nb_and, "and", PyNumber_And)
BINARY(object_nb_xor, "xor", PyNumber_Xor)
BINARY(object_nb_lshift, "lshift", PyNumber_Lshift)
BINARY(object_nb_rshift, "rshift", PyNumber_Rshift)
BINARY(object_nb_add, "add", PyNumber_Add)
BINARY(object_nb_sub, "sub", PyNumber_Subtract)
BINARY(object_nb_mul, "mul", PyNumber_Multiply)
BINARY(object_nb_div, "div", PyNumber_Divide)
BINARY(object_nb_mod, "mod", PyNumber_Remainder)
BINARY(object_nb_divmod, "divmod", PyNumber_Divmod)
BINARY(object_nb_floordiv, "floordiv", PyNumber_FloorDivide)
BINARY(object_nb_truediv, "truediv", PyNumber_TrueDivide)

BINARY_INPLACE(object_nb_ior, "or", PyNumber_InPlaceOr)
BINARY_INPLACE(object_nb_ixor, "xor", PyNumber_InPlaceXor)
BINARY_INPLACE(object_nb_iand, "and", PyNumber_InPlaceAnd)
BINARY_INPLACE(object_nb_ilshift, "lshift", PyNumber_InPlaceLshift)
BINARY_INPLACE(object_nb_irshift, "rshift", PyNumber_InPlaceRshift)
BINARY_INPLACE(object_nb_iadd, "add", PyNumber_InPlaceAdd)
BINARY_INPLACE(object_nb_isub, "sub", PyNumber_InPlaceSubtract)
BINARY_INPLACE(object_nb_imul, "mul", PyNumber_InPlaceMultiply)
BINARY_INPLACE(object_nb_idiv, "div", PyNumber_InPlaceDivide)
BINARY_INPLACE(object_nb_imod, "mod", PyNumber_InPlaceRemainder)
BINARY_INPLACE(object_nb_ifloordiv, "floordiv", PyNumber_InPlaceFloorDivide)
BINARY_INPLACE(object_nb_itruediv, "truediv", PyNumber_InPlaceTrueDivide)

#undef UNARY
#undef BINARY
#undef BINARY_INPLACE

static PyObject* object_nb_long(PyObject* self) {
	PyObject* result = CallObject(self, "__long__");
	if (!result) { PyErr_Clear(); return object_nb_int(self); }
	return result;
}

static PyObject* bin_power(PyObject* v, PyObject* w) {
	return PyNumber_Power(v, w, Py_None);
}

static PyObject* bin_inplace_power(PyObject* v, PyObject* w) {
	return PyNumber_InPlacePower(v, w, Py_None);
}

static PyObject* object_nb_pow(PyObject* v, PyObject* w, PyObject* z) {
	if (z == Py_None) {
		PyObject* result = half_binop(v, w, "__pow__", bin_power, 0);
		if (!result || result == Py_NotImplemented) {
			PyErr_Clear();
			Py_XDECREF(result);
			result = half_binop(w, v, "__rpow__", bin_power, 1);
		}
		return result;
	} else {
		return CallObject2(v, "__pow__", w, z);
	}
}

static PyObject* object_nb_ipow(PyObject* v, PyObject* w, PyObject* z) {
	if (z == Py_None) {
		PyObject* result = half_binop(v, w, "__ipow__", bin_inplace_power, 0);
		if (!result || result == Py_NotImplemented) {
			PyErr_Clear();
			Py_XDECREF(result);
			result = half_binop(v, w, "__pow__", bin_inplace_power, 0);
			if (!result || result == Py_NotImplemented) {
				PyErr_Clear();
				Py_XDECREF(result);
				result = half_binop(w, v, "__rpow__", bin_inplace_power, 1);
			}
		}
		return result;
	} else {
		return CallObject2(v, "__ipow__", w, z);
	}
}

static PyMethodDef Methods[] = {
	{ "Clear", (PyCFunction)Py_Clear, METH_NOARGS,
		"Clears the object." },
	{ "CopyFrom", (PyCFunction)Py_CopyFrom, METH_O,
		"Copies a protocol object into the current object." },
	{ "MergeFrom", (PyCFunction)Py_MergeFrom, METH_O,
		"Merges a protocol object into the current object." },
	{ "MergeFromString", (PyCFunction)Py_MergeFromString, METH_O,
		"Merges a serialized object into the current object." },
	{ "ParseFromString", (PyCFunction)Py_ParseFromPyString, METH_O,
		"Parses a serialized object into the current object." },
	{ "SerializeToString", (PyCFunction)Py_SerializeString, METH_NOARGS,
		"Serializes the object to a string, only for initialized objects." },
	{ "SerializeProperty", (PyCFunction)Py_SerializeProperty, METH_O,
		"Serializes property to a string." },
	{ "DeserializeProperty", (PyCFunction)Py_DeserializeProperty, METH_O,
		"Deserialize property from a string and return name of the property." },
	{ NULL, NULL}
};

static PyNumberMethods NbMethods = {
	object_nb_add,               /* nb_add                  */
	object_nb_sub,               /* nb_subtract             */
	object_nb_mul,               /* nb_multiply             */
	object_nb_div,               /* nb_divide               */
	object_nb_mod,               /* nb_remainder            */
	object_nb_divmod,            /* nb_divmod               */
	object_nb_pow,               /* nb_power                */
	(unaryfunc)object_nb_neg,    /* nb_negative             */
	(unaryfunc)object_nb_pos,    /* nb_positive             */
	(unaryfunc)object_nb_abs,    /* nb_absolute             */
	(inquiry)object_nb_nonzero,  /* nb_nonzero              */
	(unaryfunc)object_nb_invert, /* nb_invert               */
	object_nb_lshift,            /* nb_lshift               */
	object_nb_rshift,            /* nb_rshift               */
	object_nb_and,               /* nb_and                  */
	object_nb_xor,               /* nb_xor                  */
	object_nb_or,                /* nb_or                   */
	object_nb_coerce,            /* nb_coerce               */
	(unaryfunc)object_nb_int,    /* nb_int                  */
	(unaryfunc)object_nb_long,   /* nb_long                 */
	(unaryfunc)object_nb_float,  /* nb_float                */
	(unaryfunc)object_nb_oct,    /* nb_oct                  */
	(unaryfunc)object_nb_hex,    /* nb_hex                  */
	object_nb_iadd,              /* nb_inplace_add          */
	object_nb_isub,              /* nb_inplace_subtract     */
	object_nb_imul,              /* nb_inplace_multiply     */
	object_nb_idiv,              /* nb_inplace_divide       */
	object_nb_imod,              /* nb_inplace_remainder    */
	object_nb_ipow,              /* nb_inplace_power        */
	object_nb_ilshift,           /* nb_inplace_lshift       */
	object_nb_irshift,           /* nb_inplace_rshift       */
	object_nb_iand,              /* nb_inplace_and          */
	object_nb_ixor,              /* nb_inplace_xor          */
	object_nb_ior,               /* nb_inplace_or           */
	object_nb_floordiv,          /* nb_floor_divide         */
	object_nb_truediv,           /* nb_true_divide          */
	object_nb_ifloordiv,         /* nb_inplace_floor_divide */
	object_nb_itruediv,          /* nb_inplace_true_divide  */
	(unaryfunc)object_nb_index,  /* nb_index                */
};

static PyMappingMethods MpMethods = {
	0,                                /* mp_length        */
	(binaryfunc)object_Item,          /* mp_subscript     */
	(objobjargproc)object_AssignItem, /* mp_ass_subscript */
};

static PySequenceMethods SqMethods = {
	(lenfunc)object_Len,         /* sq_length    */
	0,                           /* sq_concat    */
	0,                           /* sq_repeat    */
	0,                           /* sq_item      */
	0,                           /* sq_slice     */
	0,                           /* sq_ass_item  */
	0,                           /* sq_ass_slice */
	(objobjproc)object_Contains, /* sq_contains  */
};

PyTypeObject BaseTypyObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".BaseObject",           /* tp_name           */
	sizeof(TypyObject),                       /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)Typy_Dealloc,                 /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	object_Compare,                           /* tp_compare        */
	(reprfunc)object_Repr,                    /* tp_repr           */
	&NbMethods,                               /* tp_as_number      */
	&SqMethods,                               /* tp_as_sequence    */
	&MpMethods,                               /* tp_as_mapping     */
	(hashfunc)object_Hash,                    /* tp_hash           */
	object_Call,                              /* tp_call           */
	(reprfunc)object_Str,                     /* tp_str            */
	(getattrofunc)Typy_GetAttr,               /* tp_getattro       */
	(setattrofunc)Typy_SetAttr,               /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags          */
	"A Typy Object",                          /* tp_doc            */
	0,                                        /* tp_traverse       */
	0,                                        /* tp_clear          */
	object_Richcompare,                       /* tp_richcompare    */
	0,                                        /* tp_weaklistoffset */
	object_Getiter,                           /* tp_iter           */
	0,                                        /* tp_iternext       */
	Methods,                                  /* tp_methods        */
};

#ifdef __cplusplus
}
#endif
