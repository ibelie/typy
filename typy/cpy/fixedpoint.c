// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject* Typy_RegisterFixedPoint(PyObject* m, PyObject* args) {
	uint8 precision;
	int32 _floor;
	TypyFixedPoint* type;
	if (!PyArg_ParseTuple(args, "bl", &precision, &_floor)) {
		return NULL;
	}

	type = (TypyFixedPoint*)malloc(sizeof(TypyFixedPoint));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "[typyd] Register FixedPoint: out of memory %d.", sizeof(TypyFixedPoint));
		return NULL;
	}

	type->fixedpoint_precision = precision;
	type->fixedpoint_floor = _floor;
	PyObject_INIT(type, &TypyFixedPointType);

	return (PyObject*)type;
}

PyObject* TypyFixedPoint_GetPyObject(TypyFixedPoint* type, TypyField* value) {
	return PyFloat_FromDouble(TypyFixedPoint_toDouble(type, *value));
}

bool TypyFixedPoint_CheckAndSet(TypyFixedPoint* type, TypyField* value, PyObject* arg, const char* err) {
	if (!PyInt_Check(arg) && !PyLong_Check(arg) && !PyFloat_Check(arg)) {
		FormatTypeError(arg, err);
		return false;
	}
	*value = TypyFixedPoint_fromDouble(type, PyFloat_AsDouble(arg));
	return true;
}

bool TypyFixedPoint_Read(TypyFixedPoint* type, TypyField* value, byte** input, size_t* length) {
	return Typy_ReadVarint32(input, length, value);
}

size_t TypyFixedPoint_Write(TypyFixedPoint* type, int tag, TypyField value, byte* output) {
	register TypyField _floor = -type->fixedpoint_floor * type->fixedpoint_precision;
	register size_t size = 0;
	if (value != _floor) {
		size = Typy_WriteTag(output, tag);
		size += Typy_WriteVariant32(output + size, value);
	}
	return size;
}

size_t TypyFixedPoint_ByteSize(TypyFixedPoint* type, int tagsize, TypyField value) {
	return value ? tagsize + IblSizeVarint(value) : 0;
}


void TypyFixedPoint_Dealloc(TypyFixedPoint* type) {
	free(type);
}

static PyObject* TypyFixedPoint_Repr(TypyFixedPoint* type) {
	return PyString_FromFormat("<type '" FULL_MODULE_NAME ".FixedPoint' precision(%d) floor(%d)>",
		type->fixedpoint_precision, type->fixedpoint_floor);
}

PyTypeObject TypyFixedPointType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".FixedPoint",           /* tp_name           */
	sizeof(TypyFixedPoint),                   /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)TypyFixedPoint_Dealloc,       /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	0,                                        /* tp_compare        */
	(reprfunc)TypyFixedPoint_Repr,            /* tp_repr           */
	0,                                        /* tp_as_number      */
	0,                                        /* tp_as_sequence    */
	0,                                        /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	0,                                        /* tp_call           */
	(reprfunc)TypyFixedPoint_Repr,            /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy FixedPoint",                      /* tp_doc            */
};
