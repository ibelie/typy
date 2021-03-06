#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

from Type import pb, Type, PythonTypes, PythonDelegate
from Proto import TypeObject, SortedMessage

def _getProperties(mcs, bases, attrs):
	prop = {}
	for a in attrs.keys():
		t = attrs[a]
		if isinstance(t, type) and issubclass(t, Type):
			t = t()
		elif not isinstance(t, Type):
			continue
		attrs.pop(a)
		prop[a] = t
	attrs['__slots__'] = tuple((k for k, p in prop.iteritems() if pb not in p.____keywords__))

	parents = attrs.setdefault('____parents__', [])
	properties = {}
	for base in bases:
		if base.__name__ in mcs.Objects:
			if base.__name__ not in parents:
				parents.append(base.__name__)
			properties.update(mcs.Objects[base.__name__].____properties__)
	properties.update(prop)

	return properties


def Json(value, slim = False):
	if value.__class__.__name__ in PythonTypes and hasattr(value, 'Json'):
		data = value.Json()
		data['_t'] = value.__class__.__name__
		return data
	elif hasattr(value, 'Json'):
		return value.Json(slim)
	elif hasattr(value, 'iteritems'):
		return {str(int(k)) if isinstance(k, int) else (k if isinstance(k, unicode) else str(k)): Json(v, slim) for k, v in value.iteritems()}
	elif hasattr(value, '__iter__'):
		return [Json(value[i], slim) for i in xrange(len(value))]
	elif type(value) in PythonDelegate:
		data = PythonDelegate[type(value)].Json(value)
		data['_t'] = value.__class__.__name__
		return data
	else:
		return value


from typy import IMPLEMENTATION_TYPE

