#include <R6502Mem.hpp>

R6502Mem::R6502Mem(int capacity, int offset)
{
    memory = new uint8_t[capacity]{0};
    size = capacity;
    this->offset = offset;
}

R6502Mem::~R6502Mem()
{
    delete[] memory;
    size = 0;
}

uint8_t R6502Mem::read(uint32_t addr)
{
    uint32_t caddr = addr - offset;
    if (caddr > size)
    {
        ERROR(R6502Mem_read, "Maliformed address: " << addr << " Offset: " << offset);
        return UINT8_MAX;
    }
    
    return memory[caddr];
}

void R6502Mem::write(uint32_t addr, uint8_t data)
{
    uint32_t caddr = addr - offset;
    if (caddr > size)
    {
        ERROR(R6502Mem_write, "Maliformed address: " << addr << " Offset: " << offset);
        return;
    }
    
    memory[caddr] = data;
}

void R6502Mem::renderMemory(uint32_t sizeToRender)
{
    if (sizeToRender == 0)
    {
        sizeToRender = size;
    }
    
    for (uint32_t i = 0; i < sizeToRender / R6502Mem::MEM_RANGE; i++)
    {
        std::cout << std::setw(3) << std::hex << i * R6502Mem::MEM_RANGE << ": ";
        for (uint32_t j = 0; j < R6502Mem::MEM_RANGE; j++)
        {
            std::cout << std::setw(2) << std::hex << static_cast<uint32_t>(memory[R6502Mem::MEM_RANGE * i + j]) << ' ';
        }

        std::cout << '\n';
    }
}