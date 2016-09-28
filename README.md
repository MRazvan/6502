# 6502
6502 emulator

In order to use it
Create a "user_defines.h" header file that defines the following (see example)

BUS_READ  - of type uint8_t(uint16_t addr)
BUS_WRITE - of type void   (uint16_t data, uint8_t data)

CPU       - pointing to the variable used as CPU

```c++
extern stCPU g_cpu;

#define CPU                      g_cpu

#define BUS_READ                 _bus_read
#define BUS_WRITE                _bus_write
```