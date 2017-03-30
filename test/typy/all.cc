// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "typy.h"

#include "PyType.h"
#include "Corpus.h"
#include "Empty.h"
#include "Fighter.h"
#include "Fighter_Part1.h"
#include "Fighter_Part2.h"
#include "Vector2.h"
#include "list.h"
#include "dict.h"

namespace typy {

bool InitModule(PyObject* m) {
	return PyType_Ready(&Dict<int32, Corpus >::_Type) >= 0 && PyType_Ready(&Dict<int32, Corpus >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, Corpus >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, List< VdesVector2 > >::_Type) >= 0 && PyType_Ready(&Dict<int32, List< VdesVector2 > >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, List< VdesVector2 > >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, List< float > >::_Type) >= 0 && PyType_Ready(&Dict<int32, List< float > >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, List< float > >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, Python<Shadow_PyType> >::_Type) >= 0 && PyType_Ready(&Dict<int32, Python<Shadow_PyType> >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, Python<Shadow_PyType> >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, SINGLE_ARG(Dict<int32, VdesVector2 >) >::_Type) >= 0 && PyType_Ready(&Dict<int32, SINGLE_ARG(Dict<int32, VdesVector2 >) >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, SINGLE_ARG(Dict<int32, VdesVector2 >) >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, SINGLE_ARG(Dict<int32, float >) >::_Type) >= 0 && PyType_Ready(&Dict<int32, SINGLE_ARG(Dict<int32, float >) >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, SINGLE_ARG(Dict<int32, float >) >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, VdesVector2 >::_Type) >= 0 && PyType_Ready(&Dict<int32, VdesVector2 >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, VdesVector2 >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, Vector2 >::_Type) >= 0 && PyType_Ready(&Dict<int32, Vector2 >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, Vector2 >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, Vfs >::_Type) >= 0 && PyType_Ready(&Dict<int32, Vfs >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, Vfs >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, float >::_Type) >= 0 && PyType_Ready(&Dict<int32, float >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, float >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<int32, string >::_Type) >= 0 && PyType_Ready(&Dict<int32, string >::IterKey_Type) >= 0 && PyType_Ready(&Dict<int32, string >::IterItem_Type) >= 0
		&& PyType_Ready(&Dict<string, bytes >::_Type) >= 0 && PyType_Ready(&Dict<string, bytes >::IterKey_Type) >= 0 && PyType_Ready(&Dict<string, bytes >::IterItem_Type) >= 0
		&& PyType_Ready(&List< Corpus >::_Type) >= 0 && PyType_Ready(&List< Corpus >::Iterator_Type) >= 0
		&& PyType_Ready(&List< Python<Shadow_PyType> >::_Type) >= 0 && PyType_Ready(&List< Python<Shadow_PyType> >::Iterator_Type) >= 0
		&& PyType_Ready(&List< VdesVector2 >::_Type) >= 0 && PyType_Ready(&List< VdesVector2 >::Iterator_Type) >= 0
		&& PyType_Ready(&List< Vector2 >::_Type) >= 0 && PyType_Ready(&List< Vector2 >::Iterator_Type) >= 0
		&& PyType_Ready(&List< VfpisVector2 >::_Type) >= 0 && PyType_Ready(&List< VfpisVector2 >::Iterator_Type) >= 0
		&& PyType_Ready(&List< Vfs >::_Type) >= 0 && PyType_Ready(&List< Vfs >::Iterator_Type) >= 0
		&& PyType_Ready(&List< bytes >::_Type) >= 0 && PyType_Ready(&List< bytes >::Iterator_Type) >= 0
		&& PyType_Ready(&List< float >::_Type) >= 0 && PyType_Ready(&List< float >::Iterator_Type) >= 0
		&& PyType_Ready(&List< string >::_Type) >= 0 && PyType_Ready(&List< string >::Iterator_Type) >= 0
		&& InitCorpus(m)
		&& Object<Empty>::Init(m)
		&& Object<Fighter>::Init(m)
		&& Object<Fighter_Part1>::Init(m)
		&& Object<Fighter_Part2>::Init(m)
		&& Object<Vector2>::Init(m)
		&& Python<Shadow_PyType>::Init(m, "PyType");
}

} // namespace typy
