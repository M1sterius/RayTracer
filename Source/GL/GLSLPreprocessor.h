#pragma once

/*
 * A header only GLSL preprocessor that implements
 * #include directive in GLSL shaders
 */

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace preputils
{
    inline const std::string INCLUDE_DIRECTIVE = "#include";
    inline const std::string DEFINE_DIRECTIVE = "#define";
    inline const std::string IFDEF_DIRECTIVE = "#ifdef";
    inline const std::string IFNDEF_DIRECTIVE = "#ifndef";

    /*
     * A utility function that helps unpack variadic arguments into a std::vector
     */
    template<typename... Args>
    inline std::vector<std::string> UnpackVariadicArgs(const Args&... args)
    {
        return std::vector<std::string> { args... };
    }

    /*
     * Returns a string that contains all lines of the file at path
     * NOTE: All lines in the file are appended with newline character to
     * make sure that the GLSL compiler will interpret them correctly
     */
    inline std::string LoadFile(const std::string& path)
    {
        std::string temp;
        std::string line;

        std::ifstream file(path);
        if (!file.is_open())
            throw std::runtime_error("GLSL PREPROCESSOR ERROR: Unable to open the source file at: " + path);

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
     * Splits the string by the given character, returns a std::vector that
     * contains the result of the split
     */
    inline std::vector<std::string> SplitByCharacter(const std::string& string, const char character = ' ')
    {
        std::vector<std::string> result;

        size_t start = 0;
        size_t end = string.find(character);

        while (end != std::string::npos)
        {
            result.push_back(string.substr(start, end - start));
            start = end + 1;
            end = string.find(character, start);
        }
        result.push_back(string.substr(start));

        return result;
    }

    /*
     * Takes in a string that contains an include directive and
     * returns extracted file path
     */
    inline std::string ParseIncludeDirective(const std::string& directive)
    {
        const size_t pathPos = directive.find('"', 0) + 1;
        return directive.substr(pathPos, directive.length() - (pathPos + 1));
    }

    /*
     * Takes in a string that contains a define directive and returns
     * a std::pair that stores the symbol in the first element
     * and the definition in the second elements
     */
    inline std::pair<std::string, std::string> ParseDefineDirective(const std::string& directive)
    {
        auto split = SplitByCharacter(directive);
        split.erase(std::remove(split.begin(), split.end(), ""), split.end());
        if (split.size() < 3)
            return {split[1], ""};
        return {split[1], split[2]};
    }

    /*
     * Replaces all #include directives with the contents of the corresponding .glsl
     * file until there are no unresolved #include directives left
     */
    inline void ResolveIncludes(std::string& main)
    {
        static constexpr size_t SAFE_LIMIT = 1024;

        size_t iterCount = 0;
        size_t pos = 0;

        /*
         * Loop until no more #include directives are found
         * Upon encountering one, parse it to get the path to file and replace
         * the directive with the content of that file
         */
        while ((pos = main.find(INCLUDE_DIRECTIVE)) != std::string::npos)
        {
            const auto directive = main.substr(pos, main.find('\n', pos) - pos);
            const auto path = ParseIncludeDirective(directive);
            ReplaceInString(main, directive, LoadFile(path));

            // Check for loop iterations limit to avoid being stuck because of self-includes
            if (iterCount > SAFE_LIMIT)
                throw std::runtime_error("GLSL PREPROCESSOR ERROR: Exceeded the maximum number of include directives");
            iterCount++;
        }
    }

    inline std::unordered_map<std::string, std::string> ResolveDefines(std::string& text)
    {
        std::unordered_map<std::string, std::string> definesTable;
        std::vector<std::string> defineLines;

        size_t beginPos = 0;
        size_t endPos = 0;

        /*
         * Loop over all lines in the text, check if the current line contains a
         * #define directive, and if so, parse it and save the definition and the value into the table
         */
        while (endPos != std::string::npos)
        {
            endPos = text.find('\n', beginPos + 1);
            auto line = text.substr(beginPos + 1, endPos - beginPos);
            beginPos = endPos;

            if (line.find(DEFINE_DIRECTIVE) != std::string::npos)
            {
                defineLines.push_back(line);
                ReplaceInString(line, "\n", "");
                const auto define = ParseDefineDirective(line);
                definesTable[define.first] = define.second;
            }
        }

        /*
         * Replace all lines that contain #define with an empty string because they
         * are no longer needed
         */
        for (const auto& line : defineLines)
            ReplaceInString(text, line, "");

        return definesTable;
    }
}

/**
 * @brief Resolves all #include directives in the shader at the given path\n
 * NOTE: All include file paths in shaders must be specified relative to the executable
 * that uses this preprocessor
 * @param shaderPath The path to the shader that will be processed
 * @return The shader main file with all #include directives replaced with
 * the contents of the corresponding included files
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

/*
 * 1) Read the main file
 * 2) Start searching in it for #include
 * 3) Upon encountering #include, parse it and load the included file
 * 4) Search for #define in the included file
 * 5) Add all defined symbols to a dictionary
 * 6) Evaluate the defined symbols inside the loaded file
 * 7) Evaluate all #ifdef statements inside the loaded file
 * 8) Copy the contents of the loaded file into the main file
 * 9) Add symbols defined in the loaded file to global dictionary of defined symbols
 * 10)
 */