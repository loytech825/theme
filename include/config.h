#pragma once

/*
    This file includes functions related to the main config file (config.conf) and not the color files
    They are separate from config_parser.h, as they deal with the internal structure only and are
    specific to this program
*/

#include "config_parser.h"

void process_config(const ConfigSection& global_config, ConfigSection& section_config, const std::string& output_dir);
void process_colors(std::unordered_map<std::string, std::string>& color_map);