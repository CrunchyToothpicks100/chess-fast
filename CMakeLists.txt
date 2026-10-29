cmake_minimum_required(VERSION 3.28)

project(chess_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main main.cpp)

target_sources(main
  PRIVATE
    FILE_SET CXX_MODULES
    FILES allMasks.cppm
)
