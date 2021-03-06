// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#ifndef TYPY_Fighter_H__
#define TYPY_Fighter_H__

#include "typy.h"

#include "Corpus.h"
#include "PyType.h"
#include "VLVector2.h"
#include "VLf.h"
#include "VbyfiVector2.h"
#include "VdesVector2.h"
#include "Vector2.h"
#include "VfisVector2.h"
#include "VfpisVector2.h"
#include "Vi.h"
#include "ViDiVfs.h"
#include "ViDif.h"
#include "ViLVfs.h"
#include "ViLf.h"
#include "ViPyType.h"

TypyHeaderBegin(Fighter);
	SINGLE_ARG(Dict<string, bytes >)* p_bd;
	List< bytes >* p_bl;
	SINGLE_ARG(Dict<int32, SINGLE_ARG(Dict<int32, VdesVector2 >) >)* p_dd;
	ViDiVfs* p_dv;
	SINGLE_ARG(Dict<int64, Corpus >)* p_ed;
	List< Corpus >* p_el;
	SINGLE_ARG(Dict<int32, SINGLE_ARG(Dict<int32, float >) >)* p_fdd;
	ViDif* p_fdv;
	List< float >* p_fl;
	SINGLE_ARG(Dict<int32, List< float > >)* p_fld;
	ViLf* p_flv;
	float p_hp;
	bool p_isAwake;
	SINGLE_ARG(Dict<int32, List< VdesVector2 > >)* p_ld;
	List< VLf >* p_ll;
	ViLVfs* p_lv;
	Vi* p_nv;
	Vector2* p_pos;
	SINGLE_ARG(Dict<int32, float >)* p_posi;
	List< Vector2 >* p_posl;
	List< VLVector2 >* p_posll;
	SINGLE_ARG(Dict<int64, Vector2 >)* p_poss;
	SINGLE_ARG(Dict<int32, Python<Shadow_PyType> >)* p_pyd;
	List< Python<Shadow_PyType> >* p_pyl;
	ViPyType* p_pyv1;
	ViPyType* p_pyv2;
	SINGLE_ARG(Dict<int32, string >)* p_sd;
	List< string >* p_sl;
	VbyfiVector2* p_v0;
	VbyfiVector2* p_v1;
	VfisVector2* p_v2;
	VfisVector2* p_v3;
	VfisVector2* p_v4;
	SINGLE_ARG(Dict<int32, VdesVector2 >)* p_vd;
	List< VfpisVector2 >* p_vl;
TypyHeaderEnd(Fighter);

#endif // TYPY_Fighter_H__
