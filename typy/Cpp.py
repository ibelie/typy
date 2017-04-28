#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

SWITCH_GENERATE_ALL = True
SWITCH_GENERATE_VARIANT = SWITCH_GENERATE_ALL and True
SWITCH_GENERATE_OBJECT = SWITCH_GENERATE_ALL and True
SWITCH_GENERATE_ENUM = SWITCH_GENERATE_ALL and True
SWITCH_DELETE_CPP = False

def _VariantSetter(properties):
	from Object import MetaObject
	from Type import Enum, List, Dict, Python, Integer, Float, Double, FixedPoint, Boolean, String, Bytes, Instance
	from_py_fields = []

	def _fromNumberPy(tag, p):
		if isinstance(p, Enum):
			return '::typy::CopyFrom(_value%d, static_cast<%s>(PyInt_AsLong(value)));' % (tag, p.pyType.__name__)
		elif isinstance(p, Integer):
			return '::typy::CopyFrom(_value%d, PyInt_AsLong(value));' % tag
		elif isinstance(p, Boolean):
			return '::typy::CopyFrom(_value%d, PyInt_AsLong(value) == 0 ? false : true);' % tag
		elif isinstance(p, Float):
			return '::typy::CopyFrom(_value%d, static_cast<float>(PyFloat_AsDouble(value)));' % tag
		elif isinstance(p, (Double, FixedPoint)):
			return '::typy::CopyFrom(_value%d, PyFloat_AsDouble(value));' % tag

	typeDict = {t: (i + 1, p, typ, info) for i, (t, p, typ, info) in enumerate(properties)}

	for t in ('Boolean', 'Integer', 'Enum', 'Double', 'Float', 'FixedPoint'):
		if t not in typeDict: continue
		tag, p, _, _ = typeDict[t]
		from_py_fields.append("""
	if (PyBool_Check(value)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		%s
		_tag = %d;
		return true;
	}""" % (tag, _fromNumberPy(tag, p), tag))
		break

	for t in ('Integer', 'Enum', 'Boolean', 'Double', 'Float', 'FixedPoint'):
		if t not in typeDict: continue
		tag, p, _, _ = typeDict[t]
		from_py_fields.append("""
	if (PyInt_Check(value) || PyLong_Check(value)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		%s
		_tag = %d;
		return true;
	}""" % (tag, _fromNumberPy(tag, p), tag))
		break

	for t in ('Double', 'Float', 'FixedPoint', 'Integer', 'Enum', 'Boolean'):
		if t not in typeDict: continue
		tag, p, _, _ = typeDict[t]
		from_py_fields.append("""
	if (PyFloat_Check(value)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		%s
		_tag = %d;
		return true;
	}""" % (tag, _fromNumberPy(tag, p), tag))
		break

	for t in ('String', 'Bytes'):
		if t not in typeDict: continue
		tag, p, _, _ = typeDict[t]
		copyFrom = '::typy::CopyFrom(_value%d, reinterpret_cast<string>(value));' % tag
		if isinstance(p, Bytes):
			copyFrom = """
		ScopedPyObjectPtr s(_DecodeString(reinterpret_cast<string>(value)));
		if (s == NULL) { return false; }
		::typy::CopyFrom(_value%d, reinterpret_cast<bytes>(s.get()));""" % tag
		from_py_fields.append("""
	if (PyUnicode_Check(value)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		%s
		_tag = %d;
		return true;
	}""" % (tag, copyFrom, tag))
		break

	for t in ('Bytes', 'String'):
		if t not in typeDict: continue
		tag, p, _, _ = typeDict[t]
		copyFrom = '::typy::CopyFrom(_value%d, reinterpret_cast<bytes>(value));' % tag
		if isinstance(p, String):
			copyFrom = """
		ScopedPyObjectPtr s(PyUnicode_FromEncodedObject(value, "utf-8", NULL));
		if (s == NULL) { return false; }
		::typy::CopyFrom(_value%d, reinterpret_cast<string>(s.get()));""" % tag
		from_py_fields.append("""
	if (PyBytes_Check(value)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		%s
		_tag = %d;
		return true;
	}""" % (tag, copyFrom, tag))
		break

	for tag, p, typ, info in typeDict.itervalues():
		if isinstance(p, Instance) and len(p.pyType) == 1 and p.pyType[0].__name__ in MetaObject.Objects:
			from_py_fields.append("""
	if (PyObject_TypeCheck(value, &Object<%s>::_Type)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		::typy::CopyFrom(_value%d, static_cast<%s*>(value));
		_tag = %d;
		return true;
	}""" % (p.pyType[0].__name__, tag, tag, p.pyType[0].__name__, tag))
		elif isinstance(p, Python):
			from_py_fields.append("""
	if (PyObject_TypeCheck(value, Python<Shadow_%s>::_Type)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		if (_value%d == NULL) { _value%d = new Python<Shadow_%s>; }
		else { _value%d->Clear(); }
		Py_INCREF(value);
		_value%d->object = value;
		_tag = %d;
		return true;
	}""" % (p.pyType.__name__, tag, tag, tag, p.pyType.__name__, tag, tag, tag))
		elif isinstance(p, List):
			from_py_fields.append("""
	if (PyObject_TypeCheck(value, &%s::_Type) || PySequence_Check(value)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		if (::typy::CheckAndSet(value, _value%d, "List of Variant expect %s, but ")) {
			_tag = %d;
			return true;
		}
		return false;
	}""" % (typ, tag, tag, info, tag))
		elif isinstance(p, Dict):
			from_py_fields.append("""
	if (PyObject_TypeCheck(value, &%s::_Type) || PyDict_Check(value)) {
		if (_tag != 0 && _tag != %d) { Clear(); }
		if (::typy::CheckAndSet(value, _value%d, "Dict of Variant expect %s, but ")) {
			_tag = %d;
			return true;
		}
		return false;
	}""" % (typ, tag, tag, info, tag))

	return from_py_fields


