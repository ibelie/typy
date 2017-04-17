#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

def setup():
	import os
	import sys
	import shutil
	from distutils.util import get_platform
	os.chdir('..')
	suffix = 'pyd' if os.name == 'nt' else 'so'
	map(os.remove, [f for f in (
		'typy/_typy.py',
		'typy/_typy.pyc',
		'test/_typy.py',
		'test/_typy.pyc',
		'test/_typyd.%s' % suffix,
		'typy/_typyd.%s' % suffix,
		'typy/_typy.%s' % suffix,
	) if os.path.isfile(f)])
	os.system('python -B setup.py build')
	typydFile = 'build/lib.%s-%s/typy/_typyd.%s' % (get_platform(), sys.version[0:3], suffix)
	os.path.isfile(typydFile) and shutil.copy(typydFile, "test/")

	from typy import pb, Instance, Python, List, Dict, Object, Integer, Boolean, Float, Double, FixedPoint, Bytes, String, Enum

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

		def __str__(self):
			return str(self.__dict__)

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
				'e': self.e,
				'p': self.p,
			})

		def __lt__(self, other):
			return self.x < other.x

		def __cmp__(self, other):
			return self.x - other.x

		def __add__(self, other):
			return Vector2(x = self.x + other.x, y = self.y + other.y)
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


