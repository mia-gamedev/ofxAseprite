# ofxAseprite
A parser/renderer for Aseprite animations in OpenFrameworks.

# How to use
Draw some animations in Aseprite and export the file as a spritesheet:
![Aseprite export](docs/img/aseprite_export.png)

Make sure that you export JSON data with frame tags and that you have at least one tag defined. Even if there is a single animation in the file, you **still** need to set up frame tags.

# Limitations
* By default Aseprite will export a **non-relative** path as the image file. 
* Exported sprite sheets **must** be exported as an array, **not** as a hash table.