def _VariantFromJson(properties):
	from Object import MetaObject
	from Type import List, Dict, Python, Instance
	from_json_objects = []
	from_json_dict = ''
	from_json_list = ''

	typeDict = {t: (i + 1, p, typ, info) for i, (t, p, typ, info) in enumerate(properties)}

	for tag, p, typ, info in typeDict.itervalues():
		if isinstance(p, Instance) and len(p.pyType) == 1 and p.pyType[0].__name__ in MetaObject.Objects:
			from_json_objects.append("""if (!strcmp(PyBytes_AS_STRING(_b.get()), "%s")) {
				if (::typy::FromJson(object->_value%d, json)) { object->_tag = %d; return object; }
			}""" % (p.pyType[0].__name__, tag, tag))
		elif isinstance(p, Python):
			from_json_objects.append("""if (!strcmp(PyBytes_AS_STRING(_b.get()), "%s")) {
				if (::typy::FromJson(object->_value%d, json)) { object->_tag = %d; return object; }
			}""" % (p.pyType.__name__, tag, tag))
		elif isinstance(p, List):
			from_json_list = """
		if (::typy::FromJson(object->_value%d, json)) { object->_tag = %d; return object; }""" % (tag, tag)
		elif isinstance(p, Dict):
			from_json_dict = """
		if (::typy::FromJson(object->_value%d, json)) { object->_tag = %d; return object; }""" % (tag, tag)

	return from_json_objects, from_json_dict, from_json_list


def _compareWrite(path, content):
	import os
	import codecs
	if os.path.isfile(path):
		with codecs.open(path, 'r', 'utf-8') as f:
			if f.read() == content:
				return
	with codecs.open(path, 'w', 'utf-8') as f:
		f.write(content)

def _shortName(prefix, name):
	import hashlib
	import base64
	if len(name) > 25:
		name = prefix + base64.b64encode(hashlib.md5(name).digest())[:-2].replace('+', '__').replace('/', '_')
	return name

def _RecordNesting(prefix, types):
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
				names.append(_RecordNesting('V', prop.pyType)[0])
		elif isinstance(prop, List):
			names.append(_RecordNesting('L', [prop.elementType])[0])
		elif isinstance(prop, Dict):
			names.append(_RecordNesting('D', [prop.keyType, prop.valueType])[0])
		else:
			names.append(name)
	name = _shortName(prefix, '%s%s' % (prefix, ''.join(names)))
	return name, properties

def _GetCppFromTypy(p, enums, pythons, variants, ref_types, container_inits, nesting = False):
	from Object import MetaObject
	from Type import pb, Enum, Simple, Instance, List, Dict, Collection
	from Type import FixedPoint, Python
	if isinstance(p, Enum):
		enums[p.pyType.__name__] = p.pyType
		ref_types.add('#include "%s.h"' % _shortName('E', p.pyType.__name__))
		return p.pyType.__name__, '', p.pyType.__name__
	elif isinstance(p, FixedPoint):
		fixedpoint = 'SINGLE_ARG(FixedPoint<%d, %d>)' % (p.precision, p.floor)
		return fixedpoint, '', fixedpoint
	elif isinstance(p, Simple):
		return p.pbType, '', p.pbType
	elif isinstance(p, Python):
		pythons.add(p.pyType.__name__)
		ref_types.add('#include "%s.h"' % _shortName('P', p.pyType.__name__))
		return 'Python<Shadow_%s>' % p.pyType.__name__, '*', 'Python<Shadow_%s>' % p.pyType.__name__
	elif isinstance(p, Instance):
		if len(p.pyType) == 1 and p.pyType[0].__name__ in MetaObject.Objects:
			ref_types.add('#include "%s.h"' % _shortName('O', p.pyType[0].__name__))
			return p.pyType[0].__name__, '*', p.pyType[0].__name__
		elif len(p.pyType) < 1 or (not nesting and pb not in p.____keywords__):
			pass
		else:
			variant, properties = _RecordNesting('V', p.pyType)
			variants[variant] = properties
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
		container_inits.add('PyType_Ready(&Dict<%s, %s >::_Type) >= 0 && PyType_Ready(&Dict<%s, %s >::IterKey_Type) >= 0 && PyType_Ready(&Dict<%s, %s >::IterValue_Type) >= 0 && PyType_Ready(&Dict<%s, %s >::IterItem_Type) >= 0' % (kName, vName, kName, vName, kName, vName, kName, vName))
		return 'SINGLE_ARG(Dict<%s, %s >)' % (kName, vName), '*', 'Dict(%s -> %s)' % (kName, vName)


