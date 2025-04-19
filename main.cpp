#include <iostream>
#include <thread>
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
    
    // memory->memory[0] = 12;
    // memory->memory[1] = 0xA9;
    // memory->memory[2] = 0x32;
    // memory->memory[3] = 0x6D;
    // memory->memory[4] = 0x00;
    // memory->memory[5] = 0x00;
    // memory->memory[6] = 0x85;
    // memory->memory[7] = 0x00;
    // R6502::IP = 1;
    
    // memory->memory[0] = 0xA2;
    // memory->memory[1] = 0x00;
    // memory->memory[2] = 0xA0;
    // memory->memory[3] = 0x32;
    // memory->memory[4] = 0x94;
    // memory->memory[5] = 0x20;
    // memory->memory[6] = 0xE8;
    // memory->memory[7] = 0x4C;
    // memory->memory[8] = 0x04;
    // memory->memory[9] = 0x00;

    memory->memory[0] = 0xA2;
    memory->memory[1] = 0x00;
    memory->memory[2] = 0xA0;
    memory->memory[3] = 0x32;
    memory->memory[4] = 0x94;
    memory->memory[5] = 0x20;
    memory->memory[6] = 0xE8;
    memory->memory[7] = 0x6C;
    memory->memory[8] = 0x0A;
    memory->memory[9] = 0x00;
    
    memory->memory[10] = 0x04;

    LOG(ProgramMemory, "Before start");
    memory->renderMemory();
    
    std::cout << "Auto run?(y/n): ";
    char ch1 = 0;
    std::cin >> ch1;
    bool autoRun = ch1 == 'y';

    int i = 0;
    char ch = 0;
    do
    {
        std::cout << std::dec << "\e[H\e[2JInstruction: " << R6502::Instructions[R6502::instr].op  << " IP: " << R6502::IP << '\n';
        cpu.clock();
        std::cout << "CPU STATE:: X: " << static_cast<int>(R6502::reg_X) << " Y: " << static_cast<int>(R6502::reg_Y) << " Acc: " << static_cast<int>(R6502::reg_Acc) << " Status: " << std::bitset<8>(R6502::reg_Status) << '\n';
        LOG(MEMORY_RENDER, "Step: " << i);
        memory->renderMemory();
        std::cout << "\n> ";

        if (autoRun)
        {
            ch = 's';
        }
        else
        {
            std::cin >> ch;
        }
    } while (ch != 'q' && i < 4);
    
    return 0;
}