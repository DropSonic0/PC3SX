/*
 * ix86 core v0.5.1
 *  Authors: linuzappz <linuzappz@pcsx.net>
 *           alexey silinov
 */

#include <stdio.h>
#include <string.h>

#include "ppc.h"

// General Purpose hardware registers
/*int cpuHWRegisters[NUM_HW_REGISTERS] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};*/

u32 *ppcPtr;
u32 *b32Ptr[32];

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

inline void Write32(u32 val)
{
	*(u32*)ppcPtr = val;
	ppcPtr++;
}
inline void write64(u64 val)
{
	*(u64*)ppcPtr = val;
	ppcPtr+=2;
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

inline u32 mod1(int to, s32 from)
{
	return ((to << 21) | (from & 0xffff));
}

inline u32 mod2(int to, int from, s32 imm)
{
	return ((to << 21) | (from << 16) | (imm & 0xffff));
}

inline u32 mod3(int to, int from, s32 imm)
{
	return ((to << 21) | (from << 16) | (imm << 11));
}

/* Link register related */
MOD0_INSTR( MFLR,  0x7C0802A6 );
MOD0_INSTR( MTLR,  0x7C0803A6 );
MOD0_INSTR( MTCTR, 0x7C0903A6 );

void BLR() { Write32(0x4e800020); }
void BGTLR() { Write32(0x4d810020); }
void BCTRL() { Write32(0x4E800421); }
void BCTR() { Write32(0x4E800420); }

/* Load ops */
MOD1_INSTR( LI,  0x38000000 );
MOD1_INSTR( LIS, 0x3c000000 );

MOD2_INSTR( LWZ, 0x80000000 );
MOD2_INSTR( LHZ, 0xa0000000 );
MOD2_INSTR( LHA, 0xa8000000 );
MOD2_INSTR( LBZ, 0x88000000 );
MOD2_INSTR( LMW, 0xb8000000 );

MOD3_INSTR( LWZX,  0x7C00002E );
MOD3_INSTR( LWBRX, 0x7C00042C );
MOD3_INSTR( LHBRX, 0x7C00062C );

/* Store ops */
MOD2_INSTR( STMW, 0xBC000000 );
MOD2_INSTR( STW,  0x90000000 );
MOD2_INSTR( STH,  0xB0000000 );
MOD2_INSTR( STB,  0x98000000 );
MOD2_INSTR( STWU, 0x94000000 );

MOD3_INSTR( STHBRX, 0x7C00072C );
MOD3_INSTR( STWBRX, 0x7C00052C );