def _GenerateVariant(path, name, properties, container_inits, enums, pythons, variants):
	from typy.google.protobuf.internal.encoder import _TagSize
	from Object import SortedMessage
	from Type import Type

	ref_types = set()
	header_fields = []
	clear_fields = []
	copy_fields = []
	merge_fields = []
	write_fields = []
	bytesize_fields = []
	read_fields = ['BEGINE_READ_CASE(%d)' % len(properties)]
	to_py_fields = []
	to_json = []

	tag = 0
	cppTypes = []
	for a, p in SortedMessage(properties, None):
		if not isinstance(p, Type):
			print "[Cpp] Warning: Variant type expect Typy, but get %s %s." % (a, p)
		typ, star, info = _GetCppFromTypy(p, enums, pythons, variants, ref_types, container_inits)
		cppTypes.append((a, p, typ, info))
		tag += 1
		readPrefix = 'FIRST' if tag == 1 else 'NEXT'
		header_fields.append('%s%s _value%d;' % (typ, star, tag))
		clear_fields.append('case %d: ::typy::Clear(_value%d); break;' % (tag, tag))
		copy_fields.append('case %d: ::typy::CopyFrom(_value%d, from._value%d); break;' % (tag, tag, tag))
		merge_fields.append('case %d: ::typy::MergeFrom(_value%d, from._value%d); break;' % (tag, tag, tag))
		write_fields.append('case %d: ::typy::Write(_tag, _value%d, output); break;' % (tag, tag))
		bytesize_fields.append('case %d: ::typy::ByteSize(total_size, %d, _value%d); break;' % (tag, _TagSize(tag), tag))
		to_py_fields.append('case %d: return ::typy::GetPyObject(_value%d);' % (tag, tag))
		read_fields.append('%s_READ_VARIANT_CASE(%d, _value%d, %s)' % (readPrefix, tag, tag, typ))
		to_json.append('case %d: return ::typy::Json(_value%d, slim);' % (tag, tag))

	read_fields.append('END_READ_CASE()')

	if SWITCH_GENERATE_VARIANT:
		_compareWrite(path + '%s.h' % name, TYPY_VARIANT_CLS_H__ % (
			name, name, '\n'.join(sorted(ref_types)), name, '\n\t'.join(header_fields), name))

		from_json_objects, from_json_dict, from_json_list = _VariantFromJson(cppTypes)
		_compareWrite(path + '%s.cc' % name, TYPY_VARIANT_CLS_CC__ % (name, name, name,
			'\n\t'.join(clear_fields), name, name,
			'\n\t'.join(copy_fields), name, name,
			'\n\t'.join(merge_fields), name,
			'\n\t'.join(write_fields), name,
			'\n\t'.join(bytesize_fields), name,
			'\n\t'.join(read_fields), name,
			'\n\t'.join(to_py_fields), name,
			''.join(_VariantSetter(cppTypes)), name,
			'\n\t\t'.join(to_json), name, name, name, name,
			'\n\t\t\t'.join(from_json_objects), from_json_dict, from_json_list, name, name, name))


def _GenerateEnum(path, name, cls):
	enum_values = []
	enum_py_values = []
	enum_py_value_inits1 = []
	enum_py_value_inits2 = []
	enum_value_get = []
	enum_value_set = []
	for i, v in sorted(cls.__enum__.iteritems(), key = lambda (k, v): k):
		enum_values.append('%s = %d,' % (v.name, i))
		enum_py_values.append('static PyObject* k%s%s;' % (name, v.name))
		enum_py_value_inits1.append('case %s: Py_INCREF(v); k%s%s = v; break;' % (v.name, name, v.name))
		enum_py_value_inits2.append('k%s%s = PyInt_FromLong(%d);' % (name, v.name, i))
		enum_value_get.append('case %s: Py_INCREF(k%s%s); return k%s%s;' % (v.name, name, v.name, name, v.name))
		enum_value_set.append('case %s:' % v.name)

	if SWITCH_GENERATE_ENUM:
		_compareWrite(path + '%s.h' % _shortName('E', name), TYPY_ENUM_H__ % (
			name, name, name, '\n\t'.join(enum_values), name, name, name))

		_compareWrite(path + '%s.cc' % _shortName('E', name), TYPY_ENUM_CC__ % (
			_shortName('E', name), name,
			'\n'.join(enum_py_values), name,
			'\n\t\t\t'.join(enum_py_value_inits1), name, name, name, name, name, name,
			'\n\t'.join(enum_py_value_inits2), name, name,
			'\n\t\t'.join(enum_value_get),
			'\n\t\t'.join(enum_value_set)))


