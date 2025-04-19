#include <R6502Bus.hpp>

R6502Bus::R6502Bus() : memory(512) {}

uint8_t R6502Bus::read(uint32_t addr)
{
    return memory.read(addr);
}

void R6502Bus::write(uint32_t addr, uint8_t data)
{
    memory.write(addr, data);
}