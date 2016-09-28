#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "user_defines.h"

/*************************************************************/
/*************************************************************/
/************************** HELPERS **************************/
/*************************************************************/
/*************************************************************/

#define UNUSED(v)               (void)v

#define PUSH16(v)               { _cpu_push((v) >> 8); _cpu_push((v) & 0xFF); }
#define PULL16                  ( _cpu_pull() | _cpu_pull() << 8 )
#define READ16(addr)            (BUS_READ(addr) | ((u16)BUS_READ(addr + 1) << 8))
#define GET_FLAGS               _cpu_getFlags()
#define SET_FLAGS(f)            _cpu_setFlags(f)
#define REG(reg)                CPU.reg
#define FLAG(val)               CPU.P.val
#define ADDRESS                 CPU.Address

#define PC                      REG(PC)
#define SP                      REG(SP)
#define A                       REG(A)
#define X                       REG(X)
#define Y                       REG(Y)
#define FLAGS                   REG(P).flags

#define FN                      FLAG(N)
#define FZ                      FLAG(Z)
#define FV                      FLAG(V)
#define FC                      FLAG(C)
#define FI                      FLAG(I)
#define FD                      FLAG(D)

#define INTERRUPT_NMI_VECTOR    0xFFFA
#define INTERRUPT_IRQ_VECTOR    0xFFFE
#define INTERRUPT_RST_VECTOR    0xFFFC

#endif 
