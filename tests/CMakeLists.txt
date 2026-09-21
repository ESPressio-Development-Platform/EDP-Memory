cmake_minimum_required(VERSION 3.16)

project(EDPMemoryTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT DEFINED EDP_SYSTEM_SOURCE_DIR)
    message(FATAL_ERROR "EDP_SYSTEM_SOURCE_DIR must point at an EDP-System checkout")
endif()

if(NOT DEFINED EDP_PLATFORM_SOURCE_DIR)
    message(FATAL_ERROR "EDP_PLATFORM_SOURCE_DIR must point at an EDP-Platform checkout")
endif()

find_package(Threads REQUIRED)

add_executable(
    edp_memory_host_tests
    host/main.cpp
)

target_include_directories(
    edp_memory_host_tests
    PRIVATE
        ../src
        ${EDP_SYSTEM_SOURCE_DIR}/src
        ${EDP_PLATFORM_SOURCE_DIR}/src
)

target_link_libraries(
    edp_memory_host_tests
    PRIVATE
        Threads::Threads
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        edp_memory_host_tests
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
    )
endif()
