#include <R6502.hpp>

R6502::R6502() {}

void R6502::setBus(R6502Bus bus)
{
    R6502::bus = bus;
}

R6502Bus R6502::bus;
int R6502::ticks = 0;
uint32_t R6502::IP = 0;
uint8_t R6502::reg_Acc = 0;
uint8_t R6502::reg_X = 0;
uint8_t R6502::reg_Y = 0;
uint8_t R6502::reg_Status = 0;
uint16_t R6502::reg_Stack = 0x1FF;
uint8_t R6502::imm = 0;
uint16_t R6502::addr = 0;
bool R6502::accumulator = false;
bool R6502::indirect = false;
bool R6502::zeroIndirect = false;

std::vector<Instruction_t> R6502::Instructions =
    {
        (Instruction_t){"BRK", R6502::IMP, R6502::BRK, 0},
        (Instruction_t){"ORA", R6502::INX, R6502::ORA, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"TSB", R6502::ZP0, R6502::TSB, 0},
        (Instruction_t){"ORA", R6502::ZP0, R6502::ORA, 0},
        (Instruction_t){"ASL", R6502::ZP0, R6502::ASL, 0},
        (Instruction_t){"RMB0", R6502::ZP0, R6502::RMB0, 0},
        (Instruction_t){"PHP", R6502::IMP, R6502::PHP, 0},
        (Instruction_t){"ORA", R6502::IMM, R6502::ORA, 0},
        (Instruction_t){"ASL", R6502::ACC, R6502::ASL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"TSB", R6502::ABS, R6502::TSB, 0},
        (Instruction_t){"ORA", R6502::ABS, R6502::ORA, 0},
        (Instruction_t){"ASL", R6502::ABS, R6502::ASL, 0},
        (Instruction_t){"BBR0", R6502::REL, R6502::BBR0, 0},
        (Instruction_t){"BPL", R6502::REL, R6502::BPL, 0},
        (Instruction_t){"ORA", R6502::INY, R6502::ORA, 0},
        (Instruction_t){"ORA", R6502::ZPI, R6502::ORA, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"TRB", R6502::ZP0, R6502::TRB, 0},
        (Instruction_t){"ORA", R6502::ZPX, R6502::ORA, 0},
        (Instruction_t){"ASL", R6502::ZPX, R6502::ASL, 0},
        (Instruction_t){"RMB1", R6502::ZP0, R6502::RMB1, 0},
        (Instruction_t){"CLC", R6502::IMP, R6502::CLC, 0},
        (Instruction_t){"ORA", R6502::ABY, R6502::ORA, 0},
        (Instruction_t){"INC", R6502::IMP, R6502::INC, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"TRB", R6502::ABS, R6502::TRB, 0},
        (Instruction_t){"ORA", R6502::ABX, R6502::ORA, 0},
        (Instruction_t){"ASL", R6502::ABX, R6502::ASL, 0},
        (Instruction_t){"BBR1", R6502::REL, R6502::BBR1, 0},
        (Instruction_t){"JSR", R6502::ABS, R6502::JSR, 0},
        (Instruction_t){"AND", R6502::INX, R6502::AND, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"BIT", R6502::ZP0, R6502::BIT, 0},
        (Instruction_t){"AND", R6502::ZP0, R6502::AND, 0},
        (Instruction_t){"ROL", R6502::ZP0, R6502::ROL, 0},
        (Instruction_t){"RMB2", R6502::ZP0, R6502::RMB2, 0},
        (Instruction_t){"PLP", R6502::IMP, R6502::PLP, 0},
        (Instruction_t){"AND", R6502::IMM, R6502::AND, 0},
        (Instruction_t){"ROL", R6502::IMP, R6502::ROL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"BIT", R6502::ABS, R6502::BIT, 0},
        (Instruction_t){"AND", R6502::ABS, R6502::AND, 0},
        (Instruction_t){"ROL", R6502::ABS, R6502::ROL, 0},
        (Instruction_t){"BBR2", R6502::REL, R6502::BBR2, 0},
        (Instruction_t){"BMI", R6502::REL, R6502::BMI, 0},
        (Instruction_t){"AND", R6502::INY, R6502::AND, 0},
        (Instruction_t){"AND", R6502::ZPI, R6502::AND, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"BIT", R6502::ZPX, R6502::BIT, 0},
        (Instruction_t){"AND", R6502::ZPX, R6502::AND, 0},
        (Instruction_t){"ROL", R6502::ZPX, R6502::ROL, 0},
        (Instruction_t){"RMB3", R6502::ZP0, R6502::RMB3, 0},
        (Instruction_t){"SEC", R6502::IMP, R6502::SEC, 0},
        (Instruction_t){"AND", R6502::ABY, R6502::AND, 0},
        (Instruction_t){"DEC", R6502::IMP, R6502::DEC, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"BIT", R6502::ABX, R6502::BIT, 0},
        (Instruction_t){"AND", R6502::ABX, R6502::AND, 0},
        (Instruction_t){"ROL", R6502::ABX, R6502::ROL, 0},
        (Instruction_t){"BBR3", R6502::REL, R6502::BBR3, 0},
        (Instruction_t){"RTI", R6502::IMP, R6502::RTI, 0},
        (Instruction_t){"EOR", R6502::INX, R6502::EOR, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"EOR", R6502::ZP0, R6502::EOR, 0},
        (Instruction_t){"LSR", R6502::ZP0, R6502::LSR, 0},
        (Instruction_t){"RMB4", R6502::ZP0, R6502::RMB4, 0},
        (Instruction_t){"PHA", R6502::IMP, R6502::PHA, 0},
        (Instruction_t){"EOR", R6502::IMM, R6502::EOR, 0},
        (Instruction_t){"LSR", R6502::ACC, R6502::LSR, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"JMP", R6502::ABS, R6502::JMP, 0},
        (Instruction_t){"EOR", R6502::ABS, R6502::EOR, 0},
        (Instruction_t){"LSR", R6502::ABS, R6502::LSR, 0},
        (Instruction_t){"BBR4", R6502::REL, R6502::BBR4, 0},
        (Instruction_t){"BVC", R6502::REL, R6502::BVC, 0},
        (Instruction_t){"EOR", R6502::INY, R6502::EOR, 0},
        (Instruction_t){"EOR", R6502::ZPI, R6502::EOR, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"EOR", R6502::ZPX, R6502::EOR, 0},
        (Instruction_t){"LSR", R6502::ZPX, R6502::LSR, 0},
        (Instruction_t){"RMB5", R6502::ZP0, R6502::RMB5, 0},
        (Instruction_t){"CLI", R6502::IMP, R6502::CLI, 0},
        (Instruction_t){"EOR", R6502::ABY, R6502::EOR, 0},
        (Instruction_t){"PHY", R6502::IMP, R6502::PHY, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"EOR", R6502::ABX, R6502::EOR, 0},
        (Instruction_t){"LSR", R6502::ABX, R6502::LSR, 0},
        (Instruction_t){"BBR5", R6502::REL, R6502::BBR5, 0},
        (Instruction_t){"RTS", R6502::IMP, R6502::RTS, 0},
        (Instruction_t){"ADC", R6502::INX, R6502::ADC, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"STZ", R6502::ZP0, R6502::STZ, 0},
        (Instruction_t){"ADC", R6502::ZP0, R6502::ADC, 0},
        (Instruction_t){"ROR", R6502::ZP0, R6502::ROR, 0},
        (Instruction_t){"RMB6", R6502::ZP0, R6502::RMB6, 0},
        (Instruction_t){"PLA", R6502::IMP, R6502::PLA, 0},
        (Instruction_t){"ADC", R6502::IMM, R6502::ADC, 0},
        (Instruction_t){"ROR", R6502::IMP, R6502::ROR, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"JMP", R6502::JIND, R6502::JMP, 0},
        (Instruction_t){"ADC", R6502::ABS, R6502::ADC, 0},
        (Instruction_t){"ROR", R6502::ABS, R6502::ROR, 0},
        (Instruction_t){"BBR6", R6502::REL, R6502::BBR6, 0},
        (Instruction_t){"BVS", R6502::REL, R6502::BVS, 0},
        (Instruction_t){"ADC", R6502::INY, R6502::ADC, 0},
        (Instruction_t){"ADC", R6502::ZPI, R6502::ADC, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"STZ", R6502::ZPX, R6502::STZ, 0},
        (Instruction_t){"ADC", R6502::ZPX, R6502::ADC, 0},
        (Instruction_t){"ROR", R6502::ZPX, R6502::ROR, 0},
        (Instruction_t){"RMB7", R6502::ZP0, R6502::RMB7, 0},
        (Instruction_t){"SEI", R6502::IMP, R6502::SEI, 0},
        (Instruction_t){"ADC", R6502::ABY, R6502::ADC, 0},
        (Instruction_t){"PLY", R6502::IMP, R6502::PLY, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"JMP", R6502::ABIX, R6502::JMP, 0},
        (Instruction_t){"ADC", R6502::ABX, R6502::ADC, 0},
        (Instruction_t){"ROR", R6502::ABX, R6502::ROR, 0},
        (Instruction_t){"BBR7", R6502::REL, R6502::BBR7, 0},
        (Instruction_t){"BRA", R6502::REL, R6502::BRA, 0},
        (Instruction_t){"STA", R6502::INX, R6502::STA, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"STY", R6502::ZP0, R6502::STY, 0},
        (Instruction_t){"STA", R6502::ZP0, R6502::STA, 0},
        (Instruction_t){"STX", R6502::ZP0, R6502::STX, 0},
        (Instruction_t){"SMB0", R6502::ZP0, R6502::SMB0, 0},
        (Instruction_t){"DEY", R6502::IMP, R6502::DEY, 0},
        (Instruction_t){"BIT", R6502::IMM, R6502::BIT, 0},
        (Instruction_t){"TXA", R6502::IMP, R6502::TXA, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"STY", R6502::ABS, R6502::STY, 0},
        (Instruction_t){"STA", R6502::ABS, R6502::STA, 0},
        (Instruction_t){"STX", R6502::ABS, R6502::STX, 0},
        (Instruction_t){"BBS0", R6502::REL, R6502::BBS0, 0},
        (Instruction_t){"BCC", R6502::REL, R6502::BCC, 0},
        (Instruction_t){"STA", R6502::INY, R6502::STA, 0},
        (Instruction_t){"STA", R6502::ZPI, R6502::STA, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"STY", R6502::ZPX, R6502::STY, 0},
        (Instruction_t){"STA", R6502::ZPX, R6502::STA, 0},
        (Instruction_t){"STX", R6502::ZPY, R6502::STX, 0},
        (Instruction_t){"SMB1", R6502::ZP0, R6502::SMB1, 0},
        (Instruction_t){"TYA", R6502::IMP, R6502::TYA, 0},
        (Instruction_t){"STA", R6502::ABY, R6502::STA, 0},
        (Instruction_t){"TXS", R6502::IMP, R6502::TXS, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"STZ", R6502::ABS, R6502::STZ, 0},
        (Instruction_t){"STA", R6502::ABX, R6502::STA, 0},
        (Instruction_t){"STZ", R6502::ABX, R6502::STZ, 0},
        (Instruction_t){"BBS1", R6502::REL, R6502::BBS1, 0},
        (Instruction_t){"LDY", R6502::IMM, R6502::LDY, 0},
        (Instruction_t){"LDA", R6502::INX, R6502::LDA, 0},
        (Instruction_t){"LDX", R6502::IMM, R6502::LDX, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"LDY", R6502::ZP0, R6502::LDY, 0},
        (Instruction_t){"LDA", R6502::ZP0, R6502::LDA, 0},
        (Instruction_t){"LDX", R6502::ZP0, R6502::LDX, 0},
        (Instruction_t){"SMB2", R6502::ZP0, R6502::SMB2, 0},
        (Instruction_t){"TAY", R6502::IMP, R6502::TAY, 0},
        (Instruction_t){"LDA", R6502::IMM, R6502::LDA, 0},
        (Instruction_t){"TAX", R6502::IMP, R6502::TAX, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"LDY", R6502::ABS, R6502::LDY, 0},
        (Instruction_t){"LDA", R6502::ABS, R6502::LDA, 0},
        (Instruction_t){"LDX", R6502::ABS, R6502::LDX, 0},
        (Instruction_t){"BBS2", R6502::REL, R6502::BBS2, 0},
        (Instruction_t){"BCS", R6502::REL, R6502::BCS, 0},
        (Instruction_t){"LDA", R6502::INY, R6502::LDA, 0},
        (Instruction_t){"LDA", R6502::ZPI, R6502::LDA, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"LDY", R6502::ZPX, R6502::LDY, 0},
        (Instruction_t){"LDA", R6502::ZPX, R6502::LDA, 0},
        (Instruction_t){"LDX", R6502::ZPY, R6502::LDX, 0},
        (Instruction_t){"SMB3", R6502::ZP0, R6502::SMB3, 0},
        (Instruction_t){"CLV", R6502::IMP, R6502::CLV, 0},
        (Instruction_t){"LDA", R6502::ABY, R6502::LDA, 0},
        (Instruction_t){"TSX", R6502::IMP, R6502::TSX, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"LDY", R6502::ABX, R6502::LDY, 0},
        (Instruction_t){"LDA", R6502::ABX, R6502::LDA, 0},
        (Instruction_t){"LDX", R6502::ABY, R6502::LDX, 0},
        (Instruction_t){"BBS3", R6502::REL, R6502::BBS3, 0},
        (Instruction_t){"CPY", R6502::IMM, R6502::CPY, 0},
        (Instruction_t){"CMP", R6502::INX, R6502::CMP, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"CPY", R6502::ZP0, R6502::CPY, 0},
        (Instruction_t){"CMP", R6502::ZP0, R6502::CMP, 0},
        (Instruction_t){"DEC", R6502::ZP0, R6502::DEC, 0},
        (Instruction_t){"SMB4", R6502::ZP0, R6502::SMB4, 0},
        (Instruction_t){"INY", R6502::IMP, R6502::INCY, 0},
        (Instruction_t){"CMP", R6502::IMM, R6502::CMP, 0},
        (Instruction_t){"DEX", R6502::IMP, R6502::DEX, 0},
        (Instruction_t){"WAI", R6502::IMP, R6502::WAI, 0},
        (Instruction_t){"CPY", R6502::ABS, R6502::CPY, 0},
        (Instruction_t){"CMP", R6502::ABS, R6502::CMP, 0},
        (Instruction_t){"DEC", R6502::ABS, R6502::DEC, 0},
        (Instruction_t){"BBS4", R6502::REL, R6502::BBS4, 0},
        (Instruction_t){"BNE", R6502::REL, R6502::BNE, 0},
        (Instruction_t){"CMP", R6502::INY, R6502::CMP, 0},
        (Instruction_t){"CMP", R6502::ZPI, R6502::CMP, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"CMP", R6502::ZPX, R6502::CMP, 0},
        (Instruction_t){"DEC", R6502::ZPX, R6502::DEC, 0},
        (Instruction_t){"SMB5", R6502::ZP0, R6502::SMB5, 0},
        (Instruction_t){"CLD", R6502::IMP, R6502::CLD, 0},
        (Instruction_t){"CMP", R6502::ABY, R6502::CMP, 0},
        (Instruction_t){"PHX", R6502::IMP, R6502::PHX, 0},
        (Instruction_t){"STP", R6502::IMP, R6502::STP, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"CMP", R6502::ABX, R6502::CMP, 0},
        (Instruction_t){"DEC", R6502::ABX, R6502::DEC, 0},
        (Instruction_t){"BBS5", R6502::REL, R6502::BBS5, 0},
        (Instruction_t){"CPX", R6502::IMM, R6502::CPX, 0},
        (Instruction_t){"SBC", R6502::INX, R6502::SBC, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"CPX", R6502::ZP0, R6502::CPX, 0},
        (Instruction_t){"SBC", R6502::ZP0, R6502::SBC, 0},
        (Instruction_t){"INC", R6502::ZP0, R6502::INC, 0},
        (Instruction_t){"SMB6", R6502::ZP0, R6502::SMB6, 0},
        (Instruction_t){"INX", R6502::IMP, R6502::INCX, 0},
        (Instruction_t){"SBC", R6502::IMM, R6502::SBC, 0},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"CPX", R6502::ABS, R6502::CPX, 0},
        (Instruction_t){"SBC", R6502::ABS, R6502::SBC, 0},
        (Instruction_t){"INC", R6502::ABS, R6502::INC, 0},
        (Instruction_t){"BBS6", R6502::REL, R6502::BBS6, 0},
        (Instruction_t){"BEQ", R6502::REL, R6502::BEQ, 0},
        (Instruction_t){"SBC", R6502::INY, R6502::SBC, 0},
        (Instruction_t){"SBC", R6502::ZPI, R6502::SBC, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"SBC", R6502::ZPX, R6502::SBC, 0},
        (Instruction_t){"INC", R6502::ZPX, R6502::INC, 0},
        (Instruction_t){"SMB7", R6502::ZP0, R6502::SMB7, 0},
        (Instruction_t){"SED", R6502::IMP, R6502::SED, 0},
        (Instruction_t){"SBC", R6502::ABY, R6502::SBC, 0},
        (Instruction_t){"PLX", R6502::IMP, R6502::PLX, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"UNIMPL", R6502::IMP, R6502::UNIMPL, 0},
        (Instruction_t){"SBC", R6502::ABX, R6502::SBC, 0},
        (Instruction_t){"INC", R6502::ABX, R6502::INC, 0},
        (Instruction_t){"BBS7", R6502::REL, R6502::BBS7, 0}};

