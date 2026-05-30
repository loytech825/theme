#pragma once

/*
This file include random helper functions
*/

#include <unordered_map>
#include "constants.h"

std::string replace_all(const std::string& orig, const std::string& from, const std::string& to);
std::string get_config_dir();
int get_color_id(const std::string& color);