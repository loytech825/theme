# Examples

Provided is a minimal example config to show most of the unclear fields

**config.conf**

```conf
[default]
path = ${output_dir}/${section_name}
format = ${color_name} #${color_value}
format_id = ${color_name} #${color_value}
comment = #

[kitty]
[waybar]
format = "@define-color ${color_name} #${color_value}"
mode = "modify-ignore"

# here we specify vs-code specific variables
[vs-code]

# everything inside the quotes is literal (except variables)
# comments can be started either with a # or the language specific comment starter
# if `comment` is set to an empty string, all lines starting with # will not be copied
config_format = "
{
    \"name\" = \"VsCode-theme test\",
    \"colors\": {
        \"foreground\": \"#${foreground}\",
        \"terminal.ansiBlack\": \"#${black}\",
    }

}"

#linter kinda breaks here lol


# set this to extension path, note the .
#path = "/path/to/extension/colors.json"
path = "${output_dir}/vs-code/colors.json"
```

The folowing file is a minimal example of a color file
**colors.conf**

```conf

black = #000000
red = #ff0000
green = #00ff00
blue = #00ff00
white = #ffffff

[kitty]
foreground = #${white}
```

---
When `theme` is called the files in the [example output](example-output/) folder are produced, as if the *example output* folder is set as the `output_dir`.