def _GenerateObject(path, name, cls, container_inits, enums, pythons, variants):
	from typy.google.protobuf.internal.encoder import _TagSize
	from Object import SortedMessage
	from Type import pb, Enum, List, Dict
	from Type import Integer, Float, Double, FixedPoint, Boolean, String, Bytes

	class ReadFieldArgs(object):
		name = None
		typename = None
		method = 'NORMAL'
		prev = 'PREV_NO_REPEATED_OBJECT'
		next = 'NEXT_NO_REPEATED_OBJECT'

	first_fields = '_cached_size'
	ref_types = set()
	header_fields = []
	clear_fields = []
	merge_fields = []
	write_fields = []
	bytesize_fields = []
	read_fields = []
	property_name = []
	property_size = []
	property_serialize = []
	property_deserialize = []
	set_property_sequence = []
	get_property_sequence = []
	from_json = []
	to_json = []
	getset_fields_part1 = []
	getset_fields_part2 = []

	tag = 0
	read_field_args = [ReadFieldArgs(), ReadFieldArgs()]
	for i, (a, p) in enumerate(SortedMessage(cls.____properties__, getattr(cls, '____propertySequence__', None))):
		typ, star, info = _GetCppFromTypy(p, enums, pythons, variants, ref_types, container_inits)

		if first_fields == '_cached_size':
			first_fields = 'p_%s' % a
		header_fields.append('%s%s p_%s;' % (typ, star, a))
		clear_fields.append('::typy::Clear(p_%s);' % a)
		merge_fields.append('::typy::MergeFrom(p_%s, from.p_%s);' % (a, a))
		set_property_sequence.append('case %d: if (!::typy::CheckAndSet(PyTuple_GET_ITEM(args, %d), p_%s, "Property \'%s\' expect %s, but ")) { return false; } break;' % (i, i, a, a, info))
		get_property_sequence.append('PyTuple_SET_ITEM(result, %d, ::typy::GetPyObject(p_%s));' % (i, a))
		getset_fields_part1.append('TYPY_GETSET(%s, p_%s, %s);' % (name, a, info))
		getset_fields_part2.append('{"%s", (getter)Get_p_%s, (setter)Set_p_%s, "Property %s"},' % (a, a, a, a))


		if pb in p.____keywords__:
			tag += 1
			tagsize = _TagSize(tag)
			write_fields.append('::typy::Write(%d, p_%s, output);' % (tag, a))
			bytesize_fields.append('::typy::ByteSize(total_size, %d, p_%s);' % (tagsize, a))
			property_name.append('"%s"' % a)
			property_size.append('case %d: ::typy::ByteSize(size, %d, p_%s); if (size == 0) { size = %d; } break;' % (tag, tagsize, a, tagsize))
			property_deserialize.append('case %d: ::typy::Clear(p_%s); break;' % (tag, a))
			property_serialize.append("""case %d:
		::typy::Write(%d, p_%s, output);
		if (output->ByteCount() <= 0) {
			::typy::WriteTag(%d, p_%s, output);
		}
		break;""" % (tag, tag, a, tag, a))
			to_json.append("""value = ::typy::Json(p_%s, slim);
	if (value != NULL) { PyDict_SetItemString(json, "%s", value); Py_DECREF(value); }""" % (a, a))
			from_json.append("""value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("%s")).get()); PyErr_Clear();
	if (value != NULL) { if (!::typy::FromJson(object->p_%s, value)) { Py_DECREF(value); return NULL; } Py_DECREF(value); }""" % (a, a))
			read_field_args.append(ReadFieldArgs())
			read_field_args[tag].name = a
			read_field_args[tag].typename = typ
			if isinstance(p, Dict):
				read_field_args[tag].method = 'REPEATED_OBJECT'
				read_field_args[tag - 1].next = 'NEXT_IS_REPEATED_OBJECT'
				read_field_args[tag + 1].prev = 'PREV_IS_REPEATED_OBJECT'
				read_field_args[tag].typename = '%s::Entry' % typ
			elif isinstance(p, List):
				eTyp, _, _ = _GetCppFromTypy(p.elementType, enums, pythons, variants, ref_types, container_inits, True)
				read_field_args[tag].typename = eTyp
				if isinstance(p.elementType, Enum):
					read_field_args[tag].method = 'REPEATED_PRIMITIVE'
				elif isinstance(p.elementType, FixedPoint):
					read_field_args[tag].method = 'REPEATED_PRIMITIVE'
				elif isinstance(p.elementType, (Integer, Float, Double, FixedPoint, Boolean)):
					read_field_args[tag].method = 'REPEATED_PRIMITIVE'
				elif isinstance(p.elementType, (String, Bytes)):
					pass
				else:
					read_field_args[tag].method = 'REPEATED_OBJECT'
					read_field_args[tag - 1].next = 'NEXT_IS_REPEATED_OBJECT'
					read_field_args[tag + 1].prev = 'PREV_IS_REPEATED_OBJECT'

	if len(read_field_args) > 2:
		for tag, args in enumerate(read_field_args):
			if tag == 0:
				read_fields.append('BEGINE_READ_CASE(%d)' % (len(read_field_args) - 2))
			elif tag == len(read_field_args) - 1:
				read_fields.append('END_READ_CASE()')
			elif args.method == 'REPEATED_OBJECT':
				read_fields.append('%s_READ_REPEATED_OBJECT_CASE(%d, p_%s, %s, %s, %s)' % (
					'FIRST' if tag == 1 else 'NEXT', tag, args.name, args.typename, args.prev, args.next))
			else:
				read_fields.append('%s_READ_%s_CASE(%d, p_%s, %s)' % (
					'FIRST' if tag == 1 else 'NEXT', args.method, tag, args.name, args.typename))
	else:
		read_fields.append('input->ExpectAtEnd();')
		read_fields.append('return true;')

	if SWITCH_GENERATE_OBJECT:
		_compareWrite(path + '%s.h' % _shortName('O', name), TYPY_OBJECT_CLS_H__ % (name, name,
			'\n'.join(sorted(ref_types)), name, '\n\t'.join(header_fields), name, name))

		_compareWrite(path + '%s.cc' % _shortName('O', name), TYPY_OBJECT_CLS_CC__ % (
			_shortName('O', name), name, name, name, name, name, first_fields, name,
			'\n\t'.join(clear_fields), name, name,
			'\n\t'.join(merge_fields), name,
			'\n\t'.join(write_fields), name,
			'\n\t'.join(bytesize_fields), name,
			'\n\t'.join(read_fields), name, name,
			'\n\t'.join(to_json), name, name, name, name, name, name,
			'\n\t'.join(from_json), name, len(read_field_args) - 2, name,
			',\n\t'.join(property_name or ['""']), name,
			'\n\t'.join(property_size or ['case 0: break;']), name,
			'\n\t'.join(property_serialize or ['case 0: break;']), name,
			'\n\t'.join(property_deserialize or ['case 0: break;']), name,
			'\n\t\t'.join(set_property_sequence or ['case 0: break;']), name, len(get_property_sequence),
			'\n\t'.join(get_property_sequence),
			'\n'.join(getset_fields_part1), name,
			'\n\t'.join(getset_fields_part2), name))


