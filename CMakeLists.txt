cmake_minimum_required(VERSION 3.17)
project(Vulkan_Template C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_policy(SET CMP0077 NEW) # set() overrides option()



#--------------------------------
# Compiler configuration
#--------------------------------

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>") # Define DEBUG in Debug builds

if(MINGW AND UNIX) # Enable cross-compilation from Linux for Windows
    message(NOTICE "MinGW detected, setting compilation for windows")
    set(WIN32 ON)
endif()

include(cmake/detect_pointer_size.cmake)



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
    if(WIN32)
        target_link_libraries(Vulkan_Template -static winpthread)
    endif()
endif()

target_link_libraries(Vulkan_Template Vulkan::Vulkan)



#--------------------------------
# Tests
#--------------------------------

if(BUILD_TESTING)
    enable_testing()

    add_subdirectory(tests)
endif()
