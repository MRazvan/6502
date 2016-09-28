#include <conio.h>
#include <iostream>

#include "defines.h"
#include "globals.h"
#include "cpu.h"


int main(void) {
    /* COLOR [background][foreground]
            0 = Black 
            8 = Gray 

            1 = Blue 
            9 = Light Blue 

            2 = Green 
            A = Light Green 

            3 = Aqua 
            B = Light Aqua 

            4 = Red 
            C = Light Red 

            5 = Purple 
            D = Light Purple 

            6 = Yellow 
            E = Light Yellow 

            7 = White 
            F = Bright White 
    */
#ifdef _WIN32
    system("Color 1F");
#endif
    _console_init();
    _cpu_reset();
    
    i16 instr_count;
    while (true) {
#ifdef _WIN32
        if (_kbhit()) {
            _input_key = _getch();
            if (_input_key >= 0x61 && _input_key <= 0x7A) {
                _input_key -= 0x20;
            }
        }
#endif
        // Execute instr_count instructions, we don't want one by one since that will slow down things quite a lot
        instr_count = 30000;
        while (instr_count--) {
            _cpu_step(); 
        }
    }

    return 0;
}
