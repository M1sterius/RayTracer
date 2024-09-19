#include "Window.hpp"
#include "RayTracer.hpp"
#include "ComputeShader.hpp"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    const auto window = Window(2560, 1440, "RayTracer");
    const auto rayTracer = RayTracer();

    while (!window.ShouldClose())
    {
        rayTracer.Update();
        window.Update();
    }
}
