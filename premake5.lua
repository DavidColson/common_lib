
local BUILD_DIR = path.join("build", _ACTION)
if _OPTIONS["cc"] ~= nil then
	BUILD_DIR = BUILD_DIR .. "_" .. _OPTIONS["cc"]
end

solution "CommonLib"
    location(BUILD_DIR)
    startproject "Tests"
    configurations { "Release", "Debug" }
    platforms "x86_64"
    architecture "x86_64"
	--flags { "FatalWarnings" }
    filter "configurations:Release"
        optimize "Full"
    filter "configurations:Debug*"
        defines { "_DEBUG", "MEMORY_TRACKING" }
        optimize "Debug"
        symbols "Full"
    filter { "system:windows", "configurations:Debug*" }
		buildoptions { "/fsanitize=address" }
		flags { "NoIncrementalLink" }
		editandcontinue "Off"
		links { "dbghelp" }
	
	dofile("commonlib.lua")

    project "Tests"
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
			"CommonLib"
		}