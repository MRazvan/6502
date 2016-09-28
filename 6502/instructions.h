#pragma once

#include "cpu.h"
#include "defines.h"
#include "globals.h"


//// https://sourceforge.net/p/nintendulator/code/HEAD/tree/nintendulator/trunk/src/CPU.cpp

#define SET_N(v)                    _cpu_setN(v)
#define SET_Z(v)                    _cpu_setZ(v)
#define SETNZ(val)                  { SET_N(val); SET_Z(val); }
#define READ                        BUS_READ(ADDRESS)
#define WRITE(val)                  BUS_WRITE(ADDRESS, val)
#define PUSH(val)                   _cpu_push(val)
#define PULL                        _cpu_pull()
#define ADD_BRANCH_CYCLES(ADDRESS)  _cpu_addBranchCycles(ADDRESS);
#define _BV(val, bit)               ((val >> bit) & 1)
#define COMPARE(a, b)               { SETNZ(a - b); FC = a >= b; }
#define BRANCH(cond)                if (cond) {\
                                        ADD_BRANCH_CYCLES(ADDRESS);\
                                        PC = ADDRESS;\
                                    }

#define INSTR(mnemonic)             void mnemonic(void)
/*************************************************************/
/*************************************************************/
/**************** INSTRUCTION IMPLEMENTATION *****************/
/*************************************************************/
/*************************************************************/

INSTR(PLP) {
    FLAGS = (PULL & 0xEF) | 0x20;
}

INSTR(PHP) {
    PUSH(FLAGS | 0x10);
}

INSTR(CLC) { FC = 0; }
INSTR(SEC) { FC = 1; }
INSTR(CLI) { FI = 0; }
INSTR(SEI) { FI = 1; }
INSTR(CLV) { FV = 0; }
INSTR(CLD) { FD = 0; }
INSTR(SED) { FD = 1; }

INSTR(BMI) { BRANCH(FN); }
INSTR(BPL) { BRANCH(!FN); }

INSTR(BVS) { BRANCH(FV); }
INSTR(BVC) { BRANCH(!FV); }

INSTR(BCS) { BRANCH(FC); }
INSTR(BCC) { BRANCH(!FC); }

INSTR(BEQ) { BRANCH(FZ); }
INSTR(BNE) { BRANCH(!FZ); }

INSTR(JMP) {PC = ADDRESS; }

INSTR(BRK) {
    PUSH16(PC);
    PHP();
    SEI();
    PC = READ16(INTERRUPT_IRQ_VECTOR);
}

INSTR(RTS) {
    PC = PULL16 + 1;
}

INSTR(RTI) {
    PLP();
    PC = PULL16;
}

INSTR(JSR) {
    PUSH16(PC - 1);
    PC = ADDRESS;
}

INSTR(ORA) {
    A = A | READ;
    SETNZ(A);
}

INSTR(ASL) {
    if (CPU.Mode == ACCUMULATOR) {
        FC = _BV(A, 7);
        A <<= 1;
        SETNZ(A);
    }
    else {
        u8 val_8 = READ;
        FC = _BV(val_8, 7);
        val_8 <<= 1;
        WRITE(val_8);
        SETNZ(val_8);
    }
}

INSTR(AND) {
    A = A & READ;
    SETNZ(A);
}


INSTR(BIT) {
    u8 val_8 = READ;
    FV = _BV(val_8, 6);
    SET_Z(val_8 & A);
    SET_N(val_8);
}

INSTR(ROL) {
    u8 c = FC;
    if (CPU.Mode == ACCUMULATOR) {
        FC = _BV(A, 7);
        A = (A << 1) | c;
        SETNZ(A);
    }
    else {
        u8 val_8 = READ;
        FC = _BV(val_8, 7);
        val_8 = (val_8 << 1) | c;
        WRITE(val_8);
        SETNZ(val_8);
    }
}

INSTR(EOR) {
    A = A ^ READ;
    SETNZ(A);
}

INSTR(LSR) {
    if (CPU.Mode == ACCUMULATOR) {
        FC = A & 1;
        A >>= 1;
        SETNZ(A);
    }
    else {
        u8 val_8 = READ;
        FC = val_8 & 1;
        val_8 >>= 1;
        WRITE(val_8);
        SETNZ(val_8);
    }
}

INSTR(PHA) { PUSH(A); }

INSTR(ADC) {
    u8 val = READ;
    u16 sum = A + val + FC;
    FC = !!(sum & 0x100);
    FV = !!(~(A ^ val) & (A ^ sum) & 0x80);
    A = sum & 0xFF;
    SETNZ(A);
}

INSTR(ROR) {
    u8 c = FC;
    if (CPU.Mode == ACCUMULATOR) {
        FC = A & 1;
        A = (A >> 1) | (c << 7);
        SETNZ(A);
    }
    else {
        u8 val_8 = READ;
        FC = val_8 & 1;
        val_8 = (val_8 >> 1) | (c << 7);
        WRITE(val_8);
        SETNZ(val_8);
    }
}
INSTR(PLA) {
    A = PULL;
    SETNZ(A);
}

