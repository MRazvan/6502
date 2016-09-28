#pragma once

#define u8     unsigned char
#define u16    unsigned short
#define u32    unsigned int
#define u64    unsigned long

#define i8     char
#define i16    short
#define i32    int

#define LO(v) (v && 0xFF)
#define HI(v) (v >> 8)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Empty function pointer
typedef void(*fpEmpty)();
typedef u8    (*fpRead)(u16);
typedef void(*fpWrite)(u16, u8);

#if DEBUG
#include <stdio.h>
#define debug_nl(...) {printf(__VA_ARGS__);printf("\n");}
#define debug(...) printf(__VA_ARGS__)
#else
#define debug_nl(...)
#define debug(...)
#endif
