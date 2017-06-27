// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "typy.h"

#ifdef TYPY_EXTENSION_PROTOBUF
#	include "extension_protobuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static TypyPython _TypyPythonBuiltin[] = { { PyObject_HEAD_INIT(NULL) NULL} };
TypyPython* TypyPythonBuiltin = _TypyPythonBuiltin;

TypyPython* Typy_RegisterPython(PyObject* m, PyObject* args) {
	char *name;
	Py_ssize_t nameLen;
	TypyPython* type;
	if (!PyArg_ParseTuple(args, "s#", &name, &nameLen)) {
		return NULL;
	}

	type = (TypyPython*)calloc(1, IBL_ALIGNED_SIZE(sizeof(TypyPython) + sizeof(char) * nameLen));
	if (!type) {
		PyErr_Format(PyExc_RuntimeError, "Register Python out of memory %zu.", sizeof(TypyPython) + nameLen);
		return NULL;
	}

	type->python_type = &PyBaseObject_Type;
	memcpy(type->python_name, name, nameLen);
	(void)PyObject_INIT(type, &TypyPythonType);

	return type;
}

PyObject* TypyPython_GetPyObject(TypyPython* type, PyObject** value) {
	if (*value) {
		Py_INCREF(*value);
		return *value;
	} else if (PyType_FastSubclass(type->python_type, Py_TPFLAGS_LIST_SUBCLASS)) {
		return PyList_New(0);
	} else if (PyType_FastSubclass(type->python_type, Py_TPFLAGS_DICT_SUBCLASS)) {
		return PyDict_New();
	}
	Py_RETURN_NONE;
}

bool TypyPython_CheckAndSet(TypyPython* type, PyObject** value, PyObject* arg, const char* err) {
	if (arg == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	} else if (PyObject_TypeCheck(arg, type->python_type) ||
		(type->python_type == &PyList_Type && PySequence_Check(arg)) ||
		(type->python_type == &PyDict_Type && PyObject_HasAttrString(arg, "iteritems"))) {
		Py_INCREF(arg);
		*value = arg;
		return true;
	} else {
		FormatTypeError(arg, err);
		return false;
	}
}

bool TypyPython_Read(TypyPython* type, PyObject** value, byte** input, size_t* length) {
	uint32 size;
	if (!Typy_ReadVarint32(input, length, &size)) {
		return false;
	} else if (size > *length) {
		return false;
	} else if (!(*value)) {
		*value = PyObject_CallObject((PyObject*)type->python_type, NULL);
	}
	if (!size) { return true; }
	if (!(*value)) { return false; }
	*input += size;
	*length -= size;
	if (type->python_read) {
		return type->python_read(*value, (*input) - size, size);
#ifdef TYPY_EXTENSION_PROTOBUF
	} else if (Protobuf_TypeCheck((PyTypeObject*)type->python_type)) {
		return Protobuf_Read(*value, (*input) - size, size);
#endif
	}
	register PyObject* data = PyBytes_FromStringAndSize((const char*)(*input) - size, size);
	register PyObject* result = PyObject_CallMethod(*value, "Deserialize", "O", data);
	Py_XDECREF(result);
	Py_XDECREF(data);
	return true;
}

size_t TypyPython_Write(TypyPython* type, PyObject** value, int tag, byte* output) {
	register size_t size = 0;
	if (tag) {
		size += Typy_WriteTag(output, tag);
	}
	if (*value) {
		if ((type->python_cachedsize || type->python_bytesize) && type->python_write) {
			register size_t length = type->python_cachedsize ? type->python_cachedsize(*value) : type->python_bytesize(*value);
			size += IblPutUvarint(output + size, length);
			return size + type->python_write(*value, output + size);
#ifdef TYPY_EXTENSION_PROTOBUF
		} else if (Protobuf_TypeCheck((PyTypeObject*)type->python_type)) {
			size += IblPutUvarint(output + size, Protobuf_CachedSize(*value));
			return size + Protobuf_Write(*value, output + size);
#endif
		}
		register PyObject* data = PyObject_CallMethod(*value, "Serialize", NULL);
		if (data) {
			register size_t length = PyBytes_GET_SIZE(data);
			size += IblPutUvarint(output + size, length);
			memcpy(output + size, PyBytes_AS_STRING(data), length);
			Py_DECREF(data);
			return size + length;
		}
	}
	output[size] = 0;
	return size + 1;
}

size_t TypyPython_ByteSize(TypyPython* type, PyObject** value, int tagsize) {
	register size_t size = 0;
	if (*value) {
		if (type->python_bytesize) {
			size = type->python_bytesize(*value);
#ifdef TYPY_EXTENSION_PROTOBUF
		} else if (Protobuf_TypeCheck((PyTypeObject*)type->python_type)) {
			size = Protobuf_ByteSize(*value);
#endif
		} else {
			register PyObject* s = PyObject_CallMethod(*value, "ByteSize", NULL);
			if (s) { size = PyInt_AsLong(s); Py_DECREF(s); }
		}
	}
	return tagsize + IblSizeVarint(size) + size;
}

