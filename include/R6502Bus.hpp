#ifndef INCLUDED_R6502_BUS_HPP
#define INCLUDED_R6502_BUS_HPP

#include <R6502Mem.hpp>

class R6502Bus
{
public:
    R6502Mem memory{256};

    R6502Bus() = default;
    ~R6502Bus() = default;

    uint8_t read(uint32_t addr);
    void write(uint32_t addr, uint8_t data);
};

#endif // INCLUDED_R6502_BUS_HPP