
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
	--filter { "system:windows", "configurations:Debug*" }
	links { "dbghelp" }
	if ASAN_Enabled then
        buildoptions { "/fsanitize=address" }
        flags { "NoIncrementalLink" }
        editandcontinue "Off"
	end

	project "common_lib_tests"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        exceptionhandling "Off"
        rtti "Off"
        debugdir ""
        files 
        {
            "tests/tests_main.cpp",
        }
		includedirs
		{
			"source/"
		}
		links
		{
			"common_lib"
		}
