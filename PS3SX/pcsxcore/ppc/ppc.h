/*
 * ppc definitions v0.5.1
 *  Authors: linuzappz <linuzappz@pcsx.net>
 *           alexey silinov
 */

#ifndef __PPC_H__
#define __PPC_H__

#ifdef __cplusplus
extern "C" {
#endif

// include basic types
#include "../PsxCommon.h"
#include "ppc_mnemonics.h"

#define NUM_HW_REGISTERS 28

/* general defines */
#define write8(val)  *(u8 *)ppcPtr = val; ppcPtr++;
#define write16(val) *(u16*)ppcPtr = val; ppcPtr+=2;
#define write32(val) *(u32*)ppcPtr = val; ppcPtr+=4;
#define write64(val) *(u64*)ppcPtr = val; ppcPtr+=8;

#define CALLFunc(FUNC) \
do { \
    u32* _opd = (u32*)(FUNC); \
    u32 _func = _opd[0]; \
    u32 _rtoc = _opd[1]; \
    ReleaseArgs(); \
    LIW(12, _rtoc); \
    if ((_func & 0x1fffffc) == _func) { \
        BLA(_func); \
    } else { \
        LIW(0, _func); \
        MTCTR(0); \
        BCTRL(); \
    } \
} while(0)

extern int cpuHWRegisters[NUM_HW_REGISTERS];

extern u32 *ppcPtr;
extern u8  *j8Ptr[32];
extern u32 *j32Ptr[32];

void ppcInit();
void ppcSetPtr(u32 *ptr);
void ppcShutdown();

void ppcAlign(int bytes);
void returnPC();
void recRun(void (*func)(), u32 hw1, u32 hw2);
u8 dynMemRead8(u32 mem);
u16 dynMemRead16(u32 mem);
u32 dynMemRead32(u32 mem);
void dynMemWrite32(u32 mem, u32 val);

#ifdef __cplusplus
}
#endif
#endif /* __PPC_H__ */
