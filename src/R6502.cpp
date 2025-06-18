#include <R6502.hpp>

#include <bitset> // DEBUG

R6502::R6502(std::unique_ptr<R6502Bus> &&bus)
{
    setBus(std::move(bus));
}

R6502::~R6502()
{
    logFile.close();
}

R6502Bus *R6502::getBus() const
{
    return bus.get();
}

void R6502::setBus(std::unique_ptr<R6502Bus> &&bus)
{
    if (bus == nullptr)
    {
        ERROR(R6502::setBus, "Bus cannot be null, CPU read writes will fail");
        return;
    }

    this->bus = std::move(bus);
}

std::unique_ptr<R6502Bus> R6502::bus;
int R6502::total_cycles = 0;
int R6502::ticks = 0;
uint32_t R6502::IP = 0;
uint8_t R6502::reg_Acc = 0;
uint8_t R6502::reg_X = 0;
uint8_t R6502::reg_Y = 0;
uint8_t R6502::reg_Status = 0;
uint16_t R6502::reg_Stack = 0x1FF;
uint8_t R6502::instr = 0;
uint8_t R6502::imm = 0;
uint16_t R6502::addr = 0;
bool R6502::accumulator = false;
bool R6502::indirect = false;
bool R6502::zeroIndirect = false;

bool R6502::running = true;

std::ofstream R6502::logFile("./cpuDebug.log");

