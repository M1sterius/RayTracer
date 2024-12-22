#include "Window.hpp"
#include "RayTracer.hpp"
#include "ShaderStructs.hpp"
#include "SleepUtility.hpp"

#include <iostream>
#include <cstdint>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    const auto window = Window(1600, 900, "RayTracer");
    auto rayTracer = RayTracer(window);

    auto s1 = Sphere{glm::vec3(-0.5, 0.4, -2.0), 0.3f, Material{glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0), 0.0, 0.0}};
    auto s2 = Sphere{glm::vec3(0.2, 0, -2.0), 0.1, Material{glm::vec4(0.4, 0.3, 0.2, 0.0), glm::vec4(0.0), 0.0, 0.0}};
    auto s3 = Sphere{glm::vec3(0.0, -0.15, -2.0), 0.1f, Material{glm::vec4(0.0, 0.0, 0.0, 0.0), glm::vec4(1.0), 1.0, 0.0}};
    auto s4 = Sphere{glm::vec3(0.3, -0.6, -2.3), 0.5f, Material{glm::vec4(1.0, 1.0, 1.0, 0.0), glm::vec4(1.0), 0.0, 0.0}};

    rayTracer.AddSphere(s1);
    rayTracer.AddSphere(s2);
    rayTracer.AddSphere(s3);
    rayTracer.AddSphere(s4);

    while (!window.ShouldClose())
    {
        window.Clear();
        rayTracer.Update();
        window.SwapBuffers();
    }
}
