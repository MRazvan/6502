#include "globals.h"
#include "instructions.h"
#include "cpu.h"

/*************************************************************/
/*************************************************************/
/******************* CPU ADDRESSING MODES ********************/
/*************************************************************/
/*************************************************************/
void    _cpu_addr_absolute(u8);
void    _cpu_addr_absolute_y(u8);
void    _cpu_addr_absolute_x(u8);
void    _cpu_addr_accumulator(u8);
void    _cpu_addr_immediate(u8);
void    _cpu_addr_implied(u8);
void    _cpu_addr_indirect_x(u8);
void    _cpu_addr_indirect_y(u8);
void    _cpu_addr_indirect(u8);
void    _cpu_addr_relative(u8);
void    _cpu_addr_zeropage(u8);
void    _cpu_addr_zeropage_x(u8);
void    _cpu_addr_zeropage_y(u8);

/*************************************************************/
/*************************************************************/
/********************** HELPER DEFINES ***********************/
/*************************************************************/
/*************************************************************/
#define WRAP(val)                    ((val) & 0xFF)
#define READ_ZP(addr)                (BUS_READ(WRAP(addr)))
#define READ16_ZP(addr)              ( READ_ZP(addr) | (READ_ZP(addr+1) << 8))
#define READ16_WRAP(addr)            (BUS_READ(addr) | ( BUS_READ(((addr) & 0xFF00) | WRAP(addr+1)) << 8))
#define SET_MODE(mode)               CPU.Mode = mode

#define ADDR_ABSOLUTE                _cpu_addr_absolute
#define ADDR_ABSOLUTE_X              _cpu_addr_absolute_x
#define ADDR_ABSOLUTE_Y              _cpu_addr_absolute_y
#define ADDR_ACCUMULATOR             _cpu_addr_accumulator
#define ADDR_IMMEDIATE               _cpu_addr_immediate
#define ADDR_IMPLIED                 _cpu_addr_implied
#define ADDR_INDIRECT_X              _cpu_addr_indirect_x
#define ADDR_INDIRECT                _cpu_addr_indirect
#define ADDR_INDIRECT_Y              _cpu_addr_indirect_y
#define ADDR_RELATIVE                _cpu_addr_relative
#define ADDR_ZEROPAGE                _cpu_addr_zeropage
#define ADDR_ZEROPAGE_X              _cpu_addr_zeropage_x
#define ADDR_ZEROPAGE_Y              _cpu_addr_zeropage_y

#define PAGE_DIFFER(a, b)            (!!(((a) ^ (b)) & 0xFF00))

/*************************************************************/
/*************************************************************/
/******************** CPU IMPLEMENTATION *********************/
/*************************************************************/
/*************************************************************/

void _cpu_nmi() {
    PUSH16(PC);
    PHP();
    PC = READ16(INTERRUPT_NMI_VECTOR);
    FI = 1;
    CPU.Cycles += 7;
}

void _cpu_irq() {
    PUSH16(PC);
    PHP();
    PC = READ16(INTERRUPT_IRQ_VECTOR);
    FI = 1;
    CPU.Cycles += 7;
}

void _cpu_init() {
    CPU.LastOpCode = 0;
}

void _cpu_reset() {
    FLAGS = 0x24;
    A = X = Y = 0x00;
    SP = 0xFD;
    PC = READ16(INTERRUPT_RST_VECTOR);
}

