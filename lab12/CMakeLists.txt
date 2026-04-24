cmake_minimum_required(VERSION 3.10)
project(SmartHome VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "include/*.h")

list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

add_library(smarthome ${SOURCES})

target_include_directories(smarthome PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(smarthome_app src/main.c)

target_link_libraries(smarthome_app PRIVATE smarthome)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(smarthome PRIVATE -g -O0)
    target_compile_options(smarthome_app PRIVATE -g -O0)
    message(STATUS "Debug build: with debug symbols, no optimization")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(smarthome PRIVATE -O3 -DNDEBUG)
    target_compile_options(smarthome_app PRIVATE -O3 -DNDEBUG)
    message(STATUS "Release build: optimized, no debug symbols")
endif()

add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/smarthome_app
    DEPENDS smarthome_app
    COMMENT "Запуск программы SmartHome"
)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Library type: $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,shared,static>")
message(STATUS "Source files: ${SOURCES}")