uint8_t R6502::setStatus(StatusFlags flag, bool value)
{
    if (value)
    {
        R6502::reg_Status |= flag;
    }
    else
    {
        R6502::reg_Status &= ~flag;
    }

    return R6502::reg_Status;
}

uint8_t R6502::toggleStatus(StatusFlags flag)
{
    R6502::reg_Status ^= flag;
    return R6502::reg_Status;
}

void R6502::clock()
{
    // waste ticks
    if (ticks > 0)
    {
        ticks--;
        return;
    }

    uint8_t instr = bus.read(IP);
    Instruction_t instruction;
    try
    {
        instruction = Instructions.at(instr);
        IP++;
    }
    catch (const std::exception &e)
    {
        ERROR(R6502_clock, "Unknown instruction: " << instr);
        return;
    }

    instruction.addrmode();
    instruction.operation();

    ticks += instruction.cycles;
}

void R6502::stack_Push(uint8_t data)
{
    bus.write(reg_Stack, data);
    reg_Stack--;
}

uint8_t R6502::stack_Pop()
{
    uint8_t data = bus.read(reg_Stack);
    reg_Stack++;
    return data;
}

// addressing modes
void R6502::ACC()
{
    accumulator = true;
}

void R6502::IMM()
{
    addr = IP;
    IP++;
}

