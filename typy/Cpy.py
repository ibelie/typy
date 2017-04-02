#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

from Cpp import _RecordVariant

TYPE_ENUM       = 0
TYPE_INT32      = 1
TYPE_INT64      = 2
TYPE_UINT32     = 3
TYPE_UINT64     = 4
TYPE_DOUBLE     = 5
TYPE_FLOAT      = 6
TYPE_BOOL       = 7
TYPE_BYTES      = 8
TYPE_STRING     = 9
TYPE_OBJECT     = 10
TYPE_VARIANT    = 11
TYPE_LIST       = 12
TYPE_DICT       = 13
TYPE_FIXEDPOINT = 14
TYPE_PYTHON     = 15
MAX_FIELD_TYPE  = 16

def _GetCpyFromTypy(p, nesting = False):
	return 1, 2, 'True'
	from Object import MetaObject
	from Type import pb, Enum, Simple, Instance, List, Dict, Collection, FixedPoint, Python
	from typy.google.protobuf.internal import wire_format
	if isinstance(p, Enum):
		enums[p.pyType.__name__] = p.pyType
		ref_types.add('#include "%s.h"' % _shortName(p.pyType.__name__))
		return p.pyType.__name__, '', p.pyType.__name__
	elif isinstance(p, FixedPoint):
		fixedpoint = 'SINGLE_ARG(FixedPoint<%d, %d>)' % (p.precision, p.floor)
		return fixedpoint, '', fixedpoint
	elif isinstance(p, Simple):
		return p.pbType, '', p.pbType
	elif isinstance(p, Python):
		pythons.add(p.pyType.__name__)
		ref_types.add('#include "%s.h"' % _shortName(p.pyType.__name__))
		return 'Python<Shadow_%s>' % p.pyType.__name__, '*', 'Python<Shadow_%s>' % p.pyType.__name__
	elif isinstance(p, Instance):
		if len(p.pyType) == 1 and p.pyType[0].__name__ in MetaObject.Objects:
			ref_types.add('#include "%s.h"' % _shortName(p.pyType[0].__name__))
			return p.pyType[0].__name__, '*', p.pyType[0].__name__
		elif len(p.pyType) < 1 or (not nesting and pb not in p.____keywords__):
			pass
		else:
			variant = _RecordVariant(p.pyType, variants)
			ref_types.add('#include "%s.h"' % variant)
			return variant, '*', 'Variant(%s)' % ', '.join([k for k, _ in variants[variant].iteritems()])
		return 'Python<PyObject>', '*', 'Python<PyObject>'
	elif isinstance(p, List):
		if isinstance(p.elementType, Collection):
			print "[Cpp] Warning: List element can not be Collection type."
		name, _, _ = _GetCppFromTypy(p.elementType, enums, pythons, variants, ref_types, container_inits, True)
		container_inits.add('PyType_Ready(&List< %s >::_Type) >= 0 && PyType_Ready(&List< %s >::Iterator_Type) >= 0' % (name, name))
		return 'List< %s >' % name, '*', 'List(%s)' % name
	elif isinstance(p, Dict):
		if not isinstance(p.keyType, Simple):
			print "[Cpp] Warning: Dict key must be Simple type."
		kName, _, _ = _GetCppFromTypy(p.keyType, enums, pythons, variants, ref_types, container_inits, True)
		vName, _, _ = _GetCppFromTypy(p.valueType, enums, pythons, variants, ref_types, container_inits, True)
		container_inits.add('PyType_Ready(&Dict<%s, %s >::_Type) >= 0 && PyType_Ready(&Dict<%s, %s >::IterKey_Type) >= 0 && PyType_Ready(&Dict<%s, %s >::IterItem_Type) >= 0' % (kName, vName, kName, vName, kName, vName))
		return 'SINGLE_ARG(Dict<%s, %s >)' % (kName, vName), '*', 'Dict(%s -> %s)' % (kName, vName)


def _GenerateObject(name, cls, codes, variants):
	from typy.google.protobuf.internal.encoder import _TagSize
	from typy.google.protobuf.internal.wire_format import PackTag
	from Object import SortedMessage
	from Type import pb

	fields = []
	sortedProperties = SortedMessage(cls.____properties__)
	for i, (a, p) in enumerate(sortedProperties):
		if pb not in p.____keywords__: continue
		wire_type, field_type, typy_type = _GetCpyFromTypy(p)
		fields.append('("%s", %d, %d, %d, %d, %s),' % ((a, PackTag(i + 1, wire_type), _TagSize(i + 1), wire_type, field_type, typy_type)))
	for i, (a, p) in enumerate(sortedProperties):
		if pb in p.____keywords__: continue
		wire_type, field_type, typy_type = _GetCpyFromTypy(p)
		fields.append('("%s", %d, %d, %d, %d, %s),' % ((a, 0, 0, wire_type, field_type, typy_type)))

	codes.append("""
%s = _typyd.Object('%s', (
	%s
))
print %s, %s()
""" % (name, name, '\n\t'.join(fields), name, name))


def GenerateDescriptor(_typyDir = None):
	import os
	import codecs
	if _typyDir is None:
		_typyDir = os.path.dirname(__file__)
	path = os.path.abspath(_typyDir) + '/_typy.py'

	from Object import MetaObject
	codes = []
	variants = {}
	for name, cls in MetaObject.Objects.iteritems():
		_GenerateObject(name, cls, codes, variants)


	with codecs.open(path, 'w', 'utf-8') as f:
		f.write(TYPY_PY__ % ''.join(codes))

TYPY_PY__ = ur"""#-*- coding: utf-8 -*-
# Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

import _typyd
setDefaultEncodingUTF8 = _typyd.setDefaultEncodingUTF8

asdf = _typyd.Variant('asdf', (
	(1, 2, 3),
))
print asdf, asdf()

Corpus = _typyd.Enum('Corpus')
print Corpus

%s
"""
