#include "emumanager.h"
#include "sc8emulator.h"
#include "cc8emulator.h"
#include <iostream>

EmuManager::EmuManager()
    :mMode(EmuManager::EmuMode::SCHIP)
{
    std::cout << "Emumanager constructor";
}

void EmuManager::StopEmulator()
{
    std::cout << "Emumanager::StopEmulator";
    //TODO call stopprocessing on emulator object, then close sdl window
    mEmulator->StopProcessing();

    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;
}

bool EmuManager::createSDLWindow()
{
    //Screen dimension constants
    constexpr int SCREEN_WIDTH_OFFSET = 0;
    constexpr int SCREEN_HEIGHT_OFFSET = 0;
    constexpr int SCREEN_WIDTH = 1280 + SCREEN_WIDTH_OFFSET;
    constexpr int SCREEN_HEIGHT = 640 + SCREEN_HEIGHT_OFFSET;

    //Create window
    gWindow = SDL_CreateWindow( "Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( gWindow == NULL )
    {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    else
    {
        //Create renderer for window
        gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
        if( gRenderer == NULL )
        {
            printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
            return false;
        }
    }
    return true;
}

//Create new SDL window and start emulator object with the filename
void EmuManager::StartEmulator(std::string filename)
{
    auto test = createSDLWindow();
    std::cout << "EmuManager::StartEmulator called with filename: " << filename;
    if(mMode == EmuManager::EmuMode::SCHIP)
    {
        mEmulator = std::make_unique<SC8Emulator>(filename, gWindow, gRenderer);
    }
    else if (mMode == EmuManager::EmuMode::LegacyC8)
    {
        mEmulator = std::make_unique<CC8Emulator>(filename, gWindow, gRenderer);
    }
    mEmulator->StartProcessing();
}

void EmuManager::ExecuteEmulatorStep()
{
    mEmulator->mainloop();
}

void EmuManager::Switchmode(EmuManager::EmuMode newMode)
{
    std::cout << "EmuManager::Switchmode";
}
