#-*- coding: utf-8 -*-
# Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

import _typyd
setDefaultEncodingUTF8 = _typyd.setDefaultEncodingUTF8

asdf = _typyd.Variant('asdf', (
	(1, 2, 3),
))
print asdf, asdf()

Corpus = _typyd.Enum('Corpus')
print Corpus

PyType = _typyd.Python('PyType')
print PyType

FixedPoint_3_5 = _typyd.FixedPoint(3, 5)
print FixedPoint_3_5

# Listxxx = _typyd.List('Listxxx', (1, 2, 3))
# print Listxxx

# Dictxxx = _typyd.Dict('Dictxxx', (1, 2, 3), (1, 2, 3))
# print Dictxxx


Fighter = _typyd.Object('Fighter', (
	("bd", 9, 1, 1, 2, True),
	("bl", 17, 1, 1, 2, True),
	("dd", 25, 1, 1, 2, True),
	("dv", 33, 1, 1, 2, True),
	("ed", 41, 1, 1, 2, True),
	("el", 49, 1, 1, 2, True),
	("fdd", 57, 1, 1, 2, True),
	("fdv", 65, 1, 1, 2, True),
	("fl", 73, 1, 1, 2, True),
	("fld", 81, 1, 1, 2, True),
	("flv", 89, 1, 1, 2, True),
	("hp", 97, 1, 1, 2, True),
	("isAwake", 105, 1, 1, 2, True),
	("ld", 113, 1, 1, 2, True),
	("ll", 121, 1, 1, 2, True),
	("lv", 129, 2, 1, 2, True),
	("nv", 137, 2, 1, 2, True),
	("pos", 145, 2, 1, 2, True),
	("posi", 153, 2, 1, 2, True),
	("posl", 161, 2, 1, 2, True),
	("poss", 169, 2, 1, 2, True),
	("pyd", 177, 2, 1, 2, True),
	("pyl", 185, 2, 1, 2, True),
	("pyv1", 193, 2, 1, 2, True),
	("pyv2", 201, 2, 1, 2, True),
	("sd", 209, 2, 1, 2, True),
	("sl", 217, 2, 1, 2, True),
	("v1", 225, 2, 1, 2, True),
	("v2", 233, 2, 1, 2, True),
	("v3", 241, 2, 1, 2, True),
	("v4", 249, 2, 1, 2, True),
	("vd", 257, 2, 1, 2, True),
	("vl", 265, 2, 1, 2, True),
))
print Fighter, Fighter()

Vector2 = _typyd.Object('Vector2', (
	("b", 9, 1, 1, 2, True),
	("e", 17, 1, 1, 2, True),
	("i", 25, 1, 1, 2, True),
	("p", 33, 1, 1, 2, True),
	("s", 41, 1, 1, 2, True),
	("x", 57, 1, 1, 2, True),
	("y", 65, 1, 1, 2, True),
	("t", 0, 0, 1, 2, True),
))
print Vector2, Vector2()

Fighter_Part2 = _typyd.Object('Fighter_Part2', (
	("bd", 9, 1, 1, 2, True),
	("bl", 17, 1, 1, 2, True),
	("ed", 25, 1, 1, 2, True),
	("el", 33, 1, 1, 2, True),
	("fl", 41, 1, 1, 2, True),
	("hp", 49, 1, 1, 2, True),
	("isAwake", 57, 1, 1, 2, True),
	("ll", 65, 1, 1, 2, True),
	("pos", 73, 1, 1, 2, True),
	("posi", 81, 1, 1, 2, True),
	("posl", 89, 1, 1, 2, True),
	("poss", 97, 1, 1, 2, True),
	("pyd", 105, 1, 1, 2, True),
	("pyl", 113, 1, 1, 2, True),
	("pyv1", 121, 1, 1, 2, True),
	("pyv2", 129, 2, 1, 2, True),
	("sd", 137, 2, 1, 2, True),
	("sl", 145, 2, 1, 2, True),
))
print Fighter_Part2, Fighter_Part2()

Empty = _typyd.Object('Empty', (
	
))
print Empty, Empty()

Fighter_Part1 = _typyd.Object('Fighter_Part1', (
	("hp", 9, 1, 1, 2, True),
	("isAwake", 17, 1, 1, 2, True),
	("pos", 25, 1, 1, 2, True),
	("posi", 33, 1, 1, 2, True),
	("posl", 41, 1, 1, 2, True),
	("poss", 49, 1, 1, 2, True),
	("pyd", 57, 1, 1, 2, True),
	("pyl", 65, 1, 1, 2, True),
	("pyv1", 73, 1, 1, 2, True),
	("pyv2", 81, 1, 1, 2, True),
))
print Fighter_Part1, Fighter_Part1()

