
# Add source to this project's executable.
add_executable(vulkan-triangle
        main.cpp
        vk_types.h
        vk_initializers.cpp
        vk_initializers.h
        vk_engine.h
        vk_engine.cpp
)

set_property(TARGET vulkan-triangle PROPERTY CXX_STANDARD 20)
target_compile_definitions(vulkan-triangle PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(vulkan-triangle PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(vulkan-triangle PUBLIC vma glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2 vkbootstrap imgui fastgltf::fastgltf)

target_precompile_headers(vulkan-triangle PUBLIC <optional> <vector> <memory> <string> <vector> <unordered_map> <glm/mat4x4.hpp> <glm/vec4.hpp> <vulkan/vulkan.h>)

add_custom_command(TARGET vulkan-triangle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:vulkan-triangle> $<TARGET_FILE_DIR:vulkan-triangle>
        COMMAND_EXPAND_LISTS
)