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

    if(mEmulator != nullptr)
    {
        mEmulator->StopProcessing();
        while(mEmulator->isProcessing())
        {

        }
        //Destroy emulator object and SDLWidget
        gWidget = nullptr;
        mEmulator = nullptr;
    }
}

void EmuManager::createSDLWindow()
{
    //Screen dimension constants
    constexpr int SCREEN_WIDTH_OFFSET = 0;
    constexpr int SCREEN_HEIGHT_OFFSET = 0;
    constexpr int SCREEN_WIDTH = 1280 + SCREEN_WIDTH_OFFSET;
    constexpr int SCREEN_HEIGHT = 640 + SCREEN_HEIGHT_OFFSET;

    gWidget = std::make_unique<SDLWidget>(this);
}

//Create new SDL window and start emulator object with the filename
void EmuManager::StartEmulator(std::string filename)
{
    createSDLWindow();
    std::cout << "EmuManager::StartEmulator called with filename: " << filename;
    if(mMode == EmuManager::EmuMode::SCHIP)
    {
        mEmulator = std::make_unique<SC8Emulator>(filename, gWidget->mWindow, gWidget->mRenderer);
    }
    else if (mMode == EmuManager::EmuMode::LegacyC8)
    {
        //mEmulator = std::make_unique<CC8Emulator>(filename, gWindow, gRenderer);
    }
    mEmulator->StartProcessing();
}

void EmuManager::ExecuteEmulatorStep()
{
    mEmulator->mainloop();
}

void EmuManager::Switchmode(EmuManager::EmuMode newMode)
{
    StopEmulator();
    mMode = newMode;
}

void EmuManager::SendSDLKeyPressedEvent(SDL_Keycode key)
{
    mEmulator->SetSDLKeyPressedEvent(key);
}

void EmuManager::SendSDLKeyReleasedEvent(SDL_Keycode key)
{
    mEmulator->SetSDLKeyReleasedEvent(key);
}