def _printFighters(fighter1, fighter2, fighter3, fighter4, fighter5):
	print "hp\t", "\t", fighter1.hp, "\t", fighter2.hp, "\t", fighter3.hp, "\t", fighter4.hp, "\t", fighter5.hp
	print "isAwake\t", "\t", fighter1.isAwake, "\t", fighter2.isAwake, "\t", fighter3.isAwake, "\t", fighter4.isAwake, "\t", fighter5.isAwake
	print "pos.x\t", "\t", fighter1.pos.x, "\t", fighter2.pos.x, "\t", fighter3.pos.x, "\t", fighter4.pos.x, "\t", fighter5.pos.x
	print "pos.y\t", "\t", fighter1.pos.y, "\t", fighter2.pos.y, "\t", fighter3.pos.y, "\t", fighter4.pos.y, "\t", fighter5.pos.y
	print "pos.s\t", "\t", fighter1.pos.s, " ", fighter2.pos.s, " ", fighter3.pos.s, " ", fighter4.pos.s, " ", fighter5.pos.s
	print "pos.b\t", "\t", fighter1.pos.b, " ", fighter2.pos.b, " ", fighter3.pos.b, " ", fighter4.pos.b, " ", fighter5.pos.b
	print "pos.e\t", "\t", fighter1.pos.e, " ", fighter2.pos.e, " ", fighter3.pos.e, " ", fighter4.pos.e, " ", fighter5.pos.e
	print "pos.p\t", "\t", fighter1.pos.p, " ", fighter2.pos.p, " ", fighter3.pos.p, " ", fighter4.pos.p, " ", fighter5.pos.p
	print "posi[123]\t", fighter1.posi[123], "\t", fighter2.posi[123], "\t", fighter3.posi[123], "\t", fighter4.posi[123], "\t", fighter5.posi[123]
	print "fl[0]\t", "\t", fighter1.fl[0], "\t", fighter2.fl[0], "\t", fighter3.fl[0], "\t", fighter4.fl[0], "\t", fighter5.fl[0]
	print "fl[1]\t", "\t", fighter1.fl[1], "\t", fighter2.fl[1], "\t", fighter3.fl[1], "\t", fighter4.fl[1], "\t", fighter5.fl[1]
	print "fl[2]\t", "\t", fighter1.fl[2], "\t", fighter2.fl[2], "\t", fighter3.fl[2], "\t", fighter4.fl[2], "\t", fighter5.fl[2]
	print "bl[0]\t", "\t", fighter1.bl[0], "\t", fighter2.bl[0], "\t", fighter3.bl[0], "\t", fighter4.bl[0], "\t", fighter5.bl[0]
	print "bl[1]\t", "\t", fighter1.bl[1], "\t", fighter2.bl[1], "\t", fighter3.bl[1], "\t", fighter4.bl[1], "\t", fighter5.bl[1]
	print "bl[2]\t", "\t", fighter1.bl[2], "\t", fighter2.bl[2], "\t", fighter3.bl[2], "\t", fighter4.bl[2], "\t", fighter5.bl[2]
	print "sl[0]\t", "\t", fighter1.sl[0], "\t", fighter2.sl[0], "\t", fighter3.sl[0], "\t", fighter4.sl[0], "\t", fighter5.sl[0]
	print "sl[1]\t", "\t", fighter1.sl[1], "\t", fighter2.sl[1], "\t", fighter3.sl[1], "\t", fighter4.sl[1], "\t", fighter5.sl[1]
	print "sl[2]\t", "\t", fighter1.sl[2], "\t", fighter2.sl[2], "\t", fighter3.sl[2], "\t", fighter4.sl[2], "\t", fighter5.sl[2]
	print "el[0]\t", "\t", fighter1.el[0], "\t", fighter2.el[0], "\t", fighter3.el[0], "\t", fighter4.el[0], "\t", fighter5.el[0]
	print "el[1]\t", "\t", fighter1.el[1], "\t", fighter2.el[1], "\t", fighter3.el[1], "\t", fighter4.el[1], "\t", fighter5.el[1]
	print "el[2]\t", "\t", fighter1.el[2], "\t", fighter2.el[2], "\t", fighter3.el[2], "\t", fighter4.el[2], "\t", fighter5.el[2]
	print "posl[0].x", "\t", fighter1.posl[0].x, "\t", fighter2.posl[0].x, "\t", fighter3.posl[0].x, "\t", fighter4.posl[0].x, "\t", fighter5.posl[0].x
	print "posl[0].y", "\t", fighter1.posl[0].y, "\t", fighter2.posl[0].y, "\t", fighter3.posl[0].y, "\t", fighter4.posl[0].y, "\t", fighter5.posl[0].y
	print "posl[0].s\t", fighter1.posl[0].s, " ", fighter2.posl[0].s, " ", fighter3.posl[0].s, " ", fighter4.posl[0].s, " ", fighter5.posl[0].s
	print "posl[0].b\t", fighter1.posl[0].b, " ", fighter2.posl[0].b, " ", fighter3.posl[0].b, " ", fighter4.posl[0].b, " ", fighter5.posl[0].b
	print "posl[0].e\t", fighter1.posl[0].e, " ", fighter2.posl[0].e, " ", fighter3.posl[0].e, " ", fighter4.posl[0].e, " ", fighter5.posl[0].e
	print "posl[1].x", "\t", fighter1.posl[1].x, "\t", fighter2.posl[1].x, "\t", fighter3.posl[1].x, "\t", fighter4.posl[1].x, "\t", fighter5.posl[1].x
	print "posl[1].y", "\t", fighter1.posl[1].y, "\t", fighter2.posl[1].y, "\t", fighter3.posl[1].y, "\t", fighter4.posl[1].y, "\t", fighter5.posl[1].y
	print "posl[1].s\t", fighter1.posl[1].s, " ", fighter2.posl[1].s, " ", fighter3.posl[1].s, " ", fighter4.posl[1].s, " ", fighter5.posl[1].s
	print "posl[1].b\t", fighter1.posl[1].b, " ", fighter2.posl[1].b, " ", fighter3.posl[1].b, " ", fighter4.posl[1].b, " ", fighter5.posl[1].b
	print "posl[1].e\t", fighter1.posl[1].e, " ", fighter2.posl[1].e, " ", fighter3.posl[1].e, " ", fighter4.posl[1].e, " ", fighter5.posl[1].e
	print "poss[321].x", "\t", fighter1.poss[321].x, "\t", fighter2.poss[321].x, "\t", fighter3.poss[321].x, "\t", fighter4.poss[321].x, "\t", fighter5.poss[321].x
	print "poss[321].y", "\t", fighter1.poss[321].y, "\t", fighter2.poss[321].y, "\t", fighter3.poss[321].y, "\t", fighter4.poss[321].y, "\t", fighter5.poss[321].y
	print "poss[321].s\t", fighter1.poss[321].s, " ", fighter2.poss[321].s, " ", fighter3.poss[321].s, " ", fighter4.poss[321].s, " ", fighter5.poss[321].s
	print "poss[321].b\t", fighter1.poss[321].b, " ", fighter2.poss[321].b, " ", fighter3.poss[321].b, " ", fighter4.poss[321].b, " ", fighter5.poss[321].b
	print "poss[321].e\t", fighter1.poss[321].e, " ", fighter2.poss[321].e, " ", fighter3.poss[321].e, " ", fighter4.poss[321].e, " ", fighter5.poss[321].e
	print "poss[231].x", "\t", fighter1.poss[231].x, "\t", fighter2.poss[231].x, "\t", fighter3.poss[231].x, "\t", fighter4.poss[231].x, "\t", fighter5.poss[231].x
	print "poss[231].y", "\t", fighter1.poss[231].y, "\t", fighter2.poss[231].y, "\t", fighter3.poss[231].y, "\t", fighter4.poss[231].y, "\t", fighter5.poss[231].y
	print "poss[231].s\t", fighter1.poss[231].s, " ", fighter2.poss[231].s, " ", fighter3.poss[231].s, " ", fighter4.poss[231].s, " ", fighter5.poss[231].s
	print "poss[231].b\t", fighter1.poss[231].b, " ", fighter2.poss[231].b, " ", fighter3.poss[231].b, " ", fighter4.poss[231].b, " ", fighter5.poss[231].b
	print "poss[231].e\t", fighter1.poss[231].e, " ", fighter2.poss[231].e, " ", fighter3.poss[231].e, " ", fighter4.poss[231].e, " ", fighter5.poss[231].e
	print u"bd[u'哈哈']\t", fighter1.bd[u'哈哈'], "\t", fighter2.bd[u'哈哈'], "\t", fighter3.bd[u'哈哈'], "\t", fighter4.bd[u'哈哈'], "\t", fighter5.bd[u'哈哈']
	print "bd['asdf']\t", fighter1.bd['asdf'], "\t", fighter2.bd['asdf'], "\t", fighter3.bd['asdf'], "\t", fighter4.bd['asdf'], "\t", fighter5.bd['asdf']
	print "sd[321]\t", "\t", fighter1.sd[321], "\t", fighter2.sd[321], "\t", fighter3.sd[321], "\t", fighter4.sd[321], "\t", fighter5.sd[321]
	print "sd[231]\t", "\t", fighter1.sd[231], "\t", fighter2.sd[231], "\t", fighter3.sd[231], "\t", fighter4.sd[231], "\t", fighter5.sd[231]
	print "ed[789]\t", "\t", fighter1.ed[789], "\t", fighter2.ed[789], "\t", fighter3.ed[789], "\t", fighter4.ed[789], "\t", fighter5.ed[789]
	print "ed[567]\t", "\t", fighter1.ed[567], "\t", fighter2.ed[567], "\t", fighter3.ed[567], "\t", fighter4.ed[567], "\t", fighter5.ed[567]
	print "ll[0][0]\t", fighter1.ll[0][0], "\t", fighter2.ll[0][0], "\t", fighter3.ll[0][0], "\t", fighter4.ll[0][0], "\t", fighter5.ll[0][0]
	print "ll[0][1]\t", fighter1.ll[0][1], "\t", fighter2.ll[0][1], "\t", fighter3.ll[0][1], "\t", fighter4.ll[0][1], "\t", fighter5.ll[0][1]
	print "ll[1][0]\t", fighter1.ll[1][0], "\t", fighter2.ll[1][0], "\t", fighter3.ll[1][0], "\t", fighter4.ll[1][0], "\t", fighter5.ll[1][0]
	print "ll[1][1]\t", fighter1.ll[1][1], "\t", fighter2.ll[1][1], "\t", fighter3.ll[1][1], "\t", fighter4.ll[1][1], "\t", fighter5.ll[1][1]
	print "ll[1][2]\t", fighter1.ll[1][2], "\t", fighter2.ll[1][2], "\t", fighter3.ll[1][2], "\t", fighter4.ll[1][2], "\t", fighter5.ll[1][2]
	print "v1\t", "\t", fighter1.v1, " ", fighter2.v1, " ", fighter3.v1, " ", fighter4.v1, " ", fighter5.v1
	print "v2\t", "\t", fighter1.v2, " ", fighter2.v2, " ", fighter3.v2, " ", fighter4.v2, " ", fighter5.v2
	print "v3.x\t", "\t", fighter1.v3.x, "\t", fighter2.v3.x, "\t", fighter3.v3.x, "\t", fighter4.v3.x, "\t", fighter5.v3.x
	print "v3.y\t", "\t", fighter1.v3.y, "\t", fighter2.v3.y, "\t", fighter3.v3.y, "\t", fighter4.v3.y, "\t", fighter5.v3.y
	print "v3.s\t", "\t", fighter1.v3.s, " ", fighter2.v3.s, " ", fighter3.v3.s, " ", fighter4.v3.s, " ", fighter5.v3.s
	print "v3.b\t", "\t", fighter1.v3.b, " ", fighter2.v3.b, " ", fighter3.v3.b, " ", fighter4.v3.b, " ", fighter5.v3.b
	print "v3.e\t", "\t", fighter1.v3.e, " ", fighter2.v3.e, " ", fighter3.v3.e, " ", fighter4.v3.e, " ", fighter5.v3.e
	print "v4\t", "\t", fighter1.v4, " ", fighter2.v4, " ", fighter3.v4, " ", fighter4.v4, " ", fighter5.v4
	print "vl[0]\t", "\t", fighter1.vl[0], " ", fighter2.vl[0], " ", fighter3.vl[0], " ", fighter4.vl[0], " ", fighter5.vl[0]
	print "vl[1]\t", "\t", fighter1.vl[1], " ", fighter2.vl[1], " ", fighter3.vl[1], " ", fighter4.vl[1], " ", fighter5.vl[1]
	print "vl[2].x\t", "\t", fighter1.vl[2].x, "\t", fighter2.vl[2].x, "\t", fighter3.vl[2].x, "\t", fighter4.vl[2].x, "\t", fighter5.vl[2].x
	print "vl[2].y\t", "\t", fighter1.vl[2].y, "\t", fighter2.vl[2].y, "\t", fighter3.vl[2].y, "\t", fighter4.vl[2].y, "\t", fighter5.vl[2].y
	print "vl[2].s\t", "\t", fighter1.vl[2].s, " ", fighter2.vl[2].s, " ", fighter3.vl[2].s, " ", fighter4.vl[2].s, " ", fighter5.vl[2].s
	print "vl[2].b\t", "\t", fighter1.vl[2].b, " ", fighter2.vl[2].b, " ", fighter3.vl[2].b, " ", fighter4.vl[2].b, " ", fighter5.vl[2].b
	print "vl[2].e\t", "\t", fighter1.vl[2].e, " ", fighter2.vl[2].e, " ", fighter3.vl[2].e, " ", fighter4.vl[2].e, " ", fighter5.vl[2].e
	print "vl[3]\t", "\t", fighter1.vl[3], " ", fighter2.vl[3], " ", fighter3.vl[3], " ", fighter4.vl[3], " ", fighter5.vl[3]
	print "vd[12]\t", "\t", fighter1.vd[12], " ", fighter2.vd[12], " ", fighter3.vd[12], " ", fighter4.vd[12], " ", fighter5.vd[12]
	print "vd[23]\t", "\t", fighter1.vd[23], " ", fighter2.vd[23], " ", fighter3.vd[23], " ", fighter4.vd[23], " ", fighter5.vd[23]
	print "vd[34].x\t", fighter1.vd[34].x, "\t", fighter2.vd[34].x, "\t", fighter3.vd[34].x, "\t", fighter4.vd[34].x, "\t", fighter5.vd[34].x
	print "vd[34].y\t", fighter1.vd[34].y, "\t", fighter2.vd[34].y, "\t", fighter3.vd[34].y, "\t", fighter4.vd[34].y, "\t", fighter5.vd[34].y
	print "vd[34].s\t", fighter1.vd[34].s, " ", fighter2.vd[34].s, " ", fighter3.vd[34].s, " ", fighter4.vd[34].s, " ", fighter5.vd[34].s
	print "vd[34].b\t", fighter1.vd[34].b, " ", fighter2.vd[34].b, " ", fighter3.vd[34].b, " ", fighter4.vd[34].b, " ", fighter5.vd[34].b
	print "vd[34].e\t", fighter1.vd[34].e, " ", fighter2.vd[34].e, " ", fighter3.vd[34].e, " ", fighter4.vd[34].e, " ", fighter5.vd[34].e
	print "vd[45]\t", "\t", fighter1.vd[45], " ", fighter2.vd[45], " ", fighter3.vd[45], " ", fighter4.vd[45], " ", fighter5.vd[45]
	print "ld[12][0]\t", fighter1.ld[12][0], " ", fighter2.ld[12][0], " ", fighter3.ld[12][0], " ", fighter4.ld[12][0], " ", fighter5.ld[12][0]
	print "ld[12][1]\t", fighter1.ld[12][1], " ", fighter2.ld[12][1], " ", fighter3.ld[12][1], " ", fighter4.ld[12][1], " ", fighter5.ld[12][1]
	print "ld[34][0].x\t", fighter1.ld[34][0].x, "\t", fighter2.ld[34][0].x, "\t", fighter3.ld[34][0].x, "\t", fighter4.ld[34][0].x, "\t", fighter5.ld[34][0].x
	print "ld[34][0].y\t", fighter1.ld[34][0].y, "\t", fighter2.ld[34][0].y, "\t", fighter3.ld[34][0].y, "\t", fighter4.ld[34][0].y, "\t", fighter5.ld[34][0].y
	print "ld[34][0].s\t", fighter1.ld[34][0].s, " ", fighter2.ld[34][0].s, " ", fighter3.ld[34][0].s, " ", fighter4.ld[34][0].s, " ", fighter5.ld[34][0].s
	print "ld[34][0].b\t", fighter1.ld[34][0].b, " ", fighter2.ld[34][0].b, " ", fighter3.ld[34][0].b, " ", fighter4.ld[34][0].b, " ", fighter5.ld[34][0].b
	print "ld[34][0].e\t", fighter1.ld[34][0].e, " ", fighter2.ld[34][0].e, " ", fighter3.ld[34][0].e, " ", fighter4.ld[34][0].e, " ", fighter5.ld[34][0].e
	print "ld[34][1]\t", fighter1.ld[34][1], " ", fighter2.ld[34][1], " ", fighter3.ld[34][1], " ", fighter4.ld[34][1], " ", fighter5.ld[34][1]
	print "fld[123][0]\t", fighter1.fld[123][0], " ", fighter2.fld[123][0], " ", fighter3.fld[123][0], " ", fighter4.fld[123][0], " ", fighter5.fld[123][0]
	print "fld[123][1]\t", fighter1.fld[123][1], " ", fighter2.fld[123][1], " ", fighter3.fld[123][1], " ", fighter4.fld[123][1], " ", fighter5.fld[123][1]
	print "pyl[0].pp\t", fighter1.pyl[0].pp, "\t", fighter2.pyl[0].pp, "\t", fighter3.pyl[0].pp, "\t", fighter4.pyl[0].pp, "\t", fighter5.pyl[0].pp
	print "pyl[0].ap\t", fighter1.pyl[0].ap, "\t", fighter2.pyl[0].ap, "\t", fighter3.pyl[0].ap, "\t", fighter4.pyl[0].ap, "\t", fighter5.pyl[0].ap
	print "pyl[1].pp\t", fighter1.pyl[1].pp, "\t", fighter2.pyl[1].pp, "\t", fighter3.pyl[1].pp, "\t", fighter4.pyl[1].pp, "\t", fighter5.pyl[1].pp
	print "pyl[1].ap\t", fighter1.pyl[1].ap, "\t", fighter2.pyl[1].ap, "\t", fighter3.pyl[1].ap, "\t", fighter4.pyl[1].ap, "\t", fighter5.pyl[1].ap
	print "pyd[321].pp\t", fighter1.pyd[321].pp, "\t", fighter2.pyd[321].pp, "\t", fighter3.pyd[321].pp, "\t", fighter4.pyd[321].pp, "\t", fighter5.pyd[321].pp
	print "pyd[321].ap\t", fighter1.pyd[321].ap, "\t", fighter2.pyd[321].ap, "\t", fighter3.pyd[321].ap, "\t", fighter4.pyd[321].ap, "\t", fighter5.pyd[321].ap
	print "pyd[654].pp\t", fighter1.pyd[654].pp, "\t", fighter2.pyd[654].pp, "\t", fighter3.pyd[654].pp, "\t", fighter4.pyd[654].pp, "\t", fighter5.pyd[654].pp
	print "pyd[654].ap\t", fighter1.pyd[654].ap, "\t", fighter2.pyd[654].ap, "\t", fighter3.pyd[654].ap, "\t", fighter4.pyd[654].ap, "\t", fighter5.pyd[654].ap
	print "pyv1\t", fighter1.pyv1, "\t", fighter2.pyv1, "\t", fighter3.pyv1, "\t", fighter4.pyv1, "\t", fighter5.pyv1
	print "pyv2.pp\t", fighter1.pyv2.pp, "\t", fighter2.pyv2.pp, "\t", fighter3.pyv2.pp, "\t", fighter4.pyv2.pp, "\t", fighter5.pyv2.pp
	print "pyv2.ap\t", fighter1.pyv2.ap, "\t", fighter2.pyv2.ap, "\t", fighter3.pyv2.ap, "\t", fighter4.pyv2.ap, "\t", fighter5.pyv2.ap
	print "dd[12][111]\t", fighter1.dd[12][111], " ", fighter2.dd[12][111], " ", fighter3.dd[12][111], " ", fighter4.dd[12][111], " ", fighter5.dd[12][111]
	print "dd[12][222]\t", fighter1.dd[12][222], " ", fighter2.dd[12][222], " ", fighter3.dd[12][222], " ", fighter4.dd[12][222], " ", fighter5.dd[12][222]
	print "dd[34][333].x\t", fighter1.dd[34][333].x, "\t", fighter2.dd[34][333].x, "\t", fighter3.dd[34][333].x, "\t", fighter4.dd[34][333].x, "\t", fighter5.dd[34][333].x
	print "dd[34][333].y\t", fighter1.dd[34][333].y, "\t", fighter2.dd[34][333].y, "\t", fighter3.dd[34][333].y, "\t", fighter4.dd[34][333].y, "\t", fighter5.dd[34][333].y
	print "dd[34][333].s\t", fighter1.dd[34][333].s, " ", fighter2.dd[34][333].s, " ", fighter3.dd[34][333].s, " ", fighter4.dd[34][333].s, " ", fighter5.dd[34][333].s
	print "dd[34][333].b\t", fighter1.dd[34][333].b, " ", fighter2.dd[34][333].b, " ", fighter3.dd[34][333].b, " ", fighter4.dd[34][333].b, " ", fighter5.dd[34][333].b
	print "dd[34][333].e\t", fighter1.dd[34][333].e, " ", fighter2.dd[34][333].e, " ", fighter3.dd[34][333].e, " ", fighter4.dd[34][333].e, " ", fighter5.dd[34][333].e
	print "dd[34][444]\t", fighter1.dd[34][444], " ", fighter2.dd[34][444], " ", fighter3.dd[34][444], " ", fighter4.dd[34][444], " ", fighter5.dd[34][444]
	print "fdd[123][12]\t", fighter1.fdd[123][12], " ", fighter2.fdd[123][12], " ", fighter3.fdd[123][12], " ", fighter4.fdd[123][12], " ", fighter5.fdd[123][12]
	print "fdd[123][23]\t", fighter1.fdd[123][23], " ", fighter2.fdd[123][23], " ", fighter3.fdd[123][23], " ", fighter4.fdd[123][23], " ", fighter5.fdd[123][23]
	print "nv\t", "\t", fighter1.nv, "\t", fighter2.nv, "\t", fighter3.nv, "\t", fighter4.nv, "\t", fighter5.nv
	print "lv[0]\t", "\t", fighter1.lv[0], "\t", fighter2.lv[0], "\t", fighter3.lv[0], "\t", fighter4.lv[0], "\t", fighter5.lv[0]
	print "lv[1]\t", "\t", fighter1.lv[1], "\t", fighter2.lv[1], "\t", fighter3.lv[1], "\t", fighter4.lv[1], "\t", fighter5.lv[1]
	print "flv[0]\t", "\t", fighter1.flv[0], "\t", fighter2.flv[0], "\t", fighter3.flv[0], "\t", fighter4.flv[0], "\t", fighter5.flv[0]
	print "flv[1]\t", "\t", fighter1.flv[1], "\t", fighter2.flv[1], "\t", fighter3.flv[1], "\t", fighter4.flv[1], "\t", fighter5.flv[1]
	print "dv[333]\t", "\t", fighter1.dv[333], "\t", fighter2.dv[333], "\t", fighter3.dv[333], "\t", fighter4.dv[333], "\t", fighter5.dv[333]
	print "dv[444]\t", "\t", fighter1.dv[444], "\t", fighter2.dv[444], "\t", fighter3.dv[444], "\t", fighter4.dv[444], "\t", fighter5.dv[444]
	print "fdv[333]\t", fighter1.fdv[333], "\t", fighter2.fdv[333], "\t", fighter3.fdv[333], "\t", fighter4.fdv[333], "\t", fighter5.fdv[333]
	print "fdv[444]\t", fighter1.fdv[444], "\t", fighter2.fdv[444], "\t", fighter3.fdv[444], "\t", fighter4.fdv[444], "\t", fighter5.fdv[444]


