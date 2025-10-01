#ifndef INCLUDED_R6502_BUS_HPP
#define INCLUDED_R6502_BUS_HPP

#include <R6502Mem.hpp>

class R6502Bus
{
public:
    R6502Bus() = default;
    virtual ~R6502Bus() = default;

    virtual uint8_t read(uint32_t addr) = 0;
    virtual void write(uint32_t addr, uint8_t data) = 0;
};

#endif // INCLUDED_R6502_BUS_HPP