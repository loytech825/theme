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
void process_config(const ConfigSection& global_config, ConfigSection& section_config, const std::string& output_dir)
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

    section_config.key_value_pairs.erase("color_name");
    section_config.key_value_pairs.erase("color_id");
    section_config.key_value_pairs.erase("color_value");

    //replace all tokens
    for(auto& pair : section_config.key_value_pairs)
    {
        auto& v = pair.second;
        v = replace_all(v, "${section_name}", section_config.name);
        v = replace_all(v, "${output_dir}", output_dir);
    }

    //replaces variables
    for(auto& [k, v] : section_config.key_value_pairs){


        if(k == "format" || k == "format_id") continue;

        //parse inserted values
        //find opening bracket ${ 
        int begin = 0;
        while((begin = v.find("${"), begin+1) != std::string::npos)
        {
            //std::cout << begin << "\n";
            //loop through closing brackets until one right after the opening is found

            int end = -1;
            while((end = v.find("}", end+1)) != std::string::npos && end < (begin+2)) {}
            if(end == std::string::npos) break;

            //holds whats between ${ and }
            std::string v_name = v.substr(begin+2, end-begin-2);

        
            /*for(const auto& var : COLOR_VARIABLES)
            {
                if(v_name == var) {begin = end; continue;}
            }*/

            //holds the same as v_name unless v_name has id, in which case it holds the id
            std::string var_name = v_name;
            if(int id = get_color_id(v_name); id != -1) v_name = std::to_string(id);

            //replace the token
            if(section_config.key_value_pairs.contains(v_name))
                v = replace_all(v, "${"+var_name+"}", section_config.key_value_pairs[v_name]);

            begin = end;
            //else 
            //    v = replace_all(v, "${"+var_name+"}", "");
        }
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
    for(auto& [k, v] : color_map)
    {
        if( int id = get_color_id(k); id != -1)
        {
            //this code changes a key in the map
            auto nh = color_map.extract(k);
            nh.key() = std::to_string(id);
            color_map.insert(std::move(nh));
        }

        //is v is of type #rrggbb or #rrggbbaa strip #
        if(v.starts_with("#") && (v.length() == 7 || v.length() == 9))
        {
            v = v.substr(1);
        }
    }

    //second loop replaces variables
    //this is separate because if a default color refers to a variable that gets
    //overridden in a section, it wont work

    for(auto& [k, v] : color_map){
        //parse inserted values
        //find opening bracket ${ 
        int begin = 0;
        while((begin = v.find("${")) != std::string::npos)
        {
            //std::cout << begin << "\n";
            //loop through closing brackets until one right after the opening is found
            int end = -1;
            while((end = v.find("}", end+1)) != std::string::npos && end < (begin+2)) {}
            if(end == std::string::npos) continue;

            //holds whats between ${ and }
            std::string v_name = v.substr(begin+2, end-begin-2);

            //holds the same as v_name unless v_name has id, in which case it holds the id
            std::string var_name = v_name;
            if(int id = get_color_id(v_name); id != -1) v_name = std::to_string(id);

            //replace the token
            if(color_map.contains(v_name))
                v = replace_all(v, "${"+var_name+"}", color_map[v_name]);
            else 
                v = replace_all(v, "${"+var_name+"}", "");
        }
    }
}