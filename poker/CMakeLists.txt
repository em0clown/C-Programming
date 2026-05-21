cmake_minimum_required(VERSION 3.14)
project(PokerGame)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_executable(poker src/poker.cpp)

target_include_directories(poker PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(poker ${GTK3_LIBRARIES})

message(STATUS "Poker Game ready")
