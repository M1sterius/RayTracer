#include "Window.hpp"
#include "RayTracer.hpp"
#include "ComputeShader.hpp"
#include "GLSLPreprocessor.h"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    const auto s = ProcessShader("Resources/Shaders/RayTracingShader.glsl",
        "Resources/Shaders"
    );
    printf(s.c_str());

    auto window = Window(2560, 1440, "RayTracer");
    auto rayTracer = RayTracer(window);

    while (!window.ShouldClose())
    {
        window.Clear();
        rayTracer.Update();
        window.SwapBuffers();
    }
}
