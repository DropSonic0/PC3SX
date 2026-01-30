/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2003  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include "../psxcommon.h"
#include "../psxmem.h"
#include "../psxcounters.h"
#include "ppc.h"
#include "../R3000A.h"
#include "../PsxHLE.h"
#include "../psxhle.h"
#include "../mdec.h"
#include "R3000AOpcodeTable.h"
#include <ppc-asm.h>
#include <ppu_intrinsics.h>

//#define NO_CONSTANT
#define RECMEM_SIZE		(7*1024*1024)

u32 psxRecLUT[0x010000] __attribute__((aligned(32)));
const char recMem[RECMEM_SIZE]  __attribute__((aligned(32), section(".text#")));	/* the recompiled blocks will be here */
const char recRAM[0x200000] __attribute__((aligned(32)));	/* and the ptr to the blocks here */
const char recROM[0x080000] __attribute__((aligned(32)));	/* and here */


#undef PC_REC
#undef PC_REC8
#undef PC_REC16
#undef PC_REC32
#define PC_REC(x)	(psxRecLUT[x >> 16] + (x & 0xffff))
#define PC_REC8(x)	(*(u8 *)PC_REC(x))
#define PC_REC16(x) (*(u16*)PC_REC(x))
#define PC_REC32(x) (*(u32*)PC_REC(x))

static u32 pc;			/* recompiler pc */
static u32 pcold;		/* recompiler oldpc */
static int count;		/* recompiler intruction count */
static int branch;		/* set for branch */
static u32 target;		/* branch target */
static u32 resp;

u32 *ptr;

u32 cop2readypc = 0;
u32 idlecyclecount = 0;

typedef struct {
	int state;
	u32 k;
	int reg;
} iRegisters;

static iRegisters iRegs[32];



#define ST_UNK      0x00
#define ST_CONST    0x01
#define ST_MAPPED   0x02

#ifdef NO_CONSTANT
#define IsConst(reg) 0
#else
#define IsConst(reg)  (iRegs[reg].state == ST_CONST)
#endif
#define IsMapped(reg) (iRegs[reg].state == ST_MAPPED)

void SetArg_OfB( arg ) {
	if(IsConst(_Rs_)) {
		LIW((arg), iRegs[_Rs_].k + _Imm_);
	}
	else {
		LWPRtoR((arg), &psxRegs.GPR.r[_Rs_]);
		if(_Imm_) ADDI((arg), (arg), _Imm_);
	}
}

static void (*recBSC[64])();
static void (*recSPC[64])();
static void (*recREG[32])();
static void (*recCP0[32])();
static void (*recCP2[64])();
static void (*recCP2BSC[32])();

static void recRecompile();
static void recError();

static void MapConst(int reg, u32 _const) {
	iRegs[reg].k = _const;
	iRegs[reg].state = ST_CONST;
}

static void iFlushReg(int reg) {
	if (IsConst(reg)) {
		LIW(r9, iRegs[reg].k);
		STWRtoPR(&psxRegs.GPR.r[reg], r9);
	}
	iRegs[reg].state = ST_UNK;
}

static void iFlushRegs() {
	int i;

	for (i = 1; i < 32; i++) {
		iFlushReg( i );
	}
}

static void UpdateCycle(u32 amount) {
	LWPRtoR(r9, &psxRegs.cycle);
	SUBI(r9, r9, amount);
	STWRtoPR(&psxRegs.cycle, r9);
}

static void StackRes() {
//	if(resp) ADDI(sp, sp, resp);
}

static void Return()
{
//	iFlushRegs(0);
//	FlushAllHWReg();
	LIW(12, ((u32*)returnPC)[1]);
	LIW(0, ((u32*)returnPC)[0]);
	MTLR(0);
	BLR();
}

static void iRet() {
    /* store cycle */
	count = idlecyclecount + (pc - pcold) / 4;
	UpdateCycle(count);
	Return();
}

