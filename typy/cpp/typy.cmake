# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

include("${_TYPY_DIR}/classes.cmake")

set(SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/google/protobuf/arena.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/message_lite.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/repeated_field.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/generated_message_util.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/wire_format_lite.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/io/coded_stream.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/io/zero_copy_stream.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/io/zero_copy_stream_impl_lite.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/atomicops_internals_x86_gcc.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/atomicops_internals_x86_msvc.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/common.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/int128.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/once.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/status.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/stringpiece.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/stringprintf.cc"
	"${PROJECT_SOURCE_DIR}/google/protobuf/stubs/structurally_valid.cc"
	"${PROJECT_SOURCE_DIR}/typy.cc"
	"${_TYPY_DIR}/all.cc"
	${CLASSES}
)
