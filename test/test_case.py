#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

from typy import pb, MetaObject, Instance, Long, Python, List, Dict, Object, Integer, Boolean, Float, Double, FixedPoint, Bytes, String, Enum

MetaObject.Objects = {}

class Corpus(Enum):
	UNIVERSAL = 0, "UNIVERSAL"
	WEB = 1, "WEB"
	IMAGES = 2, "IMAGES"
	LOCAL = 3, "LOCAL"
	NEWS = 4, "NEWS"
	PRODUCTS = 5, "PRODUCTS"
	VIDEO = 6, "VIDEO"


class PyType(object):
	def __init__(self, **kwargs):
		for k, v in kwargs.iteritems():
			setattr(self, k, v)

	@classmethod
	def FromJson(cls, data):
		return cls(**data)

	def Json(self):
		from copy import deepcopy
		return deepcopy(self.__dict__)

	def ByteSize(self):
		return len(repr(self.__dict__))

	def Serialize(self):
		return repr(self.__dict__)

	def Deserialize(self, data):
		kwargs = eval(data)
		for k, v in kwargs.iteritems():
			setattr(self, k, v)


class Empty(Object): pass

class Vector2(Object):
	____propertySequence__ = 'x', 'y'
	x = pb.Float(default = 0.0, label = '坐标X')
	y = pb.FixedPoint(precision = 1, floor = -10, default = 0.0, label = '坐标Y')
	b = pb.Bytes()
	s = pb.String()
	m = pb.Symbol()
	e = pb.Enum(Corpus, default = 3, label = 'Corpus')
	p = pb.Python(PyType, label = 'PyType')
	i = pb.Instance(Empty, label = '空')
	t = Instance

	classProp = set(['asdf', 1234])

	@property
	def TestProperty(self):
		print 'TestProperty', self

	@classmethod
	def TestClassMethod(cls, aaa = 123):
		print 'TestClassMethod', cls, aaa

	def TestMethod(self, aaa = 123):
		print 'TestMethod', aaa

	def __str__(self):
		return str({
			'x': self.x,
			'y': self.y,
			'b': self.b,
			's': self.s,
			'm': self.m,
			'e': self.e,
			'p': self.p,
		})

	def __lt__(self, other):
		return self.x < other.x

	def __cmp__(self, other):
		return self.x - other.x

	def __hash__(self):
		return hash(self.x)

	def __add__(self, other):
		return Vector2(x = self.x + other.x, y = self.y + other.y)

class Fighter_Part1(Object):
	pos = pb.Instance(Vector2, label = '坐标')
	isAwake = pb.Boolean(default = False)
	hp = pb.Float(default = 100, label = '血量')
	poss = pb.Dict(Long, Vector2, label = 'poss')
	posi = pb.Dict(Integer, Float, label = 'posi')
	posl = pb.List(Vector2, label = 'posl')
	posll = pb.List(List(Vector2), label = 'posll')
	pyl = pb.List(Python(PyType), label = 'pyl')
	pyd = pb.Dict(Integer, Python(PyType), label = 'pyd')
	pyv1 = pb.Instance(Integer, Python(PyType), label = 'pyv1')
	pyv2 = pb.Instance(Integer, Python(PyType), label = 'pyv2')

class Fighter_Part2(Fighter_Part1):
	fl = pb.List(Float, label = 'fl')
	bl = pb.List(Bytes, label = 'bl')
	sl = pb.List(String, label = 'sl')
	bd = pb.Dict(String, Bytes, label = 'bd')
	sd = pb.Dict(Integer, String, label = 'sd')
	el = pb.List(Corpus, label = 'el')
	ed = pb.Dict(Long, Corpus, label = 'ed')
	ll = pb.List(List(Float), label = 'll')

