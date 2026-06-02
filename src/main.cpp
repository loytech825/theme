#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "config.h"
#include "helpers.h"
#include "color256.h"
#include <cstring>


/*Global settings*/
std::string output_dir{""};
std::string color_file{""};
bool generate_pallete{false};

namespace fs = std::filesystem;

void print_cfg(const ConfigSection& section_config, const ConfigSection& global_colors,  const ConfigSection& section_colors, std::ostream& stream);
std::string format_value(const std::string& format, const std::string& id_format, const std::string& key, const std::string& value);
int parse_arguments(const int argc, char* argv[]);

int main(int argc, char *argv[])
{

    //testing areas
    /*std::string banana {"123456\n789\n12\n\n1011"};
    const auto vec = split_string(banana, "\n");

    for(auto s : vec)
    {
        std::cout << s << "-----";
    }

    return 0;*/

    /*std::array<Color, 16> colors {
        /*Color{0, 0, 0},
        Color{128, 0, 0},
        Color{0, 128, 0},
        Color{128, 128, 0},
        Color{0, 0, 128},
        Color{128, 0, 128},
        Color{0, 128, 128},
        Color{192, 192, 192},
        Color{128, 128, 128},
        Color{255, 0, 0},
        Color{0, 255, 0},
        Color{255, 255, 0},
        Color{0, 0, 255},
        Color{255, 0, 255},
        Color{0, 255, 255},
        Color{255, 255, 255}

        hex2rgb("282828"),
        hex2rgb("cc241d"),
        hex2rgb("98971a"),
        hex2rgb("d79921"),
        hex2rgb("458588"),
        hex2rgb("b16286"),
        hex2rgb("689d6a"),
        hex2rgb("a89984"),
        
        hex2rgb("928374"),
        hex2rgb("fb4934"),
        hex2rgb("b8bb26"),
        hex2rgb("fabd2f"),
        hex2rgb("83a598"),
        hex2rgb("d3869b"),
        hex2rgb("8ec07c"),
        hex2rgb("ebdbb2"),

    };

    print_256(generate_256(colors, colors[0], colors[7]));


    return 0;*/

    //check if arguments parse successfully
    if(parse_arguments(argc, argv)) return -1;

    if(color_file.empty()) { std::cout << "No file provided!\nDo theme -h for help\n"; return -1; }
    
    //TODO verbose?
    //std::cout << "Parsing file " << fs::absolute(color_file) << "\n";
    //std::cout << "Output directory " << fs::absolute(output_dir) << "\n";


    fs::path config_dir{get_config_dir()};
    fs::path config_path{config_dir / APP_NAME / "config.conf"};

    create_conf(config_path);


    /*
        CONFIG FILE PARSING
    */

    //if(!fs::is_regular_file(config_path)) { std::cout << "File " << config_path << " doesn't exist!\n"; return -1;}

    std::vector<ConfigSection> config;
    parse_config( fs::path{config_path}, config );

    //check if section "default" exists
    //if not create it
    ConfigSection default_section;
    default_section.name = "default";

    if(auto loc = std::find_if(config.begin(), config.end(), [](const ConfigSection& sect){ return sect.name == "default"; }); loc != config.end()) default_section = *loc;

    //process all sections to remove raw data
    for(auto& sect : config)
    {
        if(sect.name != "default")
            process_config(default_section, sect, output_dir, config_dir);
    }




    /*
        COLOR PARSING
    */
    std::vector<ConfigSection> COLORS;

    if(!fs::is_regular_file(color_file)) { std::cout << "File " << fs::absolute(color_file) << " doesn't exist!\n"; return -1; }

    parse_config( fs::path{color_file}, COLORS );

    ConfigSection default_colors;
    default_colors.name = "default";

    if(auto loc = std::find_if(COLORS.begin(), COLORS.end(), [](const ConfigSection& sect){ return sect.name == "default"; }); loc != COLORS.end()) default_colors = *loc;

    /*
        Prints colors to files
    */
    for(auto& sect : config)
    {
        /*for(const auto& [k, v] : sect.key_value_pairs)
        {
            std::cout << "\t" << k << ": " << v << "\n"; 
        }*/

        ConfigSection to_print;
        to_print.name = sect.name;

        if(auto loc = std::find_if(COLORS.begin(), COLORS.end(), [&sect](const ConfigSection& int_sect){ return int_sect.name == sect.name; }); loc != COLORS.end()) to_print = *loc;

        if(sect.name != "default")
        {
            fs::path path{sect.key_value_pairs["path"]};
            path = path.lexically_normal();

            //config specifies filename not directory
            if(path.has_extension())
            {
                if(!fs::exists(path.parent_path())) fs::create_directories(path.parent_path());
            }else
            {
                if(!fs::exists(path)) fs::create_directories(path);
                if(fs::is_directory(path)) path /= "colors.conf";
            }

            std::cout << "Writing section "  << sect.name << " to " << fs::absolute(path) << "\n";

            std::ofstream file{path};
            print_cfg(sect, default_colors, to_print, file);
        }
    }
}

