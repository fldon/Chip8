A Simple Chip8 Emulator

Currently only supports the basic (or "legacy") CH8 mode programs.
To start, run the emulator and select the file to be loaded from the GUI.
You can stop the currently running program from the main window.
 
 
 TODO:
-Make running speed configurable using the frontend

-Add schip support:
1. Add enabling and disabling commands for hires display mode in display class
Apparently, when switching, already drawn pixels are XOR’d in 1x2 vertical coloumns. The screen is NOT cleared when switching, but I’m not yet sure about the details of how it works
The space of the screen is the same in both modes, only the size of the squares is changed

It makes little sense to keep the display class as a template: just make one for the SCHIP emu that contains the higher resolution, and give it a function for switching the mode and for drawing large sprites: It can then also be used for the regular chip8 emu, without using templates

2. Add support for scrolling commands in display class
3. Hardcode hires font characters for use in new opcodes (with function for retrieving a hires character, similar to existing function for lores characters)
4. Include new opcodes and add quirks to old opcodes for schip mode



