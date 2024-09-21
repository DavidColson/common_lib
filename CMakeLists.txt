cmake_minimum_required(VERSION 3.28)

project(common_lib)

# Global options
# ------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
elseif(MSVC)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor /GR-")
endif()

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_subdirectory(tests)

# god knows why cmake wanted me to put this here
# if(COMMAND cmake_policy)
#     cmake_policy(SET CMP0003 NEW)
# endif(COMMAND cmake_policy)

# Polyscript core library
# ------------------------------------

set(SOURCES 
	"source/AABB.cpp"
	"source/AABB.h"
	"source/base64.h"
	"source/base64.cpp"
	"source/defer.h"
	"source/hashmap.h"
	"source/hashmap.inl"
	"source/json.cpp"
	"source/json.h"
	"source/light_string.cpp"
	"source/light_string.h"
	"source/log.cpp"
	"source/log.h"
	"source/maths.cpp"
	"source/maths.h"
	"source/matrix.h"
	"source/matrix.inl"
	"source/memory.cpp"
	"source/memory.h"
	"source/memory_tracker.cpp"
	"source/memory_tracker.h"
	"source/platform_debug.cpp"
	"source/platform_debug.h"
	"source/quat.h"
	"source/quat.inl"
	"source/resizable_array.h"
	"source/resizable_array.inl"
	"source/scanning.cpp"
	"source/scanning.h"
	"source/sort.h"
	"source/stack.h"
	"source/stack.inl"
	"source/string_builder.cpp"
	"source/string_builder.h"
	"source/string_hash.h"
	"source/testing.h"
	"source/types.h"
	"source/vec2.h"
	"source/vec2.inl"
	"source/vec3.h"
	"source/vec3.inl"
	"source/vec4.h"
	"source/vec4.inl")

add_library(common_lib ${SOURCES})

if( MSVC )
	target_link_libraries(common_lib dbghelp)
endif()

