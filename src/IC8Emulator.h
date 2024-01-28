#ifndef IC8EMULATOR_H
#define IC8EMULATOR_H

#include <SDL2/SDL.h>

class IC8Emulator{
public:
    virtual void StartProcessing() = 0;
    virtual void StopProcessing() = 0;
    virtual void mainloop() = 0;
    virtual void SetSDLKeyPressedEvent(SDL_Keycode key) = 0;
    virtual void SetSDLKeyReleasedEvent(SDL_Keycode key) = 0;
    virtual ~IC8Emulator(){};
};

#endif // IC8EMULATOR_H
