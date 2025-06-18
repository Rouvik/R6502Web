#include <iostream>
#include <bitset>
#include <string>
#include <fstream>

#include <helper.hpp>

#include <R6502Mem.hpp>
#include <R6502Bus.hpp>
#include <R6502.hpp>

void loadHexProgram(std::string fileName, R6502Mem *memory, uint32_t writeOffset)
{
    if (memory == nullptr)
    {
        ERROR(loadHexProgram, "FATAL: memory cannot be null");
        exit(1);
    }

    if (writeOffset > memory->size)
    {
        ERROR(loadHexProgram, "Write offset cannot be larger than memory size: " << memory->size << " offset: " << writeOffset);
        exit(1);
    }

    std::ifstream in(fileName);
    if (!in)
    {
        ERROR(loadHexProgram, "Failed to load file: " << fileName);
        exit(1);
    }

    while (!in.eof())
    {
        std::string byte;
        in >> byte;
        memory->memory[writeOffset++] = std::stoi(byte, 0, 16);
    }

    in.close();
}

void loadBinProgram(std::string fileName, R6502Mem *memory, uint32_t writeOffset)
{
    if (memory == nullptr)
    {
        ERROR(loadBinProgram, "FATAL: memory cannot be null");
        exit(1);
    }

    if (writeOffset > memory->size)
    {
        ERROR(loadBinProgram, "Write offset cannot be larger than memory size: " << memory->size << " offset: " << writeOffset);
        exit(1);
    }

    std::ifstream in(fileName, std::ios::binary);
    if (!in)
    {
        ERROR(loadBinProgram, "Failed to load file: " << fileName);
        exit(1);
    }

    while (!in.eof())
    {
        in >> memory->memory[writeOffset];
        writeOffset++;
    }

    in.close();
}

// debug Bus memory to run simple programs
class R6502MemBus : public R6502Bus
{
public:
    R6502Mem memory{0xFFFF, 0};

    uint8_t read(uint32_t address)
    {
        return memory.read(address);
    }

    void write(uint32_t address, uint8_t data)
    {
        memory.write(address, data);
    }
};

int main()
{
    R6502 cpu{std::make_unique<R6502MemBus>()};

    R6502MemBus *bus = static_cast<R6502MemBus *>(cpu.getBus());
    R6502Mem *memory = &(bus->memory);

    loadHexProgram("./programs/interrupt.hex", memory, 0x600);

    bus->write(0xFFFA, 0x11);
    bus->write(0xFFFB, 0x06);
    bus->write(0xFFFE, 0x0A);
    bus->write(0xFFFF, 0x06);

    R6502::IP = 0x600;

    LOG(ProgramMemory, "Before start");
    memory->renderMemory();

    std::cout << "Auto run?(y/n): ";
    char ch1 = 0;
    std::cin >> ch1;
    bool autoRun = ch1 == 'y';

    char ch = 0;
    do
    {
        std::cout << std::dec << "\e[H\e[2JInstruction: " << R6502::Instructions[R6502::instr].op << " IP: " << R6502::IP << '\n';
        cpu.clock();
        std::cout << "CPU STATE:: X: " << static_cast<int>(R6502::reg_X) << " Y: " << static_cast<int>(R6502::reg_Y) << " Acc: " << static_cast<int>(R6502::reg_Acc) << " Status: " << std::bitset<8>(R6502::reg_Status) << '\n';
        memory->renderMemory(512);
        std::cout << "\n> ";

        if (autoRun)
        {
            ch = 's';
        }
        else
        {
            std::cin >> ch;
        }

        if (ch == 'i')
        {
            R6502::IRQ(); // raise an interrupt
        }
        else if (ch == 'n')
        {
            R6502::NMI();
        }

    } while (ch != 'q');

    return 0;
}