std::minstd_rand R6502::rand(std::random_device{}()); // get a random seed once for the PRNG
uint8_t R6502::ANE_randomValues[7] = {0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

std::vector<Instruction_t> R6502::Instructions =
    {
        (Instruction_t){"BRK", R6502::IMP, R6502::BRK, 7},
        (Instruction_t){"ORA", R6502::INX, R6502::ORA, 6},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"SLO", R6502::INX, R6502::SLO, 8},
        (Instruction_t){"NOP", R6502::ZP0, R6502::NOP, 3},
        (Instruction_t){"ORA", R6502::ZP0, R6502::ORA, 3},
        (Instruction_t){"ASL", R6502::ZP0, R6502::ASL, 5},
        (Instruction_t){"SLO", R6502::ZP0, R6502::SLO, 5},
        (Instruction_t){"PHP", R6502::IMP, R6502::PHP, 3},
        (Instruction_t){"ORA", R6502::IMM, R6502::ORA, 2},
        (Instruction_t){"ASL", R6502::ACC, R6502::ASL, 2},
        (Instruction_t){"ANC", R6502::IMM, R6502::ANC, 2},
        (Instruction_t){"NOP", R6502::ABS, R6502::NOP, 4},
        (Instruction_t){"ORA", R6502::ABS, R6502::ORA, 4},
        (Instruction_t){"ASL", R6502::ABS, R6502::ASL, 6},
        (Instruction_t){"SLO", R6502::ABS, R6502::SLO, 6},
        (Instruction_t){"BPL", R6502::REL, R6502::BPL, 2},
        (Instruction_t){"ORA", R6502::INY, R6502::ORA, 5},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"SLO", R6502::INY, R6502::SLO, 8},
        (Instruction_t){"NOP", R6502::ZPX, R6502::NOP, 4},
        (Instruction_t){"ORA", R6502::ZPX, R6502::ORA, 4},
        (Instruction_t){"ASL", R6502::ZPX, R6502::ASL, 6},
        (Instruction_t){"SLO", R6502::ZPX, R6502::SLO, 6},
        (Instruction_t){"CLC", R6502::IMP, R6502::CLC, 2},
        (Instruction_t){"ORA", R6502::ABY, R6502::ORA, 4},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 2},
        (Instruction_t){"SLO", R6502::ABY, R6502::SLO, 7},
        (Instruction_t){"NOP", R6502::ABX, R6502::NOP, 4},
        (Instruction_t){"ORA", R6502::ABX, R6502::ORA, 4},
        (Instruction_t){"ASL", R6502::ABX, R6502::ASL, 7},
        (Instruction_t){"SLO", R6502::ABX, R6502::SLO, 7},
        (Instruction_t){"JSR", R6502::ABS, R6502::JSR, 6},
        (Instruction_t){"AND", R6502::INX, R6502::AND, 6},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"RLA", R6502::INX, R6502::RLA, 8},
        (Instruction_t){"BIT", R6502::ZP0, R6502::BIT, 3},
        (Instruction_t){"AND", R6502::ZP0, R6502::AND, 3},
        (Instruction_t){"ROL", R6502::ZP0, R6502::ROL, 5},
        (Instruction_t){"RLA", R6502::ZP0, R6502::RLA, 5},
        (Instruction_t){"PLP", R6502::IMP, R6502::PLP, 4},
        (Instruction_t){"AND", R6502::IMM, R6502::AND, 2},
        (Instruction_t){"ROL", R6502::ACC, R6502::ROL, 2},
        (Instruction_t){"ANC", R6502::IMM, R6502::ANC, 2},
        (Instruction_t){"BIT", R6502::ABS, R6502::BIT, 4},
        (Instruction_t){"AND", R6502::ABS, R6502::AND, 4},
        (Instruction_t){"ROL", R6502::ABS, R6502::ROL, 6},
        (Instruction_t){"RLA", R6502::ABS, R6502::RLA, 6},
        (Instruction_t){"BMI", R6502::REL, R6502::BMI, 2},
        (Instruction_t){"AND", R6502::INY, R6502::AND, 5},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"RLA", R6502::INY, R6502::RLA, 8},
        (Instruction_t){"NOP", R6502::ZPX, R6502::NOP, 4},
        (Instruction_t){"AND", R6502::ZPX, R6502::AND, 4},
        (Instruction_t){"ROL", R6502::ZPX, R6502::ROL, 6},
        (Instruction_t){"RLA", R6502::ZPX, R6502::RLA, 6},
        (Instruction_t){"SEC", R6502::IMP, R6502::SEC, 2},
        (Instruction_t){"AND", R6502::ABY, R6502::AND, 4},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 2},
        (Instruction_t){"RLA", R6502::ABY, R6502::RLA, 7},
        (Instruction_t){"NOP", R6502::ABX, R6502::NOP, 4},
        (Instruction_t){"AND", R6502::ABX, R6502::AND, 4},
        (Instruction_t){"ROL", R6502::ABX, R6502::ROL, 7},
        (Instruction_t){"RLA", R6502::ABX, R6502::RLA, 7},
        (Instruction_t){"RTI", R6502::IMP, R6502::RTI, 6},
        (Instruction_t){"EOR", R6502::INX, R6502::EOR, 6},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"SRE", R6502::INX, R6502::SRE, 8},
        (Instruction_t){"NOP", R6502::ZP0, R6502::NOP, 3},
        (Instruction_t){"EOR", R6502::ZP0, R6502::EOR, 3},
        (Instruction_t){"LSR", R6502::ZP0, R6502::LSR, 5},
        (Instruction_t){"SRE", R6502::ZP0, R6502::SRE, 5},
        (Instruction_t){"PHA", R6502::IMP, R6502::PHA, 3},
        (Instruction_t){"EOR", R6502::IMM, R6502::EOR, 2},
        (Instruction_t){"LSR", R6502::ACC, R6502::LSR, 2},
        (Instruction_t){"ALR", R6502::IMM, R6502::ALR, 2},
        (Instruction_t){"JMP", R6502::ABS, R6502::JMP, 3},
        (Instruction_t){"EOR", R6502::ABS, R6502::EOR, 4},
        (Instruction_t){"LSR", R6502::ABS, R6502::LSR, 6},
        (Instruction_t){"SRE", R6502::ABS, R6502::SRE, 6},
        (Instruction_t){"BVC", R6502::REL, R6502::BVC, 2},
        (Instruction_t){"EOR", R6502::INY, R6502::EOR, 5},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"SRE", R6502::INY, R6502::SRE, 8},
        (Instruction_t){"NOP", R6502::ZPX, R6502::NOP, 4},
        (Instruction_t){"EOR", R6502::ZPX, R6502::EOR, 4},
        (Instruction_t){"LSR", R6502::ZPX, R6502::LSR, 6},
        (Instruction_t){"SRE", R6502::ZPX, R6502::SRE, 6},
        (Instruction_t){"CLI", R6502::IMP, R6502::CLI, 2},
        (Instruction_t){"EOR", R6502::ABY, R6502::EOR, 4},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 2},
        (Instruction_t){"SRE", R6502::ABY, R6502::SRE, 7},
        (Instruction_t){"NOP", R6502::ABX, R6502::NOP, 4},
        (Instruction_t){"EOR", R6502::ABX, R6502::EOR, 4},
        (Instruction_t){"LSR", R6502::ABX, R6502::LSR, 7},
        (Instruction_t){"SRE", R6502::ABX, R6502::SRE, 7},
        (Instruction_t){"RTS", R6502::IMP, R6502::RTS, 6},
        (Instruction_t){"ADC", R6502::INX, R6502::ADC, 6},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"RRA", R6502::INX, R6502::RRA, 8},
        (Instruction_t){"NOP", R6502::ZP0, R6502::NOP, 3},
        (Instruction_t){"ADC", R6502::ZP0, R6502::ADC, 3},
        (Instruction_t){"ROR", R6502::ZP0, R6502::ROR, 5},
        (Instruction_t){"RRA", R6502::ZP0, R6502::RRA, 5},
        (Instruction_t){"PLA", R6502::IMP, R6502::PLA, 4},
        (Instruction_t){"ADC", R6502::IMM, R6502::ADC, 2},
        (Instruction_t){"ROR", R6502::ACC, R6502::ROR, 2},
        (Instruction_t){"ARR", R6502::IMM, R6502::ARR, 2},
        (Instruction_t){"JMP", R6502::JIND, R6502::JMP, 5},
        (Instruction_t){"ADC", R6502::ABS, R6502::ADC, 4},
        (Instruction_t){"ROR", R6502::ABS, R6502::ROR, 6},
        (Instruction_t){"RRA", R6502::ABS, R6502::RRA, 6},
        (Instruction_t){"BVS", R6502::REL, R6502::BVS, 2},
        (Instruction_t){"ADC", R6502::INY, R6502::ADC, 5},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"RRA", R6502::INY, R6502::RRA, 8},
        (Instruction_t){"NOP", R6502::ZPX, R6502::NOP, 4},
        (Instruction_t){"ADC", R6502::ZPX, R6502::ADC, 4},
        (Instruction_t){"ROR", R6502::ZPX, R6502::ROR, 6},
        (Instruction_t){"RRA", R6502::ZPX, R6502::RRA, 6},
        (Instruction_t){"SEI", R6502::IMP, R6502::SEI, 2},
        (Instruction_t){"ADC", R6502::ABY, R6502::ADC, 4},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 2},
        (Instruction_t){"RRA", R6502::ABY, R6502::RRA, 7},
        (Instruction_t){"NOP", R6502::ABX, R6502::NOP, 4},
        (Instruction_t){"ADC", R6502::ABX, R6502::ADC, 4},
        (Instruction_t){"ROR", R6502::ABX, R6502::ROR, 7},
        (Instruction_t){"RRA", R6502::ABX, R6502::RRA, 7},
        (Instruction_t){"NOP", R6502::IMM, R6502::NOP, 2},
        (Instruction_t){"STA", R6502::INX, R6502::STA, 6},
        (Instruction_t){"NOP", R6502::IMM, R6502::NOP, 2},
        (Instruction_t){"SAX", R6502::INX, R6502::SAX, 6},
        (Instruction_t){"STY", R6502::ZP0, R6502::STY, 3},
        (Instruction_t){"STA", R6502::ZP0, R6502::STA, 3},
        (Instruction_t){"STX", R6502::ZP0, R6502::STX, 3},
        (Instruction_t){"SAX", R6502::ZP0, R6502::SAX, 3},
        (Instruction_t){"DEY", R6502::IMP, R6502::DEY, 2},
        (Instruction_t){"NOP", R6502::IMM, R6502::NOP, 2},
        (Instruction_t){"TXA", R6502::IMP, R6502::TXA, 2},
        (Instruction_t){"ANE", R6502::IMM, R6502::ANE, 2},
        (Instruction_t){"STY", R6502::ABS, R6502::STY, 4},
        (Instruction_t){"STA", R6502::ABS, R6502::STA, 4},
        (Instruction_t){"STX", R6502::ABS, R6502::STX, 4},
        (Instruction_t){"SAX", R6502::ABS, R6502::SAX, 4},
        (Instruction_t){"BCC", R6502::REL, R6502::BCC, 2},
        (Instruction_t){"STA", R6502::INY, R6502::STA, 6},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"SHA", R6502::INY, R6502::SHA, 6},
        (Instruction_t){"STY", R6502::ZPX, R6502::STY, 4},
        (Instruction_t){"STA", R6502::ZPX, R6502::STA, 4},
        (Instruction_t){"STX", R6502::ZPY, R6502::STX, 4},
        (Instruction_t){"SAX", R6502::ZPY, R6502::SAX, 4},
        (Instruction_t){"TYA", R6502::IMP, R6502::TYA, 2},
        (Instruction_t){"STA", R6502::ABY, R6502::STA, 5},
        (Instruction_t){"TXS", R6502::IMP, R6502::TXS, 2},
        (Instruction_t){"TAS", R6502::ABY, R6502::TAS, 5},
        (Instruction_t){"SHY", R6502::ABX, R6502::SHY, 5},
        (Instruction_t){"STA", R6502::ABX, R6502::STA, 5},
        (Instruction_t){"SHX", R6502::ABY, R6502::SHX, 5},
        (Instruction_t){"SHA", R6502::ABY, R6502::SHA, 5},
        (Instruction_t){"LDY", R6502::IMM, R6502::LDY, 2},
        (Instruction_t){"LDA", R6502::INX, R6502::LDA, 6},
        (Instruction_t){"LDX", R6502::IMM, R6502::LDX, 2},
        (Instruction_t){"LAX", R6502::INX, R6502::LAX, 6},
        (Instruction_t){"LDY", R6502::ZP0, R6502::LDY, 3},
        (Instruction_t){"LDA", R6502::ZP0, R6502::LDA, 3},
        (Instruction_t){"LDX", R6502::ZP0, R6502::LDX, 3},
        (Instruction_t){"LAX", R6502::ZP0, R6502::LAX, 3},
        (Instruction_t){"TAY", R6502::IMP, R6502::TAY, 2},
        (Instruction_t){"LDA", R6502::IMM, R6502::LDA, 2},
        (Instruction_t){"TAX", R6502::IMP, R6502::TAX, 2},
        (Instruction_t){"LXA", R6502::IMM, R6502::LXA, 2},
        (Instruction_t){"LDY", R6502::ABS, R6502::LDY, 4},
        (Instruction_t){"LDA", R6502::ABS, R6502::LDA, 4},
        (Instruction_t){"LDX", R6502::ABS, R6502::LDX, 4},
        (Instruction_t){"LAX", R6502::ABS, R6502::LAX, 4},
        (Instruction_t){"BCS", R6502::REL, R6502::BCS, 2},
        (Instruction_t){"LDA", R6502::INY, R6502::LDA, 5},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"LAX", R6502::INY, R6502::LAX, 5},
        (Instruction_t){"LDY", R6502::ZPX, R6502::LDY, 4},
        (Instruction_t){"LDA", R6502::ZPX, R6502::LDA, 4},
        (Instruction_t){"LDX", R6502::ZPY, R6502::LDX, 4},
        (Instruction_t){"LAX", R6502::ZPY, R6502::LAX, 4},
        (Instruction_t){"CLV", R6502::IMP, R6502::CLV, 2},
        (Instruction_t){"LDA", R6502::ABY, R6502::LDA, 4},
        (Instruction_t){"TSX", R6502::IMP, R6502::TSX, 2},
        (Instruction_t){"LAS", R6502::ABY, R6502::LAS, 4},
        (Instruction_t){"LDY", R6502::ABX, R6502::LDY, 4},
        (Instruction_t){"LDA", R6502::ABX, R6502::LDA, 4},
        (Instruction_t){"LDX", R6502::ABY, R6502::LDX, 4},
        (Instruction_t){"LAX", R6502::ABY, R6502::LAX, 4},
        (Instruction_t){"CPY", R6502::IMM, R6502::CPY, 2},
        (Instruction_t){"CMP", R6502::INX, R6502::CMP, 6},
        (Instruction_t){"NOP", R6502::IMM, R6502::NOP, 2},
        (Instruction_t){"DCP", R6502::INX, R6502::DCP, 8},
        (Instruction_t){"CPY", R6502::ZP0, R6502::CPY, 3},
        (Instruction_t){"CMP", R6502::ZP0, R6502::CMP, 3},
        (Instruction_t){"DEC", R6502::ZP0, R6502::DEC, 5},
        (Instruction_t){"DCP", R6502::ZP0, R6502::DCP, 5},
        (Instruction_t){"INY", R6502::IMP, R6502::INCY, 2},
        (Instruction_t){"CMP", R6502::IMM, R6502::CMP, 2},
        (Instruction_t){"DEX", R6502::IMP, R6502::DEX, 2},
        (Instruction_t){"SBX", R6502::IMM, R6502::SBX, 2},
        (Instruction_t){"CPY", R6502::ABS, R6502::CPY, 4},
        (Instruction_t){"CMP", R6502::ABS, R6502::CMP, 4},
        (Instruction_t){"DEC", R6502::ABS, R6502::DEC, 6},
        (Instruction_t){"DCP", R6502::ABS, R6502::DCP, 6},
        (Instruction_t){"BNE", R6502::REL, R6502::BNE, 2},
        (Instruction_t){"CMP", R6502::INY, R6502::CMP, 5},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"DCP", R6502::INY, R6502::DCP, 8},
        (Instruction_t){"NOP", R6502::ZPX, R6502::NOP, 4},
        (Instruction_t){"CMP", R6502::ZPX, R6502::CMP, 4},
        (Instruction_t){"DEC", R6502::ZPX, R6502::DEC, 6},
        (Instruction_t){"DCP", R6502::ZPX, R6502::DCP, 6},
        (Instruction_t){"CLD", R6502::IMP, R6502::CLD, 2},
        (Instruction_t){"CMP", R6502::ABY, R6502::CMP, 4},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 2},
        (Instruction_t){"DCP", R6502::ABY, R6502::DCP, 7},
        (Instruction_t){"NOP", R6502::ABX, R6502::NOP, 4},
        (Instruction_t){"CMP", R6502::ABX, R6502::CMP, 4},
        (Instruction_t){"DEC", R6502::ABX, R6502::DEC, 7},
        (Instruction_t){"DCP", R6502::ABX, R6502::DCP, 7},
        (Instruction_t){"CPX", R6502::IMM, R6502::CPX, 2},
        (Instruction_t){"SBC", R6502::INX, R6502::SBC, 6},
        (Instruction_t){"NOP", R6502::IMM, R6502::NOP, 2},
        (Instruction_t){"ISC", R6502::INX, R6502::ISC, 8},
        (Instruction_t){"CPX", R6502::ZP0, R6502::CPX, 3},
        (Instruction_t){"SBC", R6502::ZP0, R6502::SBC, 3},
        (Instruction_t){"INC", R6502::ZP0, R6502::INC, 5},
        (Instruction_t){"ISC", R6502::ZP0, R6502::ISC, 5},
        (Instruction_t){"INX", R6502::IMP, R6502::INCX, 2},
        (Instruction_t){"SBC", R6502::IMM, R6502::SBC, 2},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 2},
        (Instruction_t){"USBC", R6502::IMM, R6502::USBC, 2},
        (Instruction_t){"CPX", R6502::ABS, R6502::CPX, 4},
        (Instruction_t){"SBC", R6502::ABS, R6502::SBC, 4},
        (Instruction_t){"INC", R6502::ABS, R6502::INC, 6},
        (Instruction_t){"ISC", R6502::ABS, R6502::ISC, 6},
        (Instruction_t){"BEQ", R6502::REL, R6502::BEQ, 2},
        (Instruction_t){"SBC", R6502::INY, R6502::SBC, 5},
        (Instruction_t){"JAM", R6502::IMP, R6502::JAM, 0},
        (Instruction_t){"ISC", R6502::INY, R6502::ISC, 8},
        (Instruction_t){"NOP", R6502::ZPX, R6502::NOP, 4},
        (Instruction_t){"SBC", R6502::ZPX, R6502::SBC, 4},
        (Instruction_t){"INC", R6502::ZPX, R6502::INC, 6},
        (Instruction_t){"ISC", R6502::ZPX, R6502::ISC, 6},
        (Instruction_t){"SED", R6502::IMP, R6502::SED, 2},
        (Instruction_t){"SBC", R6502::ABY, R6502::SBC, 4},
        (Instruction_t){"NOP", R6502::IMP, R6502::NOP, 2},
        (Instruction_t){"ISC", R6502::ABY, R6502::ISC, 7},
        (Instruction_t){"NOP", R6502::ABX, R6502::NOP, 4},
        (Instruction_t){"SBC", R6502::ABX, R6502::SBC, 4},
        (Instruction_t){"INC", R6502::ABX, R6502::INC, 7},
        (Instruction_t){"ISC", R6502::ABX, R6502::ISC, 7}};

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

