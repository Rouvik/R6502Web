#ifndef INCLUDED_R6502_MEM_HPP
#define INCLUDED_R6502_MEM_HPP

#include <iostream>
#include <iomanip>
#include <cstdint>

#include <helper.hpp>

class R6502Mem
{
public:
    static const uint32_t MEM_RANGE = 8;
    uint8_t *memory;
    uint32_t size;
    int offset;

    R6502Mem(int capacity, int offset = 0);
    ~R6502Mem();

    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t data);

    void renderMemory(uint32_t sizeToRender = 0);
};

#endif // INCLUDED_R6502_MEM_HPP