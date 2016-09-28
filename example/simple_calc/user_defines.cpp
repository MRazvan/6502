#include "user_defines.h"
#include "ehrom.h"
#include "simple_calc.h"

stCPU g_cpu;

void _console_init() {
    _cpu_init();

    // Not using memcpy, maybe on a different platform there is no memcpy, also it is happening in init so no performance problem
    for (int i = 0; i < sizeof(EHBASICROM); ++i) {
        _ram_[0xC000 + i] = EHBASICROM[i];
    }

}