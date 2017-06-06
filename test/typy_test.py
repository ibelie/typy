#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

SETUP_TYPYC = True
SETUP_TYPYD = True

def setup():
	import os
	import sys
	import shutil
	from distutils.util import get_platform
	os.chdir(os.path.dirname(os.path.dirname(__file__)))
	suffix = 'pyd' if os.name == 'nt' else 'so'
	global SETUP_TYPYD, SETUP_TYPYC
	map(os.remove, [f for f in (
		'test/_typy.pyc',
		'test/_typy.py' if SETUP_TYPYC else '',
		'typy/_typy.py',
		'typy/_typy.%s' % suffix,
	) if os.path.isfile(f)])
	SETUP_TYPYD and os.system('python -B setup.py build')
	typydFile = 'build/lib.%s-%s/typy/_typyd.%s' % (get_platform(), sys.version[0:3], suffix)
	os.path.isfile(typydFile) and shutil.copy(typydFile, "test/")


def _printFighters(fighter1, fighter2, fighter3, fighter4, fighter5):
	print "hp\t", "\t", fighter1.hp, "\t\t", fighter2.hp, "\t\t", fighter3.hp, "\t\t", fighter4.hp, "\t\t", fighter5.hp
	print "isAwake\t", "\t", fighter1.isAwake, "\t\t", fighter2.isAwake, "\t\t", fighter3.isAwake, "\t\t", fighter4.isAwake, "\t\t", fighter5.isAwake
	print "pos.x\t", "\t", fighter1.pos.x, "\t\t", fighter2.pos.x, "\t\t", fighter3.pos.x, "\t\t", fighter4.pos.x, "\t\t", fighter5.pos.x
	print "pos.y\t", "\t", fighter1.pos.y, "\t\t", fighter2.pos.y, "\t\t", fighter3.pos.y, "\t\t", fighter4.pos.y, "\t\t", fighter5.pos.y
	print "pos.s\t", "\t", fighter1.pos.s, "\t", fighter2.pos.s, "\t", fighter3.pos.s, "\t", fighter4.pos.s, "\t", fighter5.pos.s
	print "pos.b\t", "\t", fighter1.pos.b, "\t", fighter2.pos.b, "\t", fighter3.pos.b, "\t", fighter4.pos.b, "\t", fighter5.pos.b
	print "pos.e\t", "\t", fighter1.pos.e, "\t\t", fighter2.pos.e, "\t\t", fighter3.pos.e, "\t\t", fighter4.pos.e, "\t\t", fighter5.pos.e
	print "pos.p.pp\t", fighter1.pos.p.pp, "\t\t", fighter2.pos.p.pp, "\t\t", fighter3.pos.p.pp, "\t\t", fighter4.pos.p.pp, "\t\t", fighter5.pos.p.pp
	print "pos.p.ap\t", fighter1.pos.p.ap, "\t\t", fighter2.pos.p.ap, "\t\t", fighter3.pos.p.ap, "\t\t", fighter4.pos.p.ap, "\t\t", fighter5.pos.p.ap
	print "posi[123]\t", '%.3f' % fighter1.posi[123], "\t\t", '%.3f' % fighter2.posi[123], "\t\t", '%.3f' % fighter3.posi[123], "\t\t", '%.3f' % fighter4.posi[123], "\t\t", '%.3f' % fighter5.posi[123]
	print "fl[0]\t", "\t", '%.3f' % fighter1.fl[0], "\t\t", '%.3f' % fighter2.fl[0], "\t\t", '%.3f' % fighter3.fl[0], "\t\t", '%.3f' % fighter4.fl[0], "\t\t", '%.3f' % fighter5.fl[0]
	print "fl[1]\t", "\t", fighter1.fl[1], "\t\t", fighter2.fl[1], "\t\t", fighter3.fl[1], "\t\t", fighter4.fl[1], "\t\t", fighter5.fl[1]
	print "fl[2]\t", "\t", '%.3f' % fighter1.fl[2], "\t\t", '%.3f' % fighter2.fl[2], "\t\t", '%.3f' % fighter3.fl[2], "\t\t", '%.3f' % fighter4.fl[2], "\t\t", '%.3f' % fighter5.fl[2]
	print "bl[0]\t", "\t", fighter1.bl[0], "\t", fighter2.bl[0], "\t", fighter3.bl[0], "\t", fighter4.bl[0], "\t", fighter5.bl[0]
	print "bl[1]\t", "\t", fighter1.bl[1], "\t", fighter2.bl[1], "\t", fighter3.bl[1], "\t", fighter4.bl[1], "\t", fighter5.bl[1]
	print "bl[2]\t", "\t", fighter1.bl[2], "\t", fighter2.bl[2], "\t", fighter3.bl[2], "\t", fighter4.bl[2], "\t", fighter5.bl[2]
	print "sl[0]\t", "\t", fighter1.sl[0], "\t\t", fighter2.sl[0], "\t\t", fighter3.sl[0], "\t\t", fighter4.sl[0], "\t\t", fighter5.sl[0]
	print "sl[1]\t", "\t", fighter1.sl[1], "\t\t", fighter2.sl[1], "\t\t", fighter3.sl[1], "\t\t", fighter4.sl[1], "\t\t", fighter5.sl[1]
	print "sl[2]\t", "\t", fighter1.sl[2], "\t\t", fighter2.sl[2], "\t\t", fighter3.sl[2], "\t\t", fighter4.sl[2], "\t\t", fighter5.sl[2]
	print "el[0]\t", "\t", fighter1.el[0], "\t\t", fighter2.el[0], "\t\t", fighter3.el[0], "\t\t", fighter4.el[0], "\t\t", fighter5.el[0]
	print "el[1]\t", "\t", fighter1.el[1], "\t\t", fighter2.el[1], "\t\t", fighter3.el[1], "\t\t", fighter4.el[1], "\t\t", fighter5.el[1]
	print "el[2]\t", "\t", fighter1.el[2], "\t\t", fighter2.el[2], "\t\t", fighter3.el[2], "\t\t", fighter4.el[2], "\t\t", fighter5.el[2]
	print "posl[0].x", "\t", fighter1.posl[0].x, "\t\t", fighter2.posl[0].x, "\t\t", fighter3.posl[0].x, "\t\t", fighter4.posl[0].x, "\t\t", fighter5.posl[0].x
	print "posl[0].y", "\t", fighter1.posl[0].y, "\t\t", fighter2.posl[0].y, "\t\t", fighter3.posl[0].y, "\t\t", fighter4.posl[0].y, "\t\t", fighter5.posl[0].y
	print "posl[0].s\t", fighter1.posl[0].s, "\t", fighter2.posl[0].s, "\t", fighter3.posl[0].s, "\t", fighter4.posl[0].s, "\t", fighter5.posl[0].s
	print "posl[0].b\t", fighter1.posl[0].b, "\t", fighter2.posl[0].b, "\t", fighter3.posl[0].b, "\t", fighter4.posl[0].b, "\t", fighter5.posl[0].b
	print "posl[0].e\t", fighter1.posl[0].e, "\t\t", fighter2.posl[0].e, "\t\t", fighter3.posl[0].e, "\t\t", fighter4.posl[0].e, "\t\t", fighter5.posl[0].e
	print "posl[1]", "\t", fighter1.posl[1], "\t\t", fighter2.posl[1], "\t\t", fighter3.posl[1], "\t\t", fighter4.posl[1], "\t\t", fighter5.posl[1]
	print "posl[2].x", "\t", fighter1.posl[2].x, "\t\t", fighter2.posl[2].x, "\t\t", fighter3.posl[2].x, "\t\t", fighter4.posl[2].x, "\t\t", fighter5.posl[2].x
	print "posl[2].y", "\t", fighter1.posl[2].y, "\t\t", fighter2.posl[2].y, "\t\t", fighter3.posl[2].y, "\t\t", fighter4.posl[2].y, "\t\t", fighter5.posl[2].y
	print "posl[2].s\t", fighter1.posl[2].s, fighter2.posl[2].s, fighter3.posl[2].s, fighter4.posl[2].s, fighter5.posl[2].s
	print "posl[2].b\t", fighter1.posl[2].b, " ", fighter2.posl[2].b, " ", fighter3.posl[2].b, " ", fighter4.posl[2].b, " ", fighter5.posl[2].b
	print "posl[2].e\t", fighter1.posl[2].e, "\t", fighter2.posl[2].e, "\t", fighter3.posl[2].e, "\t", fighter4.posl[2].e, "\t", fighter5.posl[2].e
	print "poss[321].x", "\t", fighter1.poss[321].x, "\t\t", fighter2.poss[321].x, "\t\t", fighter3.poss[321].x, "\t\t", fighter4.poss[321].x, "\t\t", fighter5.poss[321].x
	print "poss[321].y", "\t", fighter1.poss[321].y, "\t\t", fighter2.poss[321].y, "\t\t", fighter3.poss[321].y, "\t\t", fighter4.poss[321].y, "\t\t", fighter5.poss[321].y
	print "poss[321].s\t", fighter1.poss[321].s, "\t", fighter2.poss[321].s, "\t", fighter3.poss[321].s, "\t", fighter4.poss[321].s, "\t", fighter5.poss[321].s
	print "poss[321].b\t", fighter1.poss[321].b, "\t", fighter2.poss[321].b, "\t", fighter3.poss[321].b, "\t", fighter4.poss[321].b, "\t", fighter5.poss[321].b
	print "poss[321].e\t", fighter1.poss[321].e, "\t\t", fighter2.poss[321].e, "\t\t", fighter3.poss[321].e, "\t\t", fighter4.poss[321].e, "\t\t", fighter5.poss[321].e
	print "poss[231].x", "\t", fighter1.poss[231].x, "\t\t", fighter2.poss[231].x, "\t\t", fighter3.poss[231].x, "\t\t", fighter4.poss[231].x, "\t\t", fighter5.poss[231].x
	print "poss[231].y", "\t", fighter1.poss[231].y, "\t\t", fighter2.poss[231].y, "\t\t", fighter3.poss[231].y, "\t\t", fighter4.poss[231].y, "\t\t", fighter5.poss[231].y
	print "poss[231].s\t", fighter1.poss[231].s, fighter2.poss[231].s, fighter3.poss[231].s, fighter4.poss[231].s, fighter5.poss[231].s
	print "poss[231].b\t", fighter1.poss[231].b, " ", fighter2.poss[231].b, " ", fighter3.poss[231].b, " ", fighter4.poss[231].b, " ", fighter5.poss[231].b
	print "poss[231].e\t", fighter1.poss[231].e, "\t", fighter2.poss[231].e, "\t", fighter3.poss[231].e, "\t", fighter4.poss[231].e, "\t", fighter5.poss[231].e
	print "poss[320]", "\t", fighter1.poss[320], "\t\t", fighter2.poss[320], "\t\t", fighter3.poss[320], "\t\t", fighter4.poss[320], "\t\t", fighter5.poss[320]
	print "poss[230]", "\t", fighter1.poss[230], "\t\t", fighter2.poss[230], "\t\t", fighter3.poss[230], "\t\t", fighter4.poss[230], "\t\t", fighter5.poss[230]
	print u"bd[u'哈哈']\t", fighter1.bd[u'哈哈'], "\t", fighter2.bd[u'哈哈'], "\t", fighter3.bd[u'哈哈'], "\t", fighter4.bd[u'哈哈'], "\t", fighter5.bd[u'哈哈']
	print "bd['asdf']\t", fighter1.bd['asdf'], "\t", fighter2.bd['asdf'], "\t", fighter3.bd['asdf'], "\t", fighter4.bd['asdf'], "\t", fighter5.bd['asdf']
	print "sd[321]\t", "\t", fighter1.sd[321], "\t\t", fighter2.sd[321], "\t\t", fighter3.sd[321], "\t\t", fighter4.sd[321], "\t\t", fighter5.sd[321]
	print "sd[231]\t", "\t", fighter1.sd[231], "\t\t", fighter2.sd[231], "\t\t", fighter3.sd[231], "\t\t", fighter4.sd[231], "\t\t", fighter5.sd[231]
	print "ed[789]\t", "\t", fighter1.ed[789], "\t\t", fighter2.ed[789], "\t\t", fighter3.ed[789], "\t\t", fighter4.ed[789], "\t\t", fighter5.ed[789]
	print "ed[567]\t", "\t", fighter1.ed[567], "\t\t", fighter2.ed[567], "\t\t", fighter3.ed[567], "\t\t", fighter4.ed[567], "\t\t", fighter5.ed[567]
	print "ll[0][0]\t", '%.3f' % fighter1.ll[0][0], "\t\t", '%.3f' % fighter2.ll[0][0], "\t\t", '%.3f' % fighter3.ll[0][0], "\t\t", '%.3f' % fighter4.ll[0][0], "\t\t", '%.3f' % fighter5.ll[0][0]
	print "ll[0][1]\t", '%.3f' % fighter1.ll[0][1], "\t\t", '%.3f' % fighter2.ll[0][1], "\t\t", '%.3f' % fighter3.ll[0][1], "\t\t", '%.3f' % fighter4.ll[0][1], "\t\t", '%.3f' % fighter5.ll[0][1]
	print "ll[1][0]\t", '%.3f' % fighter1.ll[1][0], "\t\t", '%.3f' % fighter2.ll[1][0], "\t\t", '%.3f' % fighter3.ll[1][0], "\t\t", '%.3f' % fighter4.ll[1][0], "\t\t", '%.3f' % fighter5.ll[1][0]
	print "ll[1][1]\t", '%.3f' % fighter1.ll[1][1], "\t\t", '%.3f' % fighter2.ll[1][1], "\t\t", '%.3f' % fighter3.ll[1][1], "\t\t", '%.3f' % fighter4.ll[1][1], "\t\t", '%.3f' % fighter5.ll[1][1]
	print "ll[1][2]\t", '%.3f' % fighter1.ll[1][2], "\t", '%.3f' % fighter2.ll[1][2], "\t", '%.3f' % fighter3.ll[1][2], "\t", '%.3f' % fighter4.ll[1][2], "\t", '%.3f' % fighter5.ll[1][2]
	print "v0\t", "\t", fighter1.v0, "\t\t", fighter2.v0, "\t\t", fighter3.v0, "\t\t", fighter4.v0, "\t\t", fighter5.v0
	print "v1\t", "\t", fighter1.v1, "\t\t", fighter2.v1, "\t\t", fighter3.v1, "\t\t", fighter4.v1, "\t\t", fighter5.v1
	print "v2\t", "\t", fighter1.v2, "\t\t", fighter2.v2, "\t\t", fighter3.v2, "\t\t", fighter4.v2, "\t\t", fighter5.v2
	print "v3.x\t", "\t", fighter1.v3.x, "\t\t", fighter2.v3.x, "\t\t", fighter3.v3.x, "\t\t", fighter4.v3.x, "\t\t", fighter5.v3.x
	print "v3.y\t", "\t", fighter1.v3.y, "\t\t", fighter2.v3.y, "\t\t", fighter3.v3.y, "\t\t", fighter4.v3.y, "\t\t", fighter5.v3.y
	print "v3.s\t", "\t", fighter1.v3.s, "\t", fighter2.v3.s, "\t", fighter3.v3.s, "\t", fighter4.v3.s, "\t", fighter5.v3.s
	print "v3.b\t", "\t", fighter1.v3.b, "\t", fighter2.v3.b, "\t", fighter3.v3.b, "\t", fighter4.v3.b, "\t", fighter5.v3.b
	print "v3.e\t", "\t", fighter1.v3.e, "\t\t", fighter2.v3.e, "\t\t", fighter3.v3.e, "\t\t", fighter4.v3.e, "\t\t", fighter5.v3.e
	print "v4\t", "\t", '%.3f' % fighter1.v4, "\t", '%.3f' % fighter2.v4, "\t", '%.3f' % fighter3.v4, "\t", '%.3f' % fighter4.v4, "\t", '%.3f' % fighter5.v4
	print "vl[0]\t", "\t", fighter1.vl[0], "\t\t", fighter2.vl[0], "\t\t", fighter3.vl[0], "\t\t", fighter4.vl[0], "\t\t", fighter5.vl[0]
	print "vl[1]\t", "\t", fighter1.vl[1], "\t\t", fighter2.vl[1], "\t\t", fighter3.vl[1], "\t\t", fighter4.vl[1], "\t\t", fighter5.vl[1]
	print "vl[2]\t", "\t", fighter1.vl[2], "\t\t", fighter2.vl[2], "\t\t", fighter3.vl[2], "\t\t", fighter4.vl[2], "\t\t", fighter5.vl[2]
	print "vl[3].x\t", "\t", fighter1.vl[3].x, "\t\t", fighter2.vl[3].x, "\t\t", fighter3.vl[3].x, "\t\t", fighter4.vl[3].x, "\t\t", fighter5.vl[3].x
	print "vl[3].y\t", "\t", fighter1.vl[3].y, "\t\t", fighter2.vl[3].y, "\t\t", fighter3.vl[3].y, "\t\t", fighter4.vl[3].y, "\t\t", fighter5.vl[3].y
	print "vl[3].s\t", "\t", fighter1.vl[3].s, "\t", fighter2.vl[3].s, "\t", fighter3.vl[3].s, "\t", fighter4.vl[3].s, "\t", fighter5.vl[3].s
	print "vl[3].b\t", "\t", fighter1.vl[3].b, "\t", fighter2.vl[3].b, "\t", fighter3.vl[3].b, "\t", fighter4.vl[3].b, "\t", fighter5.vl[3].b
	print "vl[3].e\t", "\t", fighter1.vl[3].e, "\t\t", fighter2.vl[3].e, "\t\t", fighter3.vl[3].e, "\t\t", fighter4.vl[3].e, "\t\t", fighter5.vl[3].e
	print "vl[4]\t", "\t", fighter1.vl[4], "\t", fighter2.vl[4], "\t", fighter3.vl[4], "\t", fighter4.vl[4], "\t", fighter5.vl[4]
	print "vl[5]\t", "\t", fighter1.vl[5], "\t\t", fighter2.vl[5], "\t\t", fighter3.vl[5], "\t\t", fighter4.vl[5], "\t\t", fighter5.vl[5]
	print "vd[0]\t", "\t", fighter1.vd[0], "\t\t", fighter2.vd[0], "\t\t", fighter3.vd[0], "\t\t", fighter4.vd[0], "\t\t", fighter5.vd[0]
	print "vd[12]\t", "\t", fighter1.vd[12], "\t\t", fighter2.vd[12], "\t\t", fighter3.vd[12], "\t\t", fighter4.vd[12], "\t\t", fighter5.vd[12]
	print "vd[23]\t", "\t", fighter1.vd[23], "\t\t", fighter2.vd[23], "\t\t", fighter3.vd[23], "\t\t", fighter4.vd[23], "\t\t", fighter5.vd[23]
	print "vd[34].x\t", fighter1.vd[34].x, "\t\t", fighter2.vd[34].x, "\t\t", fighter3.vd[34].x, "\t\t", fighter4.vd[34].x, "\t\t", fighter5.vd[34].x
	print "vd[34].y\t", fighter1.vd[34].y, "\t\t", fighter2.vd[34].y, "\t\t", fighter3.vd[34].y, "\t\t", fighter4.vd[34].y, "\t\t", fighter5.vd[34].y
	print "vd[34].s\t", fighter1.vd[34].s, fighter2.vd[34].s, fighter3.vd[34].s, fighter4.vd[34].s, fighter5.vd[34].s
	print "vd[34].b\t", fighter1.vd[34].b, " ", fighter2.vd[34].b, " ", fighter3.vd[34].b, " ", fighter4.vd[34].b, " ", fighter5.vd[34].b
	print "vd[34].e\t", fighter1.vd[34].e, "\t", fighter2.vd[34].e, "\t", fighter3.vd[34].e, "\t", fighter4.vd[34].e, "\t", fighter5.vd[34].e
	print "vd[45]\t", "\t", fighter1.vd[45], "\t", fighter2.vd[45], "\t", fighter3.vd[45], "\t", fighter4.vd[45], "\t", fighter5.vd[45]
	print "ld[12][0]\t", fighter1.ld[12][0], "\t\t", fighter2.ld[12][0], "\t\t", fighter3.ld[12][0], "\t\t", fighter4.ld[12][0], "\t\t", fighter5.ld[12][0]
	print "ld[12][1]\t", fighter1.ld[12][1], "\t\t", fighter2.ld[12][1], "\t\t", fighter3.ld[12][1], "\t\t", fighter4.ld[12][1], "\t\t", fighter5.ld[12][1]
	print "ld[34][0].x\t", fighter1.ld[34][0].x, "\t\t", fighter2.ld[34][0].x, "\t\t", fighter3.ld[34][0].x, "\t\t", fighter4.ld[34][0].x, "\t\t", fighter5.ld[34][0].x
	print "ld[34][0].y\t", fighter1.ld[34][0].y, "\t\t", fighter2.ld[34][0].y, "\t\t", fighter3.ld[34][0].y, "\t\t", fighter4.ld[34][0].y, "\t\t", fighter5.ld[34][0].y
	print "ld[34][0].s\t", fighter1.ld[34][0].s, fighter2.ld[34][0].s, fighter3.ld[34][0].s, fighter4.ld[34][0].s, fighter5.ld[34][0].s
	print "ld[34][0].b\t", fighter1.ld[34][0].b, " ", fighter2.ld[34][0].b, " ", fighter3.ld[34][0].b, " ", fighter4.ld[34][0].b, " ", fighter5.ld[34][0].b
	print "ld[34][0].e\t", fighter1.ld[34][0].e, "\t", fighter2.ld[34][0].e, "\t", fighter3.ld[34][0].e, "\t", fighter4.ld[34][0].e, "\t", fighter5.ld[34][0].e
	print "ld[34][1]\t", '%.3f' % fighter1.ld[34][1], "\t", '%.3f' % fighter2.ld[34][1], "\t", '%.3f' % fighter3.ld[34][1], "\t", '%.3f' % fighter4.ld[34][1], "\t", '%.3f' % fighter5.ld[34][1]
	print "fld[123][0]\t", '%.3f' % fighter1.fld[123][0], "\t", '%.3f' % fighter2.fld[123][0], "\t", '%.3f' % fighter3.fld[123][0], "\t", '%.3f' % fighter4.fld[123][0], "\t", '%.3f' % fighter5.fld[123][0]
	print "fld[123][1]\t", '%.3f' % fighter1.fld[123][1], "\t", '%.3f' % fighter2.fld[123][1], "\t", '%.3f' % fighter3.fld[123][1], "\t", '%.3f' % fighter4.fld[123][1], "\t", '%.3f' % fighter5.fld[123][1]
	print "pyl[0].pp\t", fighter1.pyl[0].pp, "\t\t", fighter2.pyl[0].pp, "\t\t", fighter3.pyl[0].pp, "\t\t", fighter4.pyl[0].pp, "\t\t", fighter5.pyl[0].pp
	print "pyl[0].ap\t", fighter1.pyl[0].ap, "\t\t", fighter2.pyl[0].ap, "\t\t", fighter3.pyl[0].ap, "\t\t", fighter4.pyl[0].ap, "\t\t", fighter5.pyl[0].ap
	print "pyl[1]\t", "\t", fighter1.pyl[1], "\t\t", fighter2.pyl[1], "\t\t", fighter3.pyl[1], "\t\t", fighter4.pyl[1], "\t\t", fighter5.pyl[1]
	print "pyl[2].pp\t", fighter1.pyl[2].pp, "\t\t", fighter2.pyl[2].pp, "\t\t", fighter3.pyl[2].pp, "\t\t", fighter4.pyl[2].pp, "\t\t", fighter5.pyl[2].pp
	print "pyl[2].ap\t", fighter1.pyl[2].ap, "\t\t", fighter2.pyl[2].ap, "\t\t", fighter3.pyl[2].ap, "\t\t", fighter4.pyl[2].ap, "\t\t", fighter5.pyl[2].ap
	print "pyd[321].pp\t", fighter1.pyd[321].pp, "\t\t", fighter2.pyd[321].pp, "\t\t", fighter3.pyd[321].pp, "\t\t", fighter4.pyd[321].pp, "\t\t", fighter5.pyd[321].pp
	print "pyd[321].ap\t", fighter1.pyd[321].ap, "\t\t", fighter2.pyd[321].ap, "\t\t", fighter3.pyd[321].ap, "\t\t", fighter4.pyd[321].ap, "\t\t", fighter5.pyd[321].ap
	print "pyd[654].pp\t", fighter1.pyd[654].pp, "\t\t", fighter2.pyd[654].pp, "\t\t", fighter3.pyd[654].pp, "\t\t", fighter4.pyd[654].pp, "\t\t", fighter5.pyd[654].pp
	print "pyd[654].ap\t", fighter1.pyd[654].ap, "\t\t", fighter2.pyd[654].ap, "\t\t", fighter3.pyd[654].ap, "\t\t", fighter4.pyd[654].ap, "\t\t", fighter5.pyd[654].ap
	print "pyd[320]\t", fighter1.pyd[320], "\t\t", fighter2.pyd[320], "\t\t", fighter3.pyd[320], "\t\t", fighter4.pyd[320], "\t\t", fighter5.pyd[320]
	print "pyd[650]\t", fighter1.pyd[650], "\t\t", fighter2.pyd[650], "\t\t", fighter3.pyd[650], "\t\t", fighter4.pyd[650], "\t\t", fighter5.pyd[650]
	print "pyv1\t", "\t", fighter1.pyv1, "\t\t", fighter2.pyv1, "\t\t", fighter3.pyv1, "\t\t", fighter4.pyv1, "\t\t", fighter5.pyv1
	print "pyv2.pp\t", "\t", fighter1.pyv2.pp, "\t\t", fighter2.pyv2.pp, "\t\t", fighter3.pyv2.pp, "\t\t", fighter4.pyv2.pp, "\t\t", fighter5.pyv2.pp
	print "pyv2.ap\t", "\t", fighter1.pyv2.ap, "\t\t", fighter2.pyv2.ap, "\t\t", fighter3.pyv2.ap, "\t\t", fighter4.pyv2.ap, "\t\t", fighter5.pyv2.ap
	print "dd[12][111]\t", fighter1.dd[12][111], "\t\t", fighter2.dd[12][111], "\t\t", fighter3.dd[12][111], "\t\t", fighter4.dd[12][111], "\t\t", fighter5.dd[12][111]
	print "dd[12][222]\t", fighter1.dd[12][222], "\t\t", fighter2.dd[12][222], "\t\t", fighter3.dd[12][222], "\t\t", fighter4.dd[12][222], "\t\t", fighter5.dd[12][222]
	print "dd[34][333].x\t", fighter1.dd[34][333].x, "\t\t", fighter2.dd[34][333].x, "\t\t", fighter3.dd[34][333].x, "\t\t", fighter4.dd[34][333].x, "\t\t", fighter5.dd[34][333].x
	print "dd[34][333].y\t", fighter1.dd[34][333].y, "\t\t", fighter2.dd[34][333].y, "\t\t", fighter3.dd[34][333].y, "\t\t", fighter4.dd[34][333].y, "\t\t", fighter5.dd[34][333].y
	print "dd[34][333].s\t", fighter1.dd[34][333].s, fighter2.dd[34][333].s, fighter3.dd[34][333].s, fighter4.dd[34][333].s, fighter5.dd[34][333].s
	print "dd[34][333].b\t", fighter1.dd[34][333].b, "\t", fighter2.dd[34][333].b, "\t", fighter3.dd[34][333].b, "\t", fighter4.dd[34][333].b, "\t", fighter5.dd[34][333].b
	print "dd[34][333].e\t", fighter1.dd[34][333].e, "\t", fighter2.dd[34][333].e, "\t", fighter3.dd[34][333].e, "\t", fighter4.dd[34][333].e, "\t", fighter5.dd[34][333].e
	print "dd[34][444]\t", fighter1.dd[34][444], "\t", fighter2.dd[34][444], "\t", fighter3.dd[34][444], "\t", fighter4.dd[34][444], "\t", fighter5.dd[34][444]
	print "fdd[123][12]\t", '%.3f' % fighter1.fdd[123][12], "\t", '%.3f' % fighter2.fdd[123][12], "\t", '%.3f' % fighter3.fdd[123][12], "\t", '%.3f' % fighter4.fdd[123][12], "\t", '%.3f' % fighter5.fdd[123][12]
	print "fdd[123][23]\t", '%.3f' % fighter1.fdd[123][23], "\t", '%.3f' % fighter2.fdd[123][23], "\t", '%.3f' % fighter3.fdd[123][23], "\t", '%.3f' % fighter4.fdd[123][23], "\t", '%.3f' % fighter5.fdd[123][23]
	print "nv\t", "\t", fighter1.nv, "\t\t", fighter2.nv, "\t\t", fighter3.nv, "\t\t", fighter4.nv, "\t\t", fighter5.nv
	print "lv[0]\t", "\t", fighter1.lv[0], "\t\t", fighter2.lv[0], "\t\t", fighter3.lv[0], "\t\t", fighter4.lv[0], "\t\t", fighter5.lv[0]
	print "lv[1]\t", "\t", fighter1.lv[1], "\t\t", fighter2.lv[1], "\t\t", fighter3.lv[1], "\t\t", fighter4.lv[1], "\t\t", fighter5.lv[1]
	print "flv[0]\t", "\t", '%.3f' % fighter1.flv[0], "\t", '%.3f' % fighter2.flv[0], "\t", '%.3f' % fighter3.flv[0], "\t", '%.3f' % fighter4.flv[0], "\t", '%.3f' % fighter5.flv[0]
	print "flv[1]\t", "\t", '%.3f' % fighter1.flv[1], "\t", '%.3f' % fighter2.flv[1], "\t", '%.3f' % fighter3.flv[1], "\t", '%.3f' % fighter4.flv[1], "\t", '%.3f' % fighter5.flv[1]
	print "dv[333]\t", "\t", fighter1.dv[333], "\t\t", fighter2.dv[333], "\t\t", fighter3.dv[333], "\t\t", fighter4.dv[333], "\t\t", fighter5.dv[333]
	print "dv[444]\t", "\t", fighter1.dv[444], "\t\t", fighter2.dv[444], "\t\t", fighter3.dv[444], "\t\t", fighter4.dv[444], "\t\t", fighter5.dv[444]
	print "fdv[333]\t", '%.3f' % fighter1.fdv[333], "\t", '%.3f' % fighter2.fdv[333], "\t", '%.3f' % fighter3.fdv[333], "\t", '%.3f' % fighter4.fdv[333], "\t", '%.3f' % fighter5.fdv[333]
	print "fdv[444]\t", '%.3f' % fighter1.fdv[444], "\t", '%.3f' % fighter2.fdv[444], "\t", '%.3f' % fighter3.fdv[444], "\t", '%.3f' % fighter4.fdv[444], "\t", '%.3f' % fighter5.fdv[444]


