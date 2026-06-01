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
void process_colors(std::unordered_map<std::string, std::string>& color_map)
{

    //first loop changes color names to ids
    //and colors of type #rrggbb to rrggbb

    //we need this so we dont edit what we loop through
    auto editable_map = color_map;

    for(auto& [k, v] : color_map)
    {
        //std::cout << k << ":\t " << v << "\n";
        if( int id = get_color_id(k); id != -1)
        {
            //std::cout << "Replacing " << k << " with " << id << "\n"; 
            //this code changes a key in the map
            auto nh = editable_map.extract(k);
            nh.key() = std::to_string(id);
            editable_map.insert(std::move(nh));
        }

        //is v is of type #rrggbb or #rrggbbaa strip #
        //std::cout << v.find("${") << "\t";
        if(v.starts_with("#") && (v.length() == 7 || v.length() == 9) && (v.find("${") == std::string::npos))
        {
            //std::cout << "v:" << v << "\n";
            v = v.substr(1);
        }
    }

    //second loop replaces variables
    //this is separate because if a default color refers to a variable that gets
    //overridden in a section, it wont work
    //auto map = color_map;
    for(auto& [k, v] : editable_map){
        //parse inserted values
        v = insert_variables(v, editable_map);
    }

    color_map = editable_map;
}