#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

def setup():
	import os
	os.chdir(os.path.dirname(os.path.dirname(__file__)))
	suffix = 'pyd' if os.name == 'nt' else 'so'
	map(os.remove, [f for f in (
		'test/_typy.pyc',
		'test/_typy.py',
		'typy/_typy.py',
		'typy/_typy.%s' % suffix,
	) if os.path.isfile(f)])

	from typy import pb, MetaObject, Instance, Python, List, Dict, Object, Integer, Boolean, Float, Double, FixedPoint, Bytes, String, Enum

	MetaObject.Objects = {}

	class Corpus(Enum):
		UNIVERSAL = 0, "UNIVERSAL"
		WEB = 1, "WEB"
		IMAGES = 2, "IMAGES"
		LOCAL = 3, "LOCAL"
		NEWS = 4, "NEWS"
		PRODUCTS = 5, "PRODUCTS"
		VIDEO = 6, "VIDEO"
	globals()['Corpus'] = Corpus


	class PyType(object):
		def __init__(self, **kwargs):
			for k, v in kwargs.iteritems():
				setattr(self, k, v)

		@classmethod
		def FromJson(cls, data):
			return cls(**data)

		def Json(self):
			return self.__dict__

		def ByteSize(self):
			return len(repr(self.__dict__))

		def Serialize(self):
			return repr(self.__dict__)

		def Deserialize(self, data):
			kwargs = eval(data)
			for k, v in kwargs.iteritems():
				setattr(self, k, v)
	globals()['PyType'] = PyType


	class Empty(Object): pass
	globals()['Empty'] = Empty

	class Vector2(Object):
		____propertySequence__ = 'x', 'y'
		x = pb.Float(default = 0.0, label = '坐标X')
		y = pb.FixedPoint(precision = 1, floor = -10, default = 0.0, label = '坐标Y')
		b = pb.Bytes()
		s = pb.String()
		e = pb.Enum(Corpus, default = 3, label = 'Corpus')
		p = pb.Python(PyType, label = 'PyType')
		i = pb.Instance(Empty, label = '空')
		t = Instance
	globals()['Vector2'] = Vector2

	class Fighter_Part1(Object):
		pos = pb.Instance(Vector2, label = '坐标')
		isAwake = pb.Boolean(default = False)
		hp = pb.Float(default = 100, label = '血量')
		poss = pb.Dict(Integer, Vector2, label = 'poss')
		posi = pb.Dict(Integer, Float, label = 'posi')
		posl = pb.List(Vector2, label = 'posl')
		pyl = pb.List(Python(PyType), label = 'pyl')
		pyd = pb.Dict(Integer, Python(PyType), label = 'pyd')
		pyv1 = pb.Instance(Integer, Python(PyType), label = 'pyv1')
		pyv2 = pb.Instance(Integer, Python(PyType), label = 'pyv2')
	globals()['Fighter_Part1'] = Fighter_Part1

	class Fighter_Part2(Fighter_Part1):
		fl = pb.List(Float, label = 'fl')
		bl = pb.List(Bytes, label = 'bl')
		sl = pb.List(String, label = 'sl')
		bd = pb.Dict(String, Bytes, label = 'bd')
		sd = pb.Dict(Integer, String, label = 'sd')
		el = pb.List(Corpus, label = 'el')
		ed = pb.Dict(Integer, Corpus, label = 'ed')
		ll = pb.List(List(Float), label = 'll')
	globals()['Fighter_Part2'] = Fighter_Part2

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
	globals()['Fighter'] = Fighter

	class RPG(Object):
		fighter = pb.Instance(Fighter, label = 'fighter')
	globals()['RPG'] = RPG

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
	globals()['Vector3'] = Vector3

	class SkillParam(Object):
		buckID = pb.Bytes
		targetID = pb.Bytes
		origPos = pb.Python(Vector3)
		origRot = pb.Python(Vector3)
		destPos = pb.Python(Vector3)
		destRot = pb.Python(Vector3)
		extraParam = pb.Dict(Bytes, Instance(Integer, Boolean, Float, Bytes))
		targetIDs = pb.List(Bytes)
	globals()['SkillParam'] = SkillParam

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
	globals()['onInitRuntime'] = onInitRuntime

	class playSoundVO_1(Object):
		path = pb.String
		interval = pb.Float
	globals()['playSoundVO_1'] = playSoundVO_1

	class playSoundVO_2(Object):
		path = pb.String
	globals()['playSoundVO_2'] = playSoundVO_2

	from typy import GenerateDescriptor
	GenerateDescriptor(os.path.dirname(__file__))
	import _typy

	_typy.setDefaultEncodingUTF8()
	_typy.Corpus(Corpus.__enum__)
	_typy.PyType(PyType)
	globals()['_Vector2'] = _Vector2 = _typy.Vector2()
	globals()['_Empty'] = _Empty = _typy.Empty()
	globals()['_Fighter'] = _Fighter = _typy.Fighter()

	vPy = Vector2(
		123,  # x
		45.6, # y
		s = u"哈哈哈哈",
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
	vPy2.b += " xxx"
	vPy2.e += 2
	globals()['vPy'] = vPy
	globals()['vPy2'] = vPy2

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
	globals()['fighterPy'] = fighterPy

	v = _Vector2(
		123,  # x
		45.6, # y
		s = u"哈哈哈哈",
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
	v2.b += " xxx"
	v2.e += 2
	globals()['v'] = v
	globals()['v2'] = v2

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
	globals()['fighter'] = fighter

def test_prop():
	global Vector2, Fighter, Corpus, PyType, Empty
	global _Vector2, _Fighter, _Empty
	global vPy, vPy2, fighterPy, v, v2, fighter

	propDict = {}

	def onPropertyChanged(cls, name, old, new):
		if cls.__name__ not in propDict or name not in propDict[cls.__name__]:
			import traceback
			traceback.print_stack()
			print propDict, cls.__name__, name, old, new
			return
		propDict[cls.__name__].remove(name)
		if not propDict[cls.__name__]:
			del propDict[cls.__name__]

	print 'test_prop begin'
	import _typyd
	_typyd.RegisterCallback(onPropertyChanged)
	_typyd.RegisterHandler(_Vector2)
	_typyd.RegisterHandler(_Fighter)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['v3', 'pos', 'posl', 'poss', 'vl'],
	}
	fighter.pos.x = 3.14
	assert not propDict, 'fighter.pos.x = 3.14 %s' % repr(propDict)

	propDict = {'Fighter': ['pos']}
	fighter.pos = v2
	assert not propDict, 'fighter.pos = v2 %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd'],
	}
	fighter.pos.x = 314
	assert not propDict, 'fighter.pos.x = 314 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd'],
	}
	fighter.pos.y = 31.4
	assert not propDict, 'fighter.pos.y = 31.4 %s' % repr(propDict)

	propDict = {'Fighter': ['isAwake']}
	fighter.isAwake = False
	assert not propDict, 'fighter.isAwake = False %s' % repr(propDict)

	propDict = {'Fighter': ['fl']}
	fighter.fl = (456, 7.89, 0.123)
	assert not propDict, 'fighter.fl = (456, 7.89, 0.123) %s' % repr(propDict)

	propDict = {'Fighter': ['fl']}
	fighter.fl[1] = 654
	assert not propDict, 'fighter.fl[1] = 654 %s' % repr(propDict)

	propDict = {'Fighter': ['sl']}
	fighter.sl = [u"吼吼", u"嘿嘿", u"哈哈"]
	assert not propDict, 'fighter.sl = [u"吼吼", u"嘿嘿", u"哈哈"] %s' % repr(propDict)

	propDict = {'Fighter': ['sl']}
	fighter.sl[1] = u"哼哼"
	assert not propDict, 'fighter.sl[1] = u"哼哼" %s' % repr(propDict)

	propDict = {'Fighter': ['bl']}
	fighter.bl = ["bbb 456", "ccc 7.89", "aaa 0.123"]
	assert not propDict, 'fighter.bl = ["bbb 456", "ccc 7.89", "aaa 0.123"] %s' % repr(propDict)

	propDict = {'Fighter': ['bl']}
	fighter.bl[1] = "ddd 7.89"
	assert not propDict, 'fighter.bl[1] = "ddd 7.89" %s' % repr(propDict)

	propDict = {'Fighter': ['v1']}
	fighter.v1 = None
	assert not propDict, 'fighter.v1 = None %s' % repr(propDict)

	propDict = {'Fighter': ['v2']}
	fighter.v2 = "fsda"
	assert not propDict, 'fighter.v2 = "fsda" %s' % repr(propDict)

	propDict = {'Fighter': ['v3']}
	fighter.v3 = v2
	assert not propDict, 'fighter.v3 = v2 %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.v3.x = 3141
	assert not propDict, 'fighter.v3.x = 3141 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.v3.y = 314.1
	assert not propDict, 'fighter.v3.y = 314.1 %s' % repr(propDict)

	propDict = {'Fighter': ['v4']}
	fighter.v4 = 123.345
	assert not propDict, 'fighter.v4 = 123.345 %s' % repr(propDict)

	propDict = {'Fighter': ['poss']}
	fighter.poss = {321: v, 320: None}
	assert not propDict, 'fighter.poss = {321: v, 320: None} %s' % repr(propDict)

	propDict = {'Fighter': ['poss']}
	fighter.poss[322] = v2
	assert not propDict, 'fighter.poss[322] = v2 %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.poss[322].x = 31415
	assert not propDict, 'fighter.poss[322].x = 31415 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.poss[322].y = 3141.5
	assert not propDict, 'fighter.poss[322].y = 3141.5 %s' % repr(propDict)

	propDict = {'Fighter': ['v0']}
	fighter.v0 = 56789
	assert not propDict, 'fighter.v0 = 56789 %s' % repr(propDict)

	propDict = {'Fighter': ['hp']}
	fighter.hp += 12
	assert not propDict, 'fighter.hp += 12 %s' % repr(propDict)

	propDict = {'Fighter': ['poss']}
	fighter.poss[231] = None
	assert not propDict, 'fighter.poss[231] = None %s' % repr(propDict)

	propDict = {'Fighter': ['poss']}
	fighter.poss[230] = v
	assert not propDict, 'fighter.poss[230] = v %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['posl', 'poss', 'vl'],
	}
	fighter.poss[230].x = 314159
	assert not propDict, 'fighter.poss[230].x = 314159 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['posl', 'poss', 'vl'],
	}
	fighter.poss[230].y = 31415.9
	assert not propDict, 'fighter.poss[230].y = 31415.9 %s' % repr(propDict)

	propDict = {'Fighter': ['posi']}
	fighter.posi[123] = 0.456
	assert not propDict, 'fighter.posi[123] = 0.456 %s' % repr(propDict)

	propDict = {'Fighter': ['posl']}
	fighter.posl.append(v)
	assert not propDict, 'fighter.posl.append(v) %s' % repr(propDict)

	propDict = {'Fighter': ['posl']}
	fighter.posl.append(None)
	assert not propDict, 'fighter.posl.append(None) %s' % repr(propDict)

	propDict = {'Fighter': ['posl']}
	fighter.posl.append(v2)
	assert not propDict, 'fighter.posl.append(v2) %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.posl[-1].x = 3141592
	assert not propDict, 'fighter.posl[-1].x = 3141592 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['posl', 'poss', 'vl'],
	}
	fighter.posl[-3].y = 314159.2
	assert not propDict, 'fighter.posl[-3].y = 314159.2 %s' % repr(propDict)

	propDict = {'Fighter': ['bd']}
	fighter.bd[u"哈哈"] = "bbb 456"
	assert not propDict, 'fighter.bd[u"哈哈"] = "bbb 456" %s' % repr(propDict)

	propDict = {'Fighter': ['bd']}
	fighter.bd["asdf"] = "aaa 0.123"
	assert not propDict, 'fighter.bd["asdf"] = "aaa 0.123" %s' % repr(propDict)

	propDict = {'Fighter': ['sd']}
	fighter.sd[321] = u"吼吼 2"
	assert not propDict, 'fighter.sd[321] = u"吼吼 2" %s' % repr(propDict)

	propDict = {'Fighter': ['sd']}
	fighter.sd[231] = u"哈哈 3"
	assert not propDict, 'fighter.sd[231] = u"哈哈 3" %s' % repr(propDict)

	propDict = {'Fighter': ['el']}
	fighter.el.append(Corpus.LOCAL)
	assert not propDict, 'fighter.el.append(Corpus.LOCAL) %s' % repr(propDict)

	propDict = {'Fighter': ['el']}
	fighter.el.append(Corpus.NEWS)
	assert not propDict, 'fighter.el.append(Corpus.NEWS) %s' % repr(propDict)

	propDict = {'Fighter': ['el']}
	fighter.el.append(Corpus.VIDEO)
	assert not propDict, 'fighter.el.append(Corpus.VIDEO) %s' % repr(propDict)

	propDict = {'Fighter': ['ed']}
	fighter.ed = {789: Corpus.WEB, 567: Corpus.IMAGES}
	assert not propDict, 'fighter.ed = {789: Corpus.WEB, 567: Corpus.IMAGES} %s' % repr(propDict)

	propDict = {'Fighter': ['ll']}
	fighter.ll = [[12.3, 1.23], [1.234, 12.34, 123.4]]
	assert not propDict, 'fighter.ll = [[12.3, 1.23], [1.234, 12.34, 123.4]] %s' % repr(propDict)

	propDict = {'Fighter': ['ll']}
	fighter.ll[1][1] = 0.1234
	assert not propDict, 'fighter.ll[1][1] = 0.1234 %s' % repr(propDict)

	propDict = {'Fighter': ['vl']}
	fighter.vl = [123, "adsf", None]
	assert not propDict, 'fighter.vl = [123, "adsf", None] %s' % repr(propDict)

	propDict = {'Fighter': ['vl']}
	fighter.vl[1] = v
	assert not propDict, 'fighter.vl[1] = v %s' % repr(propDict)

	propDict = {'Fighter': ['vl']}
	fighter.vl.append(345.123)
	assert not propDict, 'fighter.vl.append(345.123) %s' % repr(propDict)

	propDict = {'Fighter': ['vl']}
	fighter.vl.append(None)
	assert not propDict, 'fighter.vl.append(None) %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['posl', 'poss', 'vl'],
	}
	fighter.vl[1].x = 31415926
	assert not propDict, 'fighter.vl[1].x = 31415926 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['posl', 'poss', 'vl'],
	}
	fighter.vl[1].y = 3141592.6
	assert not propDict, 'fighter.vl[1].y = 3141592.6 %s' % repr(propDict)

	propDict = {'Fighter': ['vd']}
	fighter.vd[0] = Corpus.IMAGES
	assert not propDict, 'fighter.vd[0] = Corpus.IMAGES %s' % repr(propDict)

	propDict = {'Fighter': ['vd']}
	fighter.vd[12] = "adsf"
	assert not propDict, 'fighter.vd[12] = "adsf" %s' % repr(propDict)

	propDict = {'Fighter': ['vd']}
	fighter.vd[23] = v2
	assert not propDict, 'fighter.vd[23] = v2 %s' % repr(propDict)

	propDict = {'Fighter': ['vd']}
	fighter.vd[34] = 345.123
	assert not propDict, 'fighter.vd[34] = 345.123 %s' % repr(propDict)

	propDict = {'Fighter': ['vd']}
	fighter.vd[45] = None
	assert not propDict, 'fighter.vd[45] = None %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.vd[23].x = 314159265
	assert not propDict, 'fighter.vd[23].x = 314159265 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.vd[23].y = 31415926.5
	assert not propDict, 'fighter.vd[23].y = 31415926.5 %s' % repr(propDict)

	propDict = {'Fighter': ['ld']}
	fighter.ld = {12: [Corpus.IMAGES, "adsf"], 34: [v2, 345.123]}
	assert not propDict, 'fighter.ld = {12: [Corpus.IMAGES, "adsf"], 34: [v2, 345.123]} %s' % repr(propDict)

	propDict = {'Fighter': ['ld']}
	fighter.ld[34][1] = "asdf 111"
	assert not propDict, 'fighter.ld[34][1] = "asdf 111" %s' % repr(propDict)

	propDict = {'Fighter': ['fld']}
	fighter.fld = {123: [222.111, 345.123]}
	assert not propDict, 'fighter.fld = {123: [222.111, 345.123]} %s' % repr(propDict)

	propDict = {'Fighter': ['fld']}
	fighter.fld[123].append(111.222)
	assert not propDict, 'fighter.fld[123].append(111.222) %s' % repr(propDict)

	propDict = {'Fighter': ['pyl']}
	fighter.pyl = [PyType(pp = 123, ap = 'asdf'), None, PyType(pp = 456, ap = 'xxxx')]
	assert not propDict, 'fighter.pyl = [PyType(pp = 123, ap = "asdf"), None, PyType(pp = 456, ap = "xxxx")] %s' % repr(propDict)

	propDict = {'Fighter': ['pyl']}
	fighter.pyl[1] = PyType(pp = 789, ap = 'yyy')
	assert not propDict, 'fighter.pyl[1] = PyType(pp = 789, ap = "yyy") %s' % repr(propDict)

	propDict = {'Fighter': ['pyd']}
	fighter.pyd = {321: PyType(pp = 123, ap = 'asdf'), 654: PyType(pp = 456, ap = 'xxxx'), 320: None}
	assert not propDict, 'fighter.pyd = {321: PyType(pp = 123, ap = "asdf"), 654: PyType(pp = 456, ap = "xxxx"), 320: None} %s' % repr(propDict)

	propDict = {'Fighter': ['pyd']}
	fighter.pyd[654] = None
	assert not propDict, 'fighter.pyd[654] = None %s' % repr(propDict)

	propDict = {'Fighter': ['pyv1']}
	fighter.pyv1 = 123
	assert not propDict, 'fighter.pyv1 = 123 %s' % repr(propDict)

	propDict = {'Fighter': ['pyv2']}
	fighter.pyv2 = PyType(pp = 123, ap = 'asdf')
	assert not propDict, 'fighter.pyv2 = PyType(pp = 123, ap = "asdf") %s' % repr(propDict)

	propDict = {'Fighter': ['dd']}
	fighter.dd = {12: {111: Corpus.IMAGES, 222: "adsf"}, 34: {333: v2, 444: 345.123}}
	assert not propDict, 'fighter.dd = {12: {111: Corpus.IMAGES, 222: "adsf"}, 34: {333: v2, 444: 345.123}} %s' % repr(propDict)

	propDict = {'Fighter': ['dd']}
	fighter.dd[12][111] = v2
	assert not propDict, 'fighter.dd[12][111] = v2 %s' % repr(propDict)

	propDict = {
		'Vector2': ['x'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.dd[12][111].x = 3141592653
	assert not propDict, 'fighter.dd[12][111].x = 3141592653 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'v3'],
	}
	fighter.dd[12][111].y = 314159265.3
	assert not propDict, 'fighter.dd[12][111].y = 314159265.3 %s' % repr(propDict)

	propDict = {'Fighter': ['fdd']}
	fighter.fdd = {123: {12: 222.111, 23: 345.123}}
	assert not propDict, 'fighter.fdd = {123: {12: 222.111, 23: 345.123}} %s' % repr(propDict)

	propDict = {'Fighter': ['fdd']}
	fighter.fdd[123][23] = 123.456
	assert not propDict, 'fighter.fdd[123][23] = 123.456 %s' % repr(propDict)

	propDict = {'Fighter': ['nv']}
	fighter.nv = None
	assert not propDict, 'fighter.nv = None %s' % repr(propDict)

	propDict = {'Fighter': ['nv']}
	fighter.nv = 123456
	assert not propDict, 'fighter.nv = 123456 %s' % repr(propDict)

	propDict = {'Fighter': ['lv']}
	fighter.lv = [123, "adsf"]
	assert not propDict, 'fighter.lv = [123, "adsf"] %s' % repr(propDict)

	propDict = {'Fighter': ['lv']}
	fighter.lv.append(123.456)
	assert not propDict, 'fighter.lv.append(123.456) %s' % repr(propDict)

	propDict = {'Fighter': ['lv']}
	fighter.lv[0] = "xxxx"
	assert not propDict, 'fighter.lv[0] = "xxxx" %s' % repr(propDict)

	propDict = {'Fighter': ['flv']}
	fighter.flv = [222.111, 345.123]
	assert not propDict, 'fighter.flv = [222.111, 345.123] %s' % repr(propDict)

	propDict = {'Fighter': ['flv']}
	fighter.flv[1] = 123.456
	assert not propDict, 'fighter.flv[1] = 123.456 %s' % repr(propDict)

	propDict = {'Fighter': ['dv']}
	fighter.dv = {333: 123, 444: "adsf"}
	assert not propDict, 'fighter.dv = {333: 123, 444: "adsf"} %s' % repr(propDict)

	propDict = {'Fighter': ['dv']}
	fighter.dv[555] = 123.456
	assert not propDict, 'fighter.dv[555] = 123.456 %s' % repr(propDict)

	propDict = {'Fighter': ['dv']}
	del fighter.dv[333]
	assert not propDict, 'del fighter.dv[333] %s' % repr(propDict)

	propDict = {'Fighter': ['fdv']}
	fighter.fdv = {333: 222.111, 444: 345.123}
	assert not propDict, 'fighter.fdv = {333: 222.111, 444: 345.123} %s' % repr(propDict)

	propDict = {'Fighter': ['fdv']}
	fighter.fdv[333] = 111.222
	assert not propDict, 'fighter.fdv[333] = 111.222 %s' % repr(propDict)


	_typyd.UnregisterHandler(_Vector2)
	_typyd.UnregisterHandler(_Fighter)
	print 'test_prop end'
