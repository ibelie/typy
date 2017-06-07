// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "Python.h"
#include "typy.h"

extern void init_typyd(void);

static PyObject* callback;

static PyObject* RegisterCallback(PyObject* m, PyObject* arg) {
	callback = arg;
	Py_RETURN_NONE;
}

static PyMethodDef ModuleMethods[] = {
	{"RegisterCallback", (PyCFunction)RegisterCallback, METH_O,
		"register callback for handlers."},
	{ NULL, NULL}
};

static inline void init(void) {
	init_typyd();

	register PyObject* modules = PyImport_GetModuleDict();
	if (!modules) { return; }
	register PyObject* m = PyDict_GetItemString(modules, "_typyd");
	if (m) {
		register PyMethodDef* meth;
		for (meth = ModuleMethods; meth->ml_name; meth++) {
			register PyObject* method = PyCFunction_NewEx(meth, m, NULL);
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
"sys.path.append('/home/joungtao/typy')\n"
"sys.path.append('/home/joungtao/typy/test')\n"
"import prop_debug\n"
"prop_debug.setup()\n"
"prop_debug.test_prop()\n"
);
	Py_Finalize();
	getchar();
    return 0;
}

