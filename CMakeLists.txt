cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Create project
project(cmdseqedit
        DESCRIPTION "Edit command sequence files used for compiling maps in GoldSrc / Source engine Hammer."
        VERSION "1.0.0"
        HOMEPAGE_URL "https://github.com/craftablescience/CmdSeqEdit")
set(PROJECT_NAME_PRETTY "CmdSeqEdit")
set(PROJECT_ORGANIZATION_NAME "craftablescience" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include CMake libraries
include(CheckIPOSupported)
include(GNUInstallDirs)

# Set proper runpath
set(CMAKE_SKIP_BUILD_RPATH OFF)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_INSTALL_RPATH $ORIGIN)

# Compile with PIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compile with LTO if supported
option(CMDSEQEDIT_USE_LTO "Compile ${PROJECT_NAME_PRETTY} with LTO/IPO" OFF)
if(CMDSEQEDIT_USE_LTO)
    check_ipo_supported(RESULT CMDSEQEDIT_USE_LTO)
endif()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${CMDSEQEDIT_USE_LTO})

# Set default install directory permissions
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Macro to set up many things at once for a given target
function(cmdseqedit_configure_target TARGET)
    # Define DEBUG macro
    target_compile_definitions(${TARGET} PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

    # Set optimization flags
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        # Build with debug friendly optimizations and debug symbols (MSVC defaults are fine)
        if(UNIX OR MINGW)
            target_compile_options(${TARGET} PRIVATE -Og -g)
        endif()
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT MSVC)
            target_compile_options(${TARGET} PRIVATE -fno-limit-debug-info)
        endif()
    else()
        # Build with optimizations and don't omit stack pointer for debugging (MSVC defaults are fine)
        if(UNIX OR MINGW)
            target_compile_options(${TARGET} PRIVATE -O2 -fno-omit-frame-pointer)
        endif()
    endif()

    if(WIN32 AND MSVC)
        get_target_property(TARGET_TYPE ${TARGET} TYPE)
        get_target_property(TARGET_IS_GUI ${TARGET} WIN32_EXECUTABLE)

        # Create PDBs in release
        if(MSVC AND ((TARGET_TYPE STREQUAL "SHARED_LIBRARY") OR (TARGET_TYPE STREQUAL "EXECUTABLE")))
            target_compile_options(
                    ${TARGET} PRIVATE
                    "$<$<CONFIG:Release>:/Zi>")
            target_link_options(
                    ${TARGET} PRIVATE
                    "$<$<CONFIG:Release>:/DEBUG>"
                    "$<$<CONFIG:Release>:/OPT:REF>"
                    "$<$<CONFIG:Release>:/OPT:ICF>")
        endif()

        # Add an icon to the executable
        if(TARGET_TYPE STREQUAL "EXECUTABLE")
            target_sources(
                    ${TARGET} PRIVATE
                    "${CMAKE_CURRENT_SOURCE_DIR}/res/logo.rc")
        endif()

        # Don't show the console when running the executable
        if(MSVC AND TARGET_IS_GUI)
            target_link_options(
                    ${TARGET} PRIVATE
                    "/ENTRY:mainCRTStartup")
        endif()
    endif()
endfunction()

# Include third-party libraries
include("${CMAKE_CURRENT_SOURCE_DIR}/ext/_ext.cmake")

# Create executable
include("${CMAKE_CURRENT_SOURCE_DIR}/src/_src.cmake")

# Configure installer
include("${CMAKE_CURRENT_SOURCE_DIR}/installer/_installer.cmake")
