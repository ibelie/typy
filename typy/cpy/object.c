// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

PyObject* CallObject(PyObject* self, const char *name) {
	PyObject* method = PyDict_GetItemString(Py_TYPE(self)->tp_dict, name);
	if (method != NULL && Py_TYPE(method) == &PyMethod_Type) {
		descrgetfunc f = Py_TYPE(method)->tp_descr_get;
		if (f != NULL) {
			ScopedPyObjectPtr m(f(method, self, reinterpret_cast<PyObject*>(Py_TYPE(self))));
			return PyEval_CallObject(m.get(), NULL);
		}
	}
	return NULL;
}

Py_ssize_t tp_Len(PyObject* self) {
	ScopedPyObjectPtr result(CallObject(self, "__len__"));
	if (result != NULL) {
		return PyInt_AsSsize_t(result.get());
	}
	return -1;
}

PyObject* tp_Item(PyObject* self, PyObject* key) {
	return CallObject(self, "__getitem__", key);
}

int tp_AssignItem(PyObject* self, PyObject* key, PyObject* value) {
	ScopedPyObjectPtr result;
	if (value == NULL) {
		result.reset(CallObject(self, "__delitem__"));
	} else {
		result.reset(CallObject(self, "__setitem__", key, value));
	}
	return result == NULL? -1: 0;
}

int tp_Contains(PyObject* self, PyObject* member) {
	ScopedPyObjectPtr result(CallObject(self, "__contains__", member));
	if (result == NULL) {
		ScopedPyObjectPtr items(CallObject(self, "__iter__"));
		if (items == NULL) { return -1; }
		ScopedPyObjectPtr iter(PyObject_GetIter(items.get()));
		if (iter == NULL) { return -1; }
		PyErr_Clear();
		while (true) {
			ScopedPyObjectPtr item(PyIter_Next(iter.get()));
			if (item == NULL) { break; }
			int r = PyObject_Compare(member, item.get());
			if (PyErr_Occurred()) {
				return -1;
			} else if (r == 0) {
				return true;
			}
		}
		return false;
	}
	return PyObject_IsTrue(result.get());
}

PyObject* tp_Call(PyObject* self, PyObject* args, PyObject* kwargs) {
	PyObject* result;
	ScopedPyObjectPtr call(PyObject_GetAttrString(self, "__call__"));
	if (call == NULL || Py_EnterRecursiveCall(" in __call__")) {
		return NULL;
	}
	result = PyObject_Call(call.get(), args, kwargs);
	Py_LeaveRecursiveCall();
	return result;
}

PyObject* tp_Repr(PyObject* self) {
	PyObject* result = CallObject(self, "__repr__");
	if (result != NULL) { return result; }
	return PyString_FromFormat("<typy.%s instance at %p>", Py_TYPE(self)->tp_name, self);
}

PyObject* tp_Str(PyObject* self) {
	PyObject* result = CallObject(self, "__str__");
	if (result == NULL) {
		PyErr_Clear();
		return tp_Repr(self);
	}
	return result;
}

long tp_Hash(PyObject* self) {
	ScopedPyObjectPtr result(CallObject(self, "__hash__"));
	if (result == NULL) {
		PyErr_Clear();
		long x;
		size_t y = (size_t)self;
		/* bottom 3 or 4 bits are likely to be 0; rotate y by 4 to avoid
		   excessive hash collisions for dicts and sets */
		y = (y >> 4) | (y << (8 * SIZEOF_VOID_P - 4));
		x = (long)y;
		if (x == -1) { x = -2; }
		return x;
	} else if (PyInt_Check(result.get()) || PyLong_Check(result.get())) {
		/* This already converts a -1 result to -2. */
		return result.get()->ob_type->tp_hash(result.get());
	} else {
		return -1;
	}
}

PyObject* tp_Getiter(PyObject* self) {
	return CallObject(self, "__iter__");
}

int tp_nb_nonzero(PyObject* self) {
	return self != NULL;
}

PyObject* tp_nb_index(PyObject* self) {
	return CallObject(self, "__index__");
}

/* Try one half of a binary operator involving a class instance. */
PyObject* half_binop(PyObject* v, PyObject* w, char* opname, binaryfunc thisfunc, int swapped) {
	ScopedPyObjectPtr coerced(CallObject(v, "__coerce__", w));
	if (coerced == NULL || coerced.get() == Py_None || coerced.get() == Py_NotImplemented) {
		PyErr_Clear();
		return CallObject(v, opname, w);
	}
	PyObject* v1 = PyTuple_GetItem(coerced.get(), 0);
	w = PyTuple_GetItem(coerced.get(), 1);
	if (v1->ob_type == v->ob_type) {
		return CallObject(v1, opname, w);
	} else {
		if (Py_EnterRecursiveCall(" after coercion")) { return NULL; }
		PyObject* result;
		if (swapped) {
			result = (thisfunc)(w, v1);
		} else {
			result = (thisfunc)(v1, w);
		}
		Py_LeaveRecursiveCall();
		return result;
	}
}

int tp_nb_coerce(PyObject** pv, PyObject** pw) {
	ScopedPyObjectPtr result(CallObject(*pv, "__coerce__", *pw));
	if (result == NULL) { return -1; }
	if (result.get() == Py_None || result.get() == Py_NotImplemented) { return 1; }
	*pv = PyTuple_GetItem(result.get(), 0);
	*pw = PyTuple_GetItem(result.get(), 1);
	Py_INCREF(*pv);
	Py_INCREF(*pw);
	return 0;
}

#define UNARY(funcname, methodname) \
PyObject* funcname(PyObject* self) {             \
	return CallObject(self, methodname); \
}