u32 _cpu_step() {
#ifdef DEBUG
    #define DEBUGINSTR(id, mnemonic) debug_nl("0x%04X\t%02X - %s\t\tA=%02x X=%02x Y=%02x P=%02x SP=%02x CYC=%d", PC, id, mnemonic, A, X, Y, FLAGS, SP, CPU.Cycles);
    #define INSTR_CASE(id, mnemonic, addr_handler, size, cycles, page_cycles, handler) case id : { DEBUGINSTR(id, mnemonic); addr_handler(page_cycles); handler(); cpu_cycles = cycles; break; }
#else
    #define INSTR_CASE(id, mnemonic, addr_handler, size, cycles, page_cycles, handler) case id : { addr_handler(page_cycles); handler(); cpu_cycles = cycles; break; }
#endif

    CPU.LastOpCode = BUS_READ(PC);
    u8 cpu_cycles = 0;
    switch (CPU.LastOpCode)
    {
        INSTR_CASE( 0x00, "BRK", ADDR_IMPLIED,        2, 7, 0, BRK )
        INSTR_CASE( 0x01, "ORA", ADDR_INDIRECT_X,     2, 6, 0, ORA )
        INSTR_CASE( 0x02, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x03, "SLO", ADDR_INDIRECT_X,     2, 8, 0, SLO )
        INSTR_CASE( 0x04, "NOP", ADDR_ZEROPAGE,       2, 3, 0, NOP )
        INSTR_CASE( 0x05, "ORA", ADDR_ZEROPAGE,       2, 3, 0, ORA )
        INSTR_CASE( 0x06, "ASL", ADDR_ZEROPAGE,       2, 5, 0, ASL )
        INSTR_CASE( 0x07, "SLO", ADDR_ZEROPAGE,       2, 5, 0, SLO )
        INSTR_CASE( 0x08, "PHP", ADDR_IMPLIED,        1, 3, 0, PHP )
        INSTR_CASE( 0x09, "ORA", ADDR_IMMEDIATE,      2, 2, 0, ORA )
        INSTR_CASE( 0x0a, "ASL", ADDR_ACCUMULATOR,    1, 2, 0, ASL )
        INSTR_CASE( 0x0b, "ANC", ADDR_IMMEDIATE,      0, 2, 0, ANC )
        INSTR_CASE( 0x0c, "NOP", ADDR_ABSOLUTE,       3, 4, 0, NOP )
        INSTR_CASE( 0x0d, "ORA", ADDR_ABSOLUTE,       3, 4, 0, ORA )
        INSTR_CASE( 0x0e, "ASL", ADDR_ABSOLUTE,       3, 6, 0, ASL )
        INSTR_CASE( 0x0f, "SLO", ADDR_ABSOLUTE,       3, 6, 0, SLO )
        INSTR_CASE( 0x10, "BPL", ADDR_RELATIVE,       2, 2, 1, BPL )
        INSTR_CASE( 0x11, "ORA", ADDR_INDIRECT_Y,     2, 5, 1, ORA )
        INSTR_CASE( 0x12, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x13, "SLO", ADDR_INDIRECT_Y,     2, 8, 0, SLO )
        INSTR_CASE( 0x14, "NOP", ADDR_ZEROPAGE_X,     2, 4, 0, NOP )
        INSTR_CASE( 0x15, "ORA", ADDR_ZEROPAGE_X,     2, 4, 0, ORA )
        INSTR_CASE( 0x16, "ASL", ADDR_ZEROPAGE_X,     2, 6, 0, ASL )
        INSTR_CASE( 0x17, "SLO", ADDR_ZEROPAGE_X,     2, 6, 0, SLO )
        INSTR_CASE( 0x18, "CLC", ADDR_IMPLIED,        1, 2, 0, CLC )
        INSTR_CASE( 0x19, "ORA", ADDR_ABSOLUTE_Y,     3, 4, 1, ORA )
        INSTR_CASE( 0x1a, "NOP", ADDR_IMPLIED,        1, 2, 0, NOP )
        INSTR_CASE( 0x1b, "SLO", ADDR_ABSOLUTE_Y,     3, 7, 0, SLO )
        INSTR_CASE( 0x1c, "NOP", ADDR_ABSOLUTE_X,     3, 4, 1, NOP )
        INSTR_CASE( 0x1d, "ORA", ADDR_ABSOLUTE_X,     3, 4, 1, ORA )
        INSTR_CASE( 0x1e, "ASL", ADDR_ABSOLUTE_X,     3, 7, 0, ASL )
        INSTR_CASE( 0x1f, "SLO", ADDR_ABSOLUTE_X,     3, 7, 0, SLO )
        INSTR_CASE( 0x20, "JSR", ADDR_ABSOLUTE,       3, 6, 0, JSR )
        INSTR_CASE( 0x21, "AND", ADDR_INDIRECT_X,     2, 6, 0, AND )
        INSTR_CASE( 0x22, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x23, "RLA", ADDR_INDIRECT_X,     2, 8, 0, RLA )
        INSTR_CASE( 0x24, "BIT", ADDR_ZEROPAGE,       2, 3, 0, BIT )
        INSTR_CASE( 0x25, "AND", ADDR_ZEROPAGE,       2, 3, 0, AND )
        INSTR_CASE( 0x26, "ROL", ADDR_ZEROPAGE,       2, 5, 0, ROL )
        INSTR_CASE( 0x27, "RLA", ADDR_ZEROPAGE,       2, 5, 0, RLA )
        INSTR_CASE( 0x28, "PLP", ADDR_IMPLIED,        1, 4, 0, PLP )
        INSTR_CASE( 0x29, "AND", ADDR_IMMEDIATE,      2, 2, 0, AND )
        INSTR_CASE( 0x2a, "ROL", ADDR_ACCUMULATOR,    1, 2, 0, ROL )
        INSTR_CASE( 0x2b, "ANC", ADDR_IMMEDIATE,      0, 2, 0, ANC )
        INSTR_CASE( 0x2c, "BIT", ADDR_ABSOLUTE,       3, 4, 0, BIT )
        INSTR_CASE( 0x2d, "AND", ADDR_ABSOLUTE,       3, 4, 0, AND )
        INSTR_CASE( 0x2e, "ROL", ADDR_ABSOLUTE,       3, 6, 0, ROL )
        INSTR_CASE( 0x2f, "RLA", ADDR_ABSOLUTE,       3, 6, 0, RLA )
        INSTR_CASE( 0x30, "BMI", ADDR_RELATIVE,       2, 2, 1, BMI )
        INSTR_CASE( 0x31, "AND", ADDR_INDIRECT_Y,     2, 5, 1, AND )
        INSTR_CASE( 0x32, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x33, "RLA", ADDR_INDIRECT_Y,     2, 8, 0, RLA )
        INSTR_CASE( 0x34, "NOP", ADDR_ZEROPAGE_X,     2, 4, 0, NOP )
        INSTR_CASE( 0x35, "AND", ADDR_ZEROPAGE_X,     2, 4, 0, AND )
        INSTR_CASE( 0x36, "ROL", ADDR_ZEROPAGE_X,     2, 6, 0, ROL )
        INSTR_CASE( 0x37, "RLA", ADDR_ZEROPAGE_X,     2, 6, 0, RLA )
        INSTR_CASE( 0x38, "SEC", ADDR_IMPLIED,        1, 2, 0, SEC )
        INSTR_CASE( 0x39, "AND", ADDR_ABSOLUTE_Y,     3, 4, 1, AND )
        INSTR_CASE( 0x3a, "NOP", ADDR_IMPLIED,        1, 2, 0, NOP )
        INSTR_CASE( 0x3b, "RLA", ADDR_ABSOLUTE_Y,     3, 7, 0, RLA )
        INSTR_CASE( 0x3c, "NOP", ADDR_ABSOLUTE_X,     3, 4, 1, NOP )
        INSTR_CASE( 0x3d, "AND", ADDR_ABSOLUTE_X,     3, 4, 1, AND )
        INSTR_CASE( 0x3e, "ROL", ADDR_ABSOLUTE_X,     3, 7, 0, ROL )
        INSTR_CASE( 0x3f, "RLA", ADDR_ABSOLUTE_X,     3, 7, 0, RLA )
        INSTR_CASE( 0x40, "RTI", ADDR_IMPLIED,        1, 6, 0, RTI )
        INSTR_CASE( 0x41, "EOR", ADDR_INDIRECT_X,     2, 6, 0, EOR )
        INSTR_CASE( 0x42, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x43, "SRE", ADDR_INDIRECT_X,     2, 8, 0, SRE )
        INSTR_CASE( 0x44, "NOP", ADDR_ZEROPAGE,       2, 3, 0, NOP )
        INSTR_CASE( 0x45, "EOR", ADDR_ZEROPAGE,       2, 3, 0, EOR )
        INSTR_CASE( 0x46, "LSR", ADDR_ZEROPAGE,       2, 5, 0, LSR )
        INSTR_CASE( 0x47, "SRE", ADDR_ZEROPAGE,       2, 5, 0, SRE )
        INSTR_CASE( 0x48, "PHA", ADDR_IMPLIED,        1, 3, 0, PHA )
        INSTR_CASE( 0x49, "EOR", ADDR_IMMEDIATE,      2, 2, 0, EOR )
        INSTR_CASE( 0x4a, "LSR", ADDR_ACCUMULATOR,    1, 2, 0, LSR )
        INSTR_CASE( 0x4b, "ALR", ADDR_IMMEDIATE,      0, 2, 0, ALR )
        INSTR_CASE( 0x4c, "JMP", ADDR_ABSOLUTE,       3, 3, 0, JMP )
        INSTR_CASE( 0x4d, "EOR", ADDR_ABSOLUTE,       3, 4, 0, EOR )
        INSTR_CASE( 0x4e, "LSR", ADDR_ABSOLUTE,       3, 6, 0, LSR )
        INSTR_CASE( 0x4f, "SRE", ADDR_ABSOLUTE,       3, 6, 0, SRE )
        INSTR_CASE( 0x50, "BVC", ADDR_RELATIVE,       2, 2, 1, BVC )
        INSTR_CASE( 0x51, "EOR", ADDR_INDIRECT_Y,     2, 5, 1, EOR )
        INSTR_CASE( 0x52, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x53, "SRE", ADDR_INDIRECT_Y,     2, 8, 0, SRE )
        INSTR_CASE( 0x54, "NOP", ADDR_ZEROPAGE_X,     2, 4, 0, NOP )
        INSTR_CASE( 0x55, "EOR", ADDR_ZEROPAGE_X,     2, 4, 0, EOR )
        INSTR_CASE( 0x56, "LSR", ADDR_ZEROPAGE_X,     2, 6, 0, LSR )
        INSTR_CASE( 0x57, "SRE", ADDR_ZEROPAGE_X,     2, 6, 0, SRE )
        INSTR_CASE( 0x58, "CLI", ADDR_IMPLIED,        1, 2, 0, CLI )
        INSTR_CASE( 0x59, "EOR", ADDR_ABSOLUTE_Y,     3, 4, 1, EOR )
        INSTR_CASE( 0x5a, "NOP", ADDR_IMPLIED,        1, 2, 0, NOP )
        INSTR_CASE( 0x5b, "SRE", ADDR_ABSOLUTE_Y,     3, 7, 0, SRE )
        INSTR_CASE( 0x5c, "NOP", ADDR_ABSOLUTE_X,     3, 4, 1, NOP )
        INSTR_CASE( 0x5d, "EOR", ADDR_ABSOLUTE_X,     3, 4, 1, EOR )
        INSTR_CASE( 0x5e, "LSR", ADDR_ABSOLUTE_X,     3, 7, 0, LSR )
        INSTR_CASE( 0x5f, "SRE", ADDR_ABSOLUTE_X,     3, 7, 0, SRE )
        INSTR_CASE( 0x60, "RTS", ADDR_IMPLIED,        1, 6, 0, RTS )
        INSTR_CASE( 0x61, "ADC", ADDR_INDIRECT_X,     2, 6, 0, ADC )
        INSTR_CASE( 0x62, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x63, "RRA", ADDR_INDIRECT_X,     2, 8, 0, RRA )
        INSTR_CASE( 0x64, "NOP", ADDR_ZEROPAGE,       2, 3, 0, NOP )
        INSTR_CASE( 0x65, "ADC", ADDR_ZEROPAGE,       2, 3, 0, ADC )
        INSTR_CASE( 0x66, "ROR", ADDR_ZEROPAGE,       2, 5, 0, ROR )
        INSTR_CASE( 0x67, "RRA", ADDR_ZEROPAGE,       2, 5, 0, RRA )
        INSTR_CASE( 0x68, "PLA", ADDR_IMPLIED,        1, 4, 0, PLA )
        INSTR_CASE( 0x69, "ADC", ADDR_IMMEDIATE,      2, 2, 0, ADC )
        INSTR_CASE( 0x6a, "ROR", ADDR_ACCUMULATOR,    1, 2, 0, ROR )
        INSTR_CASE( 0x6b, "ARR", ADDR_IMMEDIATE,      0, 2, 0, ARR )
        INSTR_CASE( 0x6c, "JMP", ADDR_INDIRECT,       3, 5, 0, JMP )
        INSTR_CASE( 0x6d, "ADC", ADDR_ABSOLUTE,       3, 4, 0, ADC )
        INSTR_CASE( 0x6e, "ROR", ADDR_ABSOLUTE,       3, 6, 0, ROR )
        INSTR_CASE( 0x6f, "RRA", ADDR_ABSOLUTE,       3, 6, 0, RRA )
        INSTR_CASE( 0x70, "BVS", ADDR_RELATIVE,       2, 2, 1, BVS )
        INSTR_CASE( 0x71, "ADC", ADDR_INDIRECT_Y,     2, 5, 1, ADC )
        INSTR_CASE( 0x72, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x73, "RRA", ADDR_INDIRECT_Y,     2, 8, 0, RRA )
        INSTR_CASE( 0x74, "NOP", ADDR_ZEROPAGE_X,     2, 4, 0, NOP )
        INSTR_CASE( 0x75, "ADC", ADDR_ZEROPAGE_X,     2, 4, 0, ADC )
        INSTR_CASE( 0x76, "ROR", ADDR_ZEROPAGE_X,     2, 6, 0, ROR )
        INSTR_CASE( 0x77, "RRA", ADDR_ZEROPAGE_X,     2, 6, 0, RRA )
        INSTR_CASE( 0x78, "SEI", ADDR_IMPLIED,        1, 2, 0, SEI )
        INSTR_CASE( 0x79, "ADC", ADDR_ABSOLUTE_Y,     3, 4, 1, ADC )
        INSTR_CASE( 0x7a, "NOP", ADDR_IMPLIED,        1, 2, 0, NOP )
        INSTR_CASE( 0x7b, "RRA", ADDR_ABSOLUTE_Y,     3, 7, 0, RRA )
        INSTR_CASE( 0x7c, "NOP", ADDR_ABSOLUTE_X,     3, 4, 1, NOP )
        INSTR_CASE( 0x7d, "ADC", ADDR_ABSOLUTE_X,     3, 4, 1, ADC )
        INSTR_CASE( 0x7e, "ROR", ADDR_ABSOLUTE_X,     3, 7, 0, ROR )
        INSTR_CASE( 0x7f, "RRA", ADDR_ABSOLUTE_X,     3, 7, 0, RRA )
        INSTR_CASE( 0x80, "NOP", ADDR_IMMEDIATE,      2, 2, 0, NOP )
        INSTR_CASE( 0x81, "STA", ADDR_INDIRECT_X,     2, 6, 0, STA )
        INSTR_CASE( 0x82, "NOP", ADDR_IMMEDIATE,      0, 2, 0, NOP )
        INSTR_CASE( 0x83, "SAX", ADDR_INDIRECT_X,     2, 6, 0, SAX )
        INSTR_CASE( 0x84, "STY", ADDR_ZEROPAGE,       2, 3, 0, STY )
        INSTR_CASE( 0x85, "STA", ADDR_ZEROPAGE,       2, 3, 0, STA )
        INSTR_CASE( 0x86, "STX", ADDR_ZEROPAGE,       2, 3, 0, STX )
        INSTR_CASE( 0x87, "SAX", ADDR_ZEROPAGE,       2, 3, 0, SAX )
        INSTR_CASE( 0x88, "DEY", ADDR_IMPLIED,        1, 2, 0, DEY )
        INSTR_CASE( 0x89, "NOP", ADDR_IMMEDIATE,      0, 2, 0, NOP )
        INSTR_CASE( 0x8a, "TXA", ADDR_IMPLIED,        1, 2, 0, TXA )
        INSTR_CASE( 0x8b, "XAA", ADDR_IMMEDIATE,      0, 2, 0, XAA )
        INSTR_CASE( 0x8c, "STY", ADDR_ABSOLUTE,       3, 4, 0, STY )
        INSTR_CASE( 0x8d, "STA", ADDR_ABSOLUTE,       3, 4, 0, STA )
        INSTR_CASE( 0x8e, "STX", ADDR_ABSOLUTE,       3, 4, 0, STX )
        INSTR_CASE( 0x8f, "SAX", ADDR_ABSOLUTE,       3, 4, 0, SAX )
        INSTR_CASE( 0x90, "BCC", ADDR_RELATIVE,       2, 2, 1, BCC )
        INSTR_CASE( 0x91, "STA", ADDR_INDIRECT_Y,     2, 6, 0, STA )
        INSTR_CASE( 0x92, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0x93, "AHX", ADDR_INDIRECT_Y,     0, 6, 0, AHX )
        INSTR_CASE( 0x94, "STY", ADDR_ZEROPAGE_X,     2, 4, 0, STY )
        INSTR_CASE( 0x95, "STA", ADDR_ZEROPAGE_X,     2, 4, 0, STA )
        INSTR_CASE( 0x96, "STX", ADDR_ZEROPAGE_Y,     2, 4, 0, STX )
        INSTR_CASE( 0x97, "SAX", ADDR_ZEROPAGE_Y,     2, 4, 0, SAX )
        INSTR_CASE( 0x98, "TYA", ADDR_IMPLIED,        1, 2, 0, TYA )
        INSTR_CASE( 0x99, "STA", ADDR_ABSOLUTE_Y,     3, 5, 0, STA )
        INSTR_CASE( 0x9a, "TXS", ADDR_IMPLIED,        1, 2, 0, TXS )
        INSTR_CASE( 0x9b, "TAS", ADDR_ABSOLUTE_Y,     0, 5, 0, TAS )
        INSTR_CASE( 0x9c, "SHY", ADDR_ABSOLUTE_X,     0, 5, 0, SHY )
        INSTR_CASE( 0x9d, "STA", ADDR_ABSOLUTE_X,     3, 5, 0, STA )
        INSTR_CASE( 0x9e, "SHX", ADDR_ABSOLUTE_Y,     0, 5, 0, SHX )
        INSTR_CASE( 0x9f, "AHX", ADDR_ABSOLUTE_Y,     0, 5, 0, AHX )
        INSTR_CASE( 0xa0, "LDY", ADDR_IMMEDIATE,      2, 2, 0, LDY )
        INSTR_CASE( 0xa1, "LDA", ADDR_INDIRECT_X,     2, 6, 0, LDA )
        INSTR_CASE( 0xa2, "LDX", ADDR_IMMEDIATE,      2, 2, 0, LDX )
        INSTR_CASE( 0xa3, "LAX", ADDR_INDIRECT_X,     2, 6, 0, LAX )
        INSTR_CASE( 0xa4, "LDY", ADDR_ZEROPAGE,       2, 3, 0, LDY )
        INSTR_CASE( 0xa5, "LDA", ADDR_ZEROPAGE,       2, 3, 0, LDA )
        INSTR_CASE( 0xa6, "LDX", ADDR_ZEROPAGE,       2, 3, 0, LDX )
        INSTR_CASE( 0xa7, "LAX", ADDR_ZEROPAGE,       2, 3, 0, LAX )
        INSTR_CASE( 0xa8, "TAY", ADDR_IMPLIED,        1, 2, 0, TAY )
        INSTR_CASE( 0xa9, "LDA", ADDR_IMMEDIATE,      2, 2, 0, LDA )
        INSTR_CASE( 0xaa, "TAX", ADDR_IMPLIED,        1, 2, 0, TAX )
        INSTR_CASE( 0xab, "LAX", ADDR_IMMEDIATE,      0, 2, 0, LAX )
        INSTR_CASE( 0xac, "LDY", ADDR_ABSOLUTE,       3, 4, 0, LDY )
        INSTR_CASE( 0xad, "LDA", ADDR_ABSOLUTE,       3, 4, 0, LDA )
        INSTR_CASE( 0xae, "LDX", ADDR_ABSOLUTE,       3, 4, 0, LDX )
        INSTR_CASE( 0xaf, "LAX", ADDR_ABSOLUTE,       3, 4, 0, LAX )
        INSTR_CASE( 0xb0, "BCS", ADDR_RELATIVE,       2, 2, 1, BCS )
        INSTR_CASE( 0xb1, "LDA", ADDR_INDIRECT_Y,     2, 5, 1, LDA )
        INSTR_CASE( 0xb2, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0xb3, "LAX", ADDR_INDIRECT_Y,     2, 5, 1, LAX )
        INSTR_CASE( 0xb4, "LDY", ADDR_ZEROPAGE_X,     2, 4, 0, LDY )
        INSTR_CASE( 0xb5, "LDA", ADDR_ZEROPAGE_X,     2, 4, 0, LDA )
        INSTR_CASE( 0xb6, "LDX", ADDR_ZEROPAGE_Y,     2, 4, 0, LDX )
        INSTR_CASE( 0xb7, "LAX", ADDR_ZEROPAGE_Y,     2, 4, 0, LAX )
        INSTR_CASE( 0xb8, "CLV", ADDR_IMPLIED,        1, 2, 0, CLV )
        INSTR_CASE( 0xb9, "LDA", ADDR_ABSOLUTE_Y,     3, 4, 1, LDA )
        INSTR_CASE( 0xba, "TSX", ADDR_IMPLIED,        1, 2, 0, TSX )
        INSTR_CASE( 0xbb, "LAS", ADDR_ABSOLUTE_Y,     0, 4, 1, LAS )
        INSTR_CASE( 0xbc, "LDY", ADDR_ABSOLUTE_X,     3, 4, 1, LDY )
        INSTR_CASE( 0xbd, "LDA", ADDR_ABSOLUTE_X,     3, 4, 1, LDA )
        INSTR_CASE( 0xbe, "LDX", ADDR_ABSOLUTE_Y,     3, 4, 1, LDX )
        INSTR_CASE( 0xbf, "LAX", ADDR_ABSOLUTE_Y,     3, 4, 1, LAX )
        INSTR_CASE( 0xc0, "CPY", ADDR_IMMEDIATE,      2, 2, 0, CPY )
        INSTR_CASE( 0xc1, "CMP", ADDR_INDIRECT_X,     2, 6, 0, CMP )
        INSTR_CASE( 0xc2, "NOP", ADDR_IMMEDIATE,      0, 2, 0, NOP )
        INSTR_CASE( 0xc3, "DCP", ADDR_INDIRECT_X,     2, 8, 0, DCP )
        INSTR_CASE( 0xc4, "CPY", ADDR_ZEROPAGE,       2, 3, 0, CPY )
        INSTR_CASE( 0xc5, "CMP", ADDR_ZEROPAGE,       2, 3, 0, CMP )
        INSTR_CASE( 0xc6, "DEC", ADDR_ZEROPAGE,       2, 5, 0, DEC )
        INSTR_CASE( 0xc7, "DCP", ADDR_ZEROPAGE,       2, 5, 0, DCP )
        INSTR_CASE( 0xc8, "INY", ADDR_IMPLIED,        1, 2, 0, INY )
        INSTR_CASE( 0xc9, "CMP", ADDR_IMMEDIATE,      2, 2, 0, CMP )
        INSTR_CASE( 0xca, "DEX", ADDR_IMPLIED,        1, 2, 0, DEX )
        INSTR_CASE( 0xcb, "AXS", ADDR_IMMEDIATE,      0, 2, 0, AXS )
        INSTR_CASE( 0xcc, "CPY", ADDR_ABSOLUTE,       3, 4, 0, CPY )
        INSTR_CASE( 0xcd, "CMP", ADDR_ABSOLUTE,       3, 4, 0, CMP )
        INSTR_CASE( 0xce, "DEC", ADDR_ABSOLUTE,       3, 6, 0, DEC )
        INSTR_CASE( 0xcf, "DCP", ADDR_ABSOLUTE,       3, 6, 0, DCP )
        INSTR_CASE( 0xd0, "BNE", ADDR_RELATIVE,       2, 2, 1, BNE )
        INSTR_CASE( 0xd1, "CMP", ADDR_INDIRECT_Y,     2, 5, 1, CMP )
        INSTR_CASE( 0xd2, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0xd3, "DCP", ADDR_INDIRECT_Y,     2, 8, 0, DCP )
        INSTR_CASE( 0xd4, "NOP", ADDR_ZEROPAGE_X,     2, 4, 0, NOP )
        INSTR_CASE( 0xd5, "CMP", ADDR_ZEROPAGE_X,     2, 4, 0, CMP )
        INSTR_CASE( 0xd6, "DEC", ADDR_ZEROPAGE_X,     2, 6, 0, DEC )
        INSTR_CASE( 0xd7, "DCP", ADDR_ZEROPAGE_X,     2, 6, 0, DCP )
        INSTR_CASE( 0xd8, "CLD", ADDR_IMPLIED,        1, 2, 0, CLD )
        INSTR_CASE( 0xd9, "CMP", ADDR_ABSOLUTE_Y,     3, 4, 1, CMP )
        INSTR_CASE( 0xda, "NOP", ADDR_IMPLIED,        1, 2, 0, NOP )
        INSTR_CASE( 0xdb, "DCP", ADDR_ABSOLUTE_Y,     3, 7, 0, DCP )
        INSTR_CASE( 0xdc, "NOP", ADDR_ABSOLUTE_X,     3, 4, 1, NOP )
        INSTR_CASE( 0xdd, "CMP", ADDR_ABSOLUTE_X,     3, 4, 1, CMP )
        INSTR_CASE( 0xde, "DEC", ADDR_ABSOLUTE_X,     3, 7, 0, DEC )
        INSTR_CASE( 0xdf, "DCP", ADDR_ABSOLUTE_X,     3, 7, 0, DCP )
        INSTR_CASE( 0xe0, "CPX", ADDR_IMMEDIATE,      2, 2, 0, CPX )
        INSTR_CASE( 0xe1, "SBC", ADDR_INDIRECT_X,     2, 6, 0, SBC )
        INSTR_CASE( 0xe2, "NOP", ADDR_IMMEDIATE,      0, 2, 0, NOP )
        INSTR_CASE( 0xe3, "ISC", ADDR_INDIRECT_X,     2, 8, 0, ISC )
        INSTR_CASE( 0xe4, "CPX", ADDR_ZEROPAGE,       2, 3, 0, CPX )
        INSTR_CASE( 0xe5, "SBC", ADDR_ZEROPAGE,       2, 3, 0, SBC )
        INSTR_CASE( 0xe6, "INC", ADDR_ZEROPAGE,       2, 5, 0, INC )
        INSTR_CASE( 0xe7, "ISC", ADDR_ZEROPAGE,       2, 5, 0, ISC )
        INSTR_CASE( 0xe8, "INX", ADDR_IMPLIED,        1, 2, 0, INX )
        INSTR_CASE( 0xe9, "SBC", ADDR_IMMEDIATE,      2, 2, 0, SBC )
        INSTR_CASE( 0xea, "NOP", ADDR_IMPLIED,        1, 2, 0, NOP )
        INSTR_CASE( 0xeb, "SBC", ADDR_IMMEDIATE,      2, 2, 0, SBC ) // TODO: INVALID SBC - SHOULD BE ANOTHER INSTRUCTION TO SIMULATE THAT
        INSTR_CASE( 0xec, "CPX", ADDR_ABSOLUTE,       3, 4, 0, CPX )
        INSTR_CASE( 0xed, "SBC", ADDR_ABSOLUTE,       3, 4, 0, SBC )
        INSTR_CASE( 0xee, "INC", ADDR_ABSOLUTE,       3, 6, 0, INC )
        INSTR_CASE( 0xef, "ISC", ADDR_ABSOLUTE,       3, 6, 0, ISC )
        INSTR_CASE( 0xf0, "BEQ", ADDR_RELATIVE,       2, 2, 1, BEQ )
        INSTR_CASE( 0xf1, "SBC", ADDR_INDIRECT_Y,     2, 5, 1, SBC )
        INSTR_CASE( 0xf2, "KIL", ADDR_IMPLIED,        0, 2, 0, KIL )
        INSTR_CASE( 0xf3, "ISC", ADDR_INDIRECT_Y,     2, 8, 0, ISC )
        INSTR_CASE( 0xf4, "NOP", ADDR_ZEROPAGE_X,     2, 4, 0, NOP )
        INSTR_CASE( 0xf5, "SBC", ADDR_ZEROPAGE_X,     2, 4, 0, SBC )
        INSTR_CASE( 0xf6, "INC", ADDR_ZEROPAGE_X,     2, 6, 0, INC )
        INSTR_CASE( 0xf7, "ISC", ADDR_ZEROPAGE_X,     2, 6, 0, ISC )
        INSTR_CASE( 0xf8, "SED", ADDR_IMPLIED,        1, 2, 0, SED )
        INSTR_CASE( 0xf9, "SBC", ADDR_ABSOLUTE_Y,     3, 4, 1, SBC )
        INSTR_CASE( 0xfa, "NOP", ADDR_IMPLIED,        1, 2, 0, NOP )
        INSTR_CASE( 0xfb, "ISC", ADDR_ABSOLUTE_Y,     3, 7, 0, ISC )
        INSTR_CASE( 0xfc, "NOP", ADDR_ABSOLUTE_X,     3, 4, 1, NOP )
        INSTR_CASE( 0xfd, "SBC", ADDR_ABSOLUTE_X,     3, 4, 1, SBC )
        INSTR_CASE( 0xfe, "INC", ADDR_ABSOLUTE_X,     3, 7, 0, INC )
        INSTR_CASE( 0xff, "ISC", ADDR_ABSOLUTE_X,     3, 7, 0, ISC )
        default:
            break;
    }
    CPU.Cycles += cpu_cycles;
    return cpu_cycles;
}

