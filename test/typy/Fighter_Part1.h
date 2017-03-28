
#ifndef TYPY_Fighter_Part1_H__
#define TYPY_Fighter_Part1_H__

#include "typy.h"

#include "PyType.h"
#include "Vector2.h"
#include "ViPyType.h"

TypyHeaderBegin(Fighter_Part1);
	float p_hp;
	bool p_isAwake;
	Vector2* p_pos;
	SINGLE_ARG(Dict<int32, float >)* p_posi;
	List< Vector2 >* p_posl;
	SINGLE_ARG(Dict<int32, Vector2 >)* p_poss;
	SINGLE_ARG(Dict<int32, Python<Shadow_PyType> >)* p_pyd;
	List< Python<Shadow_PyType> >* p_pyl;
	ViPyType* p_pyv1;
	ViPyType* p_pyv2;
TypyHeaderEnd(Fighter_Part1);

#endif // TYPY_Fighter_Part1_H__
