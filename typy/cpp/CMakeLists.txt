# Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
# Use of this source code is governed by The MIT License
# that can be found in the LICENSE file.

project(typy)

cmake_minimum_required(VERSION 2.8)

include("${PROJECT_SOURCE_DIR}/typy.cmake")

set(LIBNAME "_${PROJECT_NAME}")

add_library(${LIBNAME} SHARED ${SOURCE_FILES})
include_directories(${PROJECT_SOURCE_DIR})

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /Ox /MD /W3 /DNDEBUG /EHsc")
	set(CMAKE_SHARED_LINKER_FLAGS "/DLL /nologo /INCREMENTAL:NO /EXPORT:init${LIBNAME}")
	set_target_properties(${LIBNAME} PROPERTIES SUFFIX ".pyd")

	include_directories("C:/Python27/include")
	target_link_libraries(${LIBNAME} "C:/Python27/libs/python27.lib")
else(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -DNDEBUG -g -fwrapv -O2 -Wall -fno-strict-aliasing -D_FORTIFY_SOURCE=2 -g -fstack-protector-strong -Wformat -Werror=format-security -fPIC -Wno-write-strings -Wno-unused-value")
	set_target_properties(${LIBNAME} PROPERTIES PREFIX "")

	find_package(Threads REQUIRED)
	if (CMAKE_USE_PTHREADS_INIT)
		add_definitions(-DHAVE_PTHREAD)
	endif (CMAKE_USE_PTHREADS_INIT)

	find_package(PythonLibs REQUIRED)
	if(PYTHONLIBS_FOUND)
		message("Python found " ${PYTHONLIBS_VERSION_STRING})
		include_directories(${PYTHON_INCLUDE_DIRS})
		target_link_libraries(${LIBNAME} ${PYTHON_LIBRARIES})
	endif()
endif(WIN32)
