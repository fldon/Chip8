#include "sc8emulator.h"
#include <fstream>
#include <iostream>
#include <algorithm>

SC8Emulator::SC8Emulator(std::string filename, SDL_Window *gWindow, SDL_Renderer *gRenderer)
    :dsp(gWindow, gRenderer, static_cast<uint8_t*>(reg.data() + NUM_OF_REGS - 1))
{
    //init everything and read program from file, but don't start the mainloop yet

    sdltoc8keys.insert(std::make_pair(SDLK_1, keys::KEY_ONE));
    sdltoc8keys.insert(std::make_pair(SDLK_2, keys::KEY_TWO));
    sdltoc8keys.insert(std::make_pair(SDLK_3, keys::KEY_THREE));
    sdltoc8keys.insert(std::make_pair(SDLK_q, keys::KEY_FOUR));
    sdltoc8keys.insert(std::make_pair(SDLK_w, keys::KEY_FIVE));
    sdltoc8keys.insert(std::make_pair(SDLK_e, keys::KEY_SIX));
    sdltoc8keys.insert(std::make_pair(SDLK_a, keys::KEY_SEVEN));
    sdltoc8keys.insert(std::make_pair(SDLK_s, keys::KEY_EIGHT));
    sdltoc8keys.insert(std::make_pair(SDLK_d, keys::KEY_NINE));
    sdltoc8keys.insert(std::make_pair(SDLK_x, keys::KEY_ZERO));
    sdltoc8keys.insert(std::make_pair(SDLK_4, keys::KEY_C));
    sdltoc8keys.insert(std::make_pair(SDLK_r, keys::KEY_D));
    sdltoc8keys.insert(std::make_pair(SDLK_f, keys::KEY_E));
    sdltoc8keys.insert(std::make_pair(SDLK_y, keys::KEY_A));
    sdltoc8keys.insert(std::make_pair(SDLK_c, keys::KEY_B));
    sdltoc8keys.insert(std::make_pair(SDLK_v, keys::KEY_F));
    sdltoc8keys.insert(std::make_pair(SDLK_b, keys::KEY_DEBUG));
    for(auto it = sdltoc8keys.begin(); it != sdltoc8keys.end(); ++it)
    {
        c8tosdlkeys[it->second] = it->first;
    }
    initram();

    read_input_file(filename);

    Mix_Chunk* sound = Mix_LoadWAV("../res/Beep.wav");
    if(!sound)
    {
        std::cerr << "Could not open Beep.wav. Error: " << Mix_GetError() << std::endl;
    }
    soundchannel = Mix_PlayChannel(-1, sound, -1);
    Mix_Volume(soundchannel, 2); //TODO: destructor of emulator should reset all SDL thingies and stop sounds
}

void SC8Emulator::SetSDLKeyPressedEvent(SDL_Keycode key)
{
    currentpressedkey = std::make_pair(key, true);
}

void SC8Emulator::SetSDLKeyReleasedEvent(SDL_Keycode key)
{
    currentpressedkey = std::make_pair(key, false);
}

void SC8Emulator::StartProcessing()
{
    keepProcessing = true;
}

void SC8Emulator::StopProcessing()
{
    keepProcessing = false;
}

bool SC8Emulator::isProcessing()
{
    return (keepProcessing || !Stopped);
}

/*reads filename to open from cin and copies a program into ram starting at 0x200*/
void SC8Emulator::read_input_file(std::string filename) {
    std::ifstream is(filename, std::ios_base::binary);
    short currcounter = program_counter;
    if(!is) throw std::runtime_error("Could not open file: " + filename);

    while(is) {
        is >> std::noskipws >> ram.at(currcounter);
#ifdef DEBUG
        {
            std::cerr << "Debug: Currently read instruction into ram: " << std::hex << (((unsigned int)ram[currcounter]) & 255) << std::endl;
        }
#endif
        currcounter++;
    }
}