PyObject* TypyPython_ToJson(TypyPython* type, PyObject** value, bool slim) {
	if (!slim || *value) {
		register PyObject* json = !(*value) ? PyDict_New() : (type->python_tojson ? type->python_tojson(*value) : PyObject_CallMethod(*value, "Json", NULL));
		if (!json) { return NULL; }
		register PyObject* _t = PyString_FromString(type->python_type->tp_name);
		PyDict_SetItemString(json, "_t", _t);
		Py_XDECREF(_t);
		return json;
	} else {
		return NULL;
	}
}

bool TypyPython_FromJson(TypyPython* type, PyObject** value, PyObject* json) {
	if (!json || json == Py_None) {
		Py_XDECREF(*value);
		*value = NULL;
		return true;
	}
	register PyObject* _k = NULL;
	register PyObject* _v = NULL;
	register PyObject* item = NULL;
	register PyObject* dict = PyDict_New();
	register PyObject* iter = PyObject_CallMethod(json, "iteritems", NULL);
	if (!iter) {
		FormatTypeError(json, "FromJson expect dict, but ");
		goto fromjson_failed;
	} else if (!dict) {
		goto fromjson_failed;
	}

	register bool type_check = false;
	register Py_ssize_t i, size = _PyObject_LengthHint(json, 0);
	for (i = 0; i < size; i++) {
		item = PyIter_Next(iter);
		if (!item) { goto fromjson_failed; }
		register PyObject* k = PyTuple_GET_ITEM(item, 0);
		register PyObject* v = PyTuple_GET_ITEM(item, 1);
		if (PyUnicode_Check(k)) {
			_k = PyUnicode_AsEncodedObject(k, "utf-8", NULL);
		} else if (PyBytes_Check(k)) {
			Py_XINCREF(k);
			_k = k;
		}

		if (_k && !strcmp(PyBytes_AS_STRING(_k), "_t")) {
			if (PyUnicode_Check(v)) {
				_v = PyUnicode_AsEncodedObject(v, "utf-8", NULL);
			} else if (PyBytes_Check(v)) {
				Py_XINCREF(v);
				_v = v;
			}
			if (!_v) {
				FormatTypeError(v, "Json _t expect String, but ");
				goto fromjson_failed;
			} else if (strcmp(PyBytes_AS_STRING(_v), type->python_type->tp_name)) {
				PyErr_Format(PyExc_TypeError, "Python expect '%.100s', but Json has type %.100s",
					type->python_type->tp_name, PyBytes_AS_STRING(_v));
				goto fromjson_failed;
			}
			type_check = true;
			Py_XDECREF(_v);
			_v = NULL;
			continue;
		}
		PyDict_SetItem(dict, k, v);
		Py_DECREF(item);
		item = NULL;
		Py_XDECREF(_k);
		_k = NULL;
	}
	if (!type_check) {
		FormatTypeError(json, "Json expect _t, ");
		goto fromjson_failed;
	}
	Py_XDECREF(*value);
	if (type->python_fromjson) {
		*value = type->python_fromjson(type->python_type, dict);
	} else {
		*value = PyObject_CallMethod((PyObject*)type->python_type, "FromJson", "O", dict);
	}
	Py_DECREF(iter);
	Py_DECREF(dict);
	return *value ? true : false;

fromjson_failed:
	Py_XDECREF(_k);
	Py_XDECREF(_v);
	Py_XDECREF(dict);
	Py_XDECREF(iter);
	Py_XDECREF(item);
	return false;
}

static TypyPython* TypyPython_Initialize(TypyPython* type, PyObject* args) {
	PyObject* python_type = Py_None;
	if (PyArg_ParseTuple(args, "|O", &python_type)) {
		Py_XDECREF(type->python_type);
		Py_INCREF(python_type);
		type->python_type = (PyTypeObject*)python_type;
		register TypyPython* p;
		for (p = TypyPythonBuiltin; p->python_type; p++) {
			if (p->python_type == type->python_type) {
				TypyPython_COPY(type, p);
				break;
			}
		}
	}
	Py_INCREF(type);
	return type;
}

static void TypyPython_Dealloc(TypyPython* type) {
	Py_XDECREF(type->python_type);
	free(type);
}

static PyObject* TypyPython_Repr(TypyPython* type) {
	return PyString_FromFormat("<Python '" FULL_MODULE_NAME ".%.100s'>", type->python_name);
}

PyTypeObject TypyPythonType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	FULL_MODULE_NAME ".Python",               /* tp_name           */
	sizeof(TypyPython),                       /* tp_basicsize      */
	0,                                        /* tp_itemsize       */
	(destructor)TypyPython_Dealloc,           /* tp_dealloc        */
	0,                                        /* tp_print          */
	0,                                        /* tp_getattr        */
	0,                                        /* tp_setattr        */
	0,                                        /* tp_compare        */
	(reprfunc)TypyPython_Repr,                /* tp_repr           */
	0,                                        /* tp_as_number      */
	0,                                        /* tp_as_sequence    */
	0,                                        /* tp_as_mapping     */
	PyObject_HashNotImplemented,              /* tp_hash           */
	(ternaryfunc)TypyPython_Initialize,       /* tp_call           */
	(reprfunc)TypyPython_Repr,                /* tp_str            */
	0,                                        /* tp_getattro       */
	0,                                        /* tp_setattro       */
	0,                                        /* tp_as_buffer      */
	Py_TPFLAGS_DEFAULT,                       /* tp_flags          */
	"A Typy Python",                          /* tp_doc            */
};

#ifdef __cplusplus
}
#endif
