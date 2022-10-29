cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(libNIM VERSION 0.1 DESCRIPTION "Library for Open-NIM modules." HOMEPAGE_URL "https://github.com/Open-NIM" LANGUAGES CXX)

set(MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	SET(GCC_FLAGS "-Wall -g -pedantic -pedantic-errors -ansi -Wextra -Weffc++ -std=c++20 -O0")
	add_definitions(${GCC_FLAGS})
endif()


add_subdirectory(src)
add_executable(a.out test/main.cpp)
target_include_directories(a.out PUBLIC include/serial)
target_link_libraries(a.out PUBLIC serial NIM)