static void move_to_mem(u32 from) {
	while(from < (u32)(u8*)ppcPtr) {
		__asm__ __volatile__("dcbst 0,%0" : : "r" (from));
		__asm__ __volatile__("icbi 0,%0" : : "r" (from));
		from += 4;
	}
	__asm__ __volatile__("sync");
	__asm__ __volatile__("isync");
}

static int iLoadTest() {
	u32 tmp;

	// check for load delay
	tmp = psxRegs.code >> 26;
	switch (tmp) {
		case 0x10: // COP0
			switch (_Rs_) {
				case 0x00: // MFC0
				case 0x02: // CFC0
					return 1;
			}
			break;
		case 0x12: // COP2
			switch (_Funct_) {
				case 0x00:
					switch (_Rs_) {
						case 0x00: // MFC2
						case 0x02: // CFC2
							return 1;
					}
					break;
			}
			break;
		case 0x32: // LWC2
			return 1;
		default:
			if (tmp >= 0x20 && tmp <= 0x26) { // LB/LH/LWL/LW/LBU/LHU/LWR
				return 1;
			}
			break;
	}
	return 0;
}

#define REC_TEST_BRANCH() \
	CALLFunc((uptr)psxBranchTest);

/* set a pending branch */
static void SetBranch() {
	branch = 1;
	psxRegs.code = PSXMu32(pc);
	pc += 4;

	if (iLoadTest() == 1) {
		iFlushRegs();
		LIW(r3, psxRegs.code);
		STWRtoPR(&psxRegs.code, r3);
		// store cycle 
		count = idlecyclecount + (pc - pcold) / 4;
		UpdateCycle(count);

		//LIW(r9, (uptr)&target);
		//LWZ(PPCARG2, r0, r9);
		LWMtoR(PPCARG2, (uptr)&target);
		LIW(PPCARG1, _Rt_);

		CALLFunc((u32)psxDelayTest);
		
		StackRes();
		Return();
		return;
	}

	recBSC[psxRegs.code>>26]();

	iFlushRegs();
	LWMtoR(r3, (uptr)&target);
	STWRtoPR(&psxRegs.pc, r3);
	
	REC_TEST_BRANCH();
	
	// TODO: don't return if target is compiled

#if 1
	iRet();
#else
	// maybe just happened an interruption, check so
	LWMtoR(r0, (uptr)&target);
	LWPRtoR(r9, &psxRegs.pc);
	CMPLW(r9, r0);
	BNE_L(b32Ptr[0]);

	LIW(r3, PC_REC(SWAPu32(target)));
	LWZ(r3, r3, r0);
	MTCTR(r3);
	CMPLWI(r3, 0);
	BNE_L(b32Ptr[1]);

	B_DST(b32Ptr[0]);
	iRet();

	// next bit is already compiled - jump right to it
	B_DST(b32Ptr[1]);
	//Return();
	BCTR();
#endif
}

static void iJump(u32 branchPC) {
	branch = 1;
	psxRegs.code = PSXMu32(pc);
	pc+=4;

	if (iLoadTest() == 1) {
		iFlushRegs();
		LIW(r3, psxRegs.code);
		STWRtoPR(&psxRegs.code, r3);

		count = idlecyclecount + (pc - pcold) / 4;
		UpdateCycle(count);

		LIW(PPCARG2, branchPC);
		LIW(PPCARG1, _Rt_);

		CALLFunc((u32)psxDelayTest);
				
		Return();
		return;
	}

	recBSC[psxRegs.code>>26]();

	iFlushRegs();
	LIW(r9, branchPC);
	STWRtoPR(&psxRegs.pc, r9);

	REC_TEST_BRANCH();
 
	count = idlecyclecount + (pc - pcold) / 4;
	UpdateCycle(count);
#if 1
	// always return for now...
	Return();
#else
	LIW(r0, branchPC);
	LWPRtoR(r9, &psxRegs.pc);
	CMPLW(r9, r0);
	BNE_L(b32Ptr[1]);

	LIW(r3, PC_REC(branchPC));
	LWZ(r3, r3, r0);
	MTCTR(r3);
	CMPLWI(r3, 0);
	BEQ_L(b32Ptr[2]);

	move_to_mem(PC_REC(branchPC));
	BCTR();

	B_DST(b32Ptr[1]);
	B_DST(b32Ptr[2]);
	Return();
#endif
}

