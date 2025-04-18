#include <iostream>
#include <bitset>

#include <helper.hpp>

#include <R6502Mem.hpp>
#include <R6502Bus.hpp>
#include <R6502.hpp>

int main()
{    
    R6502 cpu{};

    R6502Mem *memory = &R6502::bus.memory;

    // load the simple program
    // LDX #$32
    // STX $00
    // memory->memory[0] = 0xA2;
    // memory->memory[1] = 0x32;
    // memory->memory[2] = 0x86;
    // memory->memory[3] = 0x32;
    // memory->memory[4] = 0xA2;
    // memory->memory[5] = 0x31;
    // memory->memory[6] = 0xB4;
    // memory->memory[7] = 0x01;

    memory->memory[0] = 0xA9;
    memory->memory[1] = 0x32;
    memory->memory[2] = 0x4A;

    LOG(ProgramMemory, "Before start");
    memory->renderMemory();

    int i = 0;
    while (i++ < 4)
    {
        cpu.clock();
        std::cout << "CPU STATE:: X: " << std::dec << static_cast<int>(R6502::reg_X) << " Y: " << static_cast<int>(R6502::reg_Y) << " Acc: " << static_cast<int>(R6502::reg_Acc) << " Status: " << std::bitset<8>(R6502::reg_Status) << '\n';
        LOG(MEMORY_RENDER, "Step: " << i);
        memory->renderMemory(32);
        std::cout << '\n';
    }
    
    return 0;
}