void _cpu_push(u8 data) {
    BUS_WRITE(0x100 | SP,  data);
    SP--; // This depends on 8 bit data to wrap; If another type use SP = WRAP(SP-1);
}

u8 _cpu_pull() {
    SP++; // This depends on 8 bit data to wrap; If another type use SP = WRAP(SP+1);
    return BUS_READ(0x100 | SP);
}

void _cpu_setN(u8 data) {
    FN = (data >> 7);
}

void _cpu_setZ(u8 data) {
    FZ = (data == 0);
}

void _cpu_addBranchCycles(u16 addr) {
    CPU.Cycles++;
    if (PAGE_DIFFER(PC, addr)) {
        CPU.Cycles++;
    }
}

void    _cpu_triggerNMI() { 
    _cpu_nmi();
}

void    _cpu_triggerIRQ() { 
    _cpu_irq();
}

/*************************************************************/
/*************************************************************/
/********************* ADDRESSING MODES **********************/
/*************************************************************/
/*************************************************************/

// https://en.wikipedia.org/wiki/MOS_Technology_6502#Addressing
// Addressing modes also include 
//    implied (1 byte instructions); 
//    absolute (3 bytes); 
//    indexed absolute (3 bytes); 
//    indexed zero-page (2 bytes); 
//    relative (2 bytes); 
//    accumulator (1); 
//    indirect,x and indirect,y (2); 
//    and immediate (2)

