#include <chrono>
#include <map>
#include <array>
#include <stack>
#include <string>
#include <bitset>
#include <atomic>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "c8emudisplay.h"
#include "c8timer.h"
#include "IC8Emulator.h"

#ifndef SC8EMULATOR_H
#define SC8EMULATOR_H


class SC8Emulator : public IC8Emulator
{
public:
    SC8Emulator(std::string filename, SDL_Window *gWindow, SDL_Renderer *gRenderer);
    SC8Emulator(const SC8Emulator &rhs) = delete;
    SC8Emulator(const SC8Emulator &&rhs) = delete;
    SC8Emulator& operator=(const SC8Emulator &rhs) = delete;
    SC8Emulator& operator=(const SC8Emulator &&rhs) = delete;
    void StartProcessing() override;
    void StopProcessing() override;
    bool isProcessing() override;
    virtual ~SC8Emulator() = default;
    void mainloop() override;
    void SetSDLKeyPressedEvent(SDL_Keycode key) override;
    void SetSDLKeyReleasedEvent(SDL_Keycode key) override;

    std::vector<uint8_t> GetCurrentRegisters() const override;
    uint16_t GetCurrentPC() const override;
    uint16_t GetCurrentIReg() const override;

protected:

    virtual void read_input_file(std::string filename);
    virtual void dump_ram(std::string filename);
    virtual std::ostream& dump_emu_core(std::ostream& os);
    virtual void initsprites();
    virtual void initram();

private:
    enum class keys {
        KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE,
        KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE,
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_DEBUG
    };
    static constexpr std::chrono::duration EXEC_INTERVAL_MUS = std::chrono::microseconds(1000);

    //SDL keybinds:
    std::map<SDL_Keycode, keys> sdltoc8keys;
    std::map<keys, SDL_Keycode> c8tosdlkeys;

    static constexpr uint16_t PROGRAMSTART = 0x200; //where the program is expected to be loaded
    static constexpr std::size_t MAXRAM = 4096;
    static constexpr std::size_t NUM_OF_REGS = 16;

    //system ressources:
    std::array<uint8_t, MAXRAM> ram;
    //used to point to 16-bit RAM addresses
    std::stack<uint16_t> systemstack;
    uint16_t program_counter = PROGRAMSTART; //0x200 is where most programs start
    std::array<uint8_t, NUM_OF_REGS> reg; //V0 to VF
    uint16_t Ireg = 0; //register known as I; stores memory addresses

    C8Timer delaytimer;
    C8Timer soundtimer;
    int soundchannel;

    C8EmuDisplay dsp;

    uint16_t givecharaddress(uint8_t  c);
    uint16_t fetch();
    void execute(uint16_t cmd, const std::vector<SDL_Keycode> &keypressed);
    std::bitset<120> givesprite(char c);

    void startbeep(int channel);
    void stopbeep(int channel);

    std::pair<SDL_Keycode, bool> currentpressedkey; //true for keydown, false for keyup-event

    std::atomic_bool keepProcessing{false};
    std::atomic_bool Stopped{true};

};

#endif // SC8EMULATOR_H
