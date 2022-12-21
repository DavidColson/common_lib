
project "common_lib"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	exceptionhandling "Off"
	rtti "Off"
	files 
	{
		"source/*.cpp",
		"source/*.h",
		"source/*.inl"
	}
	filter { "system:windows", "configurations:Debug*" }
		links { "dbghelp" }
	if ASAN_Enabled then
        buildoptions { "/fsanitize=address" }
        flags { "NoIncrementalLink" }
        editandcontinue "Off"
	end