static void iBranch(u32 branchPC, int savectx) {
	iRegisters iRegsS[32];
	u32 respold=0;

	if (savectx) {
		respold = resp;
		memcpy(iRegsS, iRegs, sizeof(iRegs));
	}

	branch = 1;
	psxRegs.code = PSXMu32(pc);

	// the delay test is only made when the branch is taken
	// savectx == 0 will mean that :)
	if (savectx == 0 && iLoadTest() == 1) {
		iFlushRegs();
		LIW(r3, psxRegs.code);
		STWRtoPR(&psxRegs.code, r3);

		count = idlecyclecount + (pc + 4 - pcold) / 4;
		UpdateCycle(count);

		LIW(PPCARG2, branchPC);
		LIW(PPCARG1, _Rt_);

		CALLFunc((u32)psxDelayTest);
		
		StackRes();
		Return();
		return;
	}

	if(branchPC == pc) branchPC+=4;
	pc += 4;
	recBSC[psxRegs.code>>26]();
	
	iFlushRegs();

	LIW(r9, branchPC);
	STWRtoPR(&psxRegs.pc, r9);
	REC_TEST_BRANCH();

	count = idlecyclecount + (pc - pcold) / 4;
	UpdateCycle(count);

#if 1
	Return();
#else
	LIW(r0, branchPC);
	LWPRtoR(r9, &psxRegs.pc);
	CMPLW(r9, r0);
	BNE_L(b32Ptr[1]);

	LIW(r3, PC_REC(branchPC));
	LWZ(r3, r3, r0);
	MTCTR(r3);
	CMPLWI(r3, 0);
	BEQ_L(b32Ptr[2]);

	move_to_mem(PC_REC(branchPC));
	BCTR();

	B_DST(b32Ptr[1]);
	B_DST(b32Ptr[2]);
	Return();
#endif
	pc -= 4;
	if (savectx) {
		resp = respold;
		memcpy(iRegs, iRegsS, sizeof(iRegs));
	}
}


void iDumpRegs() {
	int i, j;

	//printf("%08x %08x\n", psxRegs.pc, psxRegs.evtCycleCountdown);
	for (i=0; i<4; i++) {
		for (j=0; j<8; j++)
			printf("%08x ", psxRegs.GPR.r[j*i]);
		printf("\n");
	}
}

void iDumpBlock(char *ptr) {
/*	FILE *f;
	u32 i;

	SysPrintf("dump1 %x:%x, %x\n", psxRegs.pc, pc, psxCurrentCycle);

	for (i = psxRegs.pc; i < pc; i+=4)
		SysPrintf("%s\n", disR3000AF(PSXMu32(i), i));

	fflush(stdout);
	f = fopen("dump1", "w");
	fwrite(ptr, 1, (u32)x86Ptr - (u32)ptr, f);
	fclose(f);
	system("ndisasmw -u dump1");
	fflush(stdout);*/
}

#define REC_FUNC(f) \
static void rec##f() { \
	iFlushRegs(); \
	LIW(r3, (u32)psxRegs.code); \
	STWRtoPR(&psxRegs.code, r3); \
	LIW(r3, (u32)pc); \
	STWRtoPR(&psxRegs.pc, r3); \
	CALLFunc((u32)psx##f); \
}

#define REC_SYS(f) \
void psx##f();\
static void rec##f() { \
	iFlushRegs(); \
	LIW(r3, (u32)psxRegs.code); \
	STWRtoPR(&psxRegs.code, r3); \
	LIW(r3, (u32)pc); \
	STWRtoPR(&psxRegs.pc, r3); \
	CALLFunc((u32)psx##f); \
	branch = 2; \
	iRet(); \
}

