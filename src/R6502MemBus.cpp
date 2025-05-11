#include <R6502MemBus.hpp>

R6502MemBus::R6502MemBus() : memory(2048, 0) {}

R6502MemBus::~R6502MemBus() {}

uint8_t R6502MemBus::read(uint32_t addr)
{
    if (addr >= 0xFF00 && addr <= 0xFFFF)   // remap addresses from 0xFF00 - 0xFFFF = 0x400 - 0x4FF
    {
        addr -= 0xFB00;
    }

    return memory.read(addr);
}

void R6502MemBus::write(uint32_t addr, uint8_t data)
{
    if (addr >= 0xFF00 && addr <= 0xFFFF)   // remap addresses from 0xFF00 - 0xFFFF = 0x400 - 0x4FF
    {
        addr -= 0xFB00;
    }
    
    memory.write(addr, data);
}