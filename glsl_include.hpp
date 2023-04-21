/*
LICENCE

MIT License

Copyright (c) 2023 Szalai Balázs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _GLSL_INCLUDE_H
#define _GLSL_INCLUDE_H

//################
//### Includes ###
//################
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>

//####################
//### Declarations ###
//####################
namespace glsl_include {

    class ShaderLoader {
    public:

        /**
         *  Init once, load any GLSL shader files by calling load_shader()
         * 
         *  @param include_kw The custom keyword to be used in the shader file to indicate an #include statement.
         */
        ShaderLoader(const char* _custom_keyword)
            : include_keyword(_custom_keyword)
        {
            already_included.reserve(10);
        }


        /**
        *   Loads a shader file and handles #include statements.
        * 
        *   @param file_path The path to the shader file to be loaded.
        * 
        *   @return A string containing the contents of the shader file, with any custom #include statements replaced by their contents.
        */
        std::string load_shader(const std::string& file_path)
        {
            // If size is 0, it indicates, that we are at the top of the recursive load stack
            bool stack_top = (already_included.size() == 0);
            if (stack_top) {
                already_included.emplace_back(file_path);
            }

            std::string ret_data = "";
            std::ifstream file(file_path);
            if (!file.good())
            {
                std::cout << "ERROR [ ShaderLoader::load_shader ]: Failed to start fstream, check if '" << file_path << "' exists\n";
                return ret_data;
            }
            if (file.is_open())
            {
                std::string line;
                while (getline(file, line))
                {
                    // Custom include statement has to be at the start of the line
                    if (line.find(include_keyword) == 0)
                    {
                        // Get path between double quotes
                        std::string rel_include_path = extract_first_between(line.substr(include_keyword.length()), '"', '"');
                        // Modify path according to os
                        #ifdef _WIN32
                            std::replace(rel_include_path.begin(), rel_include_path.end(), '/', '\\');
                        #elif __linux__
                            std::replace(rel_include_path.begin(), rel_include_path.end(), '\\', '/');
                        #endif
                        std::string full_include_path = extract_path(file_path) + rel_include_path;

                        // Avoid including self
                        if (file_path == full_include_path) {            
                            std::cout << "WARNING [ ShaderLoader::load_shader ]: '"<< file_path <<"' tried to include itself\n";
                            continue;
                        }
                        else {
                            bool include_flag = true;
                            // Check if current file already included
                            for (const auto& file_to_check : already_included)
                            {
                                // Avoid duplicate includes
                                if (file_to_check == full_include_path) {
                                    include_flag = false;
                                    break;
                                }
                            }
                            // If not yet included, push path to include vector and replace line with file contest
                            if (include_flag)
                            {
                                already_included.push_back(full_include_path);
                                // Repeat recurively
                                ret_data += load_shader(full_include_path) + "\n";
                            }
                        }
                    }
                    else {
                        ret_data += line + "\n";
                    }
                }
                file.close();
            }
            else
            {
                std::cout << "ERROR [ ShaderLoader::load_shader ]: Unable to open file '" << file_path << "'\n";
            }
            // We are back to the first call
            if (stack_top) {
                already_included.clear();
            }
            return ret_data;
        }

    private:
        // Store already included files
        std::vector<std::string> already_included;
        // Custom keyword
        std::string include_keyword;


        /**
        *   Helper function that strips filename from a path, basically getting the path to the directory containing the file
        * 
        *   @param file_path Input path
        * 
        *   @return Path without the filename, or an empty string if no separator character fround
        */
        std::string extract_path(const std::string& path)
        {
            // Find the position of the last directory separator
            std::size_t pos = path.find_last_of("\\/");
            
            // Strip fname from path
            if (pos != std::string::npos) {
                return path.substr(0, pos+1);
            }
            return "";
        }


        /**
         *  Helper function, that extracts the first substring that occurs between two characters in a given string.
         *  
         *  @param str The input string to search.
         *  @param start The starting character to search for.
         *  @param end The ending character to search for.
         *
         *  @example: extract_first_between("extract_between_underscores", '_', '_'); // will return "between"
         * 
         *  @return The extracted string (excluding the symbols themselves), or an empty string if no match is found.
         */
        std::string extract_first_between(const std::string& input, char start_symbol, char end_symbol)
        {
            size_t start_index = input.find(start_symbol);
            size_t end_index = input.find(end_symbol, start_index + 1);
            std::string extracted_string = "";
            if (start_index != std::string::npos && end_index != std::string::npos)
            {
                extracted_string = input.substr(start_index + 1, end_index - start_index - 1);
            }
            else
            {
                std::cout << "ERROR [ ShaderLoader::extract_first_between ]: Start '" << start_symbol << "' or end symbol '" << end_symbol << "' not found" << std::endl;
            }
            return extracted_string;
        }
    };
}
#endif