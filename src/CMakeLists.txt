
# Add source to this project's executable.
add_executable(vulkan-cube
        main.cpp
        vk_types.h
        vk_initializers.cpp
        vk_initializers.h
        vk_engine.h
        vk_engine.cpp
        vk_mesh.h
        vk_mesh.cpp
)

set_property(TARGET vulkan-cube PROPERTY CXX_STANDARD 20)
target_compile_definitions(vulkan-cube PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(vulkan-cube PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(vulkan-cube PUBLIC vma tinyobjloader glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2 vkbootstrap imgui fastgltf::fastgltf)

target_precompile_headers(vulkan-cube PUBLIC <optional> <vector> <memory> <string> <vector> <unordered_map> <glm/mat4x4.hpp> <glm/vec4.hpp> <vulkan/vulkan.h>)

add_custom_command(TARGET vulkan-cube POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:vulkan-cube> $<TARGET_FILE_DIR:vulkan-cube>
        COMMAND_EXPAND_LISTS
)