#include "Mesh.hpp"
#include "Window.hpp"
#include "RayTracer.hpp"
#include "ShaderStructs.hpp"
#include "SleepUtility.hpp"

#include <iostream>
#include <cstdint>

#define print(x) std::cout << x << '\n'

glm::mat4 CreateTransform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)
{
    auto transform = glm::mat4(1.0);

    transform = glm::translate(transform, pos);
    transform *= glm::mat4_cast(glm::quat(rot));
    transform = glm::scale(transform, scale);

    return glm::transpose(transform);
}

int32_t main(int32_t argc, char* argv[])
{
    const auto window = Window(1600, 900, "RayTracer");
    auto rayTracer = RayTracer(window);

    auto transform = CreateTransform(
            glm::vec3(0.0, 1.0, 0.0),
            glm::vec3(0, 0, 0),
            glm::vec3(1.0, 0.5, 1.0)
    ); // LIGHT
    auto mesh = Mesh("Resources/Meshes/Cube.obj", {glm::vec4(0.0, 0.0, 0.0, 1.0),
    glm::vec4(1.0, 1.0, 1.0, 1.0), 0.0}, transform);

    auto transform1 = CreateTransform(
        glm::vec3(0.0, -2.0, 0.0),
        glm::vec3(0, 0, 0.0),
        glm::vec3(2.0, 0.2, 2.0)
    );
    auto mesh1 = Mesh("Resources/Meshes/Cube.obj", {glm::vec4(1.0),
glm::vec4(0.0, 0.0, 0.0, 0.0), 0.0}, transform1);

    auto transform2 = CreateTransform(
            glm::vec3(0.0, 0.5, -1.9),
            glm::vec3(0, 0, 0.0),
            glm::vec3(2.0, 2.0, 0.2)
    );
    auto mesh2 = Mesh("Resources/Meshes/Cube.obj", {glm::vec4(1.0, 0.0, 0.0, 1.0),
       glm::vec4(0.0), 0.0}, transform2);

    rayTracer.AddMesh(mesh);
    rayTracer.AddMesh(mesh1);
    rayTracer.AddMesh(mesh2);

    rayTracer.SetCamPosition(glm::vec3(0.0, 0.0, 7.0));

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



