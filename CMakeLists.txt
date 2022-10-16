cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(libNIM VERSION 0.1 DESCRIPTION "Library for Open-NIM modules." HOMEPAGE_URL "https://github.com/Open-NIM" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()

add_subdirectory(src)
add_executable(a.out main.cpp)
target_include_directories(a.out PUBLIC include/serial)
target_link_libraries(a.out PUBLIC serial NIM)