#define BINARY(f, m, n) \
PyObject* f(PyObject* v, PyObject* w) {               \
	PyObject* result = half_binop(v, w, "__" m "__", n, 0);  \
	if (result == NULL || result == Py_NotImplemented) {     \
		PyErr_Clear();                                       \
		Py_XDECREF(result);                                  \
		result = half_binop(w, v, "__r" m "__", n, 1);       \
	}                                                        \
    return result;                                           \
}

#define BINARY_INPLACE(f, m, n) \
PyObject* f(PyObject* v, PyObject* w) {               \
	PyObject* result = half_binop(v, w, "__i" m "__", n, 0); \
	if (result == NULL || result == Py_NotImplemented) {     \
		PyErr_Clear();                                       \
		Py_XDECREF(result);                                  \
		result = half_binop(v, w, "__" m "__", n, 0);        \
		if (result == NULL || result == Py_NotImplemented) { \
			PyErr_Clear();                                   \
			Py_XDECREF(result);                              \
			result = half_binop(w, v, "__r" m "__", n, 1);   \
		}                                                    \
	}                                                        \
	return result;                                           \
}

UNARY(tp_nb_neg, "__neg__")
UNARY(tp_nb_pos, "__pos__")
UNARY(tp_nb_abs, "__abs__")
UNARY(tp_nb_invert, "__invert__")
UNARY(tp_nb_float, "__float__")
UNARY(tp_nb_int, "__int__")
UNARY(tp_nb_oct, "__oct__")
UNARY(tp_nb_hex, "__hex__")

BINARY(tp_nb_or, "or", PyNumber_Or)
BINARY(tp_nb_and, "and", PyNumber_And)
BINARY(tp_nb_xor, "xor", PyNumber_Xor)
BINARY(tp_nb_lshift, "lshift", PyNumber_Lshift)
BINARY(tp_nb_rshift, "rshift", PyNumber_Rshift)
BINARY(tp_nb_add, "add", PyNumber_Add)
BINARY(tp_nb_sub, "sub", PyNumber_Subtract)
BINARY(tp_nb_mul, "mul", PyNumber_Multiply)
BINARY(tp_nb_div, "div", PyNumber_Divide)
BINARY(tp_nb_mod, "mod", PyNumber_Remainder)
BINARY(tp_nb_divmod, "divmod", PyNumber_Divmod)
BINARY(tp_nb_floordiv, "floordiv", PyNumber_FloorDivide)
BINARY(tp_nb_truediv, "truediv", PyNumber_TrueDivide)

BINARY_INPLACE(tp_nb_ior, "or", PyNumber_InPlaceOr)
BINARY_INPLACE(tp_nb_ixor, "xor", PyNumber_InPlaceXor)
BINARY_INPLACE(tp_nb_iand, "and", PyNumber_InPlaceAnd)
BINARY_INPLACE(tp_nb_ilshift, "lshift", PyNumber_InPlaceLshift)
BINARY_INPLACE(tp_nb_irshift, "rshift", PyNumber_InPlaceRshift)
BINARY_INPLACE(tp_nb_iadd, "add", PyNumber_InPlaceAdd)
BINARY_INPLACE(tp_nb_isub, "sub", PyNumber_InPlaceSubtract)
BINARY_INPLACE(tp_nb_imul, "mul", PyNumber_InPlaceMultiply)
BINARY_INPLACE(tp_nb_idiv, "div", PyNumber_InPlaceDivide)
BINARY_INPLACE(tp_nb_imod, "mod", PyNumber_InPlaceRemainder)
BINARY_INPLACE(tp_nb_ifloordiv, "floordiv", PyNumber_InPlaceFloorDivide)
BINARY_INPLACE(tp_nb_itruediv, "truediv", PyNumber_InPlaceTrueDivide)

#undef UNARY
#undef BINARY
#undef BINARY_INPLACE

PyObject* tp_nb_long(PyObject* self) {
	PyObject* result = CallObject(self, "__long__");
	if (result == NULL) { PyErr_Clear(); return tp_nb_int(self); }
	return result == NULL? tp_nb_int(self): result;
}

PyObject* bin_power(PyObject* v, PyObject* w) {
	return PyNumber_Power(v, w, Py_None);
}

PyObject* bin_inplace_power(PyObject* v, PyObject* w) {
	return PyNumber_InPlacePower(v, w, Py_None);
}

PyObject* tp_nb_pow(PyObject* v, PyObject* w, PyObject* z) {
	if (z == Py_None) {
		PyObject* result = half_binop(v, w, "__pow__", bin_power, 0);
		if (result == NULL || result == Py_NotImplemented) {
			PyErr_Clear();
			Py_XDECREF(result);
			result = half_binop(w, v, "__rpow__", bin_power, 1);
		}
		return result;
	} else {
		return CallObject(v, "__pow__", w, z);
	}
}

PyObject* tp_nb_ipow(PyObject* v, PyObject* w, PyObject* z) {
	if (z == Py_None) {
		PyObject* result = half_binop(v, w, "__ipow__", bin_inplace_power, 0);
		if (result == NULL || result == Py_NotImplemented) {
			PyErr_Clear();
			Py_XDECREF(result);
			result = half_binop(v, w, "__pow__", bin_inplace_power, 0);
			if (result == NULL || result == Py_NotImplemented) {
				PyErr_Clear();
				Py_XDECREF(result);
				result = half_binop(w, v, "__rpow__", bin_inplace_power, 1);
			}
		}
		return result;
	} else {
		return CallObject(v, "__ipow__", w, z);
	}
}