def _GenerateCpp(path):
	from Object import MetaObject

	ext_modules = []
	object_types = []
	object_inits = []
	container_inits = set()
	enums = {}
	variants = {}
	pythons = set()
	for name, cls in MetaObject.Objects.iteritems():
		ext_modules.append('"${_TYPY_DIR}/%s.cc"' % _shortName('O', name))
		object_types.append('#include "%s.h"' % _shortName('O', name))
		object_inits.append('Object<%s>::Init(m)' % name)
		_GenerateObject(path, name, cls, container_inits, enums, pythons, variants)

	while variants:
		nesting_variants = {}
		for name, properties in variants.iteritems():
			ext_modules.append('"${_TYPY_DIR}/%s.cc"' % name)
			_GenerateVariant(path, name, properties, container_inits, enums, pythons, nesting_variants)
		variants = nesting_variants

	for name, cls in enums.iteritems():
		ext_modules.append('"${_TYPY_DIR}/%s.cc"' % _shortName('E', name))
		object_types.append('#include "%s.h"' % _shortName('E', name))
		object_inits.append('Init%s(m)' % name)
		_GenerateEnum(path, name, cls)

	python_types = []
	python_inits = []
	for name in pythons:
		python_types.append('#include "%s.h"' % _shortName('P', name))
		python_inits.append('Python<Shadow_%s>::Init(m, "%s")' % (name, name))
		_compareWrite(path + '%s.h' % _shortName('P', name), TYPY_PYTHON_CLS_H__ % (name, name, name, name))

	if SWITCH_GENERATE_ALL:
		_compareWrite(path + 'all.cc', TYPY_ALL_CC__ % (
			'\n'.join(sorted(python_types) + sorted(object_types)),
			'\n\t\t&& '.join(sorted(container_inits) + sorted(object_inits) + sorted(python_inits))))

	return ext_modules