class Fighter(Fighter_Part2):
	v0 = pb.Instance(Integer, Float, Bytes, Vector2, label = 'v0')
	v1 = pb.Instance(Integer, Float, Bytes, Vector2, label = 'v1')
	v2 = pb.Instance(Integer, Float, String, Vector2, label = 'v2')
	v3 = pb.Instance(Integer, Float, String, Vector2, label = 'v3')
	v4 = pb.Instance(Integer, Float, String, Vector2, label = 'v4')
	vl = pb.List(Integer, FixedPoint(3), String, Vector2, label = 'vl')
	vd = pb.Dict(Integer, (Corpus, Double, String, Vector2), label = 'vd')
	ld = pb.Dict(Integer, List(Corpus, Double, String, Vector2), label = 'ld')
	fld = pb.Dict(Integer, List(Float), label = 'fld')
	dd = pb.Dict(Integer, Dict(Integer, Corpus, Double, String, Vector2), label = 'dd')
	fdd = pb.Dict(Integer, Dict(Integer, Float), label = 'fdd')
	nv = pb.Instance(None, Integer, label = 'nv')
	lv = pb.Instance(Integer, List(Float, String), label = 'lv')
	flv = pb.Instance(Integer, List(Float), label = 'flv')
	dv = pb.Instance(Integer, Dict(Integer, Float, String), label = 'dv')
	fdv = pb.Instance(Integer, Dict(Integer, Float), label = 'fdv')

class RPG(Object):
	fighter = pb.Instance(Fighter, label = 'fighter')

import struct

class Vector3(object):
	def __init__(self, **kwargs):
		self.x = self.y = self.z = 0
		for k, v in kwargs.iteritems():
			setattr(self, k, v)

	def __str__(self):
		return str(self.__dict__)

	@classmethod
	def FromJson(cls, data):
		return cls(**data)

	def Json(self):
		return self.__dict__

	def ByteSize(self):
		return 12

	def Serialize(self):
		return struct.pack('fff', self.x, self.y, self.z)

	def Deserialize(self, data):
		self.x, self.y, self.z = struct.unpack('fff', data)

class SkillParam(Object):
	buckID = pb.Bytes
	targetID = pb.Bytes
	origPos = pb.Python(Vector3)
	origRot = pb.Python(Vector3)
	destPos = pb.Python(Vector3)
	destRot = pb.Python(Vector3)
	extraParam = pb.Dict(Bytes, Instance(Integer, Boolean, Float, Bytes))
	targetIDs = pb.List(Bytes)

class onInitRuntime(Object):
	skillID = pb.Integer
	ownerID = pb.Bytes
	skillIdx = pb.Integer
	flowID = pb.Integer
	unitIndex = pb.Integer
	uiid = pb.Integer
	prev = pb.Integer
	buckID = pb.Bytes
	params = pb.List(SkillParam)
	events = pb.List(Integer)
	specificEvents = pb.Dict(Bytes, List(Integer))
	passBuckEvents = pb.List(Integer)

class playSoundVO_1(Object):
	path = pb.String
	interval = pb.Float

class playSoundVO_2(Object):
	path = pb.String

class BooleanList(Object):
	bl = pb.List(Boolean)

