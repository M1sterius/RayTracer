#include "Window.hpp"
#include "RayTracer.hpp"
#include "ComputeShader.hpp"
#include "GLSLPreprocessor.h"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    std::string a = "a";
    std::string b = "b";
    std::string c = "c";

    auto processed = ProcessShader("Resources/Shaders/Main.glsl");
    std::cout << processed;

//    auto window = Window(1600, 900, "RayTracer");
//    auto rayTracer = RayTracer(window);
//
//    while (!window.ShouldClose())
//    {
//        window.Clear();
//        rayTracer.Update();
//        window.SwapBuffers();
//    }
}

