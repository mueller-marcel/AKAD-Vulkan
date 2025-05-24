cmake_minimum_required(VERSION 3.31)

# Setup the main project
project(linearColors)

# Define the cpp standard to be used
set(CMAKE_CXX_STANDARD 20)

# Define the entry point
add_executable(linearColors src/main.cpp)

# Add GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(linearColors PRIVATE glm::glm)

# Add SDL
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(linearColors PRIVATE SDL3::SDL3)

# Add IMGUI
find_package(imgui CONFIG REQUIRED)
target_link_libraries(linearColors PRIVATE imgui::imgui)

# Add STB
find_package(Stb REQUIRED)
target_include_directories(linearColors PRIVATE ${Stb_INCLUDE_DIR})

# Add Tiny obj loader
find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries(linearColors PRIVATE tinyobjloader::tinyobjloader)

# Add VK bootstrap
find_package(vk-bootstrap CONFIG REQUIRED)
target_link_libraries(linearColors PRIVATE vk-bootstrap::vk-bootstrap vk-bootstrap::vk-bootstrap-compiler-warnings)

# Add vulkan memory allocator
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(linearColors PRIVATE Vulkan::Headers GPUOpen::VulkanMemoryAllocator)
