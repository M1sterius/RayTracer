cmake_minimum_required(VERSION 3.29)

project(RayTracer)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})

file(GLOB IMGUI_SOURCES
    "${DEPENDENCIES_DIR}/imgui/*.h"
    "${DEPENDENCIES_DIR}/imgui/*.cpp"
)

add_executable(RayTracer
    ${DEPENDENCIES_DIR}/glad/src/glad.c
    ${IMGUI_SOURCES}

    Source/main.cpp
    Source/Window.cpp
    Source/RayTracer.cpp
    Source/GL/ComputeShader.cpp
    Source/Utilities/Stopwatch.hpp
    Source/GL/GLSLPreprocessor.h
    Source/GL/SSBO.cpp
)

target_include_directories(RayTracer PRIVATE
    ${DEPENDENCIES_DIR}/glm
    ${DEPENDENCIES_DIR}/glfw/include/GLFW
    ${DEPENDENCIES_DIR}/glad/include/glad
    ${DEPENDENCIES_DIR}/glad/include
    ${DEPENDENCIES_DIR}/imgui

    Source
    Source/Utilities
    Source/GL
    Source/Objects
)

add_subdirectory(${DEPENDENCIES_DIR}/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)

find_package(OpenGL REQUIRED)

set(LINK_LIBRARIES OpenGL::GL glfw)

if(WIN32)
    list(APPEND LINK_LIBRARIES winmm)
endif()

target_link_libraries(RayTracer PRIVATE ${LINK_LIBRARIES})

set(COPY_OUT_DIR ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE}/Resources)
add_custom_command(TARGET RayTracer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/
        ${COPY_OUT_DIR}
)
