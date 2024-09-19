#pragma once

#include "glad.h"

#include <string>
#include <fstream>
#include <iostream>

inline std::string ProcessShaderSource(const std::string& sourcePath)
{
    std::ifstream stream(sourcePath);
    std::string line;
    std::string temp;

    while (std::getline(stream, line))
    {
        temp += (line + '\n');
    }

    stream.close();

    return temp;
}

inline int GetShaderCompileInfo(const unsigned int shaderHandle)
{
    int res;

    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &res);
    if (res != GL_FALSE)
    {
        return 0;
    }

    int length;
    glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
    char* message = static_cast<char*>(alloca(length * sizeof(char)));
    glGetShaderInfoLog(shaderHandle, length, &length, message);

    std::cout << "Shader compilation error: " << message << '\n';

    return 1;
}
