#include "Window.hpp"
#include "RayTracer.hpp"
#include "ShaderStructs.hpp"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    const auto window = Window(2560, 1440, "RayTracer");
    auto rayTracer = RayTracer(window);

    auto s1 = Sphere{glm::vec3(0.0, 0.0, -2.0), 0.5f, Material{glm::vec3(1.0), glm::vec3(1.0), 1.0f}};
    rayTracer.AddSphere(s1);

    while (!window.ShouldClose())
    {
        window.Clear();
        rayTracer.Update();
        window.SwapBuffers();
    }
}