void R6502::ZP0()
{
    addr = bus.read(IP);
    IP++;
}

void R6502::INX() {}

void R6502::ZPX()
{
    addr = bus.read(IP);
    IP++;
    addr += reg_X;
}

void R6502::ZPY()
{
    addr = bus.read(IP);
    IP++;
    addr += reg_Y;
}

// instruction definitions
void R6502::LDA()
{
    reg_Acc = bus.read(addr);
}

void R6502::LDX()
{
    reg_X = bus.read(addr);
}

void R6502::LDY()
{
    reg_Y = bus.read(addr);
}

void R6502::STX()
{
    bus.write(addr, reg_X);
}

void R6502::STY()
{
    bus.write(addr, reg_Y);
}

void R6502::AND()
{
    uint8_t val = bus.read(addr);
    reg_Acc &= val;
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::ASL()
{
    uint8_t data = accumulator ? reg_Acc : bus.read(addr);
    setStatus(StatusFlags::C, data & 0x80);

    data <<= 1;
    if (accumulator)
    {
        reg_Acc = data;
        accumulator = false;
    }
    else
    {
        bus.write(addr, data);
    }
}

void R6502::BIT()
{
    uint8_t data = bus.read(addr);
    reg_Status |= data & 0xC0; // set N and V flags to bit 7 and 6
    setStatus(StatusFlags::Z, (data & reg_Acc) == 0);
}

void R6502::CMP()
{
    uint8_t data = bus.read(addr);
    setStatus(StatusFlags::C, reg_Acc >= data);                                 // if positive
    setStatus((StatusFlags)(StatusFlags::C | StatusFlags::Z), reg_Acc == data); // if zero
    setStatus(StatusFlags::N, (reg_Acc - data) & 0x80);                         // if negative MSB = 1
}

void R6502::CPX()
{
    uint8_t data = bus.read(addr);
    setStatus(StatusFlags::C, reg_X >= data);                                 // if positive
    setStatus((StatusFlags)(StatusFlags::C | StatusFlags::Z), reg_X == data); // if zero
    setStatus(StatusFlags::N, (reg_X - data) & 0x80);                         // if negative MSB = 1
}

void R6502::CPY()
{
    uint8_t data = bus.read(addr);
    setStatus(StatusFlags::C, reg_Y >= data);                                 // if positive
    setStatus((StatusFlags)(StatusFlags::C | StatusFlags::Z), reg_Y == data); // if zero
    setStatus(StatusFlags::N, (reg_Y - data) & 0x80);                         // if negative MSB = 1
}

void R6502::DEC()
{
    uint8_t data = bus.read(addr);
    data--;
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    bus.write(addr, data);
}

void R6502::EOR()
{
    uint8_t data = bus.read(addr);
    reg_Acc = reg_Acc ^ data;
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);
}

