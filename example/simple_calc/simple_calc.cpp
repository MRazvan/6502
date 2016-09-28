#include "simple_calc.h"
#include "defines.h"
#include <stdio.h>

/*************************************************************/
/*************************************************************/
/************************** CPU RAM **************************/
/*************************************************************/
/*************************************************************/
u8 _ram_[RAM_SIZE];

u8 _input_key;

void out(u8 data) {
    printf("%c", data);
}

u8 _bus_read(u16 addr)
{
    // 0xF004 - input
    if (addr == 0xF004) {
        u8 tmp = _input_key;
        _input_key = 0;
        return tmp;
    }

    return (addr < RAM_SIZE) ? _ram_[addr] : 0;
}

void _bus_write(u16 addr, u8 data) {
    // 0xF001 - output
    if (addr == 0xF001) {
        out(data);
    }
    else if (addr < RAM_SIZE) {
        _ram_[addr] = data;
    }
}