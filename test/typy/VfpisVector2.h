
#ifndef TYPY_VfpisVector2_H__
#define TYPY_VfpisVector2_H__

#include "typy.h"

#include "Vector2.h"

TypyVariantHeader(VfpisVector2, SINGLE_ARG(
	SINGLE_ARG(FixedPoint<3, 0>) _value1;
	int32 _value2;
	string _value3;
	Vector2* _value4;
));

#endif // TYPY_VfpisVector2_H__
