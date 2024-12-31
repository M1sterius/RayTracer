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

    auto transform = glm::mat4(1.0);
    glm::translate(transform, glm::vec3(2.0, 0.0, 0.0));
    transform *= glm::mat4_cast(glm::quat(glm::vec3(0.0, glm::pi<float>() / 3, 0.0)));
    glm::scale(transform, glm::vec3(0.5, 0.5, 0.5));
    glm::transpose(transform);

    auto mesh = Mesh("Resources/Meshes/Cube.obj", {glm::vec4(1.0, 0.0, 0.0, 0.0),
glm::vec4(1.0, 0.0, 0.0, 3.0), 0.0}, transform);

    rayTracer.AddMesh(mesh);

    rayTracer.SetCamPosition(glm::vec3(0.0, 0.0, 5.0));

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






