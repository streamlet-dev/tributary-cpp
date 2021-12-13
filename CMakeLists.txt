cmake_minimum_required(VERSION 3.7.2)
project(tributary)
include(CheckCCompilerFlag)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED T_CMAKE_MODULE_PATH)
    set(T_CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
endif()
set(CMAKE_MODULE_PATH "${T_CMAKE_MODULE_PATH}/modules" ${CMAKE_MODULE_PATH})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WIN32 ON)
    set(MACOS OFF)
    set(LINUX OFF)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(WIN32 OFF)
    set(MACOS ON)
    set(LINUX OFF)
else()
    set(WIN32 OFF)
    set(MACOS OFF)
    set(LINUX ON)
endif()

###################################################
# Helper to grab dependencies from remote sources #
###################################################
function (t_build_dep name cmake_file)
    if(EXISTS ${CMAKE_BINARY_DIR}/${name}-build)
        message("${Cyan}Dependency found - not rebuilding - ${CMAKE_BINARY_DIR}/${name}-build${ColorReset}")
    else()
        configure_file(${cmake_file} ${name}-download/CMakeLists.txt)

        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${name}-download )

        if(result)
            message(FATAL_ERROR "CMake step for ${name} failed: ${result}")
        endif()

        execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${name}-download )

        if(result)
            message(FATAL_ERROR "Build step for ${name} failed: ${result}")
        endif()
    endif()

    if(${name} STREQUAL arrow)
        # Overwrite arrow's CMakeLists with our custom, minimal CMakeLists.
        configure_file(${T_CMAKE_MODULE_PATH}/${name}/CMakeLists.txt ${CMAKE_BINARY_DIR}/${name}-src/cpp/ COPYONLY)
        configure_file(${T_CMAKE_MODULE_PATH}/${name}/config.h ${CMAKE_BINARY_DIR}/${name}-src/cpp/src/arrow/util/ COPYONLY)
        add_subdirectory(${CMAKE_BINARY_DIR}/${name}-src/cpp/
            ${CMAKE_BINARY_DIR}/${name}-build
            EXCLUDE_FROM_ALL)

        include_directories(${CMAKE_BINARY_DIR}/${name}-src/cpp/src/)
    elseif(${name} STREQUAL exprtk)
        # no cmakelists - just include the header
        include_directories(${CMAKE_BINARY_DIR}/${name}-src)
    elseif(${name} STREQUAL re2)
        # Overwrite re2's CMakeLists with our custom CMakeLists.
        configure_file(${T_CMAKE_MODULE_PATH}/${name}/CMakeLists.txt ${CMAKE_BINARY_DIR}/${name}-src/ COPYONLY)
        include_directories(${CMAKE_BINARY_DIR}/${name}-src)

        add_subdirectory(${CMAKE_BINARY_DIR}/${name}-src
            ${CMAKE_BINARY_DIR}/${name}-build
            EXCLUDE_FROM_ALL)
    else()
        add_subdirectory(${CMAKE_BINARY_DIR}/${name}-src
            ${CMAKE_BINARY_DIR}/${name}-build
            EXCLUDE_FROM_ALL)

        include_directories(${CMAKE_BINARY_DIR}/${name}-src/extras/${name}/include)
        include_directories(${CMAKE_BINARY_DIR}/${name}-src/include)
        include_directories(${CMAKE_BINARY_DIR}/${name}-src)
    endif()
endfunction()
##############################

#######################
# BUILD CONFIGURATION #
#######################
find_package(Color)

option(CMAKE_BUILD_TYPE "Release/Debug build" RELEASE)
option(T_CPP_BUILD_STRICT "Build the C++ with strict warnings" OFF)

if(DEFINED ENV{T_DEBUG})
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    if (NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RELEASE)
    endif()
endif()

if (NOT DEFINED T_CPP_BUILD_STRICT)
    set(T_CPP_BUILD_STRICT OFF)
endif()


set(T_CPP_SRC "${CMAKE_SOURCE_DIR}")
message("${Cyan}Building C++ binding${ColorReset}")

if (T_CPP_BUILD_STRICT)
    message("${Cyan}Building C++ with strict warnings${ColorReset}")
else()
    message("${Yellow}Building C++ without strict warnings${ColorReset}")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER )
if(CMAKE_BUILD_TYPE_LOWER STREQUAL debug)
    message("${Red}Building DEBUG${ColorReset}")
    add_definitions(-DT_DEBUG)
else()
    message("${Cyan}Building RELEASE${ColorReset}")
endif()

include_directories("${CMAKE_SOURCE_DIR}/src/include")

# Boost is a system dependency and must be present and built on the system.
find_package(Boost REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "${Red}Boost could not be located${ColorReset}")
else()
    message("${Cyan}Found Boost: `Boost_INCLUDE_DIRS`: ${Boost_INCLUDE_DIRS}, `Boost_LIBRARY_DIRS` - ${Boost_LIBRARY_DIRS} ${ColorReset}")
    include_directories( ${Boost_INCLUDE_DIRS} )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++1y")
endif()

if(WIN32)
    if(CMAKE_BUILD_TYPE_LOWER STREQUAL debug)
        set(OPT_FLAGS " \
            /DEBUG \
            /Z7 \
            /Zi
            ")
    else()
        set(OPT_FLAGS " \
            /NDEBUG \
            /O2 \
            ")
    endif()
    foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}")
    endforeach(warning)
    set(CMAKE_CXX_FLAGS " /EHsc /MP /bigobj")
    set(CMAKE_SHARED_LIBRARY_PREFIX lib)
else()
    if(CMAKE_BUILD_TYPE_LOWER STREQUAL debug)
        set(OPT_FLAGS " \
            -O0 \
            -g3 \
            ")
    else()
        set(OPT_FLAGS " \
            -O3 \
            -g0 \
            ")
    endif()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
    include_directories("/usr/local/include")
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")
    set(CMAKE_SHARED_LIBRARY_SUFFIX .so)
    # Look for the binary using @loader_path (relative to binary location)
    set(CMAKE_MACOSX_RPATH TRUE)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_NAME_DIR "@rpath/")
    SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

    # module_origin_path is the location of the binary
    if(MACOS)
        set(module_origin_path "@loader_path/")
    else()
        set(module_origin_path "\$ORIGIN")
    endif()
endif()


set(CMAKE_C_FLAGS " \
    ${CMAKE_C_FLAGS} \
    ${EXTENDED_FLAGS} \
    ${OPT_FLAGS} \
    ")

# Build exprtk for expression parsing
t_build_dep("exprtk" "${T_CMAKE_MODULE_PATH}/exprtk.txt.in")

set (SOURCE_FILES
    ${T_CPP_SRC}/src/cpp/base.cpp
    )

add_library(tributary SHARED ${SOURCE_FILES})

if(T_CPP_BUILD_STRICT AND NOT WIN32)
    target_compile_options(tributary PRIVATE -Wall -Werror)
    target_compile_options(tributary PRIVATE $<$<CONFIG:DEBUG>:-fPIC -O0>)
elseif(WIN32)
    target_compile_definitions(tributary PRIVATE T_EXPORTS=1)
    target_compile_definitions(tributary PRIVATE WIN32=1)
    target_compile_definitions(tributary PRIVATE _WIN32=1)
endif()