bool R6502::getStatus(StatusFlags flag)
{
    return (reg_Status & flag) == flag;
}

void R6502::clock()
{
#ifndef DISABLE_TIMING_TICKS
#ifndef HIDE_TICKS_DISPLAY
    LOG(clock_TICK, "TICKS: " << ticks);
#endif // HIDE_TICKS_DISPLAY

    // waste ticks
    if (ticks > 0)
    {
        ticks--;
        return;
    }
#endif // DISABLE_TIMING_TICKS

    if (!running) // if not running do nothing
    {
        return;
    }

    instr = bus.get()->read(IP);
    Instruction_t instruction;
    try
    {
        instruction = Instructions.at(instr);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Unknown instruction exception!");
    }

    logFile << std::dec << "Instruction: " << R6502::Instructions[R6502::instr].op << " IP: " << std::hex << R6502::IP << std::dec << '\n';
    logFile << "CPU STATE:: X: " << static_cast<int>(R6502::reg_X) << " Y: " << static_cast<int>(R6502::reg_Y) << " Acc: " << static_cast<int>(R6502::reg_Acc) << std::hex << " Stack: " << reg_Stack << " Status: " << std::bitset<8>(R6502::reg_Status) << '\n';

    IP++; // update Instruction pointer

    ticks = instruction.cycles;
    instruction.addrmode(); // addressing mode adds additional ticks
    instruction.operation();

    total_cycles += ticks; // count the total number of instruction cycles
}