def setup(g, _typy):
	g['Corpus'] = Corpus
	g['PyType'] = PyType
	g['Empty'] = Empty
	g['Vector2'] = Vector2
	g['Fighter_Part1'] = Fighter_Part1
	g['Fighter_Part2'] = Fighter_Part2
	g['Fighter'] = Fighter
	g['RPG'] = RPG
	g['Vector3'] = Vector3
	g['SkillParam'] = SkillParam
	g['onInitRuntime'] = onInitRuntime
	g['playSoundVO_1'] = playSoundVO_1
	g['playSoundVO_2'] = playSoundVO_2
	g['BooleanList'] = BooleanList

	_typy.setDefaultEncodingUTF8()
	_typy.Corpus(Corpus.__enum__)
	_typy.PyType(PyType)

	@property
	def TestProperty(self):
		print 'TestProperty', self

	@classmethod
	def TestClassMethod(cls, aaa = 123):
		print 'TestClassMethod', cls, cls(), aaa

	def TestMethod(self, aaa = 123):
		print 'TestMethod', aaa

	_Vector2 = _typy.Vector2({
		'classProp': set(['asdf', 1234]),
		'TestProperty': TestProperty,
		'TestClassMethod': TestClassMethod,
		'TestMethod': TestMethod,
		'TestLambda': lambda self: self.s,
		'__str__': lambda self: str({
			'x': self.x,
			'y': self.y,
			'b': self.b,
			's': self.s,
			'm': self.m,
			'e': self.e,
			'p': self.p,
		}),
		'__hash__': lambda self: hash(self.x),
		'__cmp__': lambda self, other: self.x - other.x,
		'__lt__': lambda self, other: self.x < other.x,
		'__add__': lambda self, other: self.__class__(x = self.x + other.x, y = self.y + other.y),
	})

	g['_Vector2'] = _Vector2
	g['_Empty'] = _Empty = _typy.Empty()
	g['_Fighter'] = _Fighter = _typy.Fighter()

	vPy = Vector2(
		123,  # x
		45.6, # y
		s = u"哈哈哈哈",
		m = "Hub",
		b = u"asdf 1234",
		e = Corpus.LOCAL,
		p = PyType(pp = 123, ap = 'asdf'),
		i = Empty(),
	)
	vPy2 = Vector2()
	vPy2.ParseFromString(vPy.SerializeToString())
	vPy2.x += 1000
	vPy2.y += 1000
	vPy2.s += u" 吼吼吼"
	vPy2.m += "Ga"
	vPy2.b += " xxx"
	vPy2.e += 2
	g['vPy'] = vPy
	g['vPy2'] = vPy2

	fighterPy = Fighter(
		pos = vPy,
		isAwake = True,
		fl = (0.123, 456, 7.89),
		sl = [u"哈哈", u"吼吼", u"嘿嘿"],
		bl = ["aaa 0.123", "bbb 456", "ccc 7.89"],
		v1 = 98765,
		v2 = "adsf",
		v3 = vPy,
		v4 = 345.123,
		poss = {321: vPy, 320: None},
	)
	fighterPy.v0 = None
	fighterPy.hp += 12
	fighterPy.poss[231] = vPy2
	fighterPy.poss[230] = None
	fighterPy.posi[123] = 0.456
	fighterPy.posl.append(vPy)
	fighterPy.posl.append(None)
	fighterPy.posl.append(vPy2)
	fighterPy.bd[u"哈哈"] = "aaa 0.123"
	fighterPy.bd["asdf"] = "bbb 456"
	fighterPy.sd[321] = u"哈哈 3"
	fighterPy.sd[231] = u"吼吼 2"
	fighterPy.el.append(Corpus.LOCAL)
	fighterPy.el.append(Corpus.NEWS)
	fighterPy.el.append(Corpus.VIDEO)
	fighterPy.ed = {789: Corpus.WEB, 567: Corpus.IMAGES}
	fighterPy.ll = [[12.3, 1.23], [1.234, 12.34, 123.4]]
	fighterPy.vl = [123, "adsf", None]
	fighterPy.vl.append(vPy)
	fighterPy.vl.append(345.123)
	fighterPy.vl.append(None)
	fighterPy.vd[0] = None
	fighterPy.vd[12] = Corpus.IMAGES
	fighterPy.vd[23] = "adsf"
	fighterPy.vd[34] = vPy2
	fighterPy.vd[45] = 345.123
	fighterPy.ld = {12: [Corpus.IMAGES, "adsf"], 34: [vPy2, 345.123]}
	fighterPy.fld = {123: [222.111, 345.123]}
	fighterPy.pyl = [PyType(pp = 123, ap = 'asdf'), None, PyType(pp = 456, ap = 'xxxx')]
	fighterPy.pyd = {321: PyType(pp = 123, ap = 'asdf'), 654: PyType(pp = 456, ap = 'xxxx'), 320: None}
	fighterPy.pyd[650] = None
	fighterPy.pyv1 = 123
	fighterPy.pyv2 = PyType(pp = 123, ap = 'asdf')
	fighterPy.dd = {12: {111: Corpus.IMAGES, 222: "adsf"}, 34: {333: vPy2, 444: 345.123}}
	fighterPy.fdd = {123: {12: 222.111, 23: 345.123}}
	fighterPy.nv = 123456
	fighterPy.lv = [123, "adsf"]
	fighterPy.flv = [222.111, 345.123]
	fighterPy.dv = {333: 123, 444: "adsf"}
	fighterPy.fdv = {333: 222.111, 444: 345.123}
	g['fighterPy'] = fighterPy

	v = _Vector2(
		123,  # x
		45.6, # y
		s = u"哈哈哈哈",
		m = "Hub",
		b = u"asdf 1234",
		e = Corpus.LOCAL,
		p = PyType(pp = 123, ap = 'asdf'),
		i = _Empty(),
	)
	v2 = _Vector2()
	v2.ParseFromString(v.SerializeToString())
	v2.x += 1000
	v2.y += 1000
	v2.s += u" 吼吼吼"
	v2.m += "Ga"
	v2.b += " xxx"
	v2.e += 2
	g['v'] = v
	g['v2'] = v2

	fighter = _Fighter(
		pos = v,
		isAwake = True,
		fl = (0.123, 456, 7.89),
		sl = [u"哈哈", u"吼吼", u"嘿嘿"],
		bl = ["aaa 0.123", "bbb 456", "ccc 7.89"],
		v1 = 98765,
		v2 = "adsf",
		v3 = v,
		v4 = 345.123,
		poss = {321: v, 320: None},
	)
	fighter.v0 = None
	fighter.hp += 12
	fighter.poss[231] = v2
	fighter.poss[230] = None
	fighter.posi[123] = 0.456
	fighter.posl.append(v)
	fighter.posl.append(None)
	fighter.posl.append(v2)
	fighter.bd[u"哈哈"] = "aaa 0.123"
	fighter.bd["asdf"] = "bbb 456"
	fighter.sd[321] = u"哈哈 3"
	fighter.sd[231] = u"吼吼 2"
	fighter.el.append(Corpus.LOCAL)
	fighter.el.append(Corpus.NEWS)
	fighter.el.append(Corpus.VIDEO)
	fighter.ed = {789: Corpus.WEB, 567: Corpus.IMAGES}
	fighter.ll = [[12.3, 1.23], [1.234, 12.34, 123.4]]
	fighter.vl = [123, "adsf", None]
	fighter.vl.append(v)
	fighter.vl.append(345.123)
	fighter.vl.append(None)
	fighter.vd[0] = None
	fighter.vd[12] = Corpus.IMAGES
	fighter.vd[23] = "adsf"
	fighter.vd[34] = v2
	fighter.vd[45] = 345.123
	fighter.ld = {12: [Corpus.IMAGES, "adsf"], 34: [v2, 345.123]}
	fighter.fld = {123: [222.111, 345.123]}
	fighter.pyl = [PyType(pp = 123, ap = 'asdf'), None, PyType(pp = 456, ap = 'xxxx')]
	fighter.pyd = {321: PyType(pp = 123, ap = 'asdf'), 654: PyType(pp = 456, ap = 'xxxx'), 320: None}
	fighter.pyd[650] = None
	fighter.pyv1 = 123
	fighter.pyv2 = PyType(pp = 123, ap = 'asdf')
	fighter.dd = {12: {111: Corpus.IMAGES, 222: "adsf"}, 34: {333: v2, 444: 345.123}}
	fighter.fdd = {123: {12: 222.111, 23: 345.123}}
	fighter.nv = 123456
	fighter.lv = [123, "adsf"]
	fighter.flv = [222.111, 345.123]
	fighter.dv = {333: 123, 444: "adsf"}
	fighter.fdv = {333: 222.111, 444: 345.123}
	g['fighter'] = fighter