/*
    Parse arguments and setup variables
*/
int parse_arguments(const int argc, char* argv[])
{
    //if passing setting via -C, -f

    output_dir = get_config_dir();

    //if(argc == 1) { std::cout << "Invalid usage:\n" << HELP; return -1; };
    //if(argc == 2) { color_file = argv[1]; return 0;}
    //if(argc == 3) { color_file = argv[1]; output_dir = argv[2]; return 0;}

    for(int i = 1; i < argc; i++)
    {
        //boolean arguments
        if(strcmp(argv[i], "-h") == 0) { std::cout << HELP; continue;}
        if(strcmp(argv[i], "-g") == 0) { generate_pallete = true; continue;}

        //arguments that require another field
        if(i < argc-1)
        {
            if(strcmp(argv[i], "-o") == 0) { output_dir = argv[i+1]; i++; continue; }
            if(strcmp(argv[i], "-f") == 0) { color_file = argv[i+1]; i++; continue; }  
        }

        //none of the options match
        //the first non option argument is color_file, the second is output_dir
        if( color_file.empty() )    { color_file = argv[i]; continue; }
        else if( output_dir.empty() )  { output_dir = argv[i]; continue; }
    }

    return 0;
}
/*
    This actually prints the colors into a file
*/
void print_cfg(const ConfigSection& section_config, const ConfigSection& global_colors,  const ConfigSection& section_colors, std::ostream& stream)
{

    //all of these values should have benn configured in config.cpp: process_config
    std::string format = section_config.key_value_pairs.at("format");
    std::string id_format = section_config.key_value_pairs.at("format_id");
    std::string comment = section_config.key_value_pairs.at("comment");
    std::string config_format = section_config.key_value_pairs.at("config_format");

    //whether to modify-add or modify-ignore existing values in the config
    std::string mode = section_config.key_value_pairs.at("mode");

    auto col_global = global_colors.key_value_pairs;
    auto col_section = section_colors.key_value_pairs;

    std::vector<std::string> already_printed;

    //std::cout << "Defaults: " << col_global << "\n";
    //std::cout << "Section: " << col_section << "\n";

    //unifies fields
    col_section.merge(col_global);

    
    //changes colors from names to indices wherever possible
    process_colors(col_section);
    
    /*for(const auto& [k, v] : col_section)
    {
        std::cout << "\t" << k << ":\t" << v << "\n";
    }*/

    const auto lines = split_string(config_format, "\n");

    /*
        Loop through config_format to set a more organized order of final file
    */
    for(const auto& line : lines)
    {

        //this will be the line to check for anything since we can remove whitespace and make life easier
        std::string check_line = line;

        //remove leading and trailing space
        int line_begin = line.find_first_not_of(" \t");
        int line_end = line.find_last_not_of(" \t");

        if(line_begin == std::string::npos) continue;
        if(line_begin > line_end) continue;

        check_line = line.substr(line_begin, line_end-line_begin+1);

        //split line into words
        auto words = split_string(check_line, " ");



        //split lines at tabs also
        std::vector<std::string> words_temp;
        for(auto& word : words)
        {
            auto temp = split_string(word, "\t");
            words_temp.insert(words_temp.end(), temp.begin(), temp.end());
        }
        
        words.insert(words.end(), words_temp.begin(), words_temp.end());

        //remove quotes (to get key if config is json)
        for(auto& word : words)
        {
            if(word.length() < 2) continue;
            if(word.starts_with('"') && word.ends_with('"')) word = word.substr(1, word.length()-2);
        }


        //insert a comment
        if(line.starts_with("#")) 
        {

            if(comment.empty()) continue;

            std::string out = line.substr(1);
            out = replace_all(out, "${section_name}", section_config.name); 
            out = replace_all(out, "${output_dir}", output_dir);
            out = replace_all(out, "${path}", section_config.key_value_pairs.at("path")); 

            stream << comment << line.substr(1) << "\n";
        }
        else if(line.starts_with("\n")) { stream << line; }

        //this also covers the case where the entire line is key
        //check if a key is overriden
        //FIXME: THIS ASSUMES FIRST WORD IS KEY!
        else if(col_section.contains(words[0]))
        {
            stream << format_value(format, id_format, words[0], col_section[words[0]]) << "\n";
            already_printed.emplace_back(words[0]);
        }
        //check if we have a key
        else if(int begin = line.find("${key="); begin != std::string::npos)
        {
            int end = line.find("}", begin+6);
            if(end == std::string::npos) continue;

            //here we have a valid ${key=abdec}
            std::string key = line.substr(begin+6, end-begin-6);

            if(col_section.contains(key))
            {
                stream << format_value(format, id_format, key, col_section[key]) << "\n";
                already_printed.emplace_back(key);
            }

        }
        //here we check if line contains the entire format
        //if it contains a token, its safe to assume its a formatted line
        else if(line.find("${") != std::string::npos)
        {
            /*
                Variable replace (TODO factor this into a function)
            */
            //std::cout << "calling with col_seciton from main.cpp\n";
            std::string out_line = insert_variables(line, col_section);

            //if variables couldn't be replace we skip the line to avoid random stuff
            if(out_line.find("${") != std::string::npos)
                out_line = "";

            stream << out_line << "\n";
        }
        else
        {
            stream << line << "\n";
        }
    }

    /*
        Loop through all the fields
        If color is one of the indexed ones (black, red, ...)
        Or provided as colorXXX where XXX is a number,
        Use alternate format

        ONLY ADD IF MODE IS SET TO MODIFY ADD (for vscode, we dont want this)
    */
    if(mode == "modify-add")
    {
        for(const auto [k, v] : col_section)
        {
            if(std::find(already_printed.begin(), already_printed.end(), k) != already_printed.end()) continue;
            stream << format_value(format, id_format, k, v) << "\n";
        }
    }
}

std::string format_value(const std::string& format, const std::string& id_format, const std::string& key, const std::string& value)
{

    std::string formatted = format;

    //std::cout << key << ": " << value << "\n";
    //if key is a number
    try
    {
        int id = stoi(key);

        //if its a bae color (0-255) we use the other format;
        formatted = id_format;

        if(id >= 0 && id <= 15)
            formatted = replace_all(formatted, "${color_name}", COLORS[id]);

    }catch(std::exception e) {}

    formatted = replace_all(formatted, "${color_name}", key);
    formatted = replace_all(formatted, "${color_id}", key);
    formatted = replace_all(formatted, "${color_value}", value);

    return formatted;
}