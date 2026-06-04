# Planned additions (TODO list)

- Files as variables (copy file contents into a field)
- File includes
- `file` field - instead of generating a color file, it just copies it over (ex. if a theme for a program already exists)
- Better color parsing (RGB, RGBA)
- Add 8 to 256 color generation
- Color variable indexing (eg. green0-16...), if we have a variable like `primary = green` then `primary4` should be a different shade.


Make a datastructure to hold Config data (default + vector of sections)
and to hold Color data(Default + vector of sections + generated palette)
color sections can be as map<string, Color> instead of map<string, string> for easier conversion and whatnot 