/*
 * ppc definitions v0.5.1
 *  Authors: linuzappz <linuzappz@pcsx.net>
 *           alexey silinov
 */

#ifndef __PPC_H__
#define __PPC_H__

// include basic types
#include "../PsxCommon.h"
#include "ppc_mnemonics.h"

#define PPCARG1 3
#define PPCARG2 4

void Write32(u32 val);
void Write64(u64 val);
void ReleaseArgs();

void CALLFunc(void* ptr);

extern u32 *ppcPtr;
extern u32 *b32Ptr[32];

void ppcInit();
void ppcSetPtr(u32 *ptr);
void ppcShutdown();

void ppcAlign(int bytes);
void returnPC();
void recRun(void (*func)());
u8 dynMemRead8(u32 mem);
u16 dynMemRead16(u32 mem);
u32 dynMemRead32(u32 mem);
void dynMemWrite32(u32 mem, u32 val);


#endif /* __PPC_H__ */


















