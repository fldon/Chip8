A Simple Chip8 Emulator

Currently only supports the basic (or "legacy") CH8 mode programs.
To start, run the emulator and select the file to be loaded from the GUI.
You can stop the currently running program from the main window.

Enabling the Debug mode allows you to step through individual instructions.

You can change the clock speed using the GUI.
 
 
 TODO:
-Add schip support:
1. Add enabling and disabling commands for hires display mode in display class
Apparently, when switching, already drawn pixels are XOR’d in 1x2 vertical coloumns. The screen is NOT cleared when switching, but I’m not yet sure about the details of how it works
The space of the screen is the same in both modes, only the size of the squares is changed
2. Add support for scrolling commands in display class
3. Add hires font characters for use in new opcodes
4. Include new opcodes and add quirks to old opcodes for schip mode



