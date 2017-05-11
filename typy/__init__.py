#-*- coding: utf-8 -*-
# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

__author__ = 'Joungtao'
__version__ = '0.0.1'

import os
try:
	import _typy
except ImportError:
	IMPLEMENTATION_TYPE = 'python'
else:
	try:
		import _typyd
	except ImportError:
		IMPLEMENTATION_TYPE = 'cpp'
	else:
		IMPLEMENTATION_TYPE = 'cpy'
IMPLEMENTATION_TYPE = os.getenv('TYPY_IMPLEMENTATION', IMPLEMENTATION_TYPE)
if IMPLEMENTATION_TYPE not in ('cpp', 'cpy', 'python'):
	print '[Typy] TYPY_IMPLEMENTATION must set as ("cpp", "cpy", "python"), but got %s' % IMPLEMENTATION_TYPE
	IMPLEMENTATION_TYPE = 'python'

from Object import Object, MetaObject, Json, FromJson
from Type import Type, Simple, Collection, Enum, Integer, Float, Double, FixedPoint, Boolean, Bytes, String, Instance, List, Dict, Python, pb, PythonDelegate
from Cpp import GenerateExtention
from Cpy import GenerateDescriptor
