A Simple Chip8 Emulator

Currently only supports the basic (or "legacy") CH8 mode programs.
To start, run the emulator and select the file to be loaded from the GUI.
You can stop the currently running program from the main window.
 
 
 TODO:
-Make the timer countdowns only depend on the running speed of the emulator: so the timer needs to decrease based on execution cycles, not on time
Maybe just assume that one execution cycle takes 1/600th of a second? That is of course wrong, but the only other option is to somehow get execution timings for every single opcode (which I couldn’t find so far)
Of course, this timing-idea must also be applied to the screen refresh timings, meaning that during each execution step a corresponding decrementTimer() function on the display must be called

Current Problem: I really don’t get how the three timers relate to the cycle speed. I can’t just count up to 500 cycles per second of the timers if I set the execution speed to 500hz: then the timers are WAYYYY too slow, not even close to 1 second per decrease. For some reason this was not a problem when using actual timers that ticked down per second…
How to make this more sensible? I must be missing something about this whole cycle and synchronization/timing thing

New Problem: Timedus’ Quirk program now says that I have a clipping error and that the display is “slow”. What is the clipping error and how can I make the display fast enough for the test? Earlier, I just decreased the refresh timer, of course I can no longer do that.

I think what happened: The test only works if the timers run down slower than the display (otherwise it shows “slow”) BUT: If the timers run down way too slowly, it thinks the display synch is off
But now it says “on” if I put the timers on 3000 cycles, so all good

Only problem left: Clipping gives Err2 now!


-Make running speed configurable using the frontend

-Add schip support:
1. Add enabling and disabling commands for hires display mode in display class
Apparently, when switching, already drawn pixels are XOR’d in 1x2 vertical coloumns. The screen is NOT cleared when switching, but I’m not yet sure about the details of how it works
The space of the screen is the same in both modes, only the size of the squares is changed

It makes little sense to keep the display class as a template: just make one for the SCHIP emu that contains the higher resolution, and give it a function for switching the mode and for drawing large sprites: It can then also be used for the regular chip8 emu, without using templates

2. Add support for scrolling commands in display class
3. Hardcode hires font characters for use in new opcodes (with function for retrieving a hires character, similar to existing function for lores characters)
4. Include new opcodes and add quirks to old opcodes for schip mode