void R6502::reset()
{
    total_cycles = 0;
    ticks = 0;
    IP = 0;
    reg_Acc = 0;
    reg_X = 0;
    reg_Y = 0;
    reg_Status = 0;
    reg_Stack = 0x1FF;
    instr = 0;
    imm = 0;
    addr = 0;
    accumulator = false;
    indirect = false;
    zeroIndirect = false;
}

void R6502::stack_Push(uint8_t data)
{
    bus.get()->write(reg_Stack, data);
    reg_Stack--;

    LOG(STACK_PUSH, "Data: " << static_cast<int>(data) << " Stack pointer (after write): " << reg_Stack);

#ifdef ENABLE_STACK_CHECKS
    if (reg_Stack < 0x100)
    {
        ERROR(STACK_ERROR, "Stack overflow, overwriting to zero page, program may be in unstable state");
    }
#endif
}

uint8_t R6502::stack_Pop()
{
    reg_Stack++;

#ifdef ENABLE_STACK_CHECKS
    if (reg_Stack > 0x1FF)
    {
        ERROR(STACK_ERROR, "Stack popped out of bounds, program is in an undefined state");
    }
#endif

    uint8_t data = bus.get()->read(reg_Stack);
    LOG(STACK_POP, "Data: " << static_cast<int>(data) << " Stack pointer (after read): " << reg_Stack);
    return data;
}

// addressing modes
void R6502::ACC()
{
    accumulator = true;
}

