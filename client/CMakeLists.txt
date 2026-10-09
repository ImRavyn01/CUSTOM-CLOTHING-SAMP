cmake_minimum_required(VERSION 3.15)
project(clothing_client VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
#  Windows-only (ASI plugin)
# ============================================================================
if(NOT WIN32)
    message(WARNING "Client-side ASI plugin is Windows-only. Skipping.")
    return()
endif()

# ============================================================================
#  Source files
# ============================================================================
set(CLIENT_SOURCES
    src/main.cpp
    src/hooks.cpp
    src/clothing_client.cpp
    src/rpc_handler.cpp
)

set(CLIENT_HEADERS
    src/game_sa.h
    src/samp_structs.h
    src/hooks.h
    src/clothing_client.h
    src/rpc_handler.h
)

# ============================================================================
#  Create shared library (DLL -> renamed to .asi)
# ============================================================================
add_library(clothing_client SHARED ${CLIENT_SOURCES} ${CLIENT_HEADERS})

# Output as .asi (ASI Loader compatible)
set_target_properties(clothing_client PROPERTIES
    OUTPUT_NAME "clothing"
    PDB_NAME "clothing_client"
    COMPILE_PDB_NAME "clothing_client"
    SUFFIX ".asi"
    PREFIX ""
)

# Include directories
target_include_directories(clothing_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/shared
)

# ============================================================================
#  Compiler settings
# ============================================================================
target_compile_definitions(clothing_client PRIVATE
    _WIN32
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
)

target_compile_options(clothing_client PRIVATE
    /W3
    /wd4996
    /GS-         # Disable buffer security check (for hooks)
)

# Link libraries
target_link_libraries(clothing_client PRIVATE
    winmm
    psapi
    dbghelp
)

# ============================================================================
#  Post-build: Copy to output and GTA SAMP folder
# ============================================================================
add_custom_command(TARGET clothing_client POST_BUILD
    # Copy to output/client/
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/output/client"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:clothing_client>"
        "${CMAKE_SOURCE_DIR}/output/client/clothing.asi"

    # Copy to GTA SAMP test folder
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:clothing_client>"
        "${CMAKE_SOURCE_DIR}/../GTA SAMP/clothing.asi"

    COMMENT "Copying client ASI plugin to output and GTA SAMP folders"
)
