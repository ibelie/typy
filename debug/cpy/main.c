// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "Python.h"

extern void init_typyd(void);

int main()
{
	Py_SetProgramName("testTypy");
	Py_Initialize();
	init_typyd();
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
"import build_test\n"
"build_test.SETUP_TYPYD = False\n"
"build_test.setup()\n"
"build_test.test_cpy()\n"
);
	Py_Finalize();
	getchar();
    return 0;
}

