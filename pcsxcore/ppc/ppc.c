/*
 * ix86 core v0.5.1
 *  Authors: linuzappz <linuzappz@pcsx.net>
 *           alexey silinov
 */

#include <stdio.h>
#include <string.h>

#include "ppc.h"

// General Purpose hardware registers
int cpuHWRegisters[NUM_HW_REGISTERS] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

u32 *ppcPtr;

void ppcInit() {
}
void ppcSetPtr(u32 *ptr) {
	ppcPtr = ptr;
}
void ppcAlign(int bytes) {
	// forward align
	ppcPtr = (u32*)(((u32)ppcPtr + bytes) & ~(bytes - 1));
}

void ppcShutdown() {
}

inline void CALLFunc(void* ptr) {
	u32* opd = (u32*)ptr;
	u32 func = opd[0];
	u32 rtoc = opd[1];
	
	LIW(12, rtoc);
    if ((func & 0x1fffffc) == func) {
        BLA(func);
    } else {
        LIW(0, func);
        MTCTR(0);
        BCTRL();
    }
}