def GenerateExtention(_typyDir = None, installDir = True):
	import os
	import tempfile
	if _typyDir is None:
		_typyDir = '%s/typy' % tempfile.gettempdir()
	_typyDir = os.path.abspath(_typyDir) + '/'
	if not os.path.isdir(_typyDir):
		os.mkdir(_typyDir)

	ext_modules = _GenerateCpp(_typyDir)

	_compareWrite(_typyDir + 'classes.cmake', TYPY_CLASSES_TXT__ % '\n\t'.join(ext_modules))
	path = os.path.abspath(os.path.dirname(__file__))
	if not installDir: return
	if not isinstance(installDir, basestring):
		installDir = path
	installDir = os.path.abspath(installDir)

	buildDir = _typyDir + 'build'
	if not os.path.isdir(buildDir):
		os.mkdir(buildDir)
	import shutil
	os.chdir(buildDir)
	if os.name == 'nt':
		os.system('cmake -G "Visual Studio 14 2015" -D_TYPY_DIR="%s" %s/cpp' % (_typyDir.replace('\\', '/'), path.replace('\\', '/')))
		os.system('cmake --build . --config Release --target _typy')
	else:
		os.system('cmake -D_TYPY_DIR="%s" %s/cpp' % (_typyDir, path))
		os.system('cmake --build .')

	if os.name == 'nt':
		extSurfix = 'pyd'
		buildFile = 'Release/_typy.%s' % extSurfix
	else:
		extSurfix = 'so'
		buildFile = '_typy.%s' % extSurfix
	if os.path.isfile(buildFile):
		shutil.copy(buildFile, '%s/_typy.%s' % (installDir, extSurfix))
	os.chdir(path)
	SWITCH_DELETE_CPP and shutil.rmtree(_typyDir)


TYPY_CLASSES_TXT__ = ur"""# Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

set(CLASSES
	%s
)
"""

TYPY_PYTHON_CLS_H__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#ifndef TYPY_PYTHON_%s_H__
#define TYPY_PYTHON_%s_H__

#include "typy.h"

namespace typy {

struct Shadow_%s{};

} // namespace typy
#endif // TYPY_PYTHON_%s_H__
"""

TYPY_VARIANT_CLS_H__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#ifndef TYPY_%s_H__
#define TYPY_%s_H__

#include "typy.h"

%s

TypyVariantHeader(%s, SINGLE_ARG(
	%s
));

#endif // TYPY_%s_H__
"""

TYPY_VARIANT_CLS_CC__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "%s.h"
#include "list.h"
#include "dict.h"

TypyVariantBegin(%s);

void %s::Clear() {
	switch (_tag) {
	%s
	}
	_tag = 0;
}

void %s::CopyFrom(const %s& from) {
	if (&from == this) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	%s
	}
	_tag = from._tag;
}

void %s::MergeFrom(const %s& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) {
		GOOGLE_CHECK(false) << __FILE__ << ":" << __LINE__;
	}
	if (from._tag == 0) { return; }
	if (_tag != 0 && _tag != from._tag) { Clear(); }
	switch (from._tag) {
	%s
	}
	_tag = from._tag;
}

void %s::SerializeWithCachedSizes(CodedOutputStream* output) const {
	switch (_tag) {
	%s
	}
}

int %s::ByteSize() const {
	int total_size = 0;
	switch (_tag) {
	%s
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool %s::MergePartialFromCodedStream(CodedInputStream* input) {
	%s
}

PyObject* %s::toPyObject() {
	switch (_tag) {
	%s
	default: Py_RETURN_NONE;
	}
}

bool %s::fromPyObject(PyObject* value) {
	if (value == NULL || value == Py_None) {
		Clear();
		return true;
	}%s
	return false;
}

PyObject* %s::Json(bool slim) {
	if (!slim || _tag != 0) {
		switch (_tag) {
		%s
		default: Py_RETURN_NONE;
		}
	} else {
		return NULL;
	}
}

%s* %s::FromJson(PyObject* json) {
	%s* object = new %s;
	if (PyObject_HasAttrString(json, "iteritems")) {
		ScopedPyObjectPtr _t(PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("_t")).get()));
		PyErr_Clear();
		ScopedPyObjectPtr _b;
		if (_t == NULL) {
		} else if (PyBytes_Check(_t.get())) {
			Py_INCREF(_t.get());
			_b.reset(_t.get());
		} else if (PyUnicode_Check(_t.get())) {
			_b.reset(PyUnicode_AsEncodedObject(_t.get(), "utf-8", NULL));
		}
		if (_b != NULL) {
			%s
		}%s
		PyErr_Clear();
	} else if (PyObject_HasAttrString(json, "__iter__")) {%s
	} else if (object->fromPyObject(json)) { return object; }
	delete object;
	FormatTypeError(json, "Variant.FromJson error, ");
	return NULL;
}

bool CheckAndSet(PyObject* arg, %s*& value, const char* err) {
	if (arg == Py_None) {
		delete value; value = NULL;
		return true;
	}
	if (value == NULL) {
		value = new %s;
	}
	if (!value->fromPyObject(arg)) {
		delete value; value = NULL;
		FormatTypeError(arg, err);
		return false;
	}
	return true;
}

