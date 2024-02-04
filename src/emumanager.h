#ifndef EMUMANAGER_H
#define EMUMANAGER_H
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "IC8Emulator.h"
#include "sdlwidget.h"
#include "ISDLInputReceiver.h"

/*
 * Manages current emulator
 * Encapsulates the functionality that the QT frontend calls later
 * */
class EmuManager : public ISDLINPUTRECEIVER
{
public:
    enum class EmuMode
    {
        LegacyC8,
        SCHIP
    };

    EmuManager();

    void Switchmode(EmuMode newMode); //Switches to different emu mode (unless it's already selected); stops the emu as well (essentially switches the mEmulator object)
    void StopEmulator(); //Stops the current emulator and destroys the SDL window
    void StartEmulator(std::string filename); //Starts emulator in current mode: creates new SDL window
    void ExecuteEmulatorStep(); //Executes one step of mainloop
    void SendSDLKeyPressedEvent(SDL_Keycode);
    void SendSDLKeyReleasedEvent(SDL_Keycode);

private:
    void createSDLWindow();

    std::unique_ptr<IC8Emulator> mEmulator;
    EmuMode mMode;
    //SDL_Window *gWindow;
    std::unique_ptr<SDLWidget> gWidget;
};

#endif // EMUMANAGER_H
