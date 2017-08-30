#-*- coding: utf-8 -*-
# Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

import _typyd
setDefaultEncodingUTF8 = _typyd.setDefaultEncodingUTF8

Lb = _typyd.List('Lb', (0, 8, ))
BooleanList = _typyd.Object('BooleanList', (
	('bl', 10, 1, 2, 14, Lb),
))
Empty = _typyd.Object('Empty', (
))
Dbys = _typyd.Dict('Dbys', (2, 10, ), (2, 9, ))
Lby = _typyd.List('Lby', (2, 9, ))
Corpus = _typyd.Enum('Corpus')
FixedPoint_1_s10 = _typyd.FixedPoint(1, -10)
PyType = _typyd.Python('PyType')
PyObject = _typyd.Python('PyObject')
Vector2 = _typyd.Object('Vector2', (
	('x', 13, 1, 5, 7, ),
	('y', 16, 1, 0, 5, FixedPoint_1_s10),
	('b', 26, 1, 2, 9, ),
	('e', 32, 1, 0, 0, Corpus),
	('i', 42, 1, 2, 12, Empty),
	('p', 50, 1, 2, 16, PyType),
	('s', 58, 1, 2, 10, ),
	('t', 0, 0, 2, 16, PyObject),
))
VdesVector2 = _typyd.Variant('VdesVector2', (
	('Double', 9, 1, 1, 6, ),
	('Enum', 16, 1, 0, 0, Corpus),
	('String', 26, 1, 2, 10, ),
	('Vector2', 34, 1, 2, 12, Vector2),
))
DiVdesVector2 = _typyd.Dict('DiVdesVector2', (0, 1, ), (2, 13, VdesVector2))
DiDiVdesVector2 = _typyd.Dict('DiDiVdesVector2', (0, 1, ), (2, 15, DiVdesVector2))
Vfs = _typyd.Variant('Vfs', (
	('Float', 13, 1, 5, 7, ),
	('String', 18, 1, 2, 10, ),
))
DiVfs = _typyd.Dict('DiVfs', (0, 1, ), (2, 13, Vfs))
ViDiVfs = _typyd.Variant('ViDiVfs', (
	('Dict', 10, 1, 2, 15, DiVfs),
	('Integer', 16, 1, 0, 1, ),
))
Dei = _typyd.Dict('Dei', (0, 1, ), (0, 0, Corpus))
Le = _typyd.List('Le', (0, 0, Corpus))
Dfi = _typyd.Dict('Dfi', (0, 1, ), (5, 7, ))
DiDfi = _typyd.Dict('DiDfi', (0, 1, ), (2, 15, Dfi))
ViDfi = _typyd.Variant('ViDfi', (
	('Dict', 10, 1, 2, 15, Dfi),
	('Integer', 16, 1, 0, 1, ),
))
Lf = _typyd.List('Lf', (5, 7, ))
DiLf = _typyd.Dict('DiLf', (0, 1, ), (2, 14, Lf))
ViLf = _typyd.Variant('ViLf', (
	('Integer', 8, 1, 0, 1, ),
	('List', 18, 1, 2, 14, Lf),
))
LVdesVector2 = _typyd.List('LVdesVector2', (2, 13, VdesVector2))
DiLVdesVector2 = _typyd.Dict('DiLVdesVector2', (0, 1, ), (2, 14, LVdesVector2))
VLf = _typyd.Variant('VLf', (
	('List', 10, 1, 2, 14, Lf),
))
LVLf = _typyd.List('LVLf', (2, 13, VLf))
LVfs = _typyd.List('LVfs', (2, 13, Vfs))
ViLVfs = _typyd.Variant('ViLVfs', (
	('Integer', 8, 1, 0, 1, ),
	('List', 18, 1, 2, 14, LVfs),
))
Vi = _typyd.Variant('Vi', (
	('Integer', 8, 1, 0, 1, ),
))
LVector2 = _typyd.List('LVector2', (2, 12, Vector2))
VLVector2 = _typyd.Variant('VLVector2', (
	('List', 10, 1, 2, 14, LVector2),
))
LVLVector2 = _typyd.List('LVLVector2', (2, 13, VLVector2))
DiVector2 = _typyd.Dict('DiVector2', (0, 1, ), (2, 12, Vector2))
DiPyType = _typyd.Dict('DiPyType', (0, 1, ), (2, 16, PyType))
LPyType = _typyd.List('LPyType', (2, 16, PyType))
ViPyType = _typyd.Variant('ViPyType', (
	('Integer', 8, 1, 0, 1, ),
	('PyType', 18, 1, 2, 16, PyType),
))
Dis = _typyd.Dict('Dis', (0, 1, ), (2, 10, ))
Ls = _typyd.List('Ls', (2, 10, ))
VbyfiVector2 = _typyd.Variant('VbyfiVector2', (
	('Bytes', 10, 1, 2, 9, ),
	('Float', 21, 1, 5, 7, ),
	('Integer', 24, 1, 0, 1, ),
	('Vector2', 34, 1, 2, 12, Vector2),
))
VfisVector2 = _typyd.Variant('VfisVector2', (
	('Float', 13, 1, 5, 7, ),
	('Integer', 16, 1, 0, 1, ),
	('String', 26, 1, 2, 10, ),
	('Vector2', 34, 1, 2, 12, Vector2),
))
FixedPoint_3_s0 = _typyd.FixedPoint(3, 0)
VfpisVector2 = _typyd.Variant('VfpisVector2', (
	('FixedPoint', 8, 1, 0, 5, FixedPoint_3_s0),
	('Integer', 16, 1, 0, 1, ),
	('String', 26, 1, 2, 10, ),
	('Vector2', 34, 1, 2, 12, Vector2),
))
LVfpisVector2 = _typyd.List('LVfpisVector2', (2, 13, VfpisVector2))
Fighter = _typyd.Object('Fighter', (
	('bd', 10, 1, 2, 15, Dbys),
	('bl', 18, 1, 2, 14, Lby),
	('dd', 26, 1, 2, 15, DiDiVdesVector2),
	('dv', 34, 1, 2, 13, ViDiVfs),
	('ed', 42, 1, 2, 15, Dei),
	('el', 50, 1, 2, 14, Le),
	('fdd', 58, 1, 2, 15, DiDfi),
	('fdv', 66, 1, 2, 13, ViDfi),
	('fl', 74, 1, 2, 14, Lf),
	('fld', 82, 1, 2, 15, DiLf),
	('flv', 90, 1, 2, 13, ViLf),
	('hp', 101, 1, 5, 7, ),
	('isAwake', 104, 1, 0, 8, ),
	('ld', 114, 1, 2, 15, DiLVdesVector2),
	('ll', 122, 1, 2, 14, LVLf),
	('lv', 130, 2, 2, 13, ViLVfs),
	('nv', 138, 2, 2, 13, Vi),
	('pos', 146, 2, 2, 12, Vector2),
	('posi', 154, 2, 2, 15, Dfi),
	('posl', 162, 2, 2, 14, LVector2),
	('posll', 170, 2, 2, 14, LVLVector2),
	('poss', 178, 2, 2, 15, DiVector2),
	('pyd', 186, 2, 2, 15, DiPyType),
	('pyl', 194, 2, 2, 14, LPyType),
	('pyv1', 202, 2, 2, 13, ViPyType),
	('pyv2', 210, 2, 2, 13, ViPyType),
	('sd', 218, 2, 2, 15, Dis),
	('sl', 226, 2, 2, 14, Ls),
	('v0', 234, 2, 2, 13, VbyfiVector2),
	('v1', 242, 2, 2, 13, VbyfiVector2),
	('v2', 250, 2, 2, 13, VfisVector2),
	('v3', 258, 2, 2, 13, VfisVector2),
	('v4', 266, 2, 2, 13, VfisVector2),
	('vd', 274, 2, 2, 15, DiVdesVector2),
	('vl', 282, 2, 2, 14, LVfpisVector2),
))
Fighter_Part1 = _typyd.Object('Fighter_Part1', (
	('hp', 13, 1, 5, 7, ),
	('isAwake', 16, 1, 0, 8, ),
	('pos', 26, 1, 2, 12, Vector2),
	('posi', 34, 1, 2, 15, Dfi),
	('posl', 42, 1, 2, 14, LVector2),
	('posll', 50, 1, 2, 14, LVLVector2),
	('poss', 58, 1, 2, 15, DiVector2),
	('pyd', 66, 1, 2, 15, DiPyType),
	('pyl', 74, 1, 2, 14, LPyType),
	('pyv1', 82, 1, 2, 13, ViPyType),
	('pyv2', 90, 1, 2, 13, ViPyType),
))
Fighter_Part2 = _typyd.Object('Fighter_Part2', (
	('bd', 10, 1, 2, 15, Dbys),
	('bl', 18, 1, 2, 14, Lby),
	('ed', 26, 1, 2, 15, Dei),
	('el', 34, 1, 2, 14, Le),
	('fl', 42, 1, 2, 14, Lf),
	('hp', 53, 1, 5, 7, ),
	('isAwake', 56, 1, 0, 8, ),
	('ll', 66, 1, 2, 14, LVLf),
	('pos', 74, 1, 2, 12, Vector2),
	('posi', 82, 1, 2, 15, Dfi),
	('posl', 90, 1, 2, 14, LVector2),
	('posll', 98, 1, 2, 14, LVLVector2),
	('poss', 106, 1, 2, 15, DiVector2),
	('pyd', 114, 1, 2, 15, DiPyType),
	('pyl', 122, 1, 2, 14, LPyType),
	('pyv1', 130, 2, 2, 13, ViPyType),
	('pyv2', 138, 2, 2, 13, ViPyType),
	('sd', 146, 2, 2, 15, Dis),
	('sl', 154, 2, 2, 14, Ls),
))
RPG = _typyd.Object('RPG', (
	('fighter', 10, 1, 2, 12, Fighter),
))
Vector3 = _typyd.Python('Vector3')
Vbbyfi = _typyd.Variant('Vbbyfi', (
	('Boolean', 8, 1, 0, 8, ),
	('Bytes', 18, 1, 2, 9, ),
	('Float', 29, 1, 5, 7, ),
	('Integer', 32, 1, 0, 1, ),
))
DbyVbbyfi = _typyd.Dict('DbyVbbyfi', (2, 9, ), (2, 13, Vbbyfi))
SkillParam = _typyd.Object('SkillParam', (
	('buckID', 10, 1, 2, 9, ),
	('destPos', 18, 1, 2, 16, Vector3),
	('destRot', 26, 1, 2, 16, Vector3),
	('extraParam', 34, 1, 2, 15, DbyVbbyfi),
	('origPos', 42, 1, 2, 16, Vector3),
	('origRot', 50, 1, 2, 16, Vector3),
	('targetID', 58, 1, 2, 9, ),
	('targetIDs', 66, 1, 2, 14, Lby),
))
Li = _typyd.List('Li', (0, 1, ))
LSkillParam = _typyd.List('LSkillParam', (2, 12, SkillParam))
DbyLi = _typyd.Dict('DbyLi', (2, 9, ), (2, 14, Li))
onInitRuntime = _typyd.Object('onInitRuntime', (
	('buckID', 10, 1, 2, 9, ),
	('events', 18, 1, 2, 14, Li),
	('flowID', 24, 1, 0, 1, ),
	('ownerID', 34, 1, 2, 9, ),
	('params', 42, 1, 2, 14, LSkillParam),
	('passBuckEvents', 50, 1, 2, 14, Li),
	('prev', 56, 1, 0, 1, ),
	('skillID', 64, 1, 0, 1, ),
	('skillIdx', 72, 1, 0, 1, ),
	('specificEvents', 82, 1, 2, 15, DbyLi),
	('uiid', 88, 1, 0, 1, ),
	('unitIndex', 96, 1, 0, 1, ),
))
playSoundVO_1 = _typyd.Object('playSoundVO_1', (
	('interval', 13, 1, 5, 7, ),
	('path', 18, 1, 2, 10, ),
))
playSoundVO_2 = _typyd.Object('playSoundVO_2', (
	('path', 10, 1, 2, 10, ),
))
