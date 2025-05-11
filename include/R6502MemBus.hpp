#ifndef INCLUDED_R6502_MEM_BUS_HPP
#define INCLUDED_R6502_MEM_BUS_HPP

#include <R6502Bus.hpp>

class R6502MemBus : public R6502Bus
{
public:
    R6502Mem memory;

    R6502MemBus();
    ~R6502MemBus();

    uint8_t read(uint32_t addr) override;
    void write(uint32_t addr, uint8_t data) override;
};

#endif // INCLUDED_R6502_MEM_BUS_HPP