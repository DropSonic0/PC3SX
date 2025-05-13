/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *   schultz.ryan@gmail.com, http://rschultz.ath.cx/code.php               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*
* Internal emulated HLE BIOS.
*/

#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

//extern char mcd1Written;
//extern char mcd2Written;

#include "PsxBios.h"
#include "PsxHw.h"

#define at (psxRegs.GPR.n.at)
#define v0 (psxRegs.GPR.n.v0)
#define v1 (psxRegs.GPR.n.v1)
#define a0 (psxRegs.GPR.n.a0)
#define a1 (psxRegs.GPR.n.a1)
#define a2 (psxRegs.GPR.n.a2)
#define a3 (psxRegs.GPR.n.a3)
#define t0 (psxRegs.GPR.n.t0)
#define t1 (psxRegs.GPR.n.t1)
#define t2 (psxRegs.GPR.n.t2)
#define t3 (psxRegs.GPR.n.t3)
#define t4 (psxRegs.GPR.n.t4)
#define t5 (psxRegs.GPR.n.t5)
#define t6 (psxRegs.GPR.n.t6)
#define t7 (psxRegs.GPR.n.t7)
#define s0 (psxRegs.GPR.n.s0)
#define s1 (psxRegs.GPR.n.s1)
#define s2 (psxRegs.GPR.n.s2)
#define s3 (psxRegs.GPR.n.s3)
#define s4 (psxRegs.GPR.n.s4)
#define s5 (psxRegs.GPR.n.s5)
#define s6 (psxRegs.GPR.n.s6)
#define s7 (psxRegs.GPR.n.s7)
#define t8 (psxRegs.GPR.n.t6)
#define t9 (psxRegs.GPR.n.t7)
#define k0 (psxRegs.GPR.n.k0)
#define k1 (psxRegs.GPR.n.k1)
#define gp (psxRegs.GPR.n.gp)
#define sp (psxRegs.GPR.n.sp)
#define fp (psxRegs.GPR.n.s8)
#define ra (psxRegs.GPR.n.ra)
#define pc0 (psxRegs.pc)

#define SANE_PSXM(mem) ((u8*)(psxMemRLUT[(mem) >> 16] + ((mem) & 0xffff)))
#define Ra0 ((char*)SANE_PSXM(a0))
#define Ra1 ((char*)SANE_PSXM(a1))
#define Ra2 ((char*)SANE_PSXM(a2))
#define Ra3 ((char*)SANE_PSXM(a3))
#define Rv0 ((char*)SANE_PSXM(v0))
#define Rsp ((char*)SANE_PSXM(sp))



typedef struct _malloc_chunk {
	unsigned long stat;
	unsigned long size;
	struct _malloc_chunk *fd;
	struct _malloc_chunk *bk;
} malloc_chunk;

#define INUSE 0x1

typedef struct {
	u32 desc;
	s32 status;
	s32 mode;
	u32 fhandler;
} EvCB[32];

char *biosA0n[256];
char *biosB0n[256];
char *biosC0n[256];

typedef struct {
	char name[32];
	u32  mode;
	u32  offset;
	u32  size;
	u32  mcfile;
} FileDesc;

typedef struct {
	s32 status;
	s32 mode;
	u32 reg[32];
	u32 func;
} TCB;

static u32 *jmp_int = NULL;
static int *pad_buf = NULL;
static char *pad_buf1,*pad_buf2;//shadow add
static int pad_buf1len,pad_buf2len;//shadow add


static u32 regs[35];
static EvCB *Event;
static EvCB *HwEV; // 0xf0
static EvCB *EvEV; // 0xf1
static EvCB *RcEV; // 0xf2
static EvCB *UeEV; // 0xf3
static EvCB *SwEV; // 0xf4
static EvCB *ThEV; // 0xff
static u32 *heap_addr = NULL;
static u32 SysIntRP[8];
static int CardState = -1;
static TCB Thread[8];
static int CurThread = 0;
static FileDesc FDesc[32];

