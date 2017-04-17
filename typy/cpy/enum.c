// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef __cplusplus
extern "C" {
#endif

IblMap_KEY_NUMERIC(TypyEnumMap, TypyField,
	PyObject* python;
);

TypyEnum* Typy_RegisterEnum(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyEnum* type;
	if (!PyArg_ParseTuple(args, "s#", &name, &nameLen)) {
		return NULL;
	}

	type = (TypyEnum*)malloc(sizeof(TypyEnum) + nameLen);
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Register Enum object out of memory %lu.", sizeof(TypyEnum) + nameLen);
		return NULL;
	}

	type->enum_map = TypyEnumMap_New();
	if (!type->enum_map) {
		free(type);
		PyErr_Format(PyExc_RuntimeError, "Register Enum map out of memory.");
		return NULL;
	}

	type->enum_name[nameLen] = 0;
	memcpy(type->enum_name, name, nameLen);
	(void)PyObject_INIT(type, &TypyEnumType);

	return type;
}

PyObject* TypyEnum_GetPyObject(TypyEnum* type, TypyField* value) {
	register TypyEnumMap item = (TypyEnumMap)IblMap_Get(type->enum_map, value);
	if (!item || !item->python) { Py_RETURN_NONE; }
	Py_INCREF(item->python);
	return item->python;
}

bool TypyEnum_CheckAndSet(TypyEnum* type, TypyField* value, PyObject* arg, const char* err) {
	TypyField i = (TypyField)PyInt_AsLong(arg);
	if (PyErr_Occurred()) { return false; }
	register TypyEnumMap item = (TypyEnumMap)IblMap_Get(type->enum_map, &i);
	if (!item || !item->python) { return false; }
	*value = i;
	return true;
}

static TypyEnum* TypyEnum_Initialize(TypyEnum* type, PyObject* args) {
	TypyField key;
	PyObject *k, *v;
	Py_ssize_t pos = 0;
	PyObject* enums = Py_None;
	if (PyArg_ParseTuple(args, "|O", &enums)) {
		if (PyDict_Check(enums)) {
			while (PyDict_Next(enums, &pos, &k, &v)) {
				key = (TypyField)PyInt_AsLong(k);
				if (PyErr_Occurred()) {
					return NULL;
				}
				register TypyEnumMap item = (TypyEnumMap)IblMap_Set(type->enum_map, &key);
				Py_XDECREF(item->python);
				Py_INCREF(v);
				item->python = v;
			}
		}
	}
	Py_INCREF(type);
	return type;
}

static void TypyEnum_Dealloc(TypyEnum* type) {
	if (type->enum_map) {
		register TypyEnumMap item;
		register IblMap_Item iter;
		for (iter = IblMap_Begin(type->enum_map); iter; iter = IblMap_Next(type->enum_map, iter)) {
			item = (TypyEnumMap)iter;
			Py_DECREF(item->python);
		}
		IblMap_Free(type->enum_map);
	}
	free(type);
}

static PyObject* TypyEnum_Repr(TypyEnum* type) {
	return PyString_FromFormat("<Enum '" FULL_MODULE_NAME ".%s'>", type->enum_name);
}

PyTypeObject TypyEnumType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Enum",                 /* tp_name           */
	sizeof(TypyEnum),                         /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)TypyEnum_Dealloc,             /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	0,                                        /* tp_compare        */
	(reprfunc)TypyEnum_Repr,                  /* tp_repr           */
	0,                                        /* tp_as_number      */
	0,                                        /* tp_as_sequence    */
	0,                                        /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	(ternaryfunc)TypyEnum_Initialize,         /* tp_call           */
	(reprfunc)TypyEnum_Repr,                  /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy Enum",                            /* tp_doc            */
};
