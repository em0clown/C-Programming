cmake_minimum_required(VERSION 3.14)
project(Poker)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(SOURCES
    src/main.cpp
    src/game_state.cpp
    src/game_view.cpp
    src/poker_engine.cpp
)

add_executable(poker ${SOURCES})

target_include_directories(poker PRIVATE inc ${GTK3_INCLUDE_DIRS})
target_link_libraries(poker ${GTK3_LIBRARIES})

file(COPY ${CMAKE_SOURCE_DIR}/src/table.png DESTINATION ${CMAKE_BINARY_DIR})

message(STATUS "Poker game with background and logs ready")
