#include "Window.hpp"
#include "RayTracer.hpp"
#include "ShaderStructs.hpp"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    auto window = Window(1600, 900, "RayTracer");
    auto rayTracer = RayTracer(window);

    while (!window.ShouldClose())
    {
        window.Clear();
        rayTracer.Update();
        window.SwapBuffers();
    }
}


