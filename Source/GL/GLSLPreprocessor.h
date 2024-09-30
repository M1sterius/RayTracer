#pragma once

/*
 * A header only GLSL preprocessor that implements
 * #include directive in GLSL shaders
 */

/*
 * You can modify this line to include whatever
 * OpenGL functions loader you use
 */
#include "glad.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace preputils
{
    inline const std::string& INCLUDE_DIRECTIVE = "#include";

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
            throw std::runtime_error("Unable to open the source file at: " + path);

        while (std::getline(file, line))
            temp += (line + '\n');

        file.close();

        // Cut off the last newline character in the file
        return temp.substr(0, temp.find_last_of('\n'));
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

    /*
     * Takes in a string containing an include directive and
     * returns an extracted file path
     */
    inline std::string ParseIncludeDirective(const std::string& directive)
    {
        const size_t quotesPos = directive.find('"', 0) + 1;
        return directive.substr(quotesPos, directive.length() - (quotesPos + 1));
    }

    inline void ResolveIncludes(std::string& main)
    {
        size_t pos = 0;
        while ((pos = main.find(INCLUDE_DIRECTIVE)) != std::string::npos)
        {
            const auto directive = main.substr(pos, main.find('\n', pos) - pos);
            const auto path = ParseIncludeDirective(directive);
            ReplaceInString(main, directive, LoadFile(path));
        }
    }
}

/**
 * @brief Resolves all #include directives in the shader at the give path\n
 * NOTE: All include file paths in shaders must be specified relative to the executable
 * that uses this preprocessor
 * @param shaderPath The path to the shader that will be processed
 * @return The shader main file with all #include directives replaced with
 * the contents of the corresponding incliuded files
 */
inline std::string ProcessShader(const std::string& shaderPath)
{
    auto source = preputils::LoadFile(shaderPath);
    preputils::ResolveIncludes(source);
    return source;
}

// /*
//  *
//  */
// template<typename... Args>
// inline std::string ProcessComputeShaders(const std::string& main, const Args&... rest)
// {
//     const std::vector<std::string> paths = preputils::UnpackVariadicArgs(rest...);
// }
