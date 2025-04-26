#ifndef INCLUDED_R6502_HPP
#define INCLUDED_R6502_HPP

#include <R6502Bus.hpp>
#include <vector>
#include <string>
#include <cstdint>

#define ENABLE_STACK_CHECKS // enables a check for each stack operation for checking stack overflow and underflows in the system, only logs a visual error and doesnt crash the system however

#define HIDE_TICKS_DISPLAY  // hides the ticks display for debugging purposes

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
    static R6502Bus bus;

    static std::vector<Instruction_t> Instructions;

    R6502();
    ~R6502() = default;

    static void setBus(R6502Bus bus);

    // system variables
    static int ticks;
    static uint32_t IP;
    static uint8_t instr;
    static uint8_t imm;
    static uint16_t addr;
    static bool accumulator;
    static bool indirect;
    static bool zeroIndirect;
    
    // registers
    static uint16_t reg_Stack;
    static uint8_t reg_X;
    static uint8_t reg_Y;
    static uint8_t reg_Acc;
    static uint8_t reg_Status;

    // status management
    enum StatusFlags {
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

    static void clock();

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
    static void TSB(void);
    static void ASL(void);
    static void RMB0(void);
    static void PHP(void);
    static void BBR0(void);
    static void BPL(void);
    static void TRB(void);
    static void RMB1(void);
    static void CLC(void);
    static void INC(void);
    static void BBR1(void);
    static void JSR(void);
    static void AND(void);
    static void BIT(void);
    static void ROL(void);
    static void RMB2(void);
    static void PLP(void);
    static void BBR2(void);
    static void BMI(void);
    static void RMB3(void);
    static void SEC(void);
    static void DEC(void);
    static void BBR3(void);
    static void RTI(void);
    static void EOR(void);
    static void LSR(void);
    static void RMB4(void);
    static void PHA(void);
    static void JMP(void);
    static void BBR4(void);
    static void BVC(void);
    static void RMB5(void);
    static void CLI(void);
    static void PHY(void);
    static void BBR5(void);
    static void RTS(void);
    static void ADC(void);
    static void STZ(void);
    static void ROR(void);
    static void RMB6(void);
    static void PLA(void);
    static void BBR6(void);
    static void BVS(void);
    static void RMB7(void);
    static void SEI(void);
    static void PLY(void);
    static void BBR7(void);
    static void BRA(void);
    static void STA(void);
    static void STY(void);
    static void SMB0(void);
    static void DEY(void);
    static void TXA(void);
    static void BBS0(void);
    static void BCC(void);
    static void SMB1(void);
    static void TYA(void);
    static void TXS(void);
    static void BBS1(void);
    static void LDY(void);
    static void LDA(void);
    static void SMB2(void);
    static void TAY(void);
    static void TAX(void);
    static void BBS2(void);
    static void BCS(void);
    static void SMB3(void);
    static void CLV(void);
    static void TSX(void);
    static void BBS3(void);
    static void CPY(void);
    static void CMP(void);
    static void SMB4(void);
    static void INCY(void);
    static void DEX(void);
    static void WAI(void);
    static void BBS4(void);
    static void BNE(void);
    static void SMB5(void);
    static void CLD(void);
    static void PHX(void);
    static void STP(void);
    static void BBS5(void);
    static void CPX(void);
    static void SBC(void);
    static void SMB6(void);
    static void INCX(void);
    static void NOP(void);
    static void BBS6(void);
    static void BEQ(void);
    static void SMB7(void);
    static void SED(void);
    static void PLX(void);
    static void BBS7(void);           
};

#endif // INCLUDED_R6502_HPP