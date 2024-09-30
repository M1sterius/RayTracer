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

    auto source1 = preputils::LoadFile("Resources/Shaders/Main.glsl");
    auto source2 = preputils::LoadFile("Resources/Shaders/Test.glsl");

    std::vector<std::string> sources = {source2};

    preputils::ResolveIncludes(source1, sources);

    std::cout << source1 << '\n';

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

