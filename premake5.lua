
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
        defines { "_DEBUG", "MEMORY_TRACKING" }
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
            "memory.h",
            "memory.cpp",
            "array.h",
            "hashmap.h",
            "string.h",
            "string_view.h",
            "testing.h",
            "memory_tracker.h",
            "memory_tracker.cpp"
        }