#define REC_BRANCH(f) \
static void rec##f() { \
	iFlushRegs(); \
	LIW(r3, (u32)psxRegs.code); \
	STWRtoPR(&psxRegs.code, r3); \
	LIW(r3, (u32)pc); \
	STWRtoPR(&psxRegs.pc, r3); \
	CALLFunc((u32)psx##f); \
	branch = 2; \
	iRet(); \
}


static int allocMem() {
	int i;

	for (i=0; i<0x80; i++) psxRecLUT[i + 0x0000] = (u32)&recRAM[(i & 0x1f) << 16];
	memcpy(psxRecLUT + 0x8000, psxRecLUT, 0x80 * 4);
	memcpy(psxRecLUT + 0xa000, psxRecLUT, 0x80 * 4);

	for (i=0; i<0x08; i++) psxRecLUT[i + 0xbfc0] = (u32)&recROM[i << 16];
	
	return 0;
}

static int recInit() {
	return allocMem();
}

static void recReset() {
	memset(recRAM, 0, 0x200000);
	memset(recROM, 0, 0x080000);

	ppcInit();
	ppcSetPtr((u32 *)recMem);

	branch = 0;
	memset(iRegs, 0, sizeof(iRegs));
	iRegs[0].state = ST_CONST;
	iRegs[0].k     = 0;
}

static void recShutdown() {
	ppcShutdown();
}

static void recError() {
	SysReset();
	ClosePlugins();
	SysMessage("Unrecoverable error while running recompiler\n");
	SysRunGui();
}

/*__inline*/ static void execute() {
	void (**recFunc)();
	char *p;

	p =	(char*)PC_REC(psxRegs.pc);
	/*if (p != NULL)*/ 
	/*else { recError(); return; }*/

	if (*p == 0) {
		recRecompile();
	}

	recFunc = (void (**)()) (u32)p;

	recRun(*recFunc);

}

static void recExecute() {
	for (;;)  execute();
}

static void recExecuteBlock() {
	execute();
}

static void recClear(u32 Addr, u32 Size) {
	memset((void*)PC_REC(Addr), 0, Size * 4);
}

static void recNULL() {
//	SysMessage("recUNK: %8.8x\n", psxRegs.code);
}

/*********************************************************
* goes to opcodes tables...                              *
* Format:  table[something....]                          *
*********************************************************/

#if 0
REC_SYS(SPECIAL);
REC_SYS(REGIMM);
REC_SYS(COP0);
REC_SYS(COP2);
REC_SYS(BASIC);
#else
static void recSPECIAL() {
	recSPC[_Funct_]();
}

static void recREGIMM() {
	recREG[_Rt_]();
}

static void recCOP0() {
	recCP0[_Rs_]();
}

static void recCOP2() {
	recCP2[_Funct_]();
}

static void recBASIC() {
	recCP2BSC[_Rs_]();
}
#endif


//end of Tables opcodes...

#include "recOpcodeTable.h"
#include "pGte.h"

static void (*recBSC[64])() = {
	recSPECIAL, recREGIMM, recJ   , recJAL  , recBEQ , recBNE , recBLEZ, recBGTZ,
	recADDI   , recADDIU , recSLTI, recSLTIU, recANDI, recORI , recXORI, recLUI ,
	recCOP0   , recNULL  , recCOP2, recNULL , recNULL, recNULL, recNULL, recNULL,
	recNULL   , recNULL  , recNULL, recNULL , recNULL, recNULL, recNULL, recNULL,
	recLB     , recLH    , recLWL , recLW   , recLBU , recLHU , recLWR , recNULL,
	recSB     , recSH    , recSWL , recSW   , recNULL, recNULL, recSWR , recNULL,
	recNULL   , recNULL  , recLWC2, recNULL , recNULL, recNULL, recNULL, recNULL,
	recNULL   , recNULL  , recSWC2, recHLE  , recNULL, recNULL, recNULL, recNULL
};