void R6502::CLC()
{
    setStatus(StatusFlags::C, false);
}

void R6502::SEC()
{
    setStatus(StatusFlags::C, true);
}

void R6502::CLI()
{
    setStatus(StatusFlags::I, false);
}

void R6502::SEI()
{
    setStatus(StatusFlags::I, true);
}

void R6502::CLV()
{
    setStatus(StatusFlags::V, false);
}

void R6502::CLD()
{
    setStatus(StatusFlags::D, false);
}

void R6502::SED()
{
    setStatus(StatusFlags::D, true);
}

void R6502::INC()
{
    uint8_t data = bus.read(addr);
    data++;
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    bus.write(addr, data);
}

void R6502::JMP()
{
    uint16_t jmpLoc = bus.read(addr);      // read the lower bytes of jump
    if (indirect && ((addr & 0xFF) == 0xFF)) // adjust for page boundary indirect jumps
    {
        addr &= 0xFF00;
        addr--;
        indirect = false;
    }

    uint8_t jmpHigh = bus.read(addr + 1) << 8; // read the upper address bytes of jump
    IP = jmpLoc | jmpHigh;
}

void R6502::JSR()
{
    uint16_t addrToPush = IP - 1;
    stack_Push(addrToPush & 0x00FF); // first push the lower byte
    stack_Push(addrToPush & 0xFF00); // then push the upper byte (little endian)
    IP = bus.read(addr);
}

