cmake_minimum_required(VERSION 3.16)
project(robocor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Grab everything first
file(GLOB_RECURSE ALL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/communication/**/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/storage/**/*.cpp
)

# Linux sources
set(ROBOCOR_LINUX_SOURCES ${ALL_SOURCES})
list(FILTER ROBOCOR_LINUX_SOURCES INCLUDE REGEX ".*/linux/.*")

# STM sources
set(ROBOCOR_STM_SOURCES ${ALL_SOURCES})
list(FILTER ROBOCOR_STM_SOURCES INCLUDE REGEX ".*/stm/.*")

# Common sources (exclude linux and stm)
set(ROBOCOR_COMMON_SOURCES ${ALL_SOURCES})
list(FILTER ROBOCOR_COMMON_SOURCES EXCLUDE REGEX ".*/(linux|stm|test)/.*")

message(STATUS "ROBOCOR_LINUX_SOURCES: ${ROBOCOR_LINUX_SOURCES}")
message(STATUS "ROBOCOR_STM_SOURCES: ${ROBOCOR_STM_SOURCES}")
message(STATUS "ROBOCOR_COMMON_SOURCES: ${ROBOCOR_COMMON_SOURCES}")

add_library(robocor_stm_lib STATIC
    ${ROBOCOR_COMMON_SOURCES}
    ${ROBOCOR_STM_SOURCES}
)

add_library(robocor_linux_lib STATIC
    ${ROBOCOR_COMMON_SOURCES}
    ${ROBOCOR_LINUX_SOURCES}
)

target_include_directories(robocor_stm_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(robocor_linux_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_easycom
    ${ROBOCOR_COMMON_SOURCES}
    ${ROBOCOR_LINUX_SOURCES})
