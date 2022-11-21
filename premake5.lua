
local BUILD_DIR = path.join("Build", _ACTION)
if _OPTIONS["cc"] ~= nil then
	BUILD_DIR = BUILD_DIR .. "_" .. _OPTIONS["cc"]
end

solution "CommonLib"
    location(BUILD_DIR)
    startproject "Tests"
    configurations { "Release", "Debug" }
    platforms "x86_64"
    architecture "x86_64"
    filter "configurations:Release"
        optimize "Full"
    filter "configurations:Debug*"
        defines "_DEBUG"
        optimize "Debug"
        symbols "Full"
    
    project "Tests"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        exceptionhandling "Off"
        rtti "Off"
        debugdir ""
        files 
        {
            "tests_main.cpp",
            "sys_alloc.h",
            "array.h",
            "hashmap.h",
            "string.h",
            "string.cpp",
            "string_view.h",
            "string_view.cpp",
            "testing.h"
        }