# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

project(typyc)

cmake_minimum_required(VERSION 2.8)

set(_TYPY_DIR "${PROJECT_SOURCE_DIR}/../../test/typy")
include("${_TYPY_DIR}/classes.cmake")

set(HEADER_FILES
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/abstract.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/dict.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/fixedpoint.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/list.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/object.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/shadow.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/typy.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/utils.h"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/variant.h"
)

set(SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/main.c"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/arena.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/message_lite.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/repeated_field.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/generated_message_util.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/wire_format_lite.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/io/coded_stream.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/io/zero_copy_stream.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/io/zero_copy_stream_impl_lite.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/atomicops_internals_x86_gcc.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/atomicops_internals_x86_msvc.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/common.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/int128.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/once.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/status.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/stringpiece.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/stringprintf.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/google/protobuf/stubs/structurally_valid.cc"
	"${PROJECT_SOURCE_DIR}/../../typy/cpp/typy.cc"
	"${_TYPY_DIR}/all.cc"
	${CLASSES}
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
include_directories("${PROJECT_SOURCE_DIR}/../../typy/cpp")

if(WIN32)
	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
	)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
	include_directories("C:/Python27/include")
	target_link_libraries(${PROJECT_NAME} "C:/Python27/libs/python27.lib")
else(WIN32)
	find_package(PythonLibs REQUIRED)
	if(PYTHONLIBS_FOUND)
		message("Python found " ${PYTHONLIBS_VERSION_STRING})
		include_directories(${PYTHON_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})
	endif()
endif(WIN32)
