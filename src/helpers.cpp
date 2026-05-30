#include "helpers.h"
#include <exception>
#include <pwd.h>
#include <unistd.h>

/*
    @brief replaces all occurences of `from` to `to` in a given `orig` DOES NOT MODIFY THE STRING, BUT RETURNS A NEW ONE 
*/
std::string replace_all(const std::string& orig, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;

    std::string formatted = orig;

    while(true)
    {
        start_pos = formatted.find(from, start_pos);
        if(start_pos == std::string::npos) break;

        formatted.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return formatted;
}

/*
    Returns the root folder for configs (such as ~/.config)
    to `dir`
*/
std::string get_config_dir()
{

    char* config_dir;
    std::string dir;

    //check for XDG_CONFIG first
    if((config_dir = getenv("$XDG_CONFIG_HOME")) != NULL)
    {
        return config_dir;
    }

    //then check HOME env
    if((config_dir = getenv("HOME")) != NULL)
    {
        dir = config_dir;
        dir += "/.config";
        return dir;
    }

    //neither env have been found
    config_dir = getpwuid(getuid())->pw_dir;
    dir = config_dir;
    dir += "/.config";

    return dir;
}

/*
    Turns color name to id, returns -1 if fails
    id can be higher than 15!
*/
int get_color_id(const std::string& color)
{

    if(color.starts_with("color"))
    {
        try{
            int id = stoi(color.substr(5));
            return id;
        }catch(std::exception e)
        {
            return -1;
        }
    }

    for(int i = 0; i < 16; i++)
    {
        if(COLORS[i] == color)  return i;
    }

    return -1;
}