A Simple Chip8 Emulator

Currently only supports the basic (or "legacy") CH8 mode programs.
To start, run the emulator and give the path to the program to be loaded on the command line when prompted.
 
 
 TODO:

-Add GUI Frontent, where user can restart emulator with new program and switch modes, also add debug output for registers and currently loaded instruction and PC position

1.Currently, the QT frontend “eats” the keyboard input. This is a common and known problem when using both Qt and SDL. The “solutioN” is to take the keyboard input that Qt receives, and give it to the SDL emulator structs. This means that the main loop has to be changed a bit, so that the keyboard inputs can be given from outside
2. Make the GUI stop the emulator when pressing “Stop emulator”
3. Implement the switch of modes (in the gui, the modes need to be exclusive to each other)
4. Implement debug mode in emulator and in GUI

-Make running speed configurable using the frontend

-Include debug mode, where one instruction is loaded with each keypress

-Add schip support:
1. Add enabling and disabling commands for hires display mode in display class
Apparently, when switching, already drawn pixels are XOR’d in 1x2 vertical coloumns. The screen is NOT cleared when switching, but I’m not yet sure about the details of how it works
The space of the screen is the same in both modes, only the size of the squares is changed

It makes little sense to keep the display class as a template: just make one for the SCHIP emu that contains the higher resolution, and give it a function for switching the mode and for drawing large sprites: It can then also be used for the regular chip8 emu, without using templates

2. Add support for scrolling commands in display class
3. Hardcode hires font characters for use in new opcodes (with function for retrieving a hires character, similar to existing function for lores characters)
4. Include new opcodes and add quirks to old opcodes for schip mode

-make rendering wait and timers depend on clock/execution cycles instead of time: that way, only the execution speed is relevant. For that You’d have to know the cycle timings for every single operation though...
According to Laurence Scotford, the display refresh is always synched with the timer increments! (both of them!) So it would make sense to only have one single timer counting down (or rather, to calculate the same timing value from the clock cycles for all of them)
