#ifndef C8EMUDISPLAY_H
#define C8EMUDISPLAY_H

#include <chrono>
#include <bitset>
#include <vector>
#include <SDL2/SDL.h>
#include <iostream>

using namespace std::chrono_literals;

/*Should contain 64*32 pixels in regular mode, 128*64 in hp48 mode*/
class C8EmuDisplay{
public:
    static constexpr unsigned int SCREEN_WIDTH = 128;
    static constexpr unsigned int SCREEN_HEIGHT = 64;
    C8EmuDisplay(SDL_Window*, SDL_Renderer*, uint8_t* vf);
    bool draw(std::bitset<120>, unsigned int, unsigned int); //draw lowres sprite
    bool draw(std::bitset<256>, unsigned int, unsigned int); //draw hires sprite
    void switchDisplayRes(); //swap between hires and lores
    void clearScreen();
    unsigned int refresh(unsigned int passedCycles);
private:
    //Each vector element is one line of 128 bits
    std::vector<std::bitset<SCREEN_WIDTH>> pixels = std::vector<std::bitset<SCREEN_WIDTH>> (SCREEN_HEIGHT);
    //The window we'll be rendering to
    SDL_Window* gWindow = nullptr;
    //The Renderer used by the window
    SDL_Renderer* gRenderer = nullptr;
    unsigned char * VF = nullptr; //VF register the display writes to when calling draw()

    unsigned int cyclesSinceLastSec{0};
    static constexpr unsigned int CYCLES_PER_SECOND = 500; //TODO: Find out a reasonable value for this based on the COMPAC VP
    static constexpr unsigned int CYCLES_PER_SCREEN_REFRESH = CYCLES_PER_SECOND / 60;
};

#endif // C8EMUDISPLAY_H
