# Theme - unify your colors across programs

This program is designed so you have to only define your color theme once, and automatically apply it to all your programs

## Contents

1.  [Usage](#usage)
1.  [Config file](#config-file)
1.  [Color config](#color-config)
1.  [Syntax](#syntax)
    1. [Comments](#comments)
    1. [Sections](#sections)
    1. [Variables](#variables)
1. [Installation](#installation)

---

## Usage

`theme [options] <color-file-name> <output-dir>`

First argument that doesn't belong to any option goes to \<color-file-name>, the second to \<output-dir>. \<output-dir> defaults to local config folder, checks are performed in order:
 1. XDG_CONFIG_HOME
 2. $HOME/.config

Options:

|Option|Description|
|---|---|
|-o \<output-dir>  | Override output directory|
|-f \<file-name>   | Color config path|

---

## Config file 
The config file contains [sections](#sections) with fields. Until a section is specified, all the variables will be under the `default` section (See [syntax](#syntax)). All sections defined (even if empty) will generate a color file. The config file can contain the following fields:

| Name |Description|
|------|-----------|
|`path`|Where the color file will be generated. If a directory, the program will create a file called colors.conf inside, if a file, it will put the contents there|
|`format`|What color definition should look like|
|`format-id`|If the color has an id (eg. `bright_red` or `color46`), this format will be used instead|
|`comment`|The comment initialization string (eg `--` in lua or `#` in python)

You can use [variables](#variables) in the fields. Due to the current implementation, variables can't be used on `format` and `format_id`, since they would override

Default values will be provided both when the config is generated and if a field is missing:

|Name|Default value|
|----|----|
|`path`|`${output_dir}/${section_name}`|
|`format`|`${color_name} #${color_value}`|
|`format_id`|`${color_name} #${color_value}`|
|`comment`|`#`|

## Color config
The color config file is also made up of sections. If any additional sections are specified here but not in the [config file](#config-file), they will be ignored.

Sections can contain any field with any value, and they will be put into output files according to the `format`. If a color field name is `colorXXX` where `XXX` is any number from 0-255, the `color-id` field can be used, and will return `XXX`. This also works for standard ANSI colors (`black`, `bright_red`...).

Usually `color_name` field holds the entire name of the color, unless the color is `color0` - `color15`, then it contains the ANSI color name.

Every field will be outputted.

---


## Syntax

Config is a list key-value pairs, separated by `=`:
```
color = #123456
```
Leading and trailing spaces for the key and value are ignored, so the below code is parsed equivalently:
```
    color=       #123456        
```
Values can be multiline, but they need to be enclosed int quotes:
```
multi_line_variable = "line1
line2
line3"
```
Note that if the parentheses aren't closed, it will treat the rest of the file as the same value. The value inludes leading space in each line.

### Comments

Comments are single line, initiated by `#`

### Sections
Sections are defined with a line enclosed in `[ ]`
```
[section name]
```
Note that spaces are allowed, but leading and trailing ones will be cut.
All sections in the main config file will be treated as additional outputs, even if they have no fields. When parsing, all required fields will first be searched for
in the section, then the default section and lastly set as the default values if all else fails. The default (unspecified) section is `[default]`

In the color config file, the sections override the default colors.

### Variables

When referencing a variable, the syntax `${var_name}` is used. When referencing colors, `${color0}` is equivalent to `${black}` and so on for the 15 ANSI colors. The bright variants are referents with `${bright_black}`. The variables DO NOT HAVE to be defined before they are referenced.

Variables are split into two types:
- Config variables only available in the config file
- Color variables only available in the color config

If a variable substitution can't be found, it is left unchanged

| Config variables | Description |
|---------------|-------------|
|`section_name`|the name of the section in which the variable is defined|
|`output_dir`|the path of the output directory|
| Color variables | |
|`color_name`|the name of the color (used in format) — the name is the same as the name of the field, unless the field is `color0` - `color15` in which case the name is gotten from the id|
|`color_id` |the id of the color (0-255) — only available for fields `color0` -`color255` and the standard 15 ANSI colors|
|`color_value`| the value of the field

---

## Installation

Clone the repository

```bash
git clone https://github.com/loytech825/theme.git
```

Go into the repo folder and create a folder called build, and go into build

```bash
cd theme
mkdir build
cd build
```

Build the project with cmake, and install the program with make

```bash
cmake ..
cmake --build .
sudo make install
```