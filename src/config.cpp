/*
    Source code for config.h for more info see that
*/
#include "config.h"
#include "helpers.h"
#include <iostream>
/*
    Processes the config
    merges with global
    replaces tokens
    initializes missing fields...
*/
void process_config(const ConfigSection& global_config, ConfigSection& section_config, const std::string& output_dir, const std::string& config_dir)
{
    auto cfg_global = global_config.key_value_pairs;


    //if section has defined only format, we need to override format_id as well
    if(section_config.key_value_pairs.contains("format") && !section_config.key_value_pairs.contains("format_id")) { section_config.key_value_pairs["format_id"] = section_config.key_value_pairs["format"]; }

    section_config.key_value_pairs.merge(cfg_global);

    //initializes missing fields
    if(!section_config.key_value_pairs.contains("path"))        { section_config.key_value_pairs["path"]      = DEFAULT_PATH;                               }
    if(!section_config.key_value_pairs.contains("format"))      { section_config.key_value_pairs["format"]    = DEFAULT_FORMAT;                             } 
    if(!section_config.key_value_pairs.contains("format_id"))   { section_config.key_value_pairs["format_id"] = section_config.key_value_pairs["format"];   }
    if(!section_config.key_value_pairs.contains("comment"))     { section_config.key_value_pairs["comment"]   = DEFAULT_COMMENT;                            }
    if(!section_config.key_value_pairs.contains("config_format")){section_config.key_value_pairs["config_format"]=DEFAULT_CONFIG_FORMAT;                    }
    if(!section_config.key_value_pairs.contains("mode"))        { section_config.key_value_pairs["mode"]      = DEFAULT_MODE;                    }

    //checks if mode is valid
    if(!(section_config.key_value_pairs["mode"] != "modify-add" 
        ||  section_config.key_value_pairs["mode"] != "modify-ignore")) { section_config.key_value_pairs["mode"] = DEFAULT_MODE; }

    section_config.key_value_pairs.erase("color_name");
    section_config.key_value_pairs.erase("color_id");
    section_config.key_value_pairs.erase("color_value");

    //replace all tokens
    for(auto& pair : section_config.key_value_pairs)
    {
        auto& v = pair.second;
        v = replace_all(v, "${section_name}", section_config.name);
        v = replace_all(v, "${config_dir}", section_config.name);
        v = replace_all(v, "${output_dir}", output_dir);
    }

    //replaces variables
    for(auto& [k, v] : section_config.key_value_pairs){


        if(k == "format" || k == "format_id" || k == "config_format") continue;

        v = insert_variables(v, section_config.key_value_pairs);
    }
}

/*
    changes color names to id where possible
    (note: black=0, red=1...bright_black=8, bright_red=9...)
*/
void process_colors(const ColorSection& global_colors, ColorSection& section_colors)
{

    //for each section we need to generate an entire pallete, since each section can override colors
    // -> check if base8 is present
    // -> check for bg/fg
    // -> generate pallete while preserving already defined colors
    // -> put pallete into a map(section_name -> pallete)
    // -> proceed with config generation

    //first merge globals with section
    //need this because c++
    auto section_map = section_colors.key_value_pairs;
    auto global_map = global_colors.key_value_pairs;

    section_map.merge(global_map); 


    //Changes colors to ids where possible, removes # before colors
    for(auto& [k, v] : section_colors.key_value_pairs)
    {
        if( int id = get_color_id(k); id != -1)
        {
            //this code changes a key in the map
            auto nh = section_map.extract(k);
            nh.key() = std::to_string(id);
            section_map.insert(std::move(nh));
        }

        //is v is of type #rrggbb or #rrggbbaa strip #
        if(v.starts_with("#") && (v.length() == 7 || v.length() == 9) && (v.find("${") == std::string::npos)) { v = v.substr(1); }
    }

    //here all colors are id's and all values arre RRGGBB without #

    //we check if map contains colors 1-6 and (color 0 or background) and (color 7 or foreground)
    if(     section_map.contains("1") 
        &&  section_map.contains("2")
        &&  section_map.contains("3")
        &&  section_map.contains("4")
        &&  section_map.contains("5")
        &&  section_map.contains("6")
        &&  ( section_map.contains("0") || section_map.contains("background") )
        &&  ( section_map.contains("7") || section_map.contains("foreground") )
    )
    {
        //TODO: background / foreground and color0 / color7 parsing
        /*
            BASE COLOR INITIALIZATION
        */
        //we need a base16 for the color function, palette is separate from colors anyway
        //so we can populate the first 8 and leave the others empty
        std::array<Color, 16> base8;
        if(section_map.contains("background"))  base8[0] = hex2rgb(section_map.at("background"));
        else                                    base8[0] = hex2rgb(section_map.at("0"));

        base8[1] = hex2rgb(section_map.at("1"));
        base8[1] = hex2rgb(section_map.at("2"));
        base8[1] = hex2rgb(section_map.at("3"));
        base8[1] = hex2rgb(section_map.at("4"));
        base8[1] = hex2rgb(section_map.at("5"));
        base8[1] = hex2rgb(section_map.at("6"));

        if(section_map.contains("background"))  base8[7] = hex2rgb(section_map.at("foreground"));
        else                                    base8[7] = hex2rgb(section_map.at("7"));

        /*
            GENERATE AND MERGE PALETTE
        */
        section_colors.generated_colors = generate_256(base8, base8[1], base8[7]);
    
    }


    //second loop replaces variables
    //this is separate because if a default color refers to a variable that gets
    //overridden in a section, it wont work
    //auto map = color_map;
    for(auto& [k, v] : section_map){ v = insert_variables(v, section_map, section_colors.generated_colors); }

    section_colors.key_value_pairs = section_map;
}