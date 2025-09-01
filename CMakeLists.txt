cmake_minimum_required(VERSION 3.8)
project(my_project)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(sources
  src/nvimPlugins.cpp
)

add_executable(my_app ${sources})

target_include_directories(my_app PUBLIC
  "${CMAKE_SOURCE_DIR}/include"
)

