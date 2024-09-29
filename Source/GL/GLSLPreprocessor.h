#pragma once

/*
 * A header only GLSL preprocessor that implements
 * #inlude directive in GLSL shaders
 */

/*
 * You can modify this line to include whatever
 * OpenGL functions loader you use
 */
#include "glad.h"

#include <string>
#include <vector>
#include <fstream>

namespace preputils
{
    const std::string& INCLUDE_DIRECTIVE = "#include";

    /*
     * A utility function that helps unpack variadic arguments into a std::vector
     */
    template<typename... Args>
    inline std::vector<std::string> UnpackVariadicArgs(const Args&... args)
    {
        return std::vector<std::string> { args... };
    }

    /*
     * Returns a string containing all lines of the file at path
     * appended with new line character
     */
    inline std::string LoadFile(const std::string& path)
    {
        std::string temp;
        std::string line;

        std::ifstream file(path);
        if (!file.is_open())
            throw std::runtime_error("Cannot open source file: " + path);

        while (std::getline(file, line))
            temp += (line + '\n');

        file.close();

        return temp;
    }

    /*
     * Replaces all occurrences of 'toReplace' to 'replacement' in 'string'
     */
    inline void ReplaceInString(std::string& string, const std::string& toReplace, const std::string& replacement)
    {
        size_t pos = 0;
        while ((pos = string.find(toReplace, pos)) != std::string::npos)
        {
            string.replace(pos, toReplace.length(), replacement);
            pos += replacement.length();
        }
    }
}

/*
 *
 */
template<typename... Args>
inline std::string ProcessComputeShaders(const std::string& main, const Args&... rest)
{
    const std::vector<std::string> paths = preputils::UnpackVariadicArgs(rest...);
    std::string all = main;
    for (const auto& s : paths)
        all += s;
    return all;
}
