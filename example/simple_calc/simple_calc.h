#ifndef __SIMPLE_CALC_H__
#define __SIMPLE_CALC_H__

#include "defines.h"

/*************************************************************/
/*************************************************************/
/************************** CPU RAM **************************/
/*************************************************************/
/*************************************************************/
#define RAM_SIZE                0xFFFF

extern u8 _ram_[RAM_SIZE];
extern u8 _input_key;

extern u8 _bus_read(u16 addr);
extern void _bus_write(u16 addr, u8 data);

#endif
