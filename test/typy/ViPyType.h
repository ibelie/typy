
#ifndef TYPY_ViPyType_H__
#define TYPY_ViPyType_H__

#include "typy.h"

#include "PyType.h"

TypyVariantHeader(ViPyType, SINGLE_ARG(
	int32 _value1;
	Python<Shadow_PyType>* _value2;
));

#endif // TYPY_ViPyType_H__
