// #include <iostream>
// #include <bitset>
// #include <string>
// #include <fstream>

#include <helper.hpp>

#include <R6502Mem.hpp>
#include <R6502Bus.hpp>
#include <R6502.hpp>

#include <emscripten.h>
#include <emscripten/bind.h>

EM_JS(void, notifyMemoryUpdate, (uint32_t address, uint8_t data), {
    Module.memContainer.updateSpecificViewElement(address, data);
});

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
        notifyMemoryUpdate(address, data);
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