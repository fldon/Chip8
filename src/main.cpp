#include <stdexcept>
#include "c8timer.h"
#include "cc8emulator.h"
#include "sc8emulator.h"
#include "emumanager.h"
#include "qtfrontend.h"

/*initializes the sdl subsystems used in the emulator*/
bool initsdl(SDL_Window * &gWindow, SDL_Renderer * &gRenderer) {
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

        /*
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
        */

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

int main(int argc, char*argv[]) {
    SDL_Window * gWindow = nullptr;
    SDL_Renderer *gRenderer = nullptr;
    if(!initsdl(gWindow, gRenderer)) throw std::runtime_error("Could not initialize sdl window");

    EmuManager man;

    QApplication app(argc, argv);

    QTFrontend frontend(man);

    //wait here for the frontend to give "quit" signal
    app.exec();

    //Quit SDL subsystems
    SDL_Quit();
    return 0;
}
