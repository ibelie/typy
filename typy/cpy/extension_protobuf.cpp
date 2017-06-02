// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "extension_protobuf.h"
#include "google/protobuf/message.h"
#include "google/protobuf/io/coded_stream.h"

//=============================================================================
// [joungtao] from "python/google/protobuf/pyext"
#ifndef _SHARED_PTR_H
#include <google/protobuf/stubs/shared_ptr.h>
#endif
#include <string>

namespace google {
namespace protobuf {

#ifdef _SHARED_PTR_H
using std::shared_ptr;
using ::std::string;
#else
using internal::shared_ptr;
#endif

namespace python {

typedef struct CMessage {
	PyObject_HEAD;
	shared_ptr<Message> owner;
	struct CMessage* parent;
	const FieldDescriptor* parent_field_descriptor;
	Message* message;
} CMessage;

} // namespace python
} // namespace protobuf
} // namespace google

//=============================================================================

bool Protobuf_TypeCheck(PyTypeObject* type) {
	if (type->tp_mro) {
		register Py_ssize_t i, n = PyTuple_GET_SIZE(type->tp_mro);
		for (i = 0; i < n; i++) {
			register PyTypeObject* base = (PyTypeObject*)PyTuple_GET_ITEM(type->tp_mro, i);
			if (!strcmp(base->tp_name, "google.protobuf.pyext._message.CMessage")) {
				return true;
			}
		}
	}
	return false;
}

size_t Protobuf_CachedSize(PyObject* msg) {
	int size = reinterpret_cast<::google::protobuf::python::CMessage*>(msg)->message->GetCachedSize();
	return size >= 0 ? (size_t)size : 0;
}

size_t Protobuf_ByteSize(PyObject* msg) {
	int size = reinterpret_cast<::google::protobuf::python::CMessage*>(msg)->message->ByteSize();
	return size >= 0 ? (size_t)size : 0;
}

size_t Protobuf_Write(PyObject* msg, void* output) {
	::google::protobuf::uint8* start = reinterpret_cast<::google::protobuf::uint8*>(output);
	::google::protobuf::uint8* end = reinterpret_cast<::google::protobuf::python::CMessage*>(msg)->message->SerializeWithCachedSizesToArray(start);
	return end - start;
}

bool Protobuf_Read(PyObject* msg, void* input, size_t length) {
	::google::protobuf::io::CodedInputStream stream(reinterpret_cast<const ::google::protobuf::uint8*>(input), length);
	return reinterpret_cast<::google::protobuf::python::CMessage*>(msg)->message->MergePartialFromCodedStream(&stream);
}