INSTR(STA) { WRITE(A); }
INSTR(STY) { WRITE(Y); }
INSTR(STX) { WRITE(X); }

INSTR(DEY) {
    Y--;
    SETNZ(Y);
}
INSTR(TXA) {
    A = X;
    SETNZ(A);
}

INSTR(TYA) {
    A = Y;
    SETNZ(A);
}

INSTR(TXS) { SP = X; }

INSTR(LDY) {
    Y = READ;
    SETNZ(Y);
}

INSTR(LDA) {
    A = READ;
    SETNZ(A);
}

INSTR(LDX) {
    X = READ;
    SETNZ(X);
}

INSTR(TAY) {
    Y = A;
    SETNZ(Y);
}
INSTR(TAX) {
    X = A;
    SETNZ(X);
}

INSTR(TSX) {
    X = SP;
    SETNZ(X);
}
INSTR(CPX) { COMPARE(X, READ); }
INSTR(CPY) { COMPARE(Y, READ); }
INSTR(CMP) { COMPARE(A, READ); }

INSTR(DEC) {
    u8 val_8 = READ - 1;
    WRITE(val_8);
    SETNZ(val_8);
}

INSTR(INY) {
    Y++;
    SETNZ(Y);
}

INSTR(DEX) {
    X--;
    SETNZ(X);
}

INSTR(SBC) {
    u8 val = READ;
    i16 sum = A + ~val + FC;
    FV = !!((A ^ val) & (A ^ sum) & 0x80);
    FC = !(sum & 0x100);
    A = sum & 0xFF;
    SETNZ(A);
}

INSTR(INC) {
    u8 val_8 = READ + 1;
    WRITE(val_8);
    SETNZ(val_8);
}

INSTR(INX) {
    X++;
    SETNZ(X);
}


INSTR(NOP) {/* Burn cycles */ }
INSTR(KIL) {/* Nothing to do ?? Halt the processor ?? */ }

INSTR(AXS) {/* Unofficial instr */ }
INSTR(TAS) {/* Unofficial instr */ }
INSTR(SHY) {/* Unofficial instr */ }
INSTR(SHX) {/* Unofficial instr */ }
INSTR(AHX) {/* Unofficial instr */ }
INSTR(XAA) {/* Unofficial instr */ }
INSTR(ARR) {/* Unofficial instr */ }
INSTR(ALR) {/* Unofficial instr */ }
INSTR(ANC) {/* Unofficial instr */ }
INSTR(LAS) {/* Unofficial instr */ }

INSTR(RRA) {
    /* Unofficial instr */
    // ROR + ADC
    u8 tmp = READ;
    u8 c = FC << 7;
    FC = tmp & 0x01;
    tmp = (tmp >> 1) | c;
    i16 r = A + tmp + FC;
    FV = !!(~(A ^ tmp) & (A ^ r) & 0x80);
    FC = !!(r & 0x100);
    A = r & 0xFF;
    SETNZ(A);
    WRITE(tmp);
}

INSTR(ISC) {
    /* Unofficial instr */
    // INC + SBC
    u8 tmp = READ;
    tmp++;
    i16 r = A + ~tmp + FC;
    FV = !!((A ^ tmp) & (A ^ r) & 0x80);
    FC = !(r & 0x100);
    A = r & 0xFF;
    SETNZ(A);
    WRITE(tmp);
}

INSTR(DCP) {
    /* Unofficial instr */
    // DEC + CMP
    u8 tmp = READ;
    tmp--;
    i16 r = A - tmp;
    SETNZ(r & 0xFF);
    FC = r >= 0;
    WRITE(tmp);
}

INSTR(LAX) {
    /* Unofficial instr */
    // LDA + LDX
    A = X = READ;
    SETNZ(A);
}

INSTR(SAX) {
    /* Unofficial instr */
    // STA + STX
    WRITE(A&X);
}

INSTR(SRE) {
    /* Unofficial instr */
    // LSR + EOR
    u8 tmp = READ;
    FC = tmp & 0x01;
    tmp >>= 1;
    A ^= tmp;
    SETNZ(A);
    WRITE(tmp);
}

INSTR(RLA) {
    /* Unofficial instr */
    // ROL + AND
    u8 tmp = READ;
    u8 c = FC;
    FC = _BV(tmp, 7);
    tmp = (tmp << 1) | c;
    A &= tmp;
    SETNZ(A);
    WRITE(tmp);
}

INSTR(SLO) {
    /* Unofficial instr */
    // ASL + ORA
    u8 tmp = READ;
    FC = _BV(tmp, 7);
    tmp <<= 1;
    A |= tmp;
    SETNZ(A);
    WRITE(tmp);
}