void    _cpu_addr_absolute(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(ABSOLUTE);
    ADDRESS = READ16(PC + 1);

    PC += 3;
}

void    _cpu_addr_absolute_y(u8 pageCycles) {
    SET_MODE(ABSOLUTE_Y);
    ADDRESS = READ16(PC + 1) + Y;
    if (PAGE_DIFFER(ADDRESS - Y, ADDRESS)) {
        CPU.Cycles += pageCycles;
    };

    PC += 3;
}

void    _cpu_addr_absolute_x(u8 pageCycles) {
    SET_MODE(ABSOLUTE_X);
    ADDRESS = READ16(PC + 1) + X;
    if (PAGE_DIFFER(ADDRESS - X, ADDRESS)) {
        CPU.Cycles += pageCycles;
    }

    PC += 3;
}

void    _cpu_addr_accumulator(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(ACCUMULATOR);
    ADDRESS = 0;

    PC++;
}

void    _cpu_addr_immediate(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(IMMEDIATE);
    ADDRESS = PC + 1;

    PC += 2;
}

void    _cpu_addr_implied(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(IMPLIED);
    ADDRESS = 0;

    PC++;
}

void    _cpu_addr_indirect_x(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(INDIRECT_X);
    ADDRESS = READ16_ZP(BUS_READ(PC + 1) + X);

    PC += 2;
}