static void (*recSPC[64])() = {
	recSLL , recNULL, recSRL , recSRA , recSLLV   , recNULL , recSRLV, recSRAV,
	recJR  , recJALR, recNULL, recNULL, recSYSCALL, recBREAK, recNULL, recNULL,
	recMFHI, recMTHI, recMFLO, recMTLO, recNULL   , recNULL , recNULL, recNULL,
	recMULT, recMULTU, recDIV, recDIVU, recNULL   , recNULL , recNULL, recNULL,
	recADD , recADDU, recSUB , recSUBU, recAND    , recOR   , recXOR , recNOR ,
	recNULL, recNULL, recSLT , recSLTU, recNULL   , recNULL , recNULL, recNULL,
	recNULL, recNULL, recNULL, recNULL, recNULL   , recNULL , recNULL, recNULL,
	recNULL, recNULL, recNULL, recNULL, recNULL   , recNULL , recNULL, recNULL
};

static void (*recREG[32])() = {
	recBLTZ  , recBGEZ  , recNULL, recNULL, recNULL, recNULL, recNULL, recNULL,
	recNULL  , recNULL  , recNULL, recNULL, recNULL, recNULL, recNULL, recNULL,
	recBLTZAL, recBGEZAL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL,
	recNULL  , recNULL  , recNULL, recNULL, recNULL, recNULL, recNULL, recNULL
};

static void (*recCP0[32])() = {
	recMFC0, recNULL, recCFC0, recNULL, recMTC0, recNULL, recCTC0, recNULL,
	recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL,
	recRFE , recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL,
	recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL
};

static void (*recCP2[64])() = {
	recBASIC, recRTPS , recNULL , recNULL, recNULL, recNULL , recNCLIP, recNULL, // 00
	recNULL , recNULL , recNULL , recNULL, recOP  , recNULL , recNULL , recNULL, // 08
	recDPCS , recINTPL, recMVMVA, recNCDS, recCDP , recNULL , recNCDT , recNULL, // 10
	recNULL , recNULL , recNULL , recNCCS, recCC  , recNULL , recNCS  , recNULL, // 18
	recNCT  , recNULL , recNULL , recNULL, recNULL, recNULL , recNULL , recNULL, // 20
	recSQR  , recDCPL , recDPCT , recNULL, recNULL, recAVSZ3, recAVSZ4, recNULL, // 28 
	recRTPT , recNULL , recNULL , recNULL, recNULL, recNULL , recNULL , recNULL, // 30
	recNULL , recNULL , recNULL , recNULL, recNULL, recGPF  , recGPL  , recNCCT  // 38
};

static void (*recCP2BSC[32])() = {
	recMFC2, recNULL, recCFC2, recNULL, recMTC2, recNULL, recCTC2, recNULL,
	recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL,
	recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL,
	recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL, recNULL
};

static void recRecompile() {
	//static int recCount = 0;
	char *p;
	//u32 *ptr;
	cop2readypc = 0;
	idlecyclecount = 0;
	resp = 0;

	/* if ppcPtr reached the mem limit reset whole mem */
	if (((u32)ppcPtr - (u32)recMem) >= (RECMEM_SIZE - 0x10000))
		recReset();

	ppcAlign(4);
	ptr = ppcPtr;

	// tell the LUT where to find us
	PC_REC32(psxRegs.pc) = (u32)ppcPtr;

	pcold = pc = psxRegs.pc;

	for (count=0; count<500;) {
		p = (char *)PSXM(pc);
		if (p == NULL) recError();
		psxRegs.code = SWAP32(*(u32 *)p);

		pc+=4; count++;

		recBSC[psxRegs.code>>26]();

		if (branch) {
			branch = 0;
			goto done;
		}
	}

	iFlushRegs();
	
	LIW(r9, pc);
	STWRtoPR(&psxRegs.pc, r9);

	iRet();

done:;

	move_to_mem((u32)(u8*)ptr);
}

R3000Acpu psxRec = {
	recInit,
	recReset,
	recExecute,
	recExecuteBlock,
	recClear,
	recShutdown
};