/*Fetch two bytes from the point in RAM PC points to
and return them as a short*/
uint16_t SC8Emulator::fetch() {
    if(program_counter > MAXRAM) {
        dump_emu_core(std::cerr);
        throw std::runtime_error("Fetch: PC is out of bounds of RAM!");
    }
    if(program_counter < PROGRAMSTART) {
        dump_emu_core(std::cerr);
        throw std::runtime_error("Fetch: PC points to reserved memory below 0x200!");
    }
    //This should make a short out of the two bytes returned by PC
    uint16_t a = (static_cast<uint16_t>(ram[program_counter] & 255)) << 8;
    uint16_t b = static_cast<uint16_t> (ram[program_counter+1] & 255);
#ifdef DEBUG
    std::cerr << "Debug: currently fetched instruction: " << std::hex << ((a>>8)&255) << b << std::endl;
#endif
    return static_cast<uint16_t>(a | b);
}


/*Dumps the whole ram to given output file*/
void SC8Emulator::dump_ram(std::string filename) {
    std::ofstream os(filename, std::ios_base::binary);
    std::size_t i = 0;
    while(i < MAXRAM)
    {
        os << ram[i];
        ++i;
    }
}


/*Should dump all registers to os in a coherent format*/
std::ostream& SC8Emulator::dump_emu_core(std::ostream& os) {
    dump_ram("debugoutput");
    os << std::hex << "Program counter: " << program_counter << std::endl;
    os << "Registers:\n";
    for(size_t i = 0; i < NUM_OF_REGS; i++) {
        os << "V" << i << ": " << ((unsigned int)reg[i] & 255) << std::endl;
    }
    os << std::endl;
    os << "Ireg: " << Ireg << std::endl;
    return os;
}

/*Initializes RAM and registers to state before program execution
RAM should only contain basic sprites*/
void SC8Emulator::initram() {
    for(size_t i = 0; i < MAXRAM; ++i) {
        ram[i] = 0;
    }
    for(size_t i = 0; i < NUM_OF_REGS; ++i) {
        reg[i] = 0;
    }
    initsprites();
}

/*Returns a bitset that represents the character c saved in RAM
Throws a fatal error if c isn't a valid character: this should never happen!*/
std::bitset<120> SC8Emulator::givesprite(char c) {
    if(c < '0' || c > 'F') throw std::runtime_error("givesprite: requested character is invalid!");
    size_t startval = 0x50; //TODO: 0x50 bezieht sich auf SPRITESTART weiter oben: sollte keine magic number sein
    if(c <= '9') startval += (c - '0') * 15;
    else startval += (c - 'A' + 10) * 15;
    //read in all 15 bytes into bitset:
    std::bitset<120> bs(0);
    for(size_t i = 0; i < 15; i++) {
        //TODO: the & with 255 is a hack, needed because the chars get promoted to negative ints(leading 1's) due to two's complement stuff
        bs = bs | std::bitset<120>(ram[startval + i] & 255);
        if(i < 14)
            bs <<= 8;
    }
    return bs;
}

/*Return the address in ram of the built-in character c, throw exception if c is invalid*/
uint16_t SC8Emulator::givecharaddress(uint8_t c)
{
    if(c > 0xF) throw std::runtime_error("givesprite: requested character is invalid!");
    size_t startval = 0x50; //TODO: 0x50 bezieht sich auf SPRITESTART weiter oben: sollte keine magic number sein
    startval += (c) * 15;
    return startval;
}

void SC8Emulator::startbeep(int channel)
{
    //start playing a continuous, looping sound
    Mix_Resume(channel);
}

void SC8Emulator::stopbeep(int channel)
{
    //stop playing the continuous sound
    Mix_Pause(channel);
}

