
project "common_lib"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	exceptionhandling "Off"
	rtti "Off"
	files 
	{
		"source/*.cpp",
		"source/*.h"
	}
	filter { "system:windows", "configurations:Debug*" }
		links { "dbghelp" }
        buildoptions { "/fsanitize=address" }
        flags { "NoIncrementalLink" }
        editandcontinue "Off"