// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "Python.h"
#include "typy.h"

extern void init_typyd(void);

static PyObject* callback;

static void onPropertyChanged(TypyObject* object, size_t flag, size_t i, FieldType field_type, TypyType typy_type, TypyField old, TypyField new) {
	if (callback) {
		register PyObject* old_object = abstract_GetPyObject[field_type](typy_type, &old);
		register PyObject* new_object = abstract_GetPyObject[field_type](typy_type, &new);
		PyObject_CallFunction(callback, "OsOO", Typy_TYPE(object), Typy_PropertyName(object, i), old_object, new_object);
		Py_XDECREF(old_object);
		Py_XDECREF(new_object);
	}
}

static PyObject* RegisterCallback(PyObject* m, PyObject* arg) {
	callback = arg;
	Py_RETURN_NONE;
}

static PyObject* RegisterHandler(PyObject* m, TypyMetaObject* type) {
	register size_t i;
	for (i = 0; i < type->meta_size; i++) {
		if (!Meta_HandleProperty(type, i, (TypyHandlerData)i, (TypyHandlerFunc)onPropertyChanged)) {
			PyErr_Format(PyExc_RuntimeError, "Register %s handler for %s failed.", Meta_PropertyName(type, i), Meta_NAME(type));
			return NULL;
		}
	}
	Py_RETURN_NONE;
}

static PyObject* UnregisterHandler(PyObject* m, TypyMetaObject* type) {
	register size_t i;
	for (i = 0; i < type->meta_size; i++) {
		TypyProperty_Unregister(type, (TypyHandlerData)i, (TypyHandlerFunc)onPropertyChanged);
	}
	Py_RETURN_NONE;
}

static PyMethodDef ModuleMethods[] = {
	{"RegisterCallback", (PyCFunction)RegisterCallback, METH_O,
		"register callback for handlers."},
	{"RegisterHandler", (PyCFunction)RegisterHandler, METH_O,
		"register handlers for Typy."},
	{"UnregisterHandler", (PyCFunction)UnregisterHandler, METH_O,
		"unregister handlers for Typy."},
	{ NULL, NULL}
};

static inline void init(void) {
	init_typyd();

	PyObject* modules = PyImport_GetModuleDict();
	if (!modules) { return; }
	PyObject* m = PyDict_GetItemString(modules, "_typyd");
	if (m) {
		for (PyMethodDef* meth = ModuleMethods; meth->ml_name; meth++) {
			PyObject* method = PyCFunction_NewEx(meth, m, NULL);
			if (method) {
				PyDict_SetItemString(PyModule_GetDict(m), meth->ml_name, method);
			}
		}
	}
}

int main()
{
	Py_SetProgramName("testTypy");
	Py_Initialize();
	init();
	PyRun_SimpleString(
"import sys\n"
"import os\n"
"os.chdir('../..')\n"
"sys.path.append('E:/test/typy')\n"
"sys.path.append('E:/test/typy/test')\n"
"sys.path.append('C:/Users/joung/Documents/project/typy')\n"
"sys.path.append('C:/Users/joung/Documents/project/typy/test')\n"
"sys.path.append('/home/joungtao/program/typy')\n"
"sys.path.append('/home/joungtao/program/typy/test')\n"
"import prop_debug\n"
"prop_debug.setup()\n"
"prop_debug.test_prop()\n"
);
	Py_Finalize();
	getchar();
    return 0;
}

