cmake_minimum_required(VERSION 3.31)

# Setup the main project
project(linearColors)
add_executable(linearColors main.cpp)

# Add GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(linearColors PRIVATE glm::glm)