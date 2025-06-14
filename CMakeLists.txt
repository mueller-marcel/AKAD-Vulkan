cmake_minimum_required (VERSION 3.8)

project ("vulkan_guide")

find_package(Vulkan REQUIRED)

# Add libraries
add_subdirectory(third_party)

# Set the output directories for the binaries
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# Add the directory for the source code
add_subdirectory(src)

# Find the GLSL validator otherwise print error message
find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

if (NOT GLSL_VALIDATOR)
    message(FATAL_ERROR "GLSL Validator was not found")
endif ()

# Collect all shader files by their file extension
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
    )

# Set list of SPIRV files
set(SPIRV_BINARY_FILES)

# Iterate over these files and compile them
foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})

  # Add the compiled files to the list
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

# Create a target for the compilation
add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )
