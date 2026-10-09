cmake_minimum_required(VERSION 3.15)
project(clothing_server VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
#  Source files
# ============================================================================
set(SERVER_SOURCES
    src/main.cpp
    src/natives.cpp
    src/clothing_manager.cpp
    src/network.cpp
    src/callbacks.cpp
    src/plugin.def
)

set(SERVER_HEADERS
    src/plugin.h
    src/natives.h
    src/clothing_manager.h
    src/network.h
    src/callbacks.h
)

# ============================================================================
#  Create shared library (DLL on Windows, SO on Linux)
# ============================================================================
add_library(clothing SHARED ${SERVER_SOURCES} ${SERVER_HEADERS})

# Include directories
target_include_directories(clothing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/sdk
    ${CMAKE_SOURCE_DIR}/sdk/amx
)

# ============================================================================
#  Platform-specific settings
# ============================================================================
if(WIN32)
    # Windows: output as clothing.dll
    set_target_properties(clothing PROPERTIES
        OUTPUT_NAME "clothing"
        PDB_NAME "clothing_server"
        COMPILE_PDB_NAME "clothing_server"
        SUFFIX ".dll"
        PREFIX ""
    )
    target_compile_definitions(clothing PRIVATE
        _WIN32
        WIN32
        _WINDOWS
        _CRT_SECURE_NO_WARNINGS
        _WINSOCK_DEPRECATED_NO_WARNINGS
    )
    target_link_libraries(clothing PRIVATE
        ws2_32
        psapi
    )
else()
    # Linux: output as clothing.so
    set_target_properties(clothing PROPERTIES
        OUTPUT_NAME "clothing"
        SUFFIX ".so"
        PREFIX ""
    )
    target_compile_definitions(clothing PRIVATE
        LINUX
        _GNU_SOURCE
    )
    target_link_libraries(clothing PRIVATE
        dl
        pthread
    )
    # Linux-specific flags
    target_compile_options(clothing PRIVATE
        -fno-strict-aliasing
        -Wno-write-strings
    )
endif()

# Common compile options
target_compile_options(clothing PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /wd4996>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wno-unused-function>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wno-unused-function>
)

# ============================================================================
#  Post-build: Copy to test server
# ============================================================================
add_custom_command(TARGET clothing POST_BUILD
    # Copy to output/server/plugins/
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/output/server/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:clothing>"
        "${CMAKE_SOURCE_DIR}/output/server/plugins/$<TARGET_FILE_NAME:clothing>"

    # Copy to SA-MP SERVER test folder
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/../SA-MP SERVER/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:clothing>"
        "${CMAKE_SOURCE_DIR}/../SA-MP SERVER/plugins/$<TARGET_FILE_NAME:clothing>"

    COMMENT "Copying server plugin to output and test folders"
)