def _toUnicodeJson(json):
	if isinstance(json, str):
		return unicode(json)
	elif isinstance(json, list):
		return [_toUnicodeJson(v) for v in json]
	elif isinstance(json, dict):
		return {unicode(k): _toUnicodeJson(v) for k, v in json.iteritems()}
	else:
		return json


def _build(_typy):
	global Vector2, Fighter, RPG, Corpus, PyType, Empty, Vector3, SkillParam, onInitRuntime, playSoundVO_1, playSoundVO_2, BooleanList
	global _Vector2, _Fighter, _Empty
	global vPy, vPy2, fighterPy, v, v2, fighter

	f1 = Fighter(pos = Vector2())
	assert ord(RPG(f1).SerializeToString()[1]) + 2 == len(RPG(f1).SerializeToString())
	f1.pos.b = 'asdf'
	assert ord(RPG(f1).SerializeToString()[1]) + 2 == len(RPG(f1).SerializeToString())

	print vPy, vPy.x, vPy.y, vPy.s, vPy.b, vPy.e, vPy.p, vPy.i, vPy.p.pp
	print vPy2, vPy2.x, vPy2.y, vPy2.s, vPy2.b, vPy2.e, vPy2.p, vPy2.i, vPy2.p.pp

	fighterPy2 = Fighter()

	import time
	startTime = time.time()

	_Vector2.classProp.add(False)
	print "hasattr(_Fighter, 'prop')", hasattr(_Fighter, 'prop')
	print "hasattr(_Fighter(), 'prop')", hasattr(_Fighter(), 'prop')
	print "hasattr(_Vector2, 'prop')", hasattr(_Vector2, 'prop')
	print "hasattr(_Vector2(), 'prop')", hasattr(_Vector2(), 'prop')
	print _Vector2().__class__, _Vector2
	print "isinstance(_Vector2(), _Vector2)", isinstance(_Vector2(), _Vector2)
	print "isinstance(_Vector2, type)", isinstance(_Vector2, type)
	print "issubclass(_Vector2, type)", issubclass(_Vector2, type)

	fPoslPy = Fighter()
	fPoslPy.MergeFromString(Fighter(posll = [[Vector2(), Vector2(), Vector2()], [Vector2(), Vector2()]]).SerializeToString())
	assert len(fPoslPy.posll) == 2 and len(fPoslPy.posll[0]) == 3 and len(fPoslPy.posll[1]) == 2, repr(fPoslPy.posll)
	fPosl = _Fighter()
	fPosl.MergeFromString(_Fighter(posll = [[_Vector2(), _Vector2(), _Vector2()], [_Vector2(), _Vector2()]]).SerializeToString())
	assert len(fPosl.posll) == 2 and len(fPosl.posll[0]) == 3 and len(fPosl.posll[1]) == 2, repr(fPosl.posll)

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

	vDataPy = vPy.SerializeToString()
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


	import copy
	pFighterPy0 = copy.deepcopy(fighterPy)
	pFighterPy1 = Fighter()
	pFighterPy2 = Fighter()
	pFighter0 = copy.deepcopy(fighter)
	pFighter1 = _Fighter()
	pFighter2 = _Fighter()

	pDataPy = pFighterPy0.SerializeProperty('v1')
	pData = pFighter0.SerializeProperty('v1')
	assert pFighterPy1.DeserializeProperty(pDataPy) == 'v1'
	assert pFighterPy2.DeserializeProperty(pData) == 'v1'
	assert pFighter1.DeserializeProperty(pDataPy) == 'v1'
	assert pFighter2.DeserializeProperty(pData) == 'v1'
	print 'v1', pFighterPy0.v1, pFighter0.v1, pFighterPy1.v1, pFighterPy2.v1, pFighter1.v1, pFighter2.v1

	pFighterPy0.v1 = None
	pFighter0.v1 = None
	pDataPy = pFighterPy0.SerializeProperty('v1')
	pData = pFighter0.SerializeProperty('v1')
	assert pFighterPy1.DeserializeProperty(pDataPy) == 'v1'
	assert pFighterPy2.DeserializeProperty(pData) == 'v1'
	assert pFighter1.DeserializeProperty(pDataPy) == 'v1'
	assert pFighter2.DeserializeProperty(pData) == 'v1'
	print 'v1', pFighterPy0.v1, pFighter0.v1, pFighterPy1.v1, pFighterPy2.v1, pFighter1.v1, pFighter2.v1

	pDataPy = pFighterPy0.SerializeProperty('vl')
	pData = pFighter0.SerializeProperty('vl')
	assert pFighterPy1.DeserializeProperty(pDataPy) == 'vl'
	assert pFighterPy2.DeserializeProperty(pData) == 'vl'
	assert pFighter1.DeserializeProperty(pDataPy) == 'vl'
	assert pFighter2.DeserializeProperty(pData) == 'vl'
	print 'vl', pFighterPy0.vl, pFighter0.vl, pFighterPy1.vl, pFighterPy2.vl, pFighter1.vl, pFighter2.vl

	pFighterPy0.vl = None
	pFighter0.vl = None
	pDataPy = pFighterPy0.SerializeProperty('vl')
	pData = pFighter0.SerializeProperty('vl')
	assert pFighterPy1.DeserializeProperty(pDataPy) == 'vl'
	assert pFighterPy2.DeserializeProperty(pData) == 'vl'
	assert pFighter1.DeserializeProperty(pDataPy) == 'vl'
	assert pFighter2.DeserializeProperty(pData) == 'vl'
	print 'vl', pFighterPy0.vl, pFighter0.vl, pFighterPy1.vl, pFighterPy2.vl, pFighter1.vl, pFighter2.vl

	def TestSkillParam():
		_typy.Vector3(Vector3)
		_onInitRuntime = _typy.onInitRuntime()
		json = {
			'_t': 'onInitRuntime',
			'params': [
				{
					'_t': 'SkillParam',
					'origPos': {'y': 0.0, 'x': 0.0, 'z': 0.0, '_t': 'Vector3'},
				},
			],
		}

		oPy = onInitRuntime.FromJson(json)
		o = _onInitRuntime.FromJson(json)
		oDataPy = oPy.SerializeToString()
		oData = o.SerializeToString()
		print len(oDataPy), [ord(c) for c in oDataPy]
		print len(oData), [ord(c) for c in oData]

		o.ParseFromString(oData)

	TestSkillParam()

	print copy.deepcopy(fighterPy).vd.pop(12), copy.deepcopy(fighter).vd.pop(12)
	print len(fighterPy.SerializeToString()), len(copy.deepcopy(fighterPy).SerializeToString())
	print len(fighter.SerializeToString()), len(copy.deepcopy(fighter).SerializeToString())
	print fighterPy.vd.pop(4567, None), fighter.vd.pop(4567, 'NULL')
	print fighterPy.posi.pop(4567, 'NULL'), fighter.posi.pop(4567, None)
	print 4567 in fighterPy.posi, 4567 in fighter.posi
	print fighterPy.vd.items(), fighter.vd.items()

	print Fighter.FromJson(fighter.Json()).vd.pop(12), _Fighter.FromJson(fighterPy.Json()).vd.pop(12)
	print len(fighter.SerializeToString()), len(Fighter.FromJson(_toUnicodeJson(fighter.Json(True))).SerializeToString())
	print len(fighterPy.SerializeToString()), len(_Fighter.FromJson(_toUnicodeJson(fighterPy.Json(True))).SerializeToString())
	_printFighters(fighterPy, Fighter.FromJson(fighterPy.Json()), Fighter.FromJson(fighter.Json()), _Fighter.FromJson(fighterPy.Json()), _Fighter.FromJson(fighter.Json()))

	fighterPy.vd = fighterPy.sd
	fighter.vd = fighter.sd
	fighter.vd.update(fighter.sd)
	print fighterPy.vd[321], fighterPy.vd[231], fighter.vd[321], fighter.vd[231]

	print 'fighterPy.Json(True)', Fighter().Json(True)
	print 'fighter.Json(True)', _Fighter().Json(True)

	assert Vector2(x = 123, y = 1) in {Vector2(x = 123, y = 2): None}
	assert _Vector2(x = 123, y = 1) in {_Vector2(x = 123, y = 2): None}

	print fighterPy.vd.setdefault(321, None), fighterPy.vd.setdefault(111, 987.654), fighterPy.vd[111]
	print fighter.vd.setdefault(321, None), fighter.vd.setdefault(111, 987.654), fighter.vd[111]

	_playSoundVO_1 = _typy.playSoundVO_1()
	_playSoundVO_2 = _typy.playSoundVO_2()
	playSoundVO_2().ParseFromString(playSoundVO_1(interval = 1, path = 'asdf').SerializeToString())
	_playSoundVO_2().ParseFromString(_playSoundVO_1(interval = 1, path = 'asdf').SerializeToString())

	setattr(fighterPy, 'nv', None)
	setattr(fighter, 'nv', None)
	assert type(fighterPy.nv) is type(None)
	assert type(fighter.nv) is type(None)

	print 'fighterPy.vl + fighterPy2.vl', fighterPy.vl + fighterPy2.vl
	print 'fighter.vl + fighter2.vl', fighter.vl + fighter2.vl
	print 'Fighter().vl + Fighter().vl', Fighter().vl + Fighter().vl
	print '_Fighter().vl + _Fighter().vl', _Fighter().vl + _Fighter().vl

	del fighterPy.vd[321]
	del fighter.vd[321]
	del fighterPy.vl[0]
	del fighter.vl[0]

	print copy.copy(fighterPy)
	print copy.copy(fighter)
	print copy.copy(fighterPy.vl)
	print copy.copy(fighter.vl)
	print copy.copy(fighterPy.vd)
	print copy.copy(fighter.vd)
	print copy.deepcopy(fighterPy.vl)
	print copy.deepcopy(fighter.vl)
	print copy.deepcopy(fighterPy.vd)
	print copy.deepcopy(fighter.vd)

	assert Fighter().pos is None
	assert _Fighter().pos is None

	print list(fighterPy.pyl)
	print list(fighter.pyl)

	vFpPy = Vector2()
	vFp = _Vector2()
	assert vFpPy.DeserializeProperty(Vector2(y = 123.4).SerializeProperty('y')) == 'y' and vFpPy.y == 123.4
	assert vFpPy.DeserializeProperty(_Vector2(y = 123.4).SerializeProperty('y')) == 'y' and vFpPy.y == 123.4
	assert vFp.DeserializeProperty(Vector2(y = 123.4).SerializeProperty('y')) == 'y' and vFp.y == 123.4
	assert vFp.DeserializeProperty(_Vector2(y = 123.4).SerializeProperty('y')) == 'y' and vFp.y == 123.4
	assert vFpPy.DeserializeProperty(Vector2().SerializeProperty('y')) == 'y' and vFpPy.y == -10
	assert vFpPy.DeserializeProperty(_Vector2().SerializeProperty('y')) == 'y' and vFpPy.y == -10
	assert vFp.DeserializeProperty(Vector2().SerializeProperty('y')) == 'y' and vFp.y == -10
	assert vFp.DeserializeProperty(_Vector2().SerializeProperty('y')) == 'y' and vFp.y == -10

	fR = _Fighter(posl = [_Vector2(x = 123), _Vector2(x = 456), _Vector2(x = 789)])
	print [p.x for p in fR.posl]
	fR.posl.remove(_Vector2(x = 456))
	print [p.x for p in fR.posl]
	fR.posl.append(_Vector2(x = 456))
	print [p.x for p in fR.posl]

	_BooleanList = _typy.BooleanList()
	assert str(BooleanList(bl = [False, False, True]).bl) == '[False, False, True]'
	assert str(_BooleanList(bl = [False, False, True]).bl) == '[False, False, True]'

	return time.time() - startTime

cpptime = 0
cpytime = 0

def test_cpp():
	import os
	import test_case
	from typy import GenerateExtention
	global SETUP_TYPYC
	SETUP_TYPYC and GenerateExtention('%s/typy' % os.path.abspath(os.path.dirname(__file__)))
	from typy import _typy
	import test_case
	test_case.setup(globals(), _typy)
	global cpptime, cpytime
	cpptime = _build(_typy)
	print 'time', cpptime, cpytime


def test_cpy():
	import os
	import test_case
	from typy import GenerateDescriptor
	GenerateDescriptor(os.path.dirname(__file__))
	import _typy
	import test_case
	test_case.setup(globals(), _typy)
	global cpptime, cpytime
	cpytime = _build(_typy)
	print 'time', cpptime, cpytime
