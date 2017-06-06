#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

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
		'Float': 'f',
		'Double': 'd',
		'FixedPoint': 'fp',
		'Boolean': 'b',
		'String': 's',
		'Bytes': 'by',
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

def Increment(path, proto, ignore):
	pass
