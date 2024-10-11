#include "Window.hpp"
#include "RayTracer.hpp"
#include "ComputeShader.hpp"
#include "GLSLPreprocessor.h"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    std::string text = preputils::LoadFile("Resources/Shaders/Random.glsl");
    const auto table = preputils::ResolveDefines(text);

    for (const auto& pair : table)
        printf("%s %s \n", pair.first.c_str(), pair.second.c_str());

    std::cout << text;



    // auto window = Window(2560, 1440, "RayTracer");
    // auto rayTracer = RayTracer(window);
    //
    // while (!window.ShouldClose())
    // {
    //     window.Clear();
    //     rayTracer.Update();
    //     window.SwapBuffers();
    // }
}



