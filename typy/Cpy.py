#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

from Proto import RecordNesting

TYPE_ENUM       = 0
TYPE_INT32      = 1
TYPE_INT64      = 2
TYPE_UINT32     = 3
TYPE_UINT64     = 4
TYPE_FIXEDPOINT = 5
TYPE_DOUBLE     = 6
TYPE_FLOAT      = 7
TYPE_BOOL       = 8
TYPE_BYTES      = 9
TYPE_STRING     = 10
TYPE_OBJECT     = 11
TYPE_VARIANT    = 12
TYPE_LIST       = 13
TYPE_DICT       = 14
TYPE_PYTHON     = 15
MAX_FIELD_TYPE  = 16

def _GenerateVariant(name, properties, codes, types):
	from typy.google.protobuf.internal.encoder import _TagSize
	from typy.google.protobuf.internal.wire_format import PackTag
	from Object import SortedMessage
	from Type import Type

	fields = []
	for i, (a, p) in enumerate(SortedMessage(properties, None)):
		if not isinstance(p, Type):
			print "[Cpp] Warning: Variant type expect Typy, but get %s %s." % (a, p)
		wire_type, field_type, typy_type = _GetCpyFromTypy(p, codes, types)
		fields.append("""
	('%s', %d, %d, %d, %d, %s),""" % ((a, PackTag(i + 1, wire_type), _TagSize(i + 1), wire_type, field_type, typy_type)))

	codes.append("""
%s = _typyd.Variant('%s', (%s
))""" % (name, name, ''.join(fields)))
	types.add(name)

def _GetCpyFromTypy(p, codes, types, nesting = False):
	from Object import MetaObject
	from Type import pb, Enum, Integer, Float, Double, Boolean, String, Bytes
	from Type import Instance, List, Dict, FixedPoint, Python
	from typy.google.protobuf.internal import wire_format
	if isinstance(p, Enum):
		if p.pyType.__name__ not in types:
			codes.append("""
%s = _typyd.Enum('%s')""" % (p.pyType.__name__, p.pyType.__name__))
			types.add(p.pyType.__name__)
		return wire_format.WIRETYPE_VARINT, TYPE_ENUM, p.pyType.__name__
	elif isinstance(p, FixedPoint):
		fixedpoint = 'FixedPoint_%d_%s%d' % (p.precision, '' if p.floor > 0 else 's', p.floor if p.floor > 0 else -p.floor)
		if fixedpoint not in types:
			codes.append("""
%s = _typyd.FixedPoint(%d, %d)""" % (fixedpoint, p.precision, p.floor))
			types.add(fixedpoint)
		return wire_format.WIRETYPE_VARINT, TYPE_FIXEDPOINT, fixedpoint
	elif isinstance(p, Python):
		if p.pyType.__name__ not in types:
			codes.append("""
%s = _typyd.Python('%s')""" % (p.pyType.__name__, p.pyType.__name__))
			types.add(p.pyType.__name__)
		return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_PYTHON, p.pyType.__name__
	elif isinstance(p, Integer):
		return wire_format.WIRETYPE_VARINT, TYPE_INT32, ''
	elif isinstance(p, Float):
		return wire_format.WIRETYPE_FIXED32, TYPE_FLOAT, ''
	elif isinstance(p, Double):
		return wire_format.WIRETYPE_FIXED64, TYPE_DOUBLE, ''
	elif isinstance(p, Boolean):
		return wire_format.WIRETYPE_VARINT, TYPE_BOOL, ''
	elif isinstance(p, String):
		return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_STRING, ''
	elif isinstance(p, Bytes):
		return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_BYTES, ''
	elif isinstance(p, Instance):
		if len(p.pyType) == 1 and p.pyType[0].__name__ in MetaObject.Objects:
			if p.pyType[0].__name__ not in types:
				_GenerateObject(p.pyType[0].__name__, MetaObject.Objects[p.pyType[0].__name__], codes, types)
			return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_OBJECT, p.pyType[0].__name__
		elif len(p.pyType) < 1 or (not nesting and pb not in p.____keywords__):
			pass
		else:
			variant, properties = RecordNesting('V', p.pyType)
			if variant not in types:
				_GenerateVariant(variant, properties, codes, types)
				types.add(variant)
			return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_VARIANT, variant
		if 'PyObject' not in types:
			codes.append("""
PyObject = _typyd.Python('PyObject')""")
			types.add('PyObject')
		return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_PYTHON, 'PyObject'
	elif isinstance(p, List):
		list_type, _ = RecordNesting('L', [p.elementType])
		if list_type not in types:
			wire_type, field_type, typy_type = _GetCpyFromTypy(p.elementType, codes, types, True)
			codes.append("""
%s = _typyd.List('%s', (%d, %d, %s))""" % (list_type, list_type, wire_type, field_type, typy_type))
			types.add(list_type)
		return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_LIST, list_type
	elif isinstance(p, Dict):
		dict_type, _ = RecordNesting('D', [p.keyType, p.valueType])
		if dict_type not in types:
			key_wire_type, key_field_type, key_typy_type = _GetCpyFromTypy(p.keyType, codes, types, True)
			value_wire_type, value_field_type, value_typy_type = _GetCpyFromTypy(p.valueType, codes, types, True)
			codes.append("""
%s = _typyd.Dict('%s', (%d, %d, %s), (%d, %d, %s))""" % (dict_type, dict_type,
				key_wire_type, key_field_type, key_typy_type,
				value_wire_type, value_field_type, value_typy_type))
			types.add(dict_type)
		return wire_format.WIRETYPE_LENGTH_DELIMITED, TYPE_DICT, dict_type