if IMPLEMENTATION_TYPE == 'python':

	import Hacker
	from typy.google.protobuf import message, reflection, descriptor, descriptor_pb2
	from Type import isEnum, toType, Enum, List, Dict, Collection, Instance, Python
	import copy

	MSG_TYPE_NONE = 0
	MSG_TYPE_MAP = 1
	MSG_TYPE_VARIANT = 2

	def _msgFields(name, properties, msgType, msgProto, sequence):
		clsname = name.split('.')[-1]
		msgProto.add(name = clsname)
		protoType = msgProto[-1]
		if msgType == MSG_TYPE_MAP:
			if not hasattr(protoType.options, 'map_entry'):
				raise SystemError('Protobuf version error: cannot surport map entry "%s".' % name)
			protoType.options.map_entry = True

		fields = []
		i = 0
		for a, p in SortedMessage(properties, sequence):
			if pb not in p.____keywords__ and msgType == MSG_TYPE_NONE:
				continue

			if msgType == MSG_TYPE_VARIANT:
				protoType.oneof_decl.add(name = a)

			t = getattr(descriptor.FieldDescriptor, 'TYPE_%s' % p.pbType.upper())
			field = dict(name = a, full_name = 'typy.%s.%s' % (name, a),
				index = i, number = i + 1, default_value = p.default, type = t,
				cpp_type = descriptor.FieldDescriptor._PYTHON_TO_CPP_PROTO_TYPE_MAP[t],
				label = descriptor.FieldDescriptor.LABEL_OPTIONAL, message_type = None,
				has_default_value = False, enum_type = None, containing_type = None,
				is_extension = False, extension_scope = None, options = None)

			if isinstance(p, Collection):
				field['default_value'] = []
				field['label'] = descriptor.FieldDescriptor.LABEL_REPEATED

			#[joungtao] Explicit default values are not allowed in proto3.
			# if isinstance(p, String):
			# 	protoType.field[-1].default_value = p.default
			# elif isinstance(p, Bytes):
			# 	protoType.field[-1].default_value = repr(p.default)
			# elif isinstance(p, Simple):
			# 	protoType.field[-1].default_value = repr(p.default).lower()

			protoType.field.add(name = field['name'], number = field['number'], type = field['type'], label = field['label'])
			if isinstance(p, Enum):
				field['enum_type'] = p.pyType.DESCRIPTOR
				protoType.field[-1].type_name = field['enum_type'].full_name
			elif isinstance(p, Python):
				field['message_type'] = Hacker.PythonDescriptor(p.pyType)
				protoType.field[-1].type_name = field['message_type'].full_name
			elif isinstance(p, Dict):
				assistName = '%s.%sEntry' % (name, a.title())
				field['message_type'] = [assistName] + _msgFields(assistName, dict(key = p.keyType, value = p.valueType), MSG_TYPE_MAP, protoType.nested_type, None)
				protoType.field[-1].type_name = 'typy.%s' % assistName
			elif isinstance(p, Instance):
				if len(p.pyType) == 1:
					field['message_type'] = MetaObject.Objects[p.pyType[0].__name__].DESCRIPTOR
					protoType.field[-1].type_name = field['message_type'].full_name
				elif len(p.pyType) > 1:
					assistName = '%s.%sVariant' % (name, a.title())
					field['message_type'] = [assistName] + _msgFields(assistName,
						{'Enum' if isEnum(vp) else vp.__name__: toType(vp) for vp in p.pyType if vp is not None}, MSG_TYPE_VARIANT, protoType.nested_type, None)
					protoType.field[-1].type_name = 'typy.%s' % assistName
			elif isinstance(p, List):
				if isinstance(p.elementType, Enum):
					field['enum_type'] = p.elementType.pyType.DESCRIPTOR
					protoType.field[-1].type_name = field['enum_type'].full_name
				elif isinstance(p.elementType, Python):
					field['message_type'] = Hacker.PythonDescriptor(p.elementType.pyType)
					protoType.field[-1].type_name = field['message_type'].full_name
				elif isinstance(p.elementType, Instance):
					if len(p.elementType.pyType) < 1: pass
					elif len(p.elementType.pyType) == 1 and not isinstance(p.elementType.pyType[0], List):
						field['message_type'] = MetaObject.Objects[p.elementType.pyType[0].__name__].DESCRIPTOR
						protoType.field[-1].type_name = field['message_type'].full_name
					else:
						assistName = '%s.%sVariant' % (name, a.title())
						field['message_type'] = [assistName] + _msgFields(assistName,
							{'Enum' if isEnum(vp) else vp.__name__: toType(vp) for vp in p.elementType.pyType if vp is not None}, MSG_TYPE_VARIANT, protoType.nested_type, None)
						protoType.field[-1].type_name = 'typy.%s' % assistName

			fields.append(field)
			i += 1
		return fields


	def _descriptorArgs(fields, **kwargs):
		args = dict(fields = [descriptor.FieldDescriptor(**f) for f in fields],
			is_extendable = False, containing_type = None, options = None, nested_types = [],
			extensions = [], enum_types = [], extension_ranges = [], oneofs = [],
			file = None, filename = None, serialized_start = None, serialized_end = None)
		args.update(kwargs)
		return args

	def _assistMessageType(fields, attrs, proto):
		name = fields.pop(0)
		if name.endswith('Entry'):
			options = descriptor_pb2.MessageOptions()
			if not hasattr(options, 'map_entry'):
				raise SystemError('Protobuf version error: cannot surport map entry "%s".' % name)
			options.map_entry = True
			oneofs = []
			for field in fields:
				if isinstance(field['message_type'], list):
					field['message_type'] = _assistMessageType(field['message_type'], attrs, proto)
		else:
			options = None
			oneofs = [descriptor.OneofDescriptor(
				name = 'Variant', full_name = 'typy.%s' % name,
				index = 0, containing_type = None, fields = fields)]
			for field in fields:
				if isinstance(field['message_type'], list):
					field['message_type'] = _assistMessageType(field['message_type'], attrs, proto)
				field['containing_oneof'] = oneofs[0]

		clsname = name.split('.')[-1]
		descArgs = _descriptorArgs(name = clsname, full_name = 'typy.%s' % name,
			fields = fields, options = options, oneofs = oneofs, syntax = proto.syntax)
		attrs[clsname] = reflection.GeneratedProtocolMessageType(clsname, (message.Message, ),
			dict(DESCRIPTOR = descriptor.Descriptor(**descArgs)))

		if name.endswith('Entry'):
			def entryInit(originInit):
				def _entryInit(self, **kwargs):
					originInit(self)
					for k, v in kwargs.iteritems():
						setattr(self, k, v)
				return _entryInit
			attrs[clsname].__init__ = entryInit(attrs[clsname].__init__)

		return attrs[clsname].DESCRIPTOR

	def Descriptor(attrs, name, properties):
		proto = descriptor_pb2.FileDescriptorProto()
		proto.name = 'typy.%s' % name
		proto.package = 'typy'

		fields = _msgFields(name, properties, MSG_TYPE_NONE, proto.message_type, attrs.get('____propertySequence__'))
		for field in fields:
			if field['message_type'] is None or isinstance(field['message_type'], list) or\
				field['message_type'].full_name in proto.dependency:
				continue
			proto.dependency.append(field['message_type'].full_name)

		proto.syntax = 'proto3'
		descriptor.FileDescriptor(name = proto.name, package = proto.package,
			serialized_pb = proto.SerializeToString(), syntax = proto.syntax)

		for field in fields:
			if isinstance(field['message_type'], list):
				field['message_type'] = _assistMessageType(field['message_type'], attrs, proto)

		descriptorArgs = _descriptorArgs(name = name, full_name = 'typy.%s' % name, fields = fields, options = None, syntax = proto.syntax,
			nested_types = [a.DESCRIPTOR for a in attrs.itervalues() if isinstance(a, message.Message)])
		attrs[reflection.GeneratedProtocolMessageType._DESCRIPTOR_KEY] = descriptor.Descriptor(**descriptorArgs)


	# [joungtao] Stop google.protobuf.reflection from changing slots of subclass.
	# Because the slots are already set in Object.
	class AttrsHacker(dict):
		def __setitem__(self, key, value):
			if key != '__slots__':
				super(AttrsHacker, self).__setitem__(key, value)


	class MetaObject(reflection.GeneratedProtocolMessageType):
		Objects = {}

		def __new__(mcs, clsname, bases, attrs):
			attrs['____properties__'] = _getProperties(mcs, bases, attrs)
			attrs['____sortedProperty__'] = tuple([a for a, _ in SortedMessage(attrs['____properties__'], attrs.get('____propertySequence__'))])
			Descriptor(attrs, clsname, attrs['____properties__'])

			if clsname != 'Object':
				attrs = AttrsHacker(attrs)
			cls = super(MetaObject, mcs).__new__(mcs, clsname, bases, attrs)

			if clsname != 'Object':
				if clsname in mcs.Objects and not isinstance(mcs.Objects[clsname], TypeObject):
					raise TypeError, 'Object name "%s" already exists.' % clsname
				mcs.Objects[clsname] = cls

			return cls

		def __init__(cls, clsname, bases, attrs):
			super(MetaObject, cls).__init__(clsname, bases, attrs)
			from typy.google.protobuf.internal import encoder
			from typy.google.protobuf.internal import type_checkers
			from typy.google.protobuf.internal import wire_format
			cls.fields_by_tag = {}
			cls.field_tag = {}
			for field in attrs[reflection.GeneratedProtocolMessageType._DESCRIPTOR_KEY].fields:
				tag = encoder.TagBytes(field.number, type_checkers.FIELD_TYPE_TO_WIRE_TYPE[field.type])
				cls.fields_by_tag[tag] = field
				cls.field_tag[field] = tag
				if field.label == descriptor.FieldDescriptor.LABEL_REPEATED and wire_format.IsTypePackable(field.type):
					tag = encoder.TagBytes(field.number, wire_format.WIRETYPE_LENGTH_DELIMITED)
					cls.fields_by_tag[tag] = field
					cls.field_tag[field] = tag
			Hacker.initObjectClass(cls, clsname, bases, attrs)
			cls.__repr__ = cls.__str__ = lambda s: repr(s.Json())

		def __call__(cls, *args, **kwargs):
			o = super(MetaObject, cls).__call__()
			for a, p in cls.____properties__.iteritems():
				if pb in p.____keywords__:
					continue
				elif isinstance(p, Collection):
					setattr(o, a, copy.deepcopy(p.default))
				else:
					setattr(o, a, p.default)
			for i, v in enumerate(args):
				if i < len(cls.____sortedProperty__):
					setattr(o, cls.____sortedProperty__[i], v)
				else:
					raise TypeError, 'Unsurported property number %d.' % i
			for k, v in kwargs.iteritems():
				setattr(o, k, v)
			return o

	def _VariantFromJson(m, data):
		if hasattr(data, 'iteritems'):
			if '_t' in data:
				if data['_t'] in PythonTypes:
					if PythonTypes[data['_t']] in PythonDelegate:
						return PythonDelegate[PythonTypes[data['_t']]].FromJson({k: v for k, v in data if k != '_t'})
					return PythonTypes[data['_t']].FromJson({k: v for k, v in data.iteritems() if k != '_t'})
				elif data['_t'] in MetaObject.Objects:
					return MetaObject.Objects[data['_t']].FromJson(data)
			d = m.fields_by_name['Dict']._default_constructor(m._concrete_class())
			return _DictFromJson(d, data)
		elif hasattr(data, '__iter__'):
			return _FromJson(m.fields_by_name['List'].message_type, True, data)
		else:
			return data


	def _FromJson(m, repeated, data):
		if not m: return data
		if repeated:
			if m.oneofs:
				return [_VariantFromJson(m, d) for d in data]
			elif hasattr(m, '_concrete_class') and hasattr(m._concrete_class, 'FromJson'):
				return [None if d is None else m._concrete_class.FromJson(d) for d in data]
			else:
				return data
		elif m.oneofs:
			return _VariantFromJson(m, data)
		elif data is not None and hasattr(m, '_concrete_class') and hasattr(m._concrete_class, 'FromJson'):
			return m._concrete_class.FromJson(data)
		else:
			return data

	def _DictFromJson(d, data):
		from typy.google.protobuf.internal import type_checkers
		_nestingDict = getattr(d, '_nestingDict', None)
		if _nestingDict:
			if data is None:
				return {}
			elif isinstance(d._key_checker, type_checkers.IntValueChecker):
				return {d._key_checker._TYPE(k): _DictFromJson(_nestingDict(), v) for k, v in data.iteritems()}
			elif isinstance(d._key_checker, type_checkers.TypeCheckerWithDefault):
				return {d._key_checker._acceptable_types[0](k): _DictFromJson(_nestingDict(), v) for k, v in data.iteritems()}
			else:
				return {k: _DictFromJson(_nestingDict(), v) for k, v in data.iteritems()}
		else:
			if data is None:
				return None
			elif isinstance(d._key_checker, type_checkers.IntValueChecker):
				return {d._key_checker._TYPE(k): _FromJson(getattr(d, '_message_descriptor', None), getattr(d, '_nestingList', None), v) for k, v in data.iteritems()}
			elif isinstance(d._key_checker, type_checkers.TypeCheckerWithDefault):
				return {d._key_checker._acceptable_types[0](k): _FromJson(getattr(d, '_message_descriptor', None), getattr(d, '_nestingList', None), v) for k, v in data.iteritems()}
			else:
				return {k: _FromJson(getattr(d, '_message_descriptor', None), getattr(d, '_nestingList', None), v) for k, v in data.iteritems()}

	class Object(message.Message):
		__metaclass__ = MetaObject

		def __deepcopy__(self, memo = None):
			clone = type(self)()
			clone.MergeFromString(self.SerializeToString())
			return clone

		@classmethod
		def FromJson(cls, data):
			if data is None: return None
			self = cls()
			for key, value in data.iteritems():
				if key == '_t': continue
				o = getattr(self, key, None)
				if not hasattr(o, '_key_checker'):
					fields = getattr(self, reflection.GeneratedProtocolMessageType._DESCRIPTOR_KEY).fields_by_name
					if key not in fields: continue
					field = fields[key]
					setattr(self, key, None if value is None else _FromJson(field.message_type, field.label == descriptor.FieldDescriptor.LABEL_REPEATED, value))
				else:
					setattr(self, key, None if value is None else _DictFromJson(o, value))
			return self

		def Json(self, slim = False):
			if slim:
				data = {field.name: Json(getattr(self, field.name), slim) for field, _ in self.ListFields()}
			else:
				data = {name: Json(getattr(self, name), slim) for name, p in self.____properties__.iteritems() if pb in p.____keywords__}
			data['_t'] = self.__class__.__name__
			return data

		def Args(self):
			return tuple([getattr(self, k) for k in self.____sortedProperty__])

		def SerializeProperty(self, attr):
			from io import BytesIO
			out = BytesIO()
			field = getattr(self, reflection.GeneratedProtocolMessageType._DESCRIPTOR_KEY).fields_by_name[attr]
			if field in self._fields:
				field._encoder(out.write, self._fields[field])
				data = out.getvalue()
				if data: return data
			out.write(self.field_tag[field])
			return out.getvalue()

		def DeserializeProperty(self, data):
			from typy.google.protobuf.internal import decoder
			tag, pos = decoder.ReadTag(data, 0)
			field = self.fields_by_tag.get(tag)
			if field:
				self.ClearField(field.name)
			if pos < len(data):
				self.MergeFromString(data)
			return field and field.name

