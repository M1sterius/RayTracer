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

    auto s1 = preputils::LoadFile("Resources/Shaders/Main.glsl");
    auto s2 = preputils::LoadFile("Resources/Shaders/Test.glsl");
    preputils::ReplaceInString(s1, "#include \"Test.glsl\"", s2);
    std::cout << s1;

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