void psxBios_printf() { // 0x3f
	char tmp[1024];
	char tmp2[1024];
	u32 save[4];
	char *ptmp = tmp;
	int n=1, i=0, j;

	memcpy(save, (char*)PSXM(sp), 4 * 4);

	psxMu32ref(sp) = SWAP32((u32)a0);
	psxMu32ref(sp + 4) = SWAP32((u32)a1);
	psxMu32ref(sp + 8) = SWAP32((u32)a2);
	psxMu32ref(sp + 12) = SWAP32((u32)a3);

	while (Ra0[i]) {
		switch (Ra0[i]) {
			case '%':
				j = 0;
				tmp2[j++] = '%';
_start:
				switch (Ra0[++i]) {
					case '.':
					case 'l':
						tmp2[j++] = Ra0[i]; goto _start;
					default:
						if (Ra0[i] >= '0' && Ra0[i] <= '9') {
							tmp2[j++] = Ra0[i];
							goto _start;
						}
						break;
				}
				tmp2[j++] = Ra0[i];
				tmp2[j] = 0;

				switch (Ra0[i]) {
					case 'f': case 'F':
						ptmp += sprintf(ptmp, tmp2, (float)psxMu32(sp + n * 4)); n++; break;
					case 'a': case 'A':
					case 'e': case 'E':
					case 'g': case 'G':
						ptmp += sprintf(ptmp, tmp2, (double)psxMu32(sp + n * 4)); n++; break;
					case 'p':
					case 'i':
					case 'd': case 'D':
					case 'o': case 'O':
					case 'x': case 'X':
						ptmp += sprintf(ptmp, tmp2, (unsigned int)psxMu32(sp + n * 4)); n++; break;
					case 'c':
						ptmp += sprintf(ptmp, tmp2, (unsigned char)psxMu32(sp + n * 4)); n++; break;
					case 's':
						ptmp += sprintf(ptmp, tmp2, (char*)PSXM(psxMu32(sp + n * 4))); n++; break;
                    case '%':
						*ptmp++ = Ra0[i]; break;
				}
				i++;
				break;
			default:
				*ptmp++ = Ra0[i++];
		}
	}
	*ptmp = 0;

	memcpy((char*)PSXM(sp), save, 4 * 4);

	SysPrintf(tmp);

	pc0 = ra;
}

void psxBios_putchar() { // 3d
	SysPrintf("%c", (char)a0);
	pc0 = ra;
}

void psxBios_puts() { // 3e/3f
	SysPrintf(Ra0);
	pc0 = ra;
}

void (*biosA0[256])();
void (*biosB0[256])();
void (*biosC0[256])();

void psxBiosInit() {
	 
	int i;

	for(i = 0; i < 256; i++) {
		biosA0[i] = NULL;
		biosB0[i] = NULL;
		biosC0[i] = NULL;
	}
	biosA0[0x3e] = psxBios_puts;
	biosA0[0x3f] = psxBios_printf;

	biosB0[0x3d] = psxBios_putchar;
	biosB0[0x3f] = psxBios_puts;

	if (!Config.HLE) return;

}

void psxBiosException() {

}

void psxBiosShutdown() {
}

#define bfreeze(ptr, size) \
	if (Mode == 1) memcpy(&psxR[base], ptr, size); \
	if (Mode == 0) memcpy(ptr, &psxR[base], size); \
	base+=size;

#define bfreezes(ptr) bfreeze(ptr, sizeof(ptr))
#define bfreezel(ptr) {*ptr=SWAP32p((void*)ptr); bfreeze(ptr, 4); *ptr=SWAP32p((void*)ptr);}

#define bfreezepsxMptr(ptr) \
	if (Mode == 1) { \
		if (ptr) psxRu32ref(base) = SWAPu32((u32)ptr - (u32)psxM); \
		else psxRu32ref(base) = 0; \
	} else { \
		if (psxRu32(base)) *(u8**)&ptr = (u8*)(psxM + psxRu32(base)); \
		else ptr = NULL; \
	} \
	base+=4;

void psxBiosFreeze(int Mode) {
	u32 base = 0x40000;

	bfreezepsxMptr(jmp_int);
	bfreezepsxMptr(pad_buf);
	bfreezepsxMptr(pad_buf1);
	bfreezepsxMptr(pad_buf2);
	bfreezepsxMptr(heap_addr);
	
	bfreezel(&pad_buf1len);
	bfreezel(&pad_buf2len);
	bfreezes(regs);
	bfreezes(SysIntRP);
	bfreezel(&CardState);
	bfreezes(Thread);
	bfreezel(&CurThread);
	bfreezes(FDesc);
}



