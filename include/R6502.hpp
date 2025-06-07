#ifndef INCLUDED_R6502_HPP
#define INCLUDED_R6502_HPP

#include <R6502Bus.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <random>

#include <fstream>

#define ENABLE_STACK_CHECKS // enables a check for each stack operation for checking stack overflow and underflows in the system, only logs a visual error and doesnt crash the system however

#define HIDE_TICKS_DISPLAY   // hides the ticks display for debugging purposes
#define DISABLE_TIMING_TICKS // disables clock ticks required since slowing down the CPU is unnecessary

typedef struct Instruction
{
    std::string op;
    void (*addrmode)(void);
    void (*operation)(void);
    int cycles;
} Instruction_t;

class R6502
{
public:
    static std::vector<Instruction_t> Instructions;

    R6502() = delete;

    explicit R6502(std::unique_ptr<R6502Bus> &&bus);
    ~R6502();

    void setBus(std::unique_ptr<R6502Bus> &&bus);
    R6502Bus *getBus() const;

    // system variables
    static int total_cycles;
    static int ticks;
    static uint32_t IP;
    static uint8_t instr;
    static uint8_t imm;
    static uint16_t addr;
    static bool accumulator;
    static bool indirect;
    static bool zeroIndirect;

    static bool running;

    // registers
    static uint16_t reg_Stack;
    static uint8_t reg_X;
    static uint8_t reg_Y;
    static uint8_t reg_Acc;
    static uint8_t reg_Status;

    // status management
    enum StatusFlags
    {
        C = 1,
        Z = 1 << 1,
        I = 1 << 2,
        D = 1 << 3,
        B = 1 << 4,
        U = 1 << 5,
        V = 1 << 6,
        N = 1 << 7
    };

    static uint8_t setStatus(StatusFlags flag, bool value = true);
    static uint8_t toggleStatus(StatusFlags flag);

    static bool getStatus(StatusFlags flag);

    // stack manupulation
    static void stack_Push(uint8_t data);
    static uint8_t stack_Pop();

    // heartbeat of the CPU
    static void clock();

    // resets the entire CPU state back, but doesnt modify anything else outside CPU stuff, now even memory
    void reset();

    // interrupts
    static void NMI();
    static void IRQ();

    // addressing modes
    static void ACC();
    static void IMP();
    static void IMM();
    static void INX();
    static void INY();
    static void ZP0();
    static void ZPX();
    static void ZPY();
    static void ABS();
    static void ABX();
    static void ABY();
    static void REL();
    static void ZPI();
    static void JIND();
    static void ABIX();

    // instructions
    static void UNIMPL(void);
    static void STX(void);
    static void LDX(void);
    static void BRK(void);
    static void ORA(void);
    static void ASL(void);
    static void PHP(void);
    static void BPL(void);
    static void CLC(void);
    static void INC(void);
    static void JSR(void);
    static void AND(void);
    static void BIT(void);
    static void ROL(void);
    static void PLP(void);
    static void BMI(void);
    static void SEC(void);
    static void DEC(void);
    static void RTI(void);
    static void EOR(void);
    static void LSR(void);
    static void PHA(void);
    static void JMP(void);
    static void BVC(void);
    static void CLI(void);
    static void RTS(void);
    static void ADC(void);
    static void ROR(void);
    static void PLA(void);
    static void BVS(void);
    static void SEI(void);
    static void STA(void);
    static void STY(void);
    static void DEY(void);
    static void TXA(void);
    static void BCC(void);
    static void TYA(void);
    static void TXS(void);
    static void LDY(void);
    static void LDA(void);
    static void TAY(void);
    static void TAX(void);
    static void BCS(void);
    static void CLV(void);
    static void TSX(void);
    static void CPY(void);
    static void CMP(void);
    static void INCY(void);
    static void DEX(void);
    static void BNE(void);
    static void CLD(void);
    static void CPX(void);
    static void SBC(void);
    static void INCX(void);
    static void NOP(void);
    static void BEQ(void);
    static void SED(void);

    // illegal opcodes
    static void JAM(void);
    static void SLO(void);
    static void RLA(void);
    static void ANC(void);
    static void SRE(void);
    static void RRA(void);
    static void ARR(void);
    static void SAX(void);
    static void SHX(void);
    static void SHA(void);
    static void SHY(void);
    static void LAX(void);
    static void LAS(void);
    static void DCP(void);
    static void ISC(void);
    static void ALR(void);
    static void USBC(void);
    static void ANE(void);
    static void TAS(void);
    static void LXA(void);
    static void SBX(void);

private:
    static std::minstd_rand rand;
    static uint8_t ANE_randomValues[7];
    static std::unique_ptr<R6502Bus> bus;

    static std::ofstream logFile;
};

#endif // INCLUDED_R6502_HPP