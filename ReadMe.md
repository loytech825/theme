# Thync

S**ync** your **th**emes witha single file!

Automatically generates theme files for all your programs

## Contents
- [Thync](#thync)
  - [Contents](#contents)
  - [Pre-use setup](#pre-use-setup)
  - [Usage](#usage)
  - [Config](#config)
    - [Examples](#examples)
  - [Installation](#installation)

---

## Pre-use setup
Before you can succesfully use this program, some setup is required. Ensure you set all colors in your main config files to variables (example for hyprland):

```lua
...
inactive_border = bg
...
```

and include a color file (where the variables will be defined):

```lua
require("hyprland-colors")
```

Then set this file as the path inside the [theme config file](#config-file):

```conf
...
[hyprland]
path = "path/to/hprland-colors.lua"
...
```

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

## Config

See [config.md](config.md)

### Examples

Examples are provided in the [examples](examples/) folder.

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