void R6502::LSR()
{
    if (zeroIndirect)
    {
        addr &= 0xFF; // make sure the calculated address doesnot escape the 0xFF zero page boundary
        zeroIndirect = false;
    }

    uint8_t data = accumulator ? reg_Acc : bus.read(addr);  // handle accumulator cases
    
    setStatus(StatusFlags::N, false); // always clear negative
    setStatus(StatusFlags::C, data & 0x1);
    
    data >>= 1;
    data &= 0x7F; // make sure the 7th bit is zero

    if (accumulator)    // handle accumulator case
    {
        reg_Acc = data;
        accumulator = false;
    }
}

void R6502::IMP() {}
void R6502::INY() {}
void R6502::ABS() {}
void R6502::ABX() {}
void R6502::ABY() {}
void R6502::REL() {}
void R6502::ZPI() {}
void R6502::JIND() {}
void R6502::ABIX() {}

void R6502::UNIMPL(void) {}
void R6502::BRK(void) {}
void R6502::ORA(void) {}
void R6502::TSB(void) {}
void R6502::RMB0(void) {}
void R6502::PHP(void) {}
void R6502::BBR0(void) {}
void R6502::BPL(void) {}
void R6502::TRB(void) {}
void R6502::RMB1(void) {}
void R6502::BBR1(void) {}
void R6502::ROL(void) {}
void R6502::RMB2(void) {}
void R6502::PLP(void) {}
void R6502::BBR2(void) {}
void R6502::BMI(void) {}
void R6502::RMB3(void) {}
void R6502::BBR3(void) {}
void R6502::RTI(void) {}
void R6502::RMB4(void) {}
void R6502::PHA(void) {}
void R6502::BBR4(void) {}
void R6502::BVC(void) {}
void R6502::RMB5(void) {}
void R6502::PHY(void) {}
void R6502::BBR5(void) {}
void R6502::RTS(void) {}
void R6502::ADC(void) {}
void R6502::STZ(void) {}
void R6502::ROR(void) {}
void R6502::RMB6(void) {}
void R6502::PLA(void) {}
void R6502::BBR6(void) {}
void R6502::BVS(void) {}
void R6502::RMB7(void) {}
void R6502::PLY(void) {}
void R6502::BBR7(void) {}
void R6502::BRA(void) {}
void R6502::STA(void) {}
void R6502::SMB0(void) {}
void R6502::DEY(void) {}
void R6502::TXA(void) {}
void R6502::BBS0(void) {}
void R6502::BCC(void) {}
void R6502::SMB1(void) {}
void R6502::TYA(void) {}
void R6502::TXS(void) {}
void R6502::BBS1(void) {}
void R6502::SMB2(void) {}
void R6502::TAY(void) {}
void R6502::TAX(void) {}
void R6502::BBS2(void) {}
void R6502::BCS(void) {}
void R6502::SMB3(void) {}
void R6502::TSX(void) {}
void R6502::BBS3(void) {}
void R6502::SMB4(void) {}
void R6502::INCY(void) {}
void R6502::DEX(void) {}
void R6502::WAI(void) {}
void R6502::BBS4(void) {}
void R6502::BNE(void) {}
void R6502::SMB5(void) {}
void R6502::PHX(void) {}
void R6502::STP(void) {}
void R6502::BBS5(void) {}
void R6502::SBC(void) {}
void R6502::SMB6(void) {}
void R6502::INCX(void) {}
void R6502::NOP(void) {}
void R6502::BBS6(void) {}
void R6502::BEQ(void) {}
void R6502::SMB7(void) {}
void R6502::PLX(void) {}
void R6502::BBS7(void) {}