#include "c8emudisplay.h"

C8EmuDisplay::C8EmuDisplay(SDL_Window * nWindow, SDL_Renderer *nRenderer, uint8_t* vf)
    : gWindow(nWindow), gRenderer(nRenderer), VF(vf) {
    clearScreen();
}

void C8EmuDisplay::clearScreen() {
    for(int i = 0; i < SCREEN_WIDTH; i++) {
        for(int j = 0; j < SCREEN_HEIGHT; j++) {
            pixels[j] = 0;
        }
    }
}

/*Draws a sprite and refreshes the display
Sprite is drawn starting from (xorig, yorig)
x and y loop if they are larger than screen resolution
Sprites are cut off if they are too large
If any pixel is turned off by drawing, the VF register needs to be set to 1, otherwise to 0*/
void C8EmuDisplay::draw(std::bitset<120> sprite, unsigned int xorig, unsigned int yorig) {
    xorig = xorig % SCREEN_WIDTH;
    yorig = yorig % SCREEN_HEIGHT;
    //Sprites are at most 15 bytes long per specification
    //Go through sprite bytes from left to right
    *VF = 0;
    for(size_t i = 0; i < 15; i++) {
        std::bitset<SCREEN_WIDTH> currline;// = std::bitset<SCREEN_WIDTH>() & sprite;
        for(std::size_t bit = 0; bit < 120; bit++)
        {
            currline[bit] = sprite[bit];
        }
        currline >>= ((14 - i)*8); // go through sprite lines
        currline = currline & std::bitset<SCREEN_WIDTH>(255); //only take rightmost byte
        //XOR the sprite and the screen bitset starting at xorig,yorig
        //Cut off sprites at the edges
        //TODO: XOR currently assumes that screen fits inside a long. This is not ideal and should be fixed
        if((yorig + i) < SCREEN_HEIGHT){
            //Special case: sprite gets cut off on the right side (then it has to be shifted right instead of left)
            auto shiftedline = xorig <= (SCREEN_WIDTH-8) ? (currline << (SCREEN_WIDTH - 8 - xorig)) : (currline >> (xorig - (SCREEN_WIDTH - 8)));
            if((pixels[yorig + i] & (shiftedline)).any())
            {
                *VF = 1; //TODO: test whether this works, it should
            }
            pixels[yorig + i] = std::bitset<SCREEN_WIDTH>(pixels[yorig + i] ^ (shiftedline));   //display bitset of the current line
        }
    }
}

/*calls to SDL to (re)draw all the pixels and to update the window*/
void C8EmuDisplay::refresh() {
    bool rendernow = true;

    //if(displaymode == displaymodes::ORIGCHIP)
    {
        //count time since last refresh call: only call refresh if time is larger than refresh interval
        // otherwise, set rendernow to false
        static std::chrono::time_point<std::chrono::high_resolution_clock> lastTimeRefresh = std::chrono::high_resolution_clock::now();
#ifdef CHECKFRAMETIMINGS
        static std::chrono::time_point<std::chrono::high_resolution_clock> lastFullSecond = std::chrono::high_resolution_clock::now();
        static int framecount = 0;
#endif
        static constexpr std::chrono::duration MUS_INTERVAL = 16667us;
        {
            do
            {
                const std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
                if(std::chrono::duration_cast<std::chrono::microseconds>(now - lastTimeRefresh) <  MUS_INTERVAL)
                {
                    rendernow = false;
                }
                else
                {
#ifdef CHECKFRAMETIMINGS
                    framecount++;
                    std::cout << "Time for last frame: " << std::chrono::duration_cast<std::chrono::microseconds>(now - lastTimeRefresh).count() << "mus\n";
#endif
                    lastTimeRefresh = std::chrono::high_resolution_clock::now();
                    rendernow = true;
                }
            }
            while(!rendernow);
        }
#ifdef CHECKFRAMETIMINGS
        const std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        if(std::chrono::duration_cast<std::chrono::microseconds>(now - lastFullSecond) > 1s)
        {
            lastFullSecond = std::chrono::high_resolution_clock::now();
            std::cout << "Frames in last second drawn: " << framecount << "\n";
            framecount = 0;
        }
#endif
    }

    if (rendernow)
    {
        //Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderClear( gRenderer );

        //Render red filled quad (128*64)
        for(int i = 0; i < SCREEN_WIDTH; i++) {
            for(int j = 0; j < SCREEN_HEIGHT; j++) {
                SDL_Rect fillRect = {i * 10, j * 10, 10, 10 }; //TODO: 10 as magic number should probably be replaced... It stems from the factor ten in 64 pixels to 640 as screen width
                SDL_SetRenderDrawColor( gRenderer, ((pixels[j] >> (SCREEN_WIDTH - 1 - i)) & std::bitset<SCREEN_WIDTH>(1)).any() ? 0xFF : 0x00, 0x00, 0x00, 0xFF );
                SDL_RenderFillRect( gRenderer, &fillRect );
            }
        }


        //Update screen
        SDL_RenderPresent( gRenderer );
    }
}
