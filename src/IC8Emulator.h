#ifndef IC8EMULATOR_H
#define IC8EMULATOR_H

#include <SDL2/SDL.h>
#include <vector>

class IC8Emulator{
public:
    virtual void StartProcessing() = 0;
    virtual void StopProcessing() = 0;
    virtual bool isProcessing() = 0;
    virtual void mainloop() = 0;
    virtual void SetSDLKeyPressedEvent(SDL_Keycode key) = 0;
    virtual void SetSDLKeyReleasedEvent(SDL_Keycode key) = 0;
    virtual std::vector<uint8_t> GetCurrentRegisters() const = 0;
    virtual uint16_t GetCurrentPC() const = 0;
    virtual uint16_t GetCurrentIReg() const = 0;
    virtual ~IC8Emulator(){};
};

#endif // IC8EMULATOR_H
