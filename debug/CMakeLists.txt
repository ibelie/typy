project(typyd)

cmake_minimum_required(VERSION 2.8)

set(SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/main.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/abstract.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/dict.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/enum.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/extension.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/fixedpoint.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/list.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/map.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/metaobject.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/object.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/port.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/protobuf.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/tree.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/typy.c"
	"${PROJECT_SOURCE_DIR}/../typy/cpy/variant.c"
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include_directories("${PROJECT_SOURCE_DIR}/../typy/cpy")

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