void SC8Emulator::mainloop() {
    //Main loop flag
    static constexpr unsigned int CYCLES_PER_INSTR = 10;

    std::vector<SDL_Keycode> keyPressed; // last element in keypress enum as of 25/12/2023

    //Event handler
    SDL_Event e;

//While application is running
#ifdef DEBUG
    while( !quit && std::cin.get() )
#else
    //while( !quit )
#endif
        //{
        //Handle events on queue
        //while( SDL_PollEvent( &e ) != 0 )
        if(keepProcessing && currentpressedkey.first != 0)
        {
            Stopped = false;
            /*
            switch(e.type)
            {
            //User requests quit
            case(SDL_QUIT):
            {
                quit = true;
            }break;
            case SDL_KEYDOWN:
            {
                if(std::find(keyPressed.begin(), keyPressed.end(), e.key.keysym.sym) == keyPressed.end())
                {
                    keyPressed.push_back(e.key.keysym.sym);
                }
            }break;
            case SDL_KEYUP:
            {
                keyPressed.erase(std::remove(keyPressed.begin(), keyPressed.end(), e.key.keysym.sym), keyPressed.end());
            }break;
            }
            */

            if(currentpressedkey.second)
            {
                if(std::find(keyPressed.begin(), keyPressed.end(), currentpressedkey.first) == keyPressed.end())
                {
                    keyPressed.push_back(currentpressedkey.first);
                }
            }
            else
            {
                keyPressed.erase(std::remove(keyPressed.begin(), keyPressed.end(), currentpressedkey.first), keyPressed.end());
            }
        }
    //handle keypressevents and give them to the execute function

    //decode program commands
    //excecute program commands
    if(execute(fetch(), keyPressed))
    {
        //iterate to next command (increment PC by two bytes)
        program_counter += 2;
    }
    //start or stop playing sound depending on soundtimer:
    if(soundtimer.getTimerval() > 0)
    {
        startbeep(soundchannel);
    }
    else
    {
        stopbeep(soundchannel);
    }

    unsigned int sub_cycles = dsp.refresh(CYCLES_PER_INSTR);
    delaytimer.decrement(CYCLES_PER_INSTR - sub_cycles); //Assume ten cycles per instruction (just some bullshit value to get timings okayish)
    soundtimer.decrement(CYCLES_PER_INSTR - sub_cycles);

    Stopped = true;
    //}
}

