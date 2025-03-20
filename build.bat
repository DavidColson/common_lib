@echo off
setlocal

if not exist build mkdir build

pushd build
cl ..\tests\tests_main.cpp /I ..\source /Zc:preprocessor /Od /Zi /D_DEBUG /std:c++17 /link /out:common_lib_tests.exe
popd