def _build(_typy):
	global Vector2, Fighter, Corpus, PyType, Empty, Vector3, SkillParam, onInitRuntime

	vPy = Vector2(
		123,  # x
		45.6, # y
		s = u"哈哈哈哈",
		b = u"asdf 1234",
		e = Corpus.LOCAL,
		p = PyType(pp = 123, ap = 'asdf'),
		i = Empty(),
	)
	vDataPy = vPy.SerializeToString()
	vPy2 = Vector2()
	vPy2.ParseFromString(vDataPy)
	vPy2.x += 1000
	vPy2.y += 1000
	vPy2.s += u" 吼吼吼"
	vPy2.b += " xxx"
	vPy2.e += 2

	print vPy, vPy.x, vPy.y, vPy.s, vPy.b, vPy.e, vPy.p, vPy.i, vPy.p.pp
	print vPy2, vPy2.x, vPy2.y, vPy2.s, vPy2.b, vPy2.e, vPy2.p, vPy2.i, vPy2.p.pp

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
		poss = {321: vPy},
	)
	fighterPy.hp += 12
	fighterPy.poss[231] = vPy2
	fighterPy.posi[123] = 0.456
	fighterPy.posl.append(vPy)
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
	fighterPy.vl = [123, "adsf"]
	fighterPy.vl.append(vPy)
	fighterPy.vl.append(345.123)
	fighterPy.vd[12] = Corpus.IMAGES
	fighterPy.vd[23] = "adsf"
	fighterPy.vd[34] = vPy2
	fighterPy.vd[45] = 345.123
	fighterPy.ld = {12: [Corpus.IMAGES, "adsf"], 34: [vPy2, 345.123]}
	fighterPy.fld = {123: [222.111, 345.123]}
	fighterPy.pyl = [PyType(pp = 123, ap = 'asdf'), PyType(pp = 456, ap = 'xxxx')]
	fighterPy.pyd = {321: PyType(pp = 123, ap = 'asdf'), 654: PyType(pp = 456, ap = 'xxxx')}
	fighterPy.pyv1 = 123
	fighterPy.pyv2 = PyType(pp = 123, ap = 'asdf')
	fighterPy.dd = {12: {111: Corpus.IMAGES, 222: "adsf"}, 34: {333: vPy2, 444: 345.123}}
	fighterPy.fdd = {123: {12: 222.111, 23: 345.123}}
	fighterPy.nv = 123456
	fighterPy.lv = [123, "adsf"]
	fighterPy.flv = [222.111, 345.123]
	fighterPy.dv = {333: 123, 444: "adsf"}
	fighterPy.fdv = {333: 222.111, 444: 345.123}

	fighterPy2 = Fighter()

	_typy.setDefaultEncodingUTF8()
	_typy.Corpus(Corpus.__enum__)
	_typy.PyType(PyType)

	import time
	startTime = time.time()

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
			'e': self.e,
			'p': self.p,
		}),
		'__cmp__': lambda self, other: self.x - other.x,
		'__lt__': lambda self, other: self.x < other.x,
		'__add__': lambda self, other: self.__class__(x = self.x + other.x, y = self.y + other.y),
	})
	_Empty = _typy.Empty()
	_Fighter = _typy.Fighter()
	_Vector2.classProp.add(False)
	print "hasattr(_Fighter, 'prop')", hasattr(_Fighter, 'prop')
	print "hasattr(_Fighter(), 'prop')", hasattr(_Fighter(), 'prop')
	print "hasattr(_Vector2, 'prop')", hasattr(_Vector2, 'prop')
	print "hasattr(_Vector2(), 'prop')", hasattr(_Vector2(), 'prop')
	print _Vector2().__class__, _Vector2
	print "isinstance(_Vector2(), _Vector2)", isinstance(_Vector2(), _Vector2)
	print "isinstance(_Vector2, type)", isinstance(_Vector2, type)
	print "issubclass(_Vector2, type)", issubclass(_Vector2, type)


	v = _Vector2(
		123,  # x
		45.6, # y
		s = u"哈哈哈哈",
		b = u"asdf 1234",
		e = Corpus.LOCAL,
		p = PyType(pp = 123, ap = 'asdf'),
		i = _Empty(),
	)
	print vPy.Args(), v.Args()
	print vPy, vPy.x, vPy.y, vPy.s, vPy.b, vPy.e, vPy.p, vPy.i, vPy.p.pp
	print v, v.x, v.y, v.s, v.b, v.e, v.p, v.i, v.p.pp

	print vPy.classProp
	print v.classProp

	vPy.TestProperty
	v.TestProperty

	vPy.t = "================="
	v.t = "+++++++++++++++++"
	print vPy.t, v.t

	vPy.TestClassMethod()
	vPy.TestClassMethod('cls asdf')

	_Vector2.TestClassMethod()
	v.TestClassMethod()
	v.TestClassMethod('cls asdf')

	vPy.TestMethod()
	vPy.TestMethod('asdf')

	v.TestMethod()
	v.TestMethod('asdf')
	print v.TestLambda()

	vData = v.SerializeToString()
	print len(vDataPy), [ord(d) for d in vDataPy]
	print len(vData), [ord(d) for d in vData]

	v2 = _Vector2()
	v2.ParseFromString(vData)

	print v == v2
	print v < v2
	print True if v else False

	v2.x += 1000
	v2.y += 1000
	v2.s += u" 吼吼吼"
	v2.b += " xxx"
	v2.e += 2
	print vPy2.x, vPy2.y, vPy2.s, vPy2.b, vPy2.e, vPy2.i
	print v2.x, v2.y, v2.s, v2.b, v2.e, v2.i

	print v2 == v
	print v < v2
	print v2 + v


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
		poss = {321: v},
	)
	fighter.hp += 12
	fighter.poss[231] = v2
	fighter.posi[123] = 0.456
	fighter.posl.append(v)
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
	fighter.vl = [123, "adsf"]
	fighter.vl.append(v)
	fighter.vl.append(345.123)
	fighter.vd[12] = Corpus.IMAGES
	fighter.vd[23] = "adsf"
	fighter.vd[34] = v2
	fighter.vd[45] = 345.123
	fighter.ld = {12: [Corpus.IMAGES, "adsf"], 34: [v2, 345.123]}
	fighter.fld = {123: [222.111, 345.123]}
	fighter.pyl = [PyType(pp = 123, ap = 'asdf'), PyType(pp = 456, ap = 'xxxx')]
	fighter.pyd = {321: PyType(pp = 123, ap = 'asdf'), 654: PyType(pp = 456, ap = 'xxxx')}
	fighter.pyv1 = 123
	fighter.pyv2 = PyType(pp = 123, ap = 'asdf')
	fighter.dd = {12: {111: Corpus.IMAGES, 222: "adsf"}, 34: {333: v2, 444: 345.123}}
	fighter.fdd = {123: {12: 222.111, 23: 345.123}}
	fighter.nv = 123456
	fighter.lv = [123, "adsf"]
	fighter.flv = [222.111, 345.123]
	fighter.dv = {333: 123, 444: "adsf"}
	fighter.fdv = {333: 222.111, 444: 345.123}

	fDataPy = fighterPy.SerializeToString()
	print len(fDataPy)#, [ord(d) for d in fDataPy]
	fData = fighter.SerializeToString()
	print len(fData)#, [ord(d) for d in fData]

	fighterPy2.ParseFromString(fData)

	fighter2 = _Fighter()
	fighter2.ParseFromString(fData)
	fighter3 = _Fighter()
	fighter3.ParseFromString(fDataPy)

	print "\t\t_typy python _ty->_ty _ty->py py->_ty"
	# _printFighters(fighter, fighterPy, fighter2, fighterPy2, fighter3)

	print fighter == None

	print hasattr(fighterPy.sl, '__iter__'), type(fighterPy.sl), fighterPy.sl[-1], [a for a in fighterPy.sl], map(lambda k: k, fighterPy.sl)
	print hasattr(fighterPy.vl, '__iter__'), type(fighterPy.vl), fighterPy.vl[-1], [a for a in fighterPy.vl], map(lambda k: k, fighterPy.vl)

	print fighter.vl[-1], [a for a in fighter.vl], map(lambda k: k, fighter.vl)

	print [k for k in fighter.vd], map(lambda k: k, fighter.vd)
	print [vv for vv in fighter.vd.itervalues()], {k: v for k, v in fighter.vd.iteritems()}
	print 12 in fighter.vd, 123 in fighter.vd, 'asdf' in fighter.vd
	print fighter.vd.get(12), fighter.vd.get(123), fighter.vd.get(123, 'xxx')

	print hasattr(fighter.vl, '__iter__'), hasattr(fighterPy.vl, '__iter__')
	print hasattr(fighter.vd, 'iteritems'), hasattr(fighterPy.vd, 'iteritems')

	print (lambda **kwargs: kwargs)(**fighter.bd)

	def TestPropRef():
		scopedFighter = _Fighter()
		scopedFighter.ParseFromString(fData)
		print scopedFighter.v1, scopedFighter.v2, scopedFighter.v3, scopedFighter.v4
		return scopedFighter.v1, scopedFighter.v2, scopedFighter.v3, scopedFighter.v4

	v1, v2, v3, v4 = TestPropRef()
	print v1, v2, v3, v4

	def TestListRef():
		scopedFighter = _Fighter()
		scopedFighter.ParseFromString(fData)
		print [a for a in scopedFighter.vl]
		return scopedFighter.vl

	vl = TestListRef()
	print [a for a in vl]

	def TestDictRef():
		scopedFighter = _Fighter()
		scopedFighter.ParseFromString(fData)
		print {k: v for k, v in scopedFighter.vd.iteritems()}
		return scopedFighter.vd

	vd = TestDictRef()
	print {k: v for k, v in vd.iteritems()}


	pFighterPy1 = Fighter()
	pFighterPy2 = Fighter()
	pFighter1 = _Fighter()
	pFighter2 = _Fighter()

	pDataPy = fighterPy.SerializeProperty('v1')
	pData = fighter.SerializeProperty('v1')
	print 'DeserializeProperty', pFighterPy1.DeserializeProperty(pDataPy)
	print 'DeserializeProperty', pFighterPy2.DeserializeProperty(pData)
	print 'DeserializeProperty', pFighter1.DeserializeProperty(pDataPy)
	print 'DeserializeProperty', pFighter2.DeserializeProperty(pData)
	print 'v1', fighterPy.v1, fighter.v1, pFighterPy1.v1, pFighterPy2.v1, pFighter1.v1, pFighter2.v1

	fighterPy.v1 = None
	fighter.v1 = None
	pDataPy = fighterPy.SerializeProperty('v1')
	pData = fighter.SerializeProperty('v1')
	print 'DeserializeProperty', pFighterPy1.DeserializeProperty(pDataPy)
	print 'DeserializeProperty', pFighterPy2.DeserializeProperty(pData)
	print 'DeserializeProperty', pFighter1.DeserializeProperty(pDataPy)
	print 'DeserializeProperty', pFighter2.DeserializeProperty(pData)
	print 'v1', fighterPy.v1, fighter.v1, pFighterPy1.v1, pFighterPy2.v1, pFighter1.v1, pFighter2.v1

	fighterPy.vd = fighterPy.sd
	fighter.vd = fighter.sd
	fighter.vd.update(fighter.sd)
	print fighterPy.vd[321], fighterPy.vd[231], fighter.vd[321], fighter.vd[231]

	def TestSkillParam():
		oData = '\n\x10WO2gOCkyKx6wL59B\x12\x00\x18\xae\x01"\x10WO2gOCkyKx6wL59B*8\x12\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1a\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00*\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x002\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x002\x00@\xae\x01X\x01'

		oPy = onInitRuntime()
		oPy.ParseFromString(oData)
		print oPy.Args()

		_typy.Vector3(Vector3)
		_onInitRuntime = _typy.onInitRuntime()
		o = _onInitRuntime()
		o.ParseFromString(oData)
		print o.Args()

	TestSkillParam()

	import copy
	print copy.deepcopy(fighterPy).vd.pop(321), copy.deepcopy(fighter).vd.pop(321)
	print len(fighterPy.SerializeToString()), len(copy.deepcopy(fighterPy).SerializeToString())
	print len(fighter.SerializeToString()), len(copy.deepcopy(fighter).SerializeToString())
	print fighterPy.vd.pop(4567, None), fighter.vd.pop(4567, 'NULL')
	print fighterPy.posi.pop(4567, 'NULL'), fighter.posi.pop(4567, None)
	print 4567 in fighterPy.posi, 4567 in fighter.posi
	print fighterPy.vd.items(), fighter.vd.items()

	print 'fighterPy Json'
	print fighterPy.Json()
	print 'fighter Json'
	print fighter.Json()
	print Fighter.FromJson(fighter.Json()).vd.pop(321), _Fighter.FromJson(fighterPy.Json()).vd.pop(321)
	print len(fighter.SerializeToString()), len(Fighter.FromJson(fighter.Json(True)).SerializeToString())
	print len(fighterPy.SerializeToString()), len(_Fighter.FromJson(fighterPy.Json(True)).SerializeToString())

	return time.time() - startTime

cpptime = 0
cpytime = 0

def test_cpp():
	import os
	from typy import GenerateExtention
	GenerateExtention('%s/typy' % os.path.abspath(os.path.dirname(__file__)))
	from typy import _typy
	global cpptime, cpytime
	cpptime = _build(_typy)
	print 'time', cpptime, cpytime


def test_cpy():
	import os
	from typy import GenerateDescriptor
	GenerateDescriptor(os.path.dirname(__file__))
	import _typy
	global cpptime, cpytime
	cpytime = _build(_typy)
	print 'time', cpptime, cpytime
