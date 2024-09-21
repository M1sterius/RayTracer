#include "Window.hpp"
#include "RayTracer.hpp"
#include "ComputeShader.hpp"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    const auto& window = Window(1600, 900, "RayTracer");
    const auto& rayTracer = RayTracer();

    auto shader = ComputeShader::FromFile("Resources/Shaders/TestCompute.glsl");

    while (!window.ShouldClose())
    {
        rayTracer.Update();
        window.Update();
    }
}
