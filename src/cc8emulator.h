#include <chrono>
#include <map>
#include <array>
#include <stack>
#include <string>
#include <bitset>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "display.th"
#include "c8timer.h"
#include "IC8Emulator.h"

#ifndef CC8EMULATOR_H
#define CC8EMULATOR_H


class CC8Emulator : public IC8Emulator
{
public:
    CC8Emulator(std::string filename, SDL_Window *gWindow, SDL_Renderer *gRenderer);
    CC8Emulator(const CC8Emulator &rhs) = delete;
    CC8Emulator(const CC8Emulator &&rhs) = delete;
    CC8Emulator& operator=(const CC8Emulator &rhs) = delete;
    CC8Emulator& operator=(const CC8Emulator &&rhs) = delete;
    void StartProcessing() override;
    void StopProcessing() override;
    virtual ~CC8Emulator() = default;

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
    static constexpr std::chrono::duration EXEC_INTERVAL_MUS = std::chrono::microseconds(2000);

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

    Display<64,32> dsp;

    uint16_t givecharaddress(uint8_t  c);
    uint16_t fetch();
    void execute(uint16_t cmd, const std::vector<SDL_Keycode> &keypressed);
    std::bitset<120> givesprite(char c);

    void startbeep(int channel);
    void stopbeep(int channel);
    void mainloop();

};

#endif // CC8EMULATOR_H
