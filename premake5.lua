
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
	flags { "FatalWarnings" }
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
            "resizable_array.h",
            "hashmap.h",
            "string.h",
            "string_builder.h",
            "testing.h",
            "memory_tracker.h",
            "memory_tracker.cpp",
            "linear_allocator.h",
            "linear_allocator.cpp",
            "json.cpp",
            "json.h",
            "scanning.h",
            "scanning.cpp"
        }