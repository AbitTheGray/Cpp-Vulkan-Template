cmake_minimum_required(VERSION 3.17)
project(VulkanHppTest)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON) # Just to not corrupt the project's sources

if(MINGW AND UNIX) # Enable cross-compilation from Linux for Windows
    message(NOTICE "MinGW detected, setting compilation for windows")
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
# Vulkan
#--------------------------------

if(WIN32)

    if(NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR "Environment variable VULKAN_SDK is not defined")
    endif()
    if(NOT IS_DIRECTORY "$ENV{VULKAN_SDK}")
        message(FATAL_ERROR "Environment variable VULKAN_SDK does not point to valid directory, it points to '$ENV{VULKAN_SDK}'")
    endif()

    if(NOT (".lib" IN_LIST CMAKE_FIND_LIBRARY_SUFFIXES))
        set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES};.lib")
    endif()

    set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/Include")

    # 64-bit
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(Vulkan_LIBRARIES "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")
        find_program(Vulkan_GLSLC_EXECUTABLE
                NAMES glslc
                HINTS "$ENV{VULKAN_SDK}/Bin"
                )
    # 32-bit
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(Vulkan_LIBRARIES "$ENV{VULKAN_SDK}/Lib32/vulkan-1.lib")

        find_program(Vulkan_GLSLC_EXECUTABLE
                NAMES glslc
                HINTS "$ENV{VULKAN_SDK}/Bin32"
                )
    endif()

elseif(ANDROID)

    if(${ANDROID_PLATFORM} LESS 24)
        message(FATAL_ERROR "Android version ${ANDROID_PLATFORM} does not support Vulkan. Only API version 24 and above (Android 7.0)")
    endif()

elseif(UNIX)

    if(DEFINED ENV{VULKAN_SDK})
        if(NOT IS_DIRECTORY "$ENV{VULKAN_SDK}")
            message(FATAL_ERROR "Environment variable VULKAN_SDK does not point to valid directory, it points to '$ENV{VULKAN_SDK}'")
        endif()
    endif()

    find_path(Vulkan_INCLUDE_DIRS
            NAMES vulkan/vulkan.h
            HINTS "$ENV{VULKAN_SDK}/include")
    find_library(Vulkan_LIBRARIES
            NAMES vulkan
            HINTS "$ENV{VULKAN_SDK}/lib")
    find_program(Vulkan_GLSLC_EXECUTABLE
            NAMES glslc
            HINTS "$ENV{VULKAN_SDK}/bin")
endif()

if(NOT TARGET Vulkan::Vulkan)
    add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
    set_target_properties(Vulkan::Vulkan PROPERTIES
            IMPORTED_LOCATION "${Vulkan_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIRS}")
endif()

if(Vulkan_GLSLC_EXECUTABLE AND NOT TARGET Vulkan::glslc)
    add_executable(Vulkan::glslc IMPORTED)
    set_property(TARGET Vulkan::glslc PROPERTY IMPORTED_LOCATION "${Vulkan_GLSLC_EXECUTABLE}")
endif()



#--------------------------------
# Test code
#--------------------------------

add_executable(VulkanHppTest main.cpp)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") # GCC / MinGW
    target_link_libraries(VulkanHppTest -static-libgcc -static-libstdc++)
endif()

target_link_libraries(VulkanHppTest Vulkan::Vulkan)
