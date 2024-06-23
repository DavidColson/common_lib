project(common_lib_tests)

# Polyscript tests library
# ------------------------------------

set(SOURCES 
	"tests_main.cpp")

add_executable(common_lib_tests ${SOURCES})
target_link_libraries(common_lib_tests common_lib)

target_include_directories(common_lib_tests PUBLIC
	"../source/")