void R6502::ABS()
{
    addr = bus.get()->read(IP);
    addr |= bus.get()->read(IP + 1) << 8;
    IP += 2; // point to next instruction
}

void R6502::IMM()
{
    addr = IP;
    IP++;
}

void R6502::ZP0()
{
    addr = bus.get()->read(IP);
    IP++;
}

void R6502::ZPX()
{
    addr = bus.get()->read(IP);
    IP++;
    addr = (addr + reg_X) & 0xFF; // make sure its zero paged
}

void R6502::ZPY()
{
    addr = bus.get()->read(IP);
    IP++;
    addr = (addr + reg_Y) & 0xFF; // make sure its zero paged
}

void R6502::IMP() {} // implied, does nothing for now and lets the instruction decide the addressing stuff...

void R6502::JIND()
{
    uint16_t ptr = bus.get()->read(IP);
    ptr |= bus.get()->read(IP + 1) << 8;

    uint16_t calcAddr = bus.get()->read(ptr);

    if ((ptr & 0xFF) == 0xFF)
    {
        calcAddr |= bus.get()->read(ptr & 0xFF00) << 8;
    }
    else
    {
        calcAddr |= bus.get()->read(ptr + 1) << 8;
    }

    addr = calcAddr;
    IP += 2;
}

void R6502::REL()
{
    // IP is already pointing to operand
    int8_t offset = static_cast<int8_t>(bus.get()->read(IP));

    IP++; // increment IP and skip the operand, may be overwritten by the Branch instruction on a successfull branch
    addr = IP + offset;

    if ((IP & 0xFF00) != (addr & 0xFF00)) // if page boundary crossed
    {
        ticks++; // waste another tick
    }
}

void R6502::INX()
{
    uint8_t base = bus.get()->read(IP);
    IP++;
    uint8_t lo = bus.get()->read((base + reg_X) & 0xFF);
    uint8_t hi = bus.get()->read((base + reg_X + 1) & 0xFF);
    addr = (hi << 8) | lo;
}

void R6502::INY()
{
    uint8_t base = bus.get()->read(IP);
    IP++;
    uint16_t ptr = bus.get()->read(base) | (bus.get()->read((base + 1) & 0xFF) << 8);
    addr = ptr + reg_Y;

    // page boundary cross
    if ((ptr & 0xFF00) != (addr & 0xFF00))
    {
        ticks++; // waste another tick
    }
}

void R6502::ABX()
{
    uint16_t ptr = bus.get()->read(IP) | (bus.get()->read(IP + 1) << 8);
    IP += 2;
    addr = ptr + reg_X;

    // page boundary cross
    if ((ptr & 0xFF00) != (addr & 0xFF00))
    {
        ticks++; // waste another tick
    }
}

void R6502::ABY()
{
    uint16_t ptr = bus.get()->read(IP) | (bus.get()->read(IP + 1) << 8);
    IP += 2;
    addr = ptr + reg_Y;

    // page boundary cross
    if ((ptr & 0xFF00) != (addr & 0xFF00))
    {
        ticks++; // waste another tick
    }
}

void R6502::ZPI()
{
    LOG(AddressingMode_ZPI, "\"Zero page indirect\" addressing mode is not supported yet thus the current instruction is probably broken, please use a NMOS 6502 compatible program");
}

void R6502::ABIX()
{
    LOG(AddressingMode_ABIX, "\"Absolute Indirect Indexed with X\" addressing mode is not supported yet thus the current instruction is probably broken, please use a NMOS 6502 compatible program");
}

void R6502::NMI()
{
    stack_Push((IP >> 8) & 0xFF);     // push the high bits of the instruction
    stack_Push(IP & 0xFF);            // push the lower bits of the instruction
    setStatus(StatusFlags::U, true);  // set the unused
    setStatus(StatusFlags::B, false); // unset the BRK flag since it's NMI
    stack_Push(reg_Status);
    setStatus(StatusFlags::I, true); // disable further interrupts
    IP = bus.get()->read(0xFFFA) | (bus.get()->read(0xFFFB) << 8);
}

void R6502::IRQ()
{
    if (getStatus(StatusFlags::I)) // ignore if interrupts masked
    {
        return;
    }

    stack_Push((IP >> 8) & 0xFF);     // first push the high address
    stack_Push(IP & 0xFF);            // then push the lower address
    setStatus(StatusFlags::U, true);  // set the unused
    setStatus(StatusFlags::B, false); // dont set BRK since it is not a BRK interrupt call (hardware interrupt)
    stack_Push(reg_Status);
    setStatus(StatusFlags::I, true); // disable further interrupts
    IP = bus.get()->read(0xFFFE) | (bus.get()->read(0xFFFF) << 8);
}

