#pragma once

#include "cpu.h"
#include "simple_calc.h"

extern stCPU g_cpu;

#define CPU                      g_cpu

#define BUS_READ                 _bus_read
#define BUS_WRITE                _bus_write

void _console_init();