else:
	import _typy

	# [joungtao] sys.setdefaultencoding('utf-8') to get better performance for string.
	_typy.setDefaultEncodingUTF8()

	class MetaObject(type):
		Objects = {}

		def __new__(mcs, clsname, bases, attrs):
			all_attrs = {}
			for base in bases:
				if base.__name__ in mcs.Objects:
					all_attrs.update(mcs.Objects[base.__name__].____attrs__)
			all_attrs.update(attrs)
			all_attrs['____attrs__'] = all_attrs
			if '__metaclass__' not in all_attrs:
				all_attrs['__metaclass__'] = mcs
			all_attrs['____properties__'] = _getProperties(mcs, bases, attrs)

			if hasattr(_typy, clsname):
				cls = getattr(_typy, clsname)(all_attrs)
				all_attrs.pop('____attrs__')
			else:
				if clsname != 'Object':
					print '[Typy] Class %s not found in C extension module.' % clsname
				cls = super(MetaObject, mcs).__new__(mcs, clsname, bases, attrs)
				cls.____properties__ = all_attrs['____properties__']

			if clsname != 'Object':
				if clsname in mcs.Objects and not isinstance(mcs.Objects[clsname], TypeObject):
					raise TypeError, 'Object name "%s" already exists.' % clsname
				mcs.Objects[clsname] = cls

			return cls

	class Object(object):
		__metaclass__ = MetaObject


def FromJson(data, keyType = None):
	if isinstance(data, dict):
		if '_t' not in data:
			return {keyType(k) if keyType else k: FromJson(v) for k, v in data.iteritems()}
		elif data['_t'] in PythonTypes:
			if PythonTypes[data['_t']] in PythonDelegate:
				return PythonDelegate[PythonTypes[data['_t']]].FromJson({k: v for k, v in data.iteritems() if k != '_t'})
			return PythonTypes[data['_t']].FromJson({k: v for k, v in data.iteritems() if k != '_t'})
		elif data['_t'] in MetaObject.Objects:
			return MetaObject.Objects[data['_t']].FromJson(data)
	elif isinstance(data, (list, tuple, set)):
		return [FromJson(v) for v in data]
	return data
