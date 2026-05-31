#pragma once

/*
This file include random helper functions
*/

#include <unordered_map>
#include <vector>
#include "constants.h"
#include "config_parser.h"

std::string replace_all(const std::string& orig, const std::string& from, const std::string& to);
std::string get_config_dir();
int get_color_id(const std::string& color);
std::vector<std::string> split_string(const std::string& input, const std::string& token);

/*
    @brief Replaces all occurences of ${var_name}
    @param input string
    @param variables_source where to search for variables 
*/
std::string insert_variables(const std::string& input, const std::unordered_map<std::string, std::string>& variables_source);