TypyVariantEnd(%s);
"""

TYPY_ENUM_H__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#ifndef TYPY_%s_H__
#define TYPY_%s_H__

#include "typy.h"

#define ENUM %s

namespace typy {

enum ENUM {
	%s
};

PyObject* _Init%s(PyObject* m, PyObject* arg);

bool Init%s(PyObject* m);

template <> struct Type<ENUM> {
	typedef ENUM ValueType;
	enum {
		FieldType = WireFormatLite::TYPE_ENUM,
		WireType = WireFormatLite::WIRETYPE_VARINT,
	};
};

PyObject* GetPyObject(const ENUM& value);
bool CheckAndSet(PyObject* arg, ENUM& value, const char* err);
inline void CopyFrom(ENUM& lvalue, const ENUM& rvalue) { lvalue = rvalue; }
inline void Clear(ENUM& value) { value = static_cast<ENUM>(0); }
inline void MergeFrom(ENUM& lvalue, const ENUM& rvalue) {
	if (rvalue != 0) { CopyFrom(lvalue, rvalue); } }
inline void ByteSize(int& total, int tagsize, const ENUM& value) {
	if (value != 0) { total += tagsize + WireFormatLite::EnumSize(value); } }
inline void Write(int field_number, const ENUM& value, CodedOutputStream* output) {
	if (value != 0) { WireFormatLite::WriteEnum(field_number, value, output); } }

inline bool Read(ENUM& value, CodedInputStream* input) {
	int i;
	if (WireFormatLite::ReadPrimitive<int,
		WireFormatLite::FieldType(Type<ENUM>::FieldType)>(input, &i)) {
		value = static_cast<ENUM>(i);
		return true;
	}
	return false;
}

inline PyObject* Json(const ENUM& value, bool slim) {
	return (!slim || value != 0) ? GetPyObject(value) : NULL;
}

inline bool FromJson(ENUM& value, PyObject* json) {
	return CheckAndSet(json, value, "FromJson expect Enum, but ");
}

inline void ByteSize(int& total, int tagsize, List<ENUM>* value) {
	if (value == NULL) { return; }
	int data_size = 0;
	for (int i = 0; i < value->size(); i++) {
		data_size += WireFormatLite::EnumSize(value->Get(i));
	}
	if (data_size > 0) {
		total += tagsize + WireFormatLite::Int32Size(data_size);
	}
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	value->_cached_size = data_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	total += data_size;
}

inline void Write(int field_number, List<ENUM>* value, CodedOutputStream* output) {
	if (value == NULL) { return; }
	if (value->_cached_size > 0) {
		WireFormatLite::WriteTag(field_number, WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
		output->WriteVarint32(value->_cached_size);
	}
	for (int i = 0; i < value->size(); i++) {
		WireFormatLite::WriteEnumNoTag(value->Get(i), output);
	}
}

inline bool Read(List<ENUM>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<ENUM>; }
	ENUM item;
	if (!Read(item, input)) { return false; }
	value->Add(item);
	return true;
}

inline bool ReadPacked(List<ENUM>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<ENUM>; }
	uint32 length;
	if (!input->ReadVarint32(&length)) {
		return false;
	}
	CodedInputStream::Limit limit = input->PushLimit(length);
	while (input->BytesUntilLimit() > 0) {
		ENUM item;
		if (!Read(item, input)) { return false; }
		value->Add(item);
	}
	input->PopLimit(limit);
	return true;
}

inline bool ReadRepeated(int tagsize, uint32 tag, List<ENUM>*& value, CodedInputStream* input) {
	if (value == NULL) { value = new List<ENUM>; }
	ENUM item;
	if (!Read(item, input)) { return false; }
	value->Add(item);
	int elements_already_reserved = value->Capacity() - value->size();
	while (elements_already_reserved > 0 && input->ExpectTag(tag)) {
		if (!Read(item, input)) { return false; }
		value->AddAlreadyReserved(item);
		elements_already_reserved--;
	}
	return true;
}

} /* namespace typy */

#undef ENUM

#endif // TYPY_%s_H__
"""

TYPY_ENUM_CC__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "%s.h"

#define ENUM %s

namespace typy {

%s

PyObject* _Init%s(PyObject* m, PyObject* arg) {
	if (PyDict_Check(arg)) {
		PyObject *k, *v;
		Py_ssize_t pos = 0;
		while (PyDict_Next(arg, &pos, &k, &v)) {
			long i = PyInt_AsLong(k);
			if (PyErr_Occurred()) {
				return NULL;
			}
			switch (i) {
			%s
			}
		}
		Py_RETURN_NONE;
	} else {
		FormatTypeError(arg, "%s expect a value => enum dict, but ");
		return NULL;
	}
}

static PyMethodDef %sMethod = { "%s", (PyCFunction)_Init%s, METH_O,
	"Initialize the '%s' Enum with a value dict." };

bool Init%s(PyObject* m) {
	%s
	PyCFunctionObject* method = reinterpret_cast<PyCFunctionObject*>(
		PyType_GenericAlloc(&PyCFunction_Type, 0));
	method->m_ml = &%sMethod;
	method->m_self = m;
	method->m_module = NULL;
	PyModule_AddObject(m, %sMethod.ml_name, reinterpret_cast<PyObject*>(method));
	return true;
}

PyObject* GetPyObject(const ENUM& value) {
	switch (value) {
		%s
		default:
			return PyInt_FromLong(value);
	}
}

bool CheckAndSet(PyObject* arg, ENUM& value, const char* err) {
	long i = PyInt_AsLong(arg);
	if (PyErr_Occurred()) { return false; }
	switch (i) {
		%s
			value = static_cast<ENUM>(i);
			return true;
		default:
			FormatTypeError(arg, err);
			return false;
	}
}

} /* namespace typy */

