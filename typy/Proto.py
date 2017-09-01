#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

SymbolDecodeMap = "-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"
SymbolEncodeMap = {}
for i, c in enumerate(SymbolDecodeMap):
	SymbolEncodeMap[c] = i

def SymbolEncodedLen(src):
	return ((len(src) * 6 + 7) / 8)

def EncodeSymbol(src):
	dst = ''
	n = len(src) / 4 * 4
	for si in xrange(0, n, 4):
		# Convert 4x 6bit source bytes into 3 bytes
		v = (SymbolEncodeMap[src[si + 0]] << 18) | \
			(SymbolEncodeMap[src[si + 1]] << 12) | \
			(SymbolEncodeMap[src[si + 2]] << 6) | \
			(SymbolEncodeMap[src[si + 3]] << 0)

		dst += chr(0xFF & (v >> 16))
		dst += chr(0xFF & (v >> 8))
		dst += chr(0xFF & (v >> 0))

	v = 0
	for j in xrange(len(src) - n):
		v |= SymbolEncodeMap[src[n + j]] << (18 - j * 6)
	for j in xrange(len(src) - n):
		dst += chr(0xFF & (v >> (16 - j * 8)))

	return dst

def DecodeSymbol(src):
	dst = ''
	n = len(src) / 3 * 3
	for si in xrange(0, n, 3):
		# Convert 3x 8bit source bytes into 4 bytes
		v = (ord(src[si + 0]) << 16) | \
			(ord(src[si + 1]) << 8) | \
			(ord(src[si + 2]) << 0)

		dst += SymbolDecodeMap[0x3F & (v >> 18)]
		dst += SymbolDecodeMap[0x3F & (v >> 12)]
		dst += SymbolDecodeMap[0x3F & (v >> 6)]
		dst += SymbolDecodeMap[0x3F & (v >> 0)]

	if len(src) - n == 1:
		dst += SymbolDecodeMap[0x3F & (ord(src[n]) >> 2)]
	elif len(src) - n == 2:
		v = (ord(src[n + 0]) << 8) | \
			(ord(src[n + 1]) << 0)
		dst += SymbolDecodeMap[0x3F & (v >> 10)]
		dst += SymbolDecodeMap[0x3F & (v >> 4)]

	if dst[-1] == SymbolDecodeMap[0]:
		dst = dst[:-1]
	return dst

# [joungtao] The field-number/tag in protobuf is auto generated by this sorting function.
#	The sorted field sequence of the encoding and decoding sides must match.
def SortedMessage(d, sequence):
	sequence = sequence or ()
	return [(k, d[k]) for k in sequence] + sorted([(k, v) for k, v in d.iteritems() if k not in sequence], key = lambda (k, v): k)


def CompareWrite(path, content):
	import os
	import codecs
	if os.path.isfile(path):
		with codecs.open(path, 'r', 'utf-8') as f:
			if f.read() == content:
				return
	with codecs.open(path, 'w', 'utf-8') as f:
		f.write(content)


def ShortName(prefix, name):
	import hashlib
	import base64
	if len(name) > 25:
		name = prefix + base64.b64encode(hashlib.md5(name).digest())[:-2].replace('+', '__').replace('/', '_')
	return name


def RecordNesting(prefix, types):
	from Type import toType, isEnum, List, Dict, Instance
	shortName = {
		'Integer': 'i',
		'Long': 'l',
		'Float': 'f',
		'Double': 'd',
		'FixedPoint': 'fp',
		'Boolean': 'b',
		'String': 's',
		'Bytes': 'by',
		'Symbol': 'sy',
		'Enum': 'e',
	}
	properties = {'Enum' if isEnum(p) else p.__name__: toType(p) for p in types if p is not None}
	names = sorted([shortName[k] for k in properties if k in shortName])
	for name in sorted([k for k in properties if k not in shortName]):
		prop = properties[name]
		if isinstance(prop, Instance):
			if len(prop.pyType) == 1 and not isinstance(prop.pyType[0], List):
				names.append(prop.pyType[0].__name__)
			else:
				names.append(RecordNesting('V', prop.pyType)[0])
		elif isinstance(prop, List):
			names.append(RecordNesting('L', [prop.elementType])[0])
		elif isinstance(prop, Dict):
			names.append(RecordNesting('D', [prop.keyType, prop.valueType])[0])
		else:
			names.append(name)
	name = ShortName(prefix, '%s%s' % (prefix, ''.join(names)))
	return name, properties


def ClearTypes():
	from Object import MetaObject
	MetaObject.Objects.clear()
	from typy.Enum import MetaEnum
	MetaEnum.Enums.clear()
	import Type
	Type.PythonTypes.clear()


class TypeObject(object): pass


def _ProtoFormat(t, *args):
	args = args + tuple(('%s = %s' % (k, repr(v)) for k, v in sorted(t.metadata.iteritems())))
	return '.'.join(['typy'] + sorted([k.__name__ for k in t.____keywords__]) + [t.__class__.__name__]) + ('(%s)' % ', '.join(args))


def _GetProtoFromTypy(p, codes, types):
	from Object import MetaObject
	import Type
	from Type import toType, Enum, Integer, Long, Float, Double, Boolean, String, Bytes, Symbol
	from Type import Instance, List, Dict, FixedPoint, Python
	if isinstance(p, Enum):
		if p.pyType.__name__ not in types:
			values = []
			for i, e in sorted(p.pyType.__enum__.iteritems()):
				values.append("""
	('%s', %d),""" % (e.name, i))
			codes.append("""
%s = typy.Proto.Enum('%s', %s
)""" % (p.pyType.__name__, p.pyType.__name__, ''.join(values)))
			types.add(p.pyType.__name__)
		return _ProtoFormat(p, p.pyType.__name__)
	elif isinstance(p, (Integer, Long, Float, Double, Boolean, String, Bytes, Symbol)):
		return _ProtoFormat(p)
	elif isinstance(p, FixedPoint):
		return _ProtoFormat(p, '%d, %d' % (p.precision, p.floor))
	elif isinstance(p, Python):
		if p.pyType.__name__ not in types:
			codes.append("""
%s = typy.Proto.Python('%s')""" % (p.pyType.__name__, p.pyType.__name__))
			types.add(p.pyType.__name__)
		return _ProtoFormat(p, p.pyType.__name__)
	elif isinstance(p, Instance):
		pyType = []
		for t in p.pyType:
			if t is None:
				pyType.append('None')
			elif t.__name__ in MetaObject.Objects:
				pyType.append(t.__name__)
				if t.__name__ not in types:
					_GenerateObject(t.__name__, MetaObject.Objects[t.__name__], codes, types)
			elif not isinstance(t, type) or (isinstance(t, type) and issubclass(t, (Type.Type, Type._Enum))):
				pyType.append(_GetProtoFromTypy(toType(t), codes, types))
			else:
				obj = TypeObject()
				obj.__name__ = 'PyObject'
				pyType.append(_GetProtoFromTypy(Python(obj), codes, types))
		return _ProtoFormat(p, *sorted(pyType))
	elif isinstance(p, List):
		return _ProtoFormat(p, _GetProtoFromTypy(p.elementType, codes, types))
	elif isinstance(p, Dict):
		return _ProtoFormat(p, _GetProtoFromTypy(p.keyType, codes, types), _GetProtoFromTypy(p.valueType, codes, types))


def _GenerateObject(name, cls, codes, types):
	fields = []
	sortedProperties = SortedMessage(cls.____properties__, getattr(cls, '____propertySequence__', None))
	for a, p in sortedProperties:
		fields.append("""
	('%s', %s),""" % (a, _GetProtoFromTypy(p, codes, types)))

	codes.append("""
%s = typy.Proto.Object('%s',
	%s,
	%s,%s
)""" % (name, name, repr(cls.__module__), repr(cls.____parents__), ''.join(fields)))
	types.add(name)


depends = {}
def _RecordDepends(obj, dict1, dict2):
	global depends
	for p in obj.____parents__:
		if p in dict1:
			parent = dict1[p]
		elif p in dict2:
			parent = dict2[p]
		else:
			continue
		if parent.__module__ == obj.__module__:
			continue
		modules = depends.setdefault(parent.__module__, set())
		modules.add(obj.__module__)

def _ResolveDepends(imported):
	global depends
	resolved = set()
	added = imported
	while added:
		new_added = set()
		for m in added:
			if m not in depends:
				continue
			for d in depends[m]:
				if d in resolved:
					continue
				resolved.add(d)
				new_added.add(d)
		added = new_added
	return resolved - imported


def Object(name, module, parents, *fields):
	obj = TypeObject()
	obj.isObject = True
	obj.__name__ = name
	obj.__module__ = module
	obj.____parents__ = parents
	obj.____propertySequence__ = [a for a, _ in fields]
	obj.____properties__ = properties = {}
	for a, p in fields:
		properties[a] = p
		setattr(obj, a, p)
	return obj


def Python(name):
	obj = TypeObject()
	obj.__name__ = name
	return obj

def Enum(name, *fields):
	obj = TypeObject()
	obj.isEnum = True
	obj.__name__ = name
	obj.__enum__ = {}
	for a, p in fields:
		obj.__enum__[p] = TypeObject()
		obj.__enum__[p].name = a
	return obj


def Increment(path, proto_file, ignore):
	import os
	import sys
	import imp
	import codecs

	import Type
	Type.PythonTypes.clear()
	path = path.replace('\\', '/')
	if os.path.isfile(proto_file):
		with codecs.open(proto_file, 'r', 'utf-8') as f:
			proto = imp.new_module('typy.proto')
			proto.__package__ = 'typy'
			try:
				exec str(f.read()) in proto.__dict__
				for obj in proto.__dict__.itervalues():
					if hasattr(obj, "____parents__"):
						_RecordDepends(obj, proto.__dict__, {})
			except:
				proto.timestamps = {}
	else:
		class _Proto(object): pass
		proto = _Proto()
		proto.timestamps = {}

	imported = set()
	proto.hasIncrement = False
	def _scanScripts(sub):
		for i in os.listdir('%s/%s' % (path, sub)):
			sys.stdout.write('.')
			sys.stdout.flush()
			p = '%s/%s' % (sub, i) if sub else i
			fp = '%s/%s' % (path, p)
			if p in ignore or p[:-1] in ignore:
				continue
			elif os.path.isdir(fp):
				_scanScripts(p)
			elif fp in proto.timestamps and str(os.stat(fp).st_mtime) == proto.timestamps[fp]:
				continue
			elif i.endswith(('.py', '.pyc')):
				n = i.rpartition('.')[0]
				if not n.replace('_', '').isalnum() or n == '__init__' or (i.endswith('.pyc') and os.path.isfile(fp[:-1])) or \
					(sub and not os.path.isfile('%s/%s/__init__.py' % (path, sub)) and not os.path.isfile('%s/%s/__init__.pyc' % (path, sub))):
					continue
				proto.timestamps[fp] = str(os.stat(fp).st_mtime)
				m = n if not sub else '%s.%s' % (sub.replace('/', '.'), n)
				print '\n[Typy] Incremental proto:', m
				__import__(m)
				imported.add(m)
				proto.hasIncrement = True

	if os.path.isfile(path) and path.endswith('.py'):
		with codecs.open(path, 'r', 'utf-8') as f:
			exec str(f.read()) in {}
		proto.timestamps[path] = str(os.stat(path).st_mtime)
		proto.hasIncrement = True
	elif os.path.isdir(path):
		_scanScripts('')

	from Object import MetaObject
	for obj in MetaObject.Objects.itervalues():
		_RecordDepends(obj, MetaObject.Objects, proto.__dict__)
	for m in _ResolveDepends(imported):
		print '\n[Typy] Dependent proto:', m
		__import__(m)
		imported.add(m)
		proto.hasIncrement = True

	for n, c in MetaObject.Objects.iteritems():
		setattr(proto, n, c)
	for n in dir(proto):
		c = getattr(proto, n)
		if hasattr(c, '____properties__') and (not isinstance(c, TypeObject) or getattr(c, 'isObject', False)):
			MetaObject.Objects[n] = c

	if not proto.hasIncrement:
		return False

	codes = []
	types = set()
	for name, cls in sorted(MetaObject.Objects.iteritems(), key = lambda (k, v): k):
		if name not in types:
			_GenerateObject(name, cls, codes, types)
	CompareWrite(proto_file, TYPY_PROTO__ % ('\n\t'.join(['\'%s\': \'%s\',' % (p, t) for p, t in sorted(proto.timestamps.iteritems())]), ''.join(codes)))

	return True


TYPY_PROTO__ = ur"""#-*- coding: utf-8 -*-
# Generated by the typy Proto.Increment.  DO NOT EDIT!

import typy

timestamps = {
	%s
}
%s
"""
