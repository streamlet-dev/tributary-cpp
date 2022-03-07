cmake_minimum_required(VERSION 3.7.2)
project(tributary)
include(CheckCCompilerFlag)

set(CMAKE_CXX_STANDARD 20)
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

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
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

option(T_CPP_TESTS "Build the C++ tests" ON)

if(DEFINED ENV{T_DEBUG})
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    if(NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RELEASE)
    endif()
endif()

set(T_CPP_SRC "${CMAKE_SOURCE_DIR}")
message("${Cyan}Building C++ binding${ColorReset}")

message("${Cyan}Building C++ with strict warnings${ColorReset}")

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER )
if(CMAKE_BUILD_TYPE_LOWER STREQUAL debug)
    message("${Red}Building DEBUG${ColorReset}")
    add_definitions(-DT_DEBUG)
else()
    message("${Cyan}Building RELEASE${ColorReset}")
endif()

include_directories("${CMAKE_SOURCE_DIR}/include")

# Boost is a system dependency and must be present and built on the system.
find_package(Boost REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "${Red}Boost could not be located${ColorReset}")
else()
    message("${Cyan}Found Boost: `Boost_INCLUDE_DIRS`: ${Boost_INCLUDE_DIRS}, `Boost_LIBRARY_DIRS` - ${Boost_LIBRARY_DIRS} ${ColorReset}")
    include_directories( ${Boost_INCLUDE_DIRS} )
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await /EHsc /MP /bigobj")
elseif(MACOS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fcoroutines -Wall")
endif()

if(WIN32)
    add_definitions(-DBOOST_UUID_FORCE_AUTO_LINK)
    foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}")
    endforeach(warning)
    if(CMAKE_BUILD_TYPE_LOWER STREQUAL debug)
        set(CMAKE_CXX_FLAGS " \
            ${CMAKE_CXX_FLAGS} \
            /DEBUG \
            /Z7 \
            /Zi \
            ")
    else()
        set(CMAKE_CXX_FLAGS " \
            ${CMAKE_CXX_FLAGS} \
            /NDEBUG \
            /O2 \
            ")
    endif()
else()
    if(CMAKE_BUILD_TYPE_LOWER STREQUAL debug)
        set(CMAKE_CXX_FLAGS " \
            ${CMAKE_CXX_FLAGS} \
            -O0 \
            -g3 \
            ")
    else()
        set(CMAKE_CXX_FLAGS " \
            ${CMAKE_CXX_FLAGS} \
            -O3 \
            -DNDEBUG \
            -g0 \
            ")
    endif()

    include_directories("/usr/local/include")
    # SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
endif()


# Build exprtk for expression parsing
t_build_dep("exprtk" "${T_CMAKE_MODULE_PATH}/exprtk.txt.in")

# t_build_dep("libuv" "${T_CMAKE_MODULE_PATH}/libuv.txt.in")
t_build_dep("cppcoro" "${T_CMAKE_MODULE_PATH}/cppcoro.txt.in")

set (SOURCE_FILES
    ${T_CPP_SRC}/src/base.cpp
    ${T_CPP_SRC}/src/lazy.cpp
    ${T_CPP_SRC}/src/streaming.cpp
    ${T_CPP_SRC}/src/utils.cpp
    )

add_library(tributary SHARED ${SOURCE_FILES})
target_link_libraries(tributary cppcoro)

if(WIN32)
    target_compile_definitions(tributary PUBLIC T_EXPORTS=1 PRIVATE WIN32=1 _WIN32=1)
elseif(WIN32)
    target_compile_options(tributary PRIVATE -Wall -Werror $<$<CONFIG:DEBUG>:-fPIC -O0>)
endif()

if(T_CPP_TESTS)
    add_executable(test_nongtest ${T_CPP_SRC}/test/test_nongtest.cpp)
    target_link_libraries(test_nongtest tributary)

    find_package(GTest REQUIRED)
    include(GoogleTest)

    if(${GTEST_INCLUDE_DIRS})
        include_directories(${GTEST_INCLUDE_DIRS})
    endif()

    enable_testing()


    if (WIN32)
        target_compile_definitions(test_nongtest PUBLIC WIN32=1 _WIN32=1)

        add_executable(test_streaming ${T_CPP_SRC}/test/test_streaming.cpp ${SOURCE_FILES})
        target_compile_definitions(test_streaming PRIVATE T_EXPORTS=1 WIN32=1 _WIN32=1)
        target_link_libraries(test_streaming GTest::Main)
    else()
        target_compile_options(test_nongtest PRIVATE -Wall -Werror $<$<CONFIG:DEBUG>:-fPIC -O0>)

        add_executable(test_streaming ${T_CPP_SRC}/test/test_streaming.cpp)
        target_link_libraries(test_streaming PRIVATE GTest::Main tributary)
    endif()

    gtest_discover_tests(test_streaming)

endif()