#undef ENUM
"""

TYPY_OBJECT_CLS_H__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#ifndef TYPY_%s_H__
#define TYPY_%s_H__

#include "typy.h"

%s

TypyHeaderBegin(%s);
	%s
TypyHeaderEnd(%s);

#endif // TYPY_%s_H__
"""

TYPY_OBJECT_CLS_CC__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "%s.h"
#include "list.h"
#include "dict.h"

TypyObjectBegin(%s);

%s::%s() : Message() {
	PyObject_INIT(this, &Object<%s>::_Type);
	ZERO(%s, %s, _cached_size);
}

void %s::Clear() {
	%s
}

void %s::MergeFrom(const %s& from) {
	if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
	%s
}

void %s::SerializeWithCachedSizes(CodedOutputStream* output) const {
	%s
}

int %s::ByteSize() const {
	int total_size = 0;
	%s
	GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
	_cached_size = total_size;
	GOOGLE_SAFE_CONCURRENT_WRITES_END();
	return total_size;
}

bool %s::MergePartialFromCodedStream(CodedInputStream* input) {
	%s
}

PyObject* %s::Json(bool slim) {
	PyObject* json = PyDict_New();
	if (json == NULL) { return NULL; }
	PyObject* value = PyString_FromString(%s::Name);
	if (value == NULL) { Py_DECREF(json); return NULL; }
	PyDict_SetItemString(json, "_t", value);
	Py_DECREF(value);
	%s
	return json;
}

%s* %s::FromJson(PyObject* json) {
	if (!PyObject_HasAttrString(json, "iteritems")) {
		FormatTypeError(json, "FromJson expect dict, but ");
		return NULL;
	}
	PyObject* value = PyObject_GetItem(json, ScopedPyObjectPtr(PyString_FromString("_t")).get());
	if (value == NULL) {
		FormatTypeError(json, "Json expect _t, ");
		return NULL;
	} else if (PyUnicode_Check(value)) {
		PyObject* _value = PyUnicode_AsEncodedObject(value, "utf-8", NULL);
		Py_DECREF(value);
		value = _value;
	} else if (!PyBytes_Check(value)) {
		FormatTypeError(value, "Json _t expect String, but ");
		return NULL;
	}
	if (strcmp(PyBytes_AS_STRING(value), %s::Name)) {
		PyErr_Format(PyExc_TypeError, "Object expect '%%.100s', but Json has type %%.100s",
			%s::Name, PyBytes_AS_STRING(value));
		Py_DECREF(value);
		return NULL;
	}
	PyErr_Clear();
	Py_DECREF(value);
	%s* object = new %s();
	%s
	return object;
}

// ===================================================================

const int %s::PropertyCount = %d;
char* %s::Properties[] = {
	%s
};

int %s::PropertyByteSize(int tag) const {
	int size = 0;
	switch(tag) {
	%s
	}
	return size;
}

void %s::SerializeProperty(CodedOutputStream* output, int tag) const {
	switch(tag) {
	%s
	}
}

int %s::DeserializeProperty(CodedInputStream* input) {
	const void* data;
	int size;
	input->GetDirectBufferPointerInline(&data, &size);
	CodedInputStream tagInput(reinterpret_cast<const uint8*>(data), size);
	::std::pair<uint32, bool> p = tagInput.ReadTagWithCutoff(_MAXTAG(25)
		<= 0x7F ? 0x7F : (_MAXTAG(25) <= 0x3FFF ? 0x3FFF : _MAXTAG(25)));
	uint32 tag = p.first;
	if (!p.second) { return 0; }

	int index = WireFormatLite::GetTagFieldNumber(tag);
	switch(index) {
	%s
	}

	if (!tagInput.ExpectAtEnd()) {
		MergePartialFromCodedStream(input);
	}
	return index;
}

bool %s::SetPropertySequence(PyObject* args) {
	for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++) {
		switch(i) {
		%s
		default: PyErr_Format(PyExc_TypeError, "Unsurported property number %%lu.", i); return false;
		}
	}
	return true;
}

PyObject* %s::GetPropertySequence() {
	PyObject* result = PyTuple_New(%d);
	if (result == NULL) { return result; }
	%s
	return result;
}

// ===================================================================

%s

template <> PyGetSetDef Object<%s>::GetSet[] = {
	%s
	{NULL}
};

TypyObjectEnd(%s);
"""

TYPY_ALL_CC__ = ur"""// Generated by the typy Cpp.GenerateExtention.  DO NOT EDIT!

#include "typy.h"

%s
#include "list.h"
#include "dict.h"

namespace typy {

bool InitModule(PyObject* m) {
	return %s;
}

} // namespace typy
"""
