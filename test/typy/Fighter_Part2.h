
#ifndef TYPY_Fighter_Part2_H__
#define TYPY_Fighter_Part2_H__

#include "typy.h"

#include "Corpus.h"
#include "PyType.h"
#include "Vector2.h"
#include "ViPyType.h"

TypyHeaderBegin(Fighter_Part2);
	SINGLE_ARG(Dict<string, bytes >)* p_bd;
	List< bytes >* p_bl;
	SINGLE_ARG(Dict<int32, Corpus >)* p_ed;
	List< Corpus >* p_el;
	List< float >* p_fl;
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
	SINGLE_ARG(Dict<int32, string >)* p_sd;
	List< string >* p_sl;
TypyHeaderEnd(Fighter_Part2);

#endif // TYPY_Fighter_Part2_H__
