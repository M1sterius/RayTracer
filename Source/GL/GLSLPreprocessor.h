#pragma once

/*
 * A header only GLSL preprocessor that implements
 * #include directive in GLSL shaders
 */

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

namespace preputils
{
    inline const std::string INCLUDE_DIRECTIVE = "#include";

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

    inline std::string FindFileInDirs(const std::string& path, const std::vector<std::string>& dirs)
    {
        std::string source;

        try { source = LoadFile(path); }
        catch (const std::exception&)
        {
            for (const auto& dir : dirs)
            {
                try {source = LoadFile(dir + "/" + path); }
                catch (const std::exception&) { continue; }
                return source;
            }

            throw std::runtime_error("Error! Unable to find: " + path + " in include directories.");
        }

        return source;
    }

    /*
     * Replaces all #include directives with the contents of the corresponding .glsl
     * file until there are no unresolved #include directives left
     */
    inline void ResolveIncludes(std::string& main, const std::vector<std::string>& dirs)
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

            auto fileSource = FindFileInDirs(path, dirs);
            ReplaceInString(main, directive, fileSource);

            // Check for loop iterations limit to avoid being stuck because of self-includes
            if (iterCount > SAFE_LIMIT)
                throw std::runtime_error("GLSL PREPROCESSOR ERROR: Exceeded the maximum number of include directives");
            iterCount++;
        }
    }
}

/**
 * @brief Resolves all #include directives in the shader at the given path\n
 * @param mainPath The path to the shader that will be processed
 * @param includeDirs The directories where the preprocessor will search for included files
 * @return The shader main file with all #include directives replaced with
 * the contents of the corresponding included files
 */
template<typename... Args>
inline std::string ProcessShader(const std::string& mainPath, const Args&... includeDirs)
{
    const auto unpackedDirs = preputils::UnpackVariadicArgs(includeDirs...);
    auto source = preputils::LoadFile(mainPath);
    preputils::ResolveIncludes(source, unpackedDirs);
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

// inline const std::string DEFINE_DIRECTIVE = "#define";
// inline const std::string IFDEF_DIRECTIVE = "#ifdef";
// inline const std::string IFNDEF_DIRECTIVE = "#ifndef";


/*
 * Takes in a string that contains a define directive and returns
 * a std::pair that stores the symbol in the first element
 * and the definition in the second elements
 */
// inline std::pair<std::string, std::string> ParseDefineDirective(const std::string& directive)
// {
//     auto split = SplitByCharacter(directive);
//     split.erase(std::remove(split.begin(), split.end(), ""), split.end());
//     if (split.size() < 3)
//         return {split[1], ""};
//     return {split[1], split[2]};
// }
//
// inline std::unordered_map<std::string, std::string> FindDefines(std::string& text)
// {
//     std::unordered_map<std::string, std::string> definesTable;
//     std::string reconstructedString;
//
//     size_t beginPos = 0;
//     size_t endPos = 0;
//
//     /*
//      * Loop over all lines in the text, check if the current line contains a
//      * #define directive, and if so, parse it and save the definition and the value into the table
//      */
//     while (endPos != std::string::npos)
//     {
//         endPos = text.find('\n', beginPos + 1);
//         auto line = text.substr(beginPos, endPos - beginPos);
//         beginPos = endPos;
//
//         if (line.find(DEFINE_DIRECTIVE) != std::string::npos)
//         {
//             ReplaceInString(line, "\n", "");
//             const auto define = ParseDefineDirective(line);
//             definesTable[define.first] = define.second;
//             continue;
//         }
//
//         reconstructedString += line;
//     }
//
//     /*
//      * Reconstruct the initial string without the lines containing #define
//      * because they are no longer needed
//      */
//     text = reconstructedString;
//
//     return definesTable;
// }