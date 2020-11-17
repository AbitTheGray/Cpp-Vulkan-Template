cmake_minimum_required(VERSION 3.17)
project(Vulkan_Template C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_policy(SET CMP0077 NEW) # set() overrides option()

#--------------------------------
# Debug Options
#--------------------------------

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>") # Define DEBUG in Debug builds

#--------------------------------
# Compiler configuration
#--------------------------------

if(MINGW)
    message(NOTICE "MinGW detected, setting compilation to Windows")
    set(WIN32 ON)
endif()

#--------------------------------
# 32-bit / 64-bit detection
#--------------------------------

if(NOT DEFINED CMAKE_SIZEOF_VOID_P)
    message(NOTICE "CMAKE_SIZEOF_VOID_P not defined, detecting...")

    set(archdetect_c_code "
#define COMPILE_TIME_ASSERT(val, comp)\
    char preprocessor_array[val == comp ? -1 : 0];

COMPILE_TIME_ASSERT(sizeof(void*), 4) // SIZEOF_VOID_P 4
COMPILE_TIME_ASSERT(sizeof(void*), 8) // SIZEOF_VOID_P 8
")
    file(WRITE "${CMAKE_BINARY_DIR}/SIZEOF_VOID_P.c" "${archdetect_c_code}")

    enable_language(C)

    try_run(
        RUN_RESULT_DONT_CARE
        COMPILE_RESULT_DONT_CARE
        "${CMAKE_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/SIZEOF_VOID_P.c"
        COMPILE_OUTPUT_VARIABLE SIZEOF_VOID_P
    )

    # Parse value from try_run output
    string(REGEX MATCH "SIZEOF_VOID_P ([a-zA-Z0-9_]+)" SIZEOF_VOID_P "${SIZEOF_VOID_P}")

    # Remove value marker
    string(REPLACE "SIZEOF_VOID_P " "" SIZEOF_VOID_P "${SIZEOF_VOID_P}")

    if ((${SIZEOF_VOID_P} STREQUAL "") OR (${SIZEOF_VOID_P} EQUAL 0))
        message(FATAL_ERROR "Invalid, unrecognized or unsupported void* size ${SIZEOF_VOID_P}")
    endif()
    set(CMAKE_SIZEOF_VOID_P ${SIZEOF_VOID_P}) # Store in CMAKE_ procedure

    message(NOTICE "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")
endif()

#--------------------------------
# External libraries
#--------------------------------

add_subdirectory(lib)

#--------------------------------
# Source
#--------------------------------

file(GLOB_RECURSE PROJECT_SOURCE src/*.hpp src/*.cpp src/*.h src/*.c)
add_library(Vulkan_Template STATIC ${PROJECT_SOURCE})
target_include_directories(Vulkan_Template PUBLIC src/)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") # GCC / MinGW
    target_link_libraries(Vulkan_Template -static-libgcc -static-libstdc++)
endif()

target_link_libraries(Vulkan_Template Vulkan::Vulkan)

#--------------------------------
# Tests
#--------------------------------

if(BUILD_TESTING)
    include(CTest)
    enable_testing()

    add_subdirectory(tests)
endif()