// instruction definitions
void R6502::ADC()
{
    uint8_t data = bus.get()->read(addr);
    uint16_t sum = reg_Acc + data + getStatus(StatusFlags::C); // sum = Acc + fetched_data + Carry bit(1/0)

    // Overflow if: Accumulator and fetched_data have SAME SIGN but Accumulator and result have DIFFERENT SIGN
    setStatus(StatusFlags::V, (~(reg_Acc ^ data) & (reg_Acc ^ sum)) & 0x80);

    reg_Acc = static_cast<uint8_t>(sum & 0xFF); // make sure its 8-bit to fit within the Accumulator

    setStatus(StatusFlags::C, sum > 0xFF);
    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::SBC()
{
    uint8_t data = bus.get()->read(addr);

    // in 6502 subtraction is performed by 1's complement of data and then adding to Accumulator, then adding carry as in a borrow scheme
    // the calculation actually extends to => Acc = Acc - data - (1 - Carry) => Acc = Acc + (~data) + Carry
    uint16_t sum = reg_Acc + (~data) + getStatus(StatusFlags::C); // sum = Acc + (~fetched_data) + Carry bit(1/0)

    // Overflow if: inverse of Accumulator and fetched_data have DIFFERENT SIGN respectively but Accumulator and result have DIFFERENT SIGN
    setStatus(StatusFlags::V, ((~reg_Acc ^ data) & (reg_Acc ^ sum)) & 0x80);
    setStatus(StatusFlags::C, reg_Acc >= data); // no borrow if Accumulator is greater than operand (obviously)

    reg_Acc = static_cast<uint8_t>(sum & 0xFF); // make sure its 8-bit to fit within the Accumulator

    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::LDA()
{
    reg_Acc = bus.get()->read(addr);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
    setStatus(StatusFlags::Z, reg_Acc == 0);

    if (IP - 3 == 0x8022)
    {
        LOG(PRECISE_DBG, "ACC: " << std::dec << static_cast<int>(reg_Acc));
    }
}

void R6502::LDX()
{
    reg_X = bus.get()->read(addr);
    setStatus(StatusFlags::N, reg_X & 0x80);
    setStatus(StatusFlags::Z, reg_X == 0);
}

void R6502::LDY()
{
    reg_Y = bus.get()->read(addr);
    setStatus(StatusFlags::N, reg_Y & 0x80);
    setStatus(StatusFlags::Z, reg_Y == 0);
}

void R6502::STX()
{
    bus.get()->write(addr, reg_X);
}

void R6502::STY()
{
    bus.get()->write(addr, reg_Y);
}

void R6502::AND()
{
    uint8_t val = bus.get()->read(addr);
    reg_Acc &= val;
    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::ASL()
{
    uint8_t data = accumulator ? reg_Acc : bus.get()->read(addr);
    setStatus(StatusFlags::C, data & 0x80);

    data <<= 1;

    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    if (accumulator)
    {
        reg_Acc = data;
        accumulator = false;
    }
    else
    {
        bus.get()->write(addr, data);
    }
}

void R6502::BIT()
{
    uint8_t data = bus.get()->read(addr);
    reg_Status |= data & 0xC0; // set N and V flags to bit 7 and 6
    setStatus(StatusFlags::Z, (data & reg_Acc) == 0);
}

void R6502::BPL()
{
    if (!getStatus(StatusFlags::N))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::BMI()
{
    if (getStatus(StatusFlags::N))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::BVC()
{
    if (!getStatus(StatusFlags::V))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::BVS()
{
    if (getStatus(StatusFlags::V))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::BCC()
{
    if (!getStatus(StatusFlags::C))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::BCS()
{
    if (getStatus(StatusFlags::C))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::BNE()
{
    if (!getStatus(StatusFlags::Z))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::BEQ()
{
    if (getStatus(StatusFlags::Z))
    {
        IP = addr;
        ticks++;
    }
}

void R6502::CMP()
{
    uint8_t data = bus.get()->read(addr);
    setStatus(StatusFlags::C, reg_Acc >= data);         // if positive
    setStatus(StatusFlags::Z, reg_Acc == data);         // if zero
    setStatus(StatusFlags::N, (reg_Acc - data) & 0x80); // if negative MSB = 1
}

void R6502::CPX()
{
    uint8_t data = bus.get()->read(addr);
    setStatus(StatusFlags::C, reg_X >= data);           // if positive
    setStatus(StatusFlags::Z, reg_X == data);           // if zero
    setStatus(StatusFlags::N, (reg_X - data) & 0x80);   // if negative MSB = 1
}

void R6502::CPY()
{
    uint8_t data = bus.get()->read(addr);
    setStatus(StatusFlags::C, reg_Y >= data);           // if positive
    setStatus(StatusFlags::Z, reg_Y == data);           // if zero
    setStatus(StatusFlags::N, (reg_Y - data) & 0x80);   // if negative MSB = 1
}

void R6502::DEC()
{
    uint8_t data = bus.get()->read(addr);
    data--;
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    bus.get()->write(addr, data);
}

void R6502::EOR()
{
    uint8_t data = bus.get()->read(addr);
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
    uint8_t data = bus.get()->read(addr);
    data++;
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    bus.get()->write(addr, data);
}

void R6502::JMP()
{
    IP = addr;
}

void R6502::JSR()
{
    uint16_t addrToPush = IP - 1;   // since IP already pointed to next instruction, decrement the address to get pointer to byte before the next instruction
    stack_Push((addrToPush >> 8) & 0xFF); // first push the upper byte
    stack_Push(static_cast<uint8_t>(addrToPush & 0xFF));        // then push the lower byte (little endian)
    IP = addr;

    LOG(JSR_DEBUG, "Pushed to stack: " << addrToPush);
}

void R6502::LSR()
{
    uint8_t data = accumulator ? reg_Acc : bus.get()->read(addr); // handle accumulator cases

    setStatus(StatusFlags::N, false); // always clear negative
    setStatus(StatusFlags::C, data & 0x1);

    data >>= 1;
    data &= 0x7F; // make sure the 7th bit is zero

    setStatus(StatusFlags::Z, data == 0);

    if (accumulator) // handle accumulator case
    {
        reg_Acc = data;
        accumulator = false;
    }
}

void R6502::NOP()
{
    // do nothing
}

void R6502::ORA()
{
    uint8_t data = bus.get()->read(addr);
    reg_Acc |= data;
    setStatus(StatusFlags::N, reg_Acc & 0x80);
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::TAX()
{
    reg_X = reg_Acc;
    setStatus(StatusFlags::Z, reg_X == 0);
    setStatus(StatusFlags::N, reg_X & 0x80);
}

void R6502::TXA()
{
    reg_Acc = reg_X;
    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::DEX()
{
    reg_X--;
    setStatus(StatusFlags::Z, reg_X == 0);
    setStatus(StatusFlags::N, reg_X & 0x80);
}

void R6502::INCX()
{
    reg_X++;
    setStatus(StatusFlags::Z, reg_X == 0);
    setStatus(StatusFlags::N, reg_X & 0x80);
}

void R6502::TAY()
{
    reg_Y = reg_Acc;
    setStatus(StatusFlags::Z, reg_Y == 0);
    setStatus(StatusFlags::N, reg_Y & 0x80);
}

void R6502::TYA()
{
    reg_Acc = reg_Y;
    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::DEY()
{
    reg_Y--;
    setStatus(StatusFlags::Z, reg_Y == 0);
    setStatus(StatusFlags::N, reg_Y & 0x80);
}

void R6502::INCY()
{
    reg_Y++;
    setStatus(StatusFlags::Z, reg_Y == 0);
    setStatus(StatusFlags::N, reg_Y & 0x80);
}

void R6502::ROL()
{
    uint8_t data = accumulator ? reg_Acc : bus.get()->read(addr);
    uint8_t new_carry = data & 0x80;                // preserve the old carry
    data = (data << 1) | getStatus(StatusFlags::C); // add in the bit from carry
    setStatus(StatusFlags::C, new_carry);           // set the new carry bit back
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    if (accumulator)
    {
        reg_Acc = data;
        accumulator = false;
    }
    else
    {
        bus.get()->write(addr, data);
    }
}

void R6502::ROR()
{
    uint8_t data = accumulator ? reg_Acc : bus.get()->read(addr);
    uint8_t new_carry = data & 0x1;                        // preserve the old carry
    data = (data >> 1) | (getStatus(StatusFlags::C) << 7); // add in the bit from carry
    setStatus(StatusFlags::C, new_carry);                  // set the new carry bit back
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    if (accumulator)
    {
        reg_Acc = data;
        accumulator = false;
    }
    else
    {
        bus.get()->write(addr, data);
    }
}

void R6502::RTS()
{
    uint16_t retAddr = stack_Pop(); // pops the lower byte first
    retAddr |= stack_Pop() << 8;    // pops the next higher bytes
    retAddr++;
    IP = retAddr;
}

void R6502::STA()
{
    bus.get()->write(addr, reg_Acc);
}

void R6502::TXS()
{
    reg_Stack = static_cast<uint16_t>(reg_X | 0x100); // reg_X can only store values 0x00 - 0xFF and so does the stack logically,
                                                      // but we know the stack extends from 0x100 - 0x1FF so we OR with 0x100 to extend the reg_X value to correct page
}

void R6502::TSX()
{
    reg_X = reg_Stack & 0xFF; // again make sure the stack address is properly extracted to register X
    setStatus(StatusFlags::Z, reg_X == 0);
    setStatus(StatusFlags::N, reg_X & 0x80);
}

void R6502::PHA()
{
    stack_Push(reg_Acc);
}

void R6502::PLA()
{
    reg_Acc = stack_Pop();
    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::PHP()
{
    uint8_t pushStatus = reg_Status;
    pushStatus |= static_cast<StatusFlags>(StatusFlags::U | StatusFlags::B); // set the Unused and break flag in the pushed version of stack
    stack_Push(pushStatus);
}

void R6502::PLP()
{
    reg_Status = stack_Pop();
}

void R6502::BRK(void)
{
    // assert(IP > 0x200 && "Invalid program stack, exitting to prevent crashes");
    return; // ignore BRK for now
    IP++;
    uint8_t ret = IP + 1;
    stack_Push((ret >> 8) & 0xFF);                                              // push higher bits of address
    stack_Push(ret & 0xFF);                                                     // then push the lower address bits
    setStatus(static_cast<StatusFlags>(StatusFlags::B | StatusFlags::U), true); // set both the BRK and unused flags since this is a BRK instruction
    stack_Push(reg_Status);
    setStatus(StatusFlags::I, true); // disable interrupts
    IP = bus.get()->read(0xFFFE) | (bus.get()->read(0xFFFF) << 8);
}

void R6502::RTI(void)
{
    reg_Status = stack_Pop();              // get status to original
    IP = stack_Pop() | (stack_Pop() << 8); // reconstruct the pushed address and assign to IP
}

// -------------- ILLEGAL INSTRUCTIONS --------------
void R6502::JAM(void)
{
    running = false; // prevent CPU from running JAM the CPU
}

void R6502::SLO(void)
{
    uint8_t data = bus.get()->read(addr);
    setStatus(StatusFlags::C, data & 0x80);
    data <<= 1;

    reg_Acc |= data;
    setStatus(StatusFlags::N, reg_Acc & 0x80);
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::RLA(void)
{
    uint8_t data = bus.get()->read(addr);
    uint8_t old_carry = data & 0x80;                // preserve the old carry
    data = (data << 1) | getStatus(StatusFlags::C); // insert the new carry
    setStatus(StatusFlags::C, old_carry);           // set the new carry bit back
    setStatus(StatusFlags::N, data & 0x80);

    reg_Acc &= data;
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::ANC(void)
{
    uint8_t data = bus.get()->read(addr);
    reg_Acc &= data;

    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(static_cast<StatusFlags>(StatusFlags::N | StatusFlags::C), reg_Acc & 0x80);
}

void R6502::SRE(void)
{
    uint8_t data = bus.get()->read(addr);

    setStatus(StatusFlags::C, data & 0x1);

    data >>= 1;
    data &= 0x7F; // make sure the 7th bit is zero

    reg_Acc = reg_Acc ^ data;
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);
}

void R6502::RRA(void)
{
    uint8_t data = bus.get()->read(addr);
    uint8_t new_carry = data & 0x1;                        // preserve the old carry
    data = (data >> 1) | (getStatus(StatusFlags::C) << 7); // add in the bit from carry
    setStatus(StatusFlags::C, new_carry);                  // set the new carry bit back

    uint16_t sum = reg_Acc + data + getStatus(StatusFlags::C); // sum = Acc + fetched_data + Carry bit(1/0)

    // Overflow if: Accumulator and fetched_data have SAME SIGN but Accumulator and result have DIFFERENT SIGN
    setStatus(StatusFlags::V, (~(reg_Acc ^ data) & (reg_Acc ^ sum)) & 0x80);

    reg_Acc = static_cast<uint8_t>(sum & 0xFF); // make sure its 8-bit to fit within the Accumulator

    setStatus(StatusFlags::C, sum > 0xFF);
    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::ARR(void)
{
    uint8_t data = bus.get()->read(addr);
    reg_Acc &= data;

    uint8_t new_carry = data & 0x1;                              // preserve the old carry
    reg_Acc = (reg_Acc >> 1) | (getStatus(StatusFlags::C) << 7); // add in the bit from carry
    setStatus(StatusFlags::C, new_carry);                        // set the new carry bit back

    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
    setStatus(StatusFlags::V, (reg_Acc & 0x40) ^ (reg_Acc & 0x20)); // XOR of bit 6 and bit 5
}

void R6502::SAX(void)
{
    uint8_t data = reg_Acc & reg_X;
    setStatus(StatusFlags::Z, data == 0);
    setStatus(StatusFlags::N, data & 0x80);

    bus.get()->write(addr, data);
}

void R6502::SHX(void)
{
    uint8_t highByte = (addr >> 8) & 0xFF; // high byte of address
    uint8_t data = reg_Y & (highByte + 1);

    bus.get()->write(addr, data);
}

void R6502::SHA(void)
{
    uint8_t highByte = (addr >> 8) & 0xFF; // high byte of address
    uint8_t data = reg_Acc & reg_X & (highByte + 1);

    bus.get()->write(addr, data);
}

void R6502::SHY(void)
{
    uint8_t highByte = (addr >> 8) & 0xFF; // high byte of address
    uint8_t data = reg_Y & reg_X & (highByte + 1);

    bus.get()->write(addr, data);
}

void R6502::LAX(void)
{
    uint8_t data = bus.get()->read(addr);
    reg_Acc = data;
    reg_X = data;

    setStatus(StatusFlags::N, data & 0x80);
    setStatus(StatusFlags::Z, data == 0);
}

void R6502::LAS(void)
{
    uint8_t data = bus.get()->read(addr);
    reg_Acc = data & reg_Stack;
    reg_X = reg_Acc;
    reg_Stack = reg_Acc;

    setStatus(StatusFlags::N, reg_Acc & 0x80);
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::DCP(void)
{
    uint8_t data = bus.get()->read(addr) - 1;                                   // decrement
    setStatus(StatusFlags::C, reg_Acc >= data);                                 // if positive
    setStatus((StatusFlags)(StatusFlags::C | StatusFlags::Z), reg_Acc == data); // if zero
    setStatus(StatusFlags::N, (reg_Acc - data) & 0x80);                         // if negative MSB = 1
}

void R6502::ISC(void)
{
    uint8_t data = bus.get()->read(addr) + 1; // increment

    // in 6502 subtraction is performed by 1's complement of data and then adding to Accumulator, then adding carry as in a borrow scheme
    // the calculation actually extends to => Acc = Acc - data - (1 - Carry) => Acc = Acc + (~data) + Carry
    uint16_t sum = reg_Acc + (~data) + getStatus(StatusFlags::C); // sum = Acc + (~fetched_data) + Carry bit(1/0)

    // Overflow if: inverse of Accumulator and fetched_data have DIFFERENT SIGN respectively but Accumulator and result have DIFFERENT SIGN
    setStatus(StatusFlags::V, ((~reg_Acc ^ data) & (reg_Acc ^ sum)) & 0x80);
    setStatus(StatusFlags::C, reg_Acc >= data); // no borrow if Accumulator is greater than operand (obviously)

    reg_Acc = static_cast<uint8_t>(sum & 0xFF); // make sure its 8-bit to fit within the Accumulator

    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::ALR(void)
{
    uint8_t data = bus.get()->read(addr);
    reg_Acc &= data;

    setStatus(StatusFlags::C, reg_Acc & 0x1); // save bit 0 in carry
    reg_Acc = (reg_Acc >> 1) & 0x7F;          // Right shift and Zeroed high bit

    setStatus(StatusFlags::N, false); // always zero due to the shift
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::USBC(void)
{
    uint8_t data = bus.get()->read(addr);

    // in 6502 subtraction is performed by 1's complement of data and then adding to Accumulator, then adding carry as in a borrow scheme
    // the calculation actually extends to => Acc = Acc - data - (1 - Carry) => Acc = Acc + (~data) + Carry
    uint16_t sum = reg_Acc + (~data) + getStatus(StatusFlags::C); // sum = Acc + (~fetched_data) + Carry bit(1/0)

    // Overflow if: inverse of Accumulator and fetched_data have DIFFERENT SIGN respectively but Accumulator and result have DIFFERENT SIGN
    setStatus(StatusFlags::V, ((~reg_Acc ^ data) & (reg_Acc ^ sum)) & 0x80);
    setStatus(StatusFlags::C, reg_Acc >= data); // no borrow if Accumulator is greater than operand (obviously)

    reg_Acc = static_cast<uint8_t>(sum & 0xFF); // make sure its 8-bit to fit within the Accumulator

    setStatus(StatusFlags::Z, reg_Acc == 0);
    setStatus(StatusFlags::N, reg_Acc & 0x80);
}

void R6502::ANE(void)
{
    uint_fast32_t randomIndex = rand() % 8;
    uint8_t randomValue = randomIndex == 0 ? reg_Acc : ANE_randomValues[randomIndex - 1]; // get a random value for ANE

    uint8_t data = bus.get()->read(addr);

    reg_Acc = randomValue & reg_X & data;

    setStatus(StatusFlags::N, reg_Acc & 0x80);
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::LXA(void)
{
    uint_fast32_t randomIndex = rand() % 8;
    uint8_t randomValue = randomIndex == 0 ? reg_Acc : ANE_randomValues[randomIndex - 1]; // get a random value for ANE

    uint8_t data = bus.get()->read(addr);

    reg_Acc = randomValue & data;
    reg_X = reg_Acc;

    setStatus(StatusFlags::N, reg_Acc & 0x80);
    setStatus(StatusFlags::Z, reg_Acc == 0);
}

void R6502::TAS(void)
{
    uint8_t data = reg_Acc & reg_X;
    bus.get()->write(reg_Stack, data);

    data &= ((addr >> 8) & 0xFF) + 1;

    bus.get()->write(addr, data);
}

void R6502::SBX(void)
{
    uint8_t data = bus.get()->read(addr);
    reg_X &= reg_Acc;
    setStatus(StatusFlags::C, reg_X >= data);

    reg_X -= data;
    setStatus(StatusFlags::N, reg_X & 0x80);
    setStatus(StatusFlags::Z, reg_X == 0);
}

// unimplemented illegal instructions
void R6502::UNIMPL(void) {} // this is an instruction completely unimplemented and probably does something wrong to the CPU, so we instead do nothing in the emulator