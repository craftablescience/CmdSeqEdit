# Qt
if(WIN32 AND NOT DEFINED QT_BASEDIR)
    message(FATAL_ERROR "Please define your QT install dir with -DQT_BASEDIR=\"C:/your/qt6/here\"")
endif()

if(DEFINED QT_BASEDIR)
    string(REPLACE "\\" "/" QT_BASEDIR "${QT_BASEDIR}")

    # Add it to the prefix path so find_package can find it
    list(APPEND CMAKE_PREFIX_PATH "${QT_BASEDIR}")
    set(QT_INCLUDE "${QT_BASEDIR}/include")
    message(STATUS "Using ${QT_INCLUDE} as the Qt include directory")
endif()

if(WIN32)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(QT_LIB_SUFFIX "d" CACHE STRING "" FORCE)
    else()
        set(QT_LIB_SUFFIX "" CACHE STRING "" FORCE)
    endif()
endif()

# CMake has an odd policy that links a special link lib for Qt on newer versions of CMake
cmake_policy(SET CMP0020 NEW)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# sourcepp
set(SOURCEPP_LIBS_START_ENABLED OFF CACHE INTERNAL "" FORCE)
set(SOURCEPP_USE_KVPP            ON CACHE INTERNAL "" FORCE)
set(SOURCEPP_USE_STEAMPP         ON CACHE INTERNAL "" FORCE)
set(SOURCEPP_USE_TOOLPP          ON CACHE INTERNAL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sourcepp")