def _GenerateObject(name, cls, codes, types):
	from typy.google.protobuf.internal.encoder import _TagSize
	from typy.google.protobuf.internal.wire_format import PackTag
	from Object import SortedMessage
	from Type import pb

	fields = []
	sortedProperties = SortedMessage(cls.____properties__, getattr(cls, '____propertySequence__', None))
	tag = 0
	for a, p in sortedProperties:
		if pb not in p.____keywords__: continue
		tag += 1
		wire_type, field_type, typy_type = _GetCpyFromTypy(p, codes, types)
		fields.append("""
	('%s', %d, %d, %d, %d, %s),""" % ((a, PackTag(tag, wire_type), _TagSize(tag), wire_type, field_type, typy_type)))
	for a, p in sortedProperties:
		if pb in p.____keywords__: continue
		wire_type, field_type, typy_type = _GetCpyFromTypy(p, codes, types)
		fields.append("""
	('%s', %d, %d, %d, %d, %s),""" % ((a, 0, 0, wire_type, field_type, typy_type)))

	codes.append("""
%s = _typyd.Object('%s', (%s
))""" % (name, name, ''.join(fields)))
	types.add(name)


def GenerateDescriptor(_typyDir = None):
	import os
	import codecs
	if _typyDir is None:
		_typyDir = os.path.dirname(__file__)
	path = os.path.abspath(_typyDir) + '/_typy.py'

	from Object import MetaObject
	codes = []
	types = set()
	for name, cls in sorted(MetaObject.Objects.iteritems(), key = lambda (k, v): k):
		if name not in types:
			_GenerateObject(name, cls, codes, types)

	content = TYPY_PY__ % ''.join(codes)
	if os.path.isfile(path):
		with codecs.open(path, 'r', 'utf-8') as f:
			if f.read() == content:
				return
	with codecs.open(path, 'w', 'utf-8') as f:
		f.write(content)

TYPY_PY__ = ur"""#-*- coding: utf-8 -*-
# Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

import _typyd
setDefaultEncodingUTF8 = _typyd.setDefaultEncodingUTF8
%s
"""
