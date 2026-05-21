cmake_minimum_required(VERSION 3.28)
project(poker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(poker ${SOURCES})

target_include_directories(poker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(poker PRIVATE PkgConfig::GTK3)

if(MSVC)
    target_compile_options(poker PRIVATE /W4)
else()
    target_compile_options(poker PRIVATE -Wall -Wextra -Wpedantic)
endif()