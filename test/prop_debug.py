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

	import test_case
	from typy import GenerateDescriptor
	GenerateDescriptor(os.path.dirname(__file__))
	import _typy
	test_case.setup(globals(), _typy)


def test_prop():
	global Vector2, Fighter, Corpus, PyType, Empty
	global _Vector2, _Fighter, _Empty
	global vPy, vPy2, fighterPy, v, v2, fighter

	propDict = {}

	def onPropertyChanged(obj, name, old, new):
		cls = obj.__class__.__name__
		assert cls in propDict and name in propDict[cls], (propDict, obj, name, old, new)
		print cls, name, old, new
		propDict[cls].remove(name)
		if not propDict[cls]:
			del propDict[cls]

	_Vector2.RegisterHandler(onPropertyChanged, 'x')
	_Vector2.RegisterHandler(onPropertyChanged, 'y')
	_Fighter.RegisterHandler(onPropertyChanged)

	print 'test_prop begin'

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
	fighter.posi[123] = 0.4567
	assert not propDict, 'fighter.posi[123] = 0.4567 %s' % repr(propDict)

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
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'dd', 'v3'],
	}
	fighter.dd[12][111].x = 3141592653
	assert not propDict, 'fighter.dd[12][111].x = 3141592653 %s' % repr(propDict)

	propDict = {
		'Vector2': ['y'],
		'Fighter': ['pos', 'posl', 'poss', 'vd', 'ld', 'dd', 'dd', 'v3'],
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

	print 'test_prop end'
