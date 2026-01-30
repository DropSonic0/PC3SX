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

#include "../gte.h"
#include "ppc.h"


static const unsigned char psxCP2time[64] = {
	2, 16, 1,  1,  1,  1, 8,  1, // 00
	1, 1,  1,  1,  6,  1, 1,  1, // 08
	8, 8,  8,  19, 13, 1, 44, 1, // 10
	1, 1,  1,  17, 11, 1, 14, 1, // 18
	30, 1, 1,  1,  1,  1, 1,  1, // 20
	5, 8,  17, 1,  1,  5, 6,  1, // 28
	23, 1, 1,  1,  1,  1, 1,  1, // 30
	1, 1,  1,  1,  1,  6, 5,  39 // 38
};

#define CP2_FUNC(f) \
static void rec##f() { \
	if (pc < cop2readypc) idlecyclecount += (cop2readypc - pc) >> 2; \
	iFlushRegs(); \
	LIW(r9, (u32)psxRegs.code); \
	STWRtoPR(&psxRegs.code, r9); \
	CALLFunc((u32)gte##f); \
	cop2readypc = pc + (psxCP2time[_fFunct_(psxRegs.code)] << 2); \
}

#define CP2_FUNCNC(f) \
static void rec##f() { \
	if (pc < cop2readypc) idlecyclecount += (cop2readypc - pc) >> 2; \
	iFlushRegs(); \
	LIW(r9, (u32)psxRegs.code); \
	STWRtoPR(&psxRegs.code, r9); \
	CALLFunc((u32)gte##f); \
	cop2readypc = pc + (psxCP2time[_fFunct_(psxRegs.code)] << 2); \
}

// GTE function callers
CP2_FUNC(MFC2);
CP2_FUNC(MTC2);
CP2_FUNC(CFC2);
CP2_FUNC(CTC2);
CP2_FUNC(LWC2);
CP2_FUNC(SWC2);

CP2_FUNCNC(RTPS);
CP2_FUNC(OP);
CP2_FUNCNC(NCLIP);
CP2_FUNCNC(DPCS);
CP2_FUNCNC(INTPL);
CP2_FUNC(MVMVA);
CP2_FUNCNC(NCDS);
CP2_FUNCNC(NCDT);
CP2_FUNCNC(CDP);
CP2_FUNCNC(NCCS);
CP2_FUNCNC(CC);
CP2_FUNCNC(NCS);
CP2_FUNCNC(NCT);
CP2_FUNC(SQR);
CP2_FUNCNC(DCPL);
CP2_FUNCNC(DPCT);
CP2_FUNCNC(AVSZ3);
CP2_FUNCNC(AVSZ4);
CP2_FUNCNC(RTPT);
CP2_FUNC(GPF);
CP2_FUNC(GPL);
CP2_FUNCNC(NCCT);

