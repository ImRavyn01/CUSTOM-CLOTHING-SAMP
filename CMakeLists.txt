cmake_minimum_required(VERSION 3.15)
project(CJClothingPlugin VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
#  Global settings
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

# Per-config output directories (avoid Debug/Release subfolders)
foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_SOURCE_DIR}/output)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_SOURCE_DIR}/output)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_SOURCE_DIR}/output)
endforeach()

# ============================================================================
#  Shared include directory
# ============================================================================
set(SHARED_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/shared)
set(SDK_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/sdk)

# ============================================================================
#  Sub-projects
# ============================================================================

# Server-side plugin (.dll on Windows, .so on Linux)
add_subdirectory(server)

# Client-side ASI plugin (Windows only)
if(WIN32)
    add_subdirectory(client)
endif()

# ============================================================================
#  Post-build: Copy outputs to correct folders
# ============================================================================
add_custom_target(copy_outputs ALL
    COMMENT "Copying build outputs to project folders..."
)

# Copy .inc file to output
add_custom_command(TARGET copy_outputs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/output/server/include"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/include/clothing.inc"
        "${CMAKE_SOURCE_DIR}/output/server/include/clothing.inc"
    COMMENT "Copying clothing.inc to output"
)

# Copy .inc to SA-MP server test folder
add_custom_command(TARGET copy_outputs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/../SA-MP SERVER/pawno/include"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/include/clothing.inc"
        "${CMAKE_SOURCE_DIR}/../SA-MP SERVER/pawno/include/clothing.inc"
    COMMENT "Copying clothing.inc to SA-MP server test folder"
)
