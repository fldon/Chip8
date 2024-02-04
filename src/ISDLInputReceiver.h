#ifndef ISDLINPUTRECEIVER_H
#define ISDLINPUTRECEIVER_H
#include <SDL2/SDL.h>

class ISDLINPUTRECEIVER
{
public:
    virtual void SendSDLKeyPressedEvent(SDL_Keycode) = 0;
    virtual void SendSDLKeyReleasedEvent(SDL_Keycode) = 0;
};

#endif // ISDLINPUTRECEIVER_H
