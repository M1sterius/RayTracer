#include "Window.hpp"
#include "RayTracer.hpp"
#include "ComputeShader.hpp"
#include "GLSLPreprocessor.h"

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char* argv[])
{
    std::string directive = "#define symbol definition \n #define symbol1 definition";
    preputils::ResolveDefines(directive);

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
