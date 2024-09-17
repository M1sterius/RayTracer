cmake_minimum_required(VERSION 3.29)

project(RayTracer)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})

add_executable(RayTracer
    ${DEPENDENCIES_DIR}/glad/src/glad.c

    Source/main.cpp
)

target_include_directories(RayTracer PRIVATE
    ${DEPENDENCIES_DIR}/glm
    ${DEPENDENCIES_DIR}/glfw/include
    ${DEPENDENCIES_DIR}/glad/include/glad
    ${DEPENDENCIES_DIR}/glad/include
)