#include <iostream>
#include<fstream>
#include <stdexcept>
#include "display.th"
#include "c8timer.h"
#include "cc8emulator.h"

//TODO Make this mode configurable to SCHIP instruction set in frontend
enum class modes { ORIGCHIP };
constexpr modes mode = modes::ORIGCHIP;

/*initializes the sdl subsystems used in the emulator
TODO: needs to initialize keyboard input and audio as well*/
bool initsdl(SDL_Window * &gWindow, SDL_Renderer * &gRenderer) {

    //Screen dimension constants
    const int SCREEN_WIDTH_OFFSET = 0;
    const int SCREEN_HEIGHT_OFFSET = 0;
    const int SCREEN_WIDTH = 640 + SCREEN_WIDTH_OFFSET;
    const int SCREEN_HEIGHT = 320 + SCREEN_HEIGHT_OFFSET;

    //The window we'll be rendering to
    gWindow = NULL;
    //The window renderer
    gRenderer = NULL;
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Create window
        gWindow = SDL_CreateWindow( "Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
        }

        constexpr int samplingFrequency = 44100;
        Uint16 audioFormat = AUDIO_S16SYS;      // 16 Bits per sample
        constexpr int numChannels = 2;              // stereo
        constexpr int chunkSize = 4096;             // default per documentation
        if(Mix_OpenAudio(samplingFrequency, audioFormat, numChannels, chunkSize) == -1)
        {
            throw std::runtime_error(std::string("Could not start SDL_mixer. Error: ") + Mix_GetError());
            return -1;
        }
    }

    return success;

}

int main() {
    SDL_Window * gWindow = nullptr;
    SDL_Renderer *gRenderer = nullptr;
    if(!initsdl(gWindow, gRenderer)) throw std::runtime_error("Could not initialize sdl window");

    std::string filename = "";
    std::cout << "Please enter a file to load: ";
    std::cin >> filename;

    CC8Emulator emu(filename, gWindow, gRenderer);

    emu.StartProcessing(); //TODO: move all the managing stuff into EmuManager

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
    return 0;
}
