
project "CommonLib"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	exceptionhandling "Off"
	rtti "Off"
	files 
	{
		"source/memory.h",
		"source/memory.cpp",
		"source/resizable_array.h",
		"source/hashmap.h",
		"source/light_string.h",
		"source/light_string.cpp",
		"source/string_builder.h",
		"source/testing.h",
		"source/memory_tracker.h",
		"source/memory_tracker.cpp",
		"source/linear_allocator.h",
		"source/linear_allocator.cpp",
		"source/json.cpp",
		"source/json.h",
		"source/scanning.h",
		"source/scanning.cpp",
		"source/sort.h",
		"source/log.h",
		"source/log.cpp",
		"source/defer.h",
		"source/platform_debug.h",
		"source/platform_debug.cpp"
	}
	filter { "system:windows", "configurations:Debug*" }
		links { "dbghelp" }