/*TODO:
Decode and execute the input command according to the c8 specs*/
bool SC8Emulator::execute(uint16_t cmd, const std::vector<SDL_Keycode> &keypressed) {
    //decode cmd into four 4-bit nibbles:
    unsigned char nibbles[4] = {0,0,0,0};
    for(int i = 0; i < 4; i++) {
        nibbles[i] = ( (cmd >> (3-i) * 4)  & 15 ); //bit masking with 00001111

    }
#ifdef DEBUG
    std::cerr << "Debug: currently read nibbles: " << std::hex << ((short)(nibbles[0])&15) << ((short)(nibbles[1])&15) << ((short)(nibbles[2])&15)<< ((short)(nibbles[3])&15) << std::endl;
#endif
    //huge switch-case for all operations using the 4 nibbles:
    //TODO: These are the variables used in the switch case, they can't be declared inside it: better to find a nice solution for this
    uint16_t res_1 = 0;    //for operations that use a short for calculations
    uint8_t res_2 = 0;     //for operations that use a char
    std::bitset<120> spriteset = 0;  //for the draw operation

    switch(nibbles[0]) {
    case 0:
        //there's only 00E0(clrscrn) and 00EE(rtrnstck)
        switch(nibbles[3]) {
        case 0:
            //00E0: clear screen
            dsp.clearScreen();
            break;
        case 0xE:
            //00EE: pop address from stack and set PC to it
            if(systemstack.empty()) {
                dump_emu_core(std::cerr);
                throw std::runtime_error("00EE instruction: Stack is empty!");
            }
            program_counter = systemstack.top();
            systemstack.pop();
            break;
        }
        break;

    case 1:
        //Jump (1NNN): set PC to NNN
        res_1 = 0;
        for(int i = 1; i < 4; i++) {
            res_1 = res_1 | nibbles[i] ;
            if(i < 3)
                res_1 <<= 4;
        }
        program_counter = res_1;
        program_counter -= 2; //TODO: This is a hack, because the counter is incremented in run_emu after execute()
        break;

    case 2:
        //Jump (2NNN): Push PC to stack, then move it to NNN (See 1NNN for reference)
        systemstack.push(program_counter);
        res_1 = 0;
        for(int i = 1; i < 4; i++) {
            res_1 = res_1 | nibbles[i] ;
            if(i < 3)
                res_1 <<= 4;
        }
        program_counter = res_1;
        program_counter -= 2; //TODO: This is a hack, because the counter is incremented in run_emu after execute()
        break;

    case 3:
        //3XNN: will skip an instruction if the value in VX is equal to NN
        res_2 = 0;
        for(int i = 2; i < 4; i++) {
            res_2 = res_2 | nibbles[i];
            if(i < 4 - 1) // dont shift last nibble
            {
                res_2 <<= 4;
            }
        }
        if(reg[nibbles[1]] == res_2) program_counter += 2;
        break;

    case 4:
        //4XNN: will skip an instruction if the value in VX is NOT equal to NN
        res_2 = 0;
        for(int i = 2; i < 4; i++) {
            res_2 = res_2 | nibbles[i];
            if(i < 4 - 1) // dont shift last nibble
            {
                res_2 <<= 4;
            }
        }
        if(reg[nibbles[1]] != res_2) program_counter += 2;
        break;

    case 5:
        //5XY0: skips an instruction if VX and VY are equal
        if(reg[nibbles[1]] == reg[nibbles[2]]) program_counter += 2;
        break;

    case 6:
        //6XNN: Set register X to NN
        res_2 = 0;
        for(int i = 2; i < 4; i++) {
            res_2 = res_2 | nibbles[i];
            if(i < 4 - 1) // dont shift last nibble
            {
                res_2 <<= 4;
            }
        }
        reg[nibbles[1]] = res_2;
        break;

    case 7:
        //7XNN: Add NN to register X (in 7XNN) Does not set the carry flag if the register overflows
        res_2 = 0;
        for(int i = 2; i < 4; i++) {
            res_2 = res_2 | nibbles[i];
            if(i < 3)
            {
                res_2 <<= 4;
            }
        }
        reg[nibbles[1]] += res_2;
        break;

        //arithmetic operations that set VF
        //Form: 8XYN
        //The last nibble determines the operation on register contents X and Y
    case 8:
    {
        switch(nibbles[3])
        {
        //8XY0: Set VX to the VALUE of VY
        case 0:
        {
            reg[nibbles[1]] = reg[nibbles[2]];
        }break;
            //8XY1: Set VX to Value of VX OR'd with value of VY
            //VY is not affected
        case 1:
        {
            reg[nibbles[1]] = reg[nibbles[1]] | reg[nibbles[2]];
            //if(mode == modes::ORIGCHIP)
            reg[NUM_OF_REGS - 1] = 0;
        }break;
            //8XY2: Set VX to Value of VX AND'd with value of VY
            //VY is not affected
        case 2:
        {
            reg[nibbles[1]] = reg[nibbles[1]] & reg[nibbles[2]];
            //if(mode == modes::ORIGCHIP)
            reg[NUM_OF_REGS - 1] = 0;
        }break;
            //8XY3: Set VX to Value of VX XOR'd with value of VY
            //VY is not affected
        case 3:
        {
            reg[nibbles[1]] = reg[nibbles[1]] ^ reg[nibbles[2]];
            //if(mode == modes::ORIGCHIP)
            reg[NUM_OF_REGS - 1] = 0;
        }break;
            /*8XY4: Add VY-value to VX-value
            If result overflows a value of 255, VF is set to 1, otherwise to 0*/
        case 4:
        {
            bool overflow = false;
            unsigned int param1 = reg[nibbles[1]];
            unsigned int param2 = reg[nibbles[2]];
            unsigned int res = param1 + param2;
            unsigned char test = res;
            if (res != test) // test whether overflow happened
            {
                overflow = true;
            }
            reg[nibbles[1]] = res;
            if(overflow)
            {
                reg[NUM_OF_REGS-1] = 1;
            }
            else{
                reg[NUM_OF_REGS-1] = 0;
            }
        }break;
            //8XY5: Subtract VY from VX and put it in VX, with underflow in VF
        case 5:
        {
            bool underflow = false;
            uint8_t param1 = reg[nibbles[1]];
            uint8_t param2 = reg[nibbles[2]];
            uint32_t res = param1 - param2;
            if (param2 > param1) // test whether underflow happened
            {
                underflow = true;
            }
            reg[nibbles[1]] = res;
            //Note: This is purposely inverted from addition
            if(underflow)
            {
                reg[NUM_OF_REGS-1] = 0;
            }
            else{
                reg[NUM_OF_REGS-1] = 1;
            }
        }break;
            //8XY7: Subtract VX from VY and put it in VX, with underflow in VF
        case 7:
        {
            bool underflow = false;
            uint8_t param1 = reg[nibbles[1]];
            uint8_t param2 = reg[nibbles[2]];
            uint32_t res = param2 - param1;
            if (param1 > param2) // test whether underflow happened
            {
                underflow = true;
            }
            reg[nibbles[1]] = res;
            //Note: This is purposely inverted from addition
            if(underflow)
            {
                reg[NUM_OF_REGS-1] = 0;
            }
            else{
                reg[NUM_OF_REGS-1] = 1;
            }
        }break;
            //8XY6: Shift VX right
            //If in OrigChip mode, first copy VY in VX, otherwise skip that step
        case 6:
        {
            //if(mode == modes::ORIGCHIP)
            reg[nibbles[1]] = reg[nibbles[2]];
            bool bitshiftedout = false;
            //If shifted out bit is 1, set Overflow register
            if(reg[nibbles[1]] & 1)
            {
                bitshiftedout = true;
            }
            reg[nibbles[1]] >>= 1;
            if(bitshiftedout)
            {
                reg[NUM_OF_REGS-1] = 1;
            }
            else
            {
                reg[NUM_OF_REGS-1] = 0;
            }
        }break;
            //8XYE: Shift VX right
            //If in OrigChip mode, first copy VY in VX, otherwise skip that step
        case 0xE:
        {
            //if(mode == modes::ORIGCHIP)
            reg[nibbles[1]] = reg[nibbles[2]];
            //If shifted out(to the left) bit is 1, set Overflow register to 1, otherwise 0
            bool bitshiftedout = false;
            if(reg[nibbles[1]]>>7 & 1)
            {
                bitshiftedout = true;
            }
            reg[nibbles[1]] <<= 1;
            if(bitshiftedout)
            {
                reg[NUM_OF_REGS-1] = 1;
            }
            else
            {
                reg[NUM_OF_REGS-1] = 0;
            }
        }break;
        }


    }break;

    case 9:
        //9XY0: skips an instruction if VX and VY are NOT equal
        if(reg[nibbles[1]] != reg[nibbles[2]]) program_counter += 2;
        break;

    case 0xA:
        //ANNN : Sets the index register I to value NNN
        res_1 = 0;
        for(int i = 1; i < 4; i++) {
            res_1 = res_1 | (nibbles[i] & 15);
            if(i < 3)
            {
                res_1 <<= 4;
            }
        }
        Ireg = res_1;
        break;

        /*Jump instruction. Ambiguous*/
    case 0xB:
    {
        //BNNN: original behaviour: jump to NNN, and add the value of V0
        //if(mode == modes::ORIGCHIP)
        //Get value of NNN
        res_1 = 0;
        for(int i = 1; i < 4; i++) {
            res_1 = res_1 | ((short)nibbles[i] & 15);
            if(i < 3)
            {
                res_1 <<= 4;
            }
        }
        program_counter = res_1 + reg[0];
        program_counter-=2; //Decrement because PC is incremented after execute command
    }break;

        /*CXNN: Generate random number, binary AND it with NN and put it in VX*/
    case 0xC:
    {
        int rn = rand();
        res_2 = 0;
        for(int i = 2; i < 4; i++) {
            res_2 = res_2 | nibbles[i];
            if(i < 3)
            {
                res_2 <<= 4;
            }
        }
        reg[nibbles[1]] = rn & res_2;
    }break;
    case 0xD:
        //DXYN: Display/draw N pixel tall sprite from Ireg at (X,Y) given by VX and VY-value stores
        //x = reg[nibbles[1]];
        //y = reg[nibbles[2]];
        //n = nibbles[3];
        //construct bitset for draw:
        if(Ireg >= MAXRAM - nibbles[3]) {
            dump_emu_core(std::cerr);
            throw std::runtime_error("DXYN: Ireg is too large for the sprite dimensions!");
        }
        if(nibbles[3] == 0) {
            dump_emu_core(std::cerr);
            throw std::runtime_error("DXYN: N is 0! Pointless or faulty draw instruction!");
        }
        spriteset = 0;
        for(int i = 0; i < nibbles[3]; i++) {
            spriteset = spriteset | std::bitset<120>(ram[Ireg + i] & 255);
            if(i < nibbles[3] - 1) //To not shift too far
            {
                spriteset <<= 8;
            }
        }
        spriteset <<= 8 * (14-(nibbles[3]-1)); //all sprites are 15 bytes long, so shift 14 bytes max
        if(!dsp.draw(spriteset, reg[nibbles[1]], reg[nibbles[2]]))
        {
            return false;
        }
        break;

    case 0xE:
    {

        //EX9E: if the key corresponding to value in VX is pressed, skip the next instruction
        if(nibbles[2] == 0x9 && nibbles[3] == 0xE)
        {
            SDL_Keycode keytocheck = c8tosdlkeys[static_cast<keys>(reg[nibbles[1]])];
            auto it = std::find(keypressed.begin(), keypressed.end(), keytocheck);
            if(it != keypressed.end())
            {
                program_counter += 2;
            }
        }

        //EXA1: if the key corresponding to value in VX is NOT pressed, skip the next instruction
        if(nibbles[2] == 0xA && nibbles[3] == 0x1)
        {
            SDL_Keycode keytocheck = c8tosdlkeys[static_cast<keys>(reg[nibbles[1]])];
            auto it = std::find(keypressed.begin(), keypressed.end(), keytocheck);
            if(it == keypressed.end())
            {
                program_counter += 2;
            }
        }
    }

    case 0xF:
    {
        //FX07: Sets VX to current delay timer value
        if(nibbles[2] == 0 && nibbles[3] == 7)
        {
            reg[nibbles[1]] = delaytimer.getTimerval();
        }
        //FX15: Sets delay timer value to VX
        if(nibbles[2] == 1 && nibbles[3] == 5)
        {
            delaytimer.setTimerval(reg[nibbles[1]]);
        }
        //FX18: Sets sound timer value to VX
        if(nibbles[2] == 1 && nibbles[3] == 8)
        {
            soundtimer.setTimerval(reg[nibbles[1]]);
        }

        //FX0A: Get Key: decrements PC unless a key is pressed and released: if it is, put that keys hex value in VX
        if(nibbles[2] == 0x0 && nibbles[3] == 0xA)
        {
            static bool waspressed = false;
            static SDL_Keycode currpress = 0;
            if(!keypressed.empty())
            {
                waspressed = true;
                currpress = keypressed.at(0);
            }
            if(waspressed && keypressed.empty())
            {
                waspressed = false;
                reg[nibbles[1]] = static_cast<unsigned char>(sdltoc8keys[currpress]);
                currpress = 0;
            }
            else
            {
                program_counter -= 2; //go back one instruction
            }
        }

        //FX1E: Add VX value to Ireg
        if(nibbles[2] == 1 && nibbles[3] == 0xE)
        {
            bool overflow = false;
            short temp = Ireg;
            Ireg += reg[nibbles[1]];
            if(Ireg < temp) overflow = true;
        }
        //FX29: Store last nibble of VX value in Ireg
        if(nibbles[2] == 2 && nibbles[3] == 9)
        {
            unsigned char lookupchar = reg[nibbles[1]]; // This is original behaviour, but it shouldn't hurt to use the whole char...
            Ireg = givecharaddress(lookupchar);
        }
        //FX33: Decimal conversion of number in VX, stored at the three places indicated by Ireg
        if(nibbles[2] == 3 && nibbles[3] == 3)
        {
            unsigned char res = reg[nibbles[1]];
            //Fill ram with three decimal values representing res starting at Ireg position
            for(size_t i = 0; i < 3 ; ++i)
            {
                unsigned char temp = res % 10;
                ram[Ireg + 2 - i] = temp;
                res /= 10;
            }
        }
        //FX55: Store memory of V0 to VX inclusive in memory addresses starting with the one in I
        if(nibbles[2] == 5 && nibbles[3] == 5)
        {
            //Get X
            unsigned short numbofregs = nibbles[1];
            //Store memory of V0 to VX inclusive in memory addresses starting with the one in I
            for(std::size_t i = 0; i <= numbofregs; i++)
            {
                ram[Ireg + i] = reg[i];
            }
            //if(mode == modes::ORIGCHIP)
            Ireg = Ireg + numbofregs + 1;
        }
        //FX65: Load memory into V0 to VX inclusive from memory addresses starting with the one in I
        if(nibbles[2] == 6 && nibbles[3] == 5)
        {
            //Get X
            unsigned short numbofregs = nibbles[1];
            //Store memory of V0 to VX inclusive in memory addresses starting with the one in I
            for(std::size_t i = 0; i <= numbofregs; i++)
            {
                reg[i] = ram[Ireg + i];
            }
            //if(mode == modes::ORIGCHIP)
            Ireg = Ireg + numbofregs + 1;
        }
    }break;

    default:
        dump_emu_core(std::cerr);
        std::cerr << "Invalid Nibble: " << (unsigned int)nibbles[0] << std::endl;
        throw std::runtime_error("execute: First nibble of operation is invalid!");
        break;
    }
    return true;
}

/*Insert sprites for 0 to F in the area from spritestart to spriteend - 5 bytes each
Usually, sprites are saved from 0x50 to 0x9F, with 5 bytes each, if every sprite has 15 bits, they stop at 0x140
All regular sprites are 15 bytes maximum, so we pad the font sprites to 15 as well
Ideally, this should be read from the config file instad of being hardcoded here*/
void SC8Emulator::initsprites() {
    const size_t SPRITESTART = 0x50;
    const size_t SPRITEEND = 0x140;
    size_t curr = SPRITESTART;
    //0 :
    ram.at(curr) = 0xF0;
    ram.at(curr+1) = 0x90;
    ram.at(curr+2) = 0x90;
    ram.at(curr+3) = 0x90;
    ram.at(curr+4) = 0xF0;
    curr += 15; //go to next sprite slot
    //1:
    ram.at(curr) = 0x20;
    ram.at(curr+1) = 0x60;
    ram.at(curr+2) = 0x20;
    ram.at(curr+3) = 0x20;
    ram.at(curr+4) = 0x70;
    curr += 15; //go to next sprite slot
    //2:
    ram.at(curr) = 0xF0;
    ram.at(curr+1) = 0x10;
    ram.at(curr+2) = 0xF0;
    ram.at(curr+3) = 0x80;
    ram.at(curr+4) = 0xF0;
    curr += 15; //go to next sprite slot
    //3:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x10;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x10;
    ram[curr+4] = 0xF0;
    curr += 15; //go to next sprite slot
    //4:
    ram[curr] = 0x90;
    ram[curr+1] = 0x90;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x10;
    ram[curr+4] = 0x10;
    curr += 15; //go to next sprite slot
    //5:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x80;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x10;
    ram[curr+4] = 0xF0;
    curr += 15; //go to next sprite slot
    //6:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x80;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x90;
    ram[curr+4] = 0xF0;
    curr += 15; //go to next sprite slot
    //7:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x10;
    ram[curr+2] = 0x20;
    ram[curr+3] = 0x40;
    ram[curr+4] = 0x40;
    curr += 15; //go to next sprite slot
    //8:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x90;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x90;
    ram[curr+4] = 0xF0;
    curr += 15; //go to next sprite slot
    //9:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x90;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x10;
    ram[curr+4] = 0xF0;
    curr += 15; //go to next sprite slot
    //A:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x90;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x90;
    ram[curr+4] = 0x90;
    curr += 15; //go to next sprite slot
    //B:
    ram[curr] = 0xE0;
    ram[curr+1] = 0x90;
    ram[curr+2] = 0xE0;
    ram[curr+3] = 0x90;
    ram[curr+4] = 0xE0;
    curr += 15; //go to next sprite slot
    //C:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x80;
    ram[curr+2] = 0x80;
    ram[curr+3] = 0x80;
    ram[curr+4] = 0xF0;
    curr += 15; //go to next sprite slot
    //D:
    ram[curr] = 0xE0;
    ram[curr+1] = 0x90;
    ram[curr+2] = 0x90;
    ram[curr+3] = 0x90;
    ram[curr+4] = 0xE0;
    curr += 15; //go to next sprite slot
    //E:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x80;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x80;
    ram[curr+4] = 0xF0;
    curr += 15; //go to next sprite slot
    //F:
    ram[curr] = 0xF0;
    ram[curr+1] = 0x80;
    ram[curr+2] = 0xF0;
    ram[curr+3] = 0x80;
    ram[curr+4] = 0x80;
    if(curr > SPRITEEND) throw std::runtime_error("Sprite memory exceeded during allocation!");
    curr += 15; //go to next sprite slot
}

std::vector<uint8_t> SC8Emulator::GetCurrentRegisters() const
{
    std::vector<uint8_t> resvec{};
    for(auto &regval : reg)
    {
        resvec.push_back(regval);
    }
    return resvec;
}

uint16_t SC8Emulator::GetCurrentPC() const
{
    return program_counter;
}
uint16_t SC8Emulator::GetCurrentIReg() const
{
    return Ireg;
}
