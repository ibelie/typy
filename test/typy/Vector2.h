
#ifndef TYPY_Vector2_H__
#define TYPY_Vector2_H__

#include "typy.h"

#include "Corpus.h"
#include "Empty.h"
#include "PyType.h"

TypyHeaderBegin(Vector2);
	bytes p_b;
	Corpus p_e;
	Empty* p_i;
	Python<Shadow_PyType>* p_p;
	string p_s;
	Python<PyObject>* p_t;
	float p_x;
	SINGLE_ARG(FixedPoint<1, -10>) p_y;
TypyHeaderEnd(Vector2);

#endif // TYPY_Vector2_H__