void    _cpu_addr_indirect_y(u8 pageCycles) {
    SET_MODE(INDIRECT_Y);
    ADDRESS = READ16_ZP(BUS_READ(PC + 1)) + Y;
    if (PAGE_DIFFER(ADDRESS - Y, ADDRESS)) {
        CPU.Cycles += pageCycles;
    }

    PC += 2;
}

void    _cpu_addr_indirect(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(INDIRECT);
    ADDRESS = READ16_WRAP(READ16(PC + 1));

    PC += 3;
}

void    _cpu_addr_relative(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(RELATIVE);
    ADDRESS = BUS_READ(PC + 1);
    if (ADDRESS < 0x80) {
        ADDRESS = PC + 2 + ADDRESS;
    }
    else {
        ADDRESS = PC + 2 + ADDRESS - 0x100;
    }

    PC += 2;
}

void    _cpu_addr_zeropage(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(ZEROPAGE);
    ADDRESS = BUS_READ(PC + 1);

    PC += 2;
}

void    _cpu_addr_zeropage_x(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(ZEROPAGE_X);
    ADDRESS = (BUS_READ(PC + 1) + X) & 0xFF;
    
    PC += 2;
}

void    _cpu_addr_zeropage_y(u8 pageCycles) {
    UNUSED(pageCycles);
    SET_MODE(ZEROPAGE_Y);
    ADDRESS = (BUS_READ(PC + 1) + Y) & 0xFF;

    PC += 2;
}
