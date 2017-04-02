#include "Python.h"

extern void init_typyd(void);

int main()
{
	Py_SetProgramName("testTypy");
	Py_Initialize();
	init_typyd();
	PyRun_SimpleString(
"import sys\n"
"sys.path.append('E:/test/typy/test')\n"
"sys.path.append('C:/Users/joung/Documents/project/typy/test')\n"
"import _typy\n"
);
	Py_Finalize();
	getchar();
    return 0;
}

