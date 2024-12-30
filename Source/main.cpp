#include "Mesh.hpp"
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

    auto s1 = Sphere_GLSL{glm::vec3(0.1, -3.1, -1.0), 3.0f, Material_GLSL{glm::vec4(1.0, 1.0, 1.0, 0.0), glm::vec4(0.0), 0.0}};
    auto s2 = Sphere_GLSL{glm::vec3(0.38, -0.05, -0.85), 0.27f, Material_GLSL{glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0), 0.0}};
    auto s3 = Sphere_GLSL{glm::vec3(-0.05, -0.05, -0.71), 0.15f, Material_GLSL{glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0), 0.0}};
    auto s4 = Sphere_GLSL{glm::vec3(-0.3, -0.05, -0.57), 0.11f, Material_GLSL{glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0), 0.0}};
    auto s5 = Sphere_GLSL{glm::vec3(-1.4, 0.5, 0.0), 0.9f, Material_GLSL{glm::vec4(0.0), glm::vec4(1.0, 1.0, 1.0, 3.0), 0.0}};

    rayTracer.AddSphere(s1);
    rayTracer.AddSphere(s2);
    rayTracer.AddSphere(s3);
    rayTracer.AddSphere(s4);
    rayTracer.AddSphere(s5);

    auto mesh = Mesh("Resources/Meshes/Cube.obj", {}, glm::vec3(0.0));

    auto sw = Stopwatch();
    while (!window.ShouldClose())
    {
        sw.Start();

        window.Clear();
        rayTracer.Update();
        window.SwapBuffers();

        LimitFPS(sw.Stop().AsSeconds(), 240);
    }
}

