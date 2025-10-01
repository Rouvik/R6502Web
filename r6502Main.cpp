// #include <iostream>
// #include <bitset>
// #include <string>
// #include <fstream>

#include <helper.hpp>

#include <R6502Mem.hpp>
#include <R6502Bus.hpp>
#include <R6502.hpp>

#include <emscripten/bind.h>

/*
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
*/

// debug Bus memory to run simple programs
class R6502MemBus : public R6502Bus
{
public:
    R6502Mem memory{0x10000, 0};

    uint8_t read(uint32_t address)
    {
        return memory.read(address);
    }

    void write(uint32_t address, uint8_t data)
    {
        memory.write(address, data);
    }
};

uintptr_t initialiseCPU()
{
    R6502 *cpu = new R6502(std::make_unique<R6502MemBus>());
    return (uintptr_t) cpu;
}

void freeCPU(uintptr_t cpu_ptr)
{
    delete reinterpret_cast<R6502 *>(cpu_ptr);
}

uintptr_t getMemoryPtr(uintptr_t cpu_ptr)
{
    return (uintptr_t) (reinterpret_cast<R6502MemBus *>(reinterpret_cast<R6502 *>(cpu_ptr)->getBus())->memory.memory);
}

void clockCPUFromJS(uintptr_t cpu_ptr)
{
    reinterpret_cast<R6502 *>(cpu_ptr)->clock();
}

void resetCPUFromJS(uintptr_t cpu_ptr)
{
    reinterpret_cast<R6502 *>(cpu_ptr)->reset();
}

void callNMIFromJS(uintptr_t cpu_ptr)
{
    reinterpret_cast<R6502 *>(cpu_ptr)->NMI();
}

void callIRQFromJS(uintptr_t cpu_ptr)
{
    reinterpret_cast<R6502 *>(cpu_ptr)->IRQ();
}

std::string getInstructionName(int instruction)
{
    return R6502::Instructions[instruction].op;
}

EMSCRIPTEN_BINDINGS(R6502_module)
{
    emscripten::function("initialiseCPU", &initialiseCPU);
    emscripten::function("freeCPU", &freeCPU);
    emscripten::function("getMemoryPtr", &getMemoryPtr);
    emscripten::function("clockCPUFromJS", &clockCPUFromJS);
    emscripten::function("resetCPUFromJS", &resetCPUFromJS);
    emscripten::function("callIRQFromJS", &callIRQFromJS);
    emscripten::function("callNMIFromJS", &callNMIFromJS);
    emscripten::function("getInstructionName", &getInstructionName);

    emscripten::class_<R6502>("R6502")
    .class_property("total_cycles", &R6502::total_cycles)
    .class_property("ticks", &R6502::ticks)
    .class_property("IP", &R6502::IP)
    .class_property("instr", &R6502::instr)
    .class_property("imm", &R6502::imm)
    .class_property("addr", &R6502::addr)
    .class_property("accumulator", &R6502::accumulator)
    .class_property("indirect", &R6502::indirect)
    .class_property("zeroIndirect", &R6502::zeroIndirect)
    .class_property("running", &R6502::running)
    .class_property("reg_Stack", &R6502::reg_Stack)
    .class_property("reg_X", &R6502::reg_X)
    .class_property("reg_Y", &R6502::reg_Y)
    .class_property("reg_Acc", &R6502::reg_Acc)
    .class_property("reg_Status", &R6502::reg_Status);
}

/*
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
*/