cmake_minimum_required(VERSION 3.31)
project(linearColors)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

add_executable(linearColors main.cpp)

target_link_libraries(linearColors PRIVATE glfw)
