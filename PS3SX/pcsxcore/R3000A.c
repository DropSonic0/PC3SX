/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
* R3000A (PSX CPU) functions.
*/

#include "R3000A.h"

psxRegisters psxRegs;
R3000Acpu *psxCpu = NULL;

int psxInit() {
	SysPrintf("psxInit: starting\n");
	psxCpu = &psxInt;
	if (Config.Cpu == CPU_DYNAREC) {
#ifdef PSXREC
		SysPrintf("psxInit: using Dynarec\n");
		psxCpu = &psxRec;
#else
		SysPrintf("psxInit: Dynarec not available, using Interpreter\n");
#endif
	} else {
		SysPrintf("psxInit: using Interpreter\n");
	}

	if (psxCpu->Init() == -1) return -1;

	SysPrintf("psxInit: psxMemInit\n");
	if (psxMemInit() == -1) return -1;

	SysPrintf("psxInit done\n");
	return 0;
}

void psxReset() {
	SysPrintf("psxReset: starting\n");
	psxCpu->Reset();

	psxRegs.pc = 0xbfc00000; // Start in bootstrap

	psxRegs.CP0.r[12] = 0x10900000; // COP0 enabled | BEV = 1 | TS = 1

	SysPrintf("psxReset: psxMemReset\n");
	psxMemReset();
	SysPrintf("psxReset: psxHwReset\n");
	psxHwReset();
	SysPrintf("psxReset: psxBiosInit\n");
	psxBiosInit();

	if (!Config.HLE) {
		SysPrintf("psxReset: Loading BIOS\n");
#ifdef MDFNPS3
		if (!MDFNPCSXGetBios(psxR)) {
			SysPrintf("psxReset: MDFNPCSXGetBios FAILED\n");
		}
#else
		FILE *f;
		char path[MAXPATHLEN];
		sprintf(path, "%s/%s", Config.BiosDir, Config.Bios);
		f = fopen(path, "rb");
		if (f == NULL) {
			SysPrintf("psxReset: Could not open BIOS %s\n", path);
		} else {
			fread(psxR, 1, 0x80000, f);
			fclose(f);
		}
#endif
	}

	SysPrintf("psxReset done\n");
}

void psxShutdown() {
	psxCpu->Shutdown();
	psxMemShutdown();
}

void psxException(u32 code, u32 bd) {
	psxRegs.CP0.n.Cause = code;

	if (bd) {
		psxRegs.CP0.n.Cause |= 0x80000000;
		psxRegs.CP0.n.EPC = psxRegs.pc - 4;
	} else {
		psxRegs.CP0.n.EPC = psxRegs.pc;
	}

	if (psxRegs.CP0.n.Status & 0x400000) psxRegs.pc = 0xbfc00180;
	else psxRegs.pc = 0x80000080;

	psxRegs.CP0.n.Status = (psxRegs.CP0.n.Status & ~0x3f) |
						   ((psxRegs.CP0.n.Status & 0xf) << 2);
}

void psxBranchTest() {
	if (psxHu32(0x1070) & psxHu32(0x1074)) {
		if ((psxRegs.CP0.n.Status & 0x401) == 0x401) {
#ifdef PSXHW_LOG
			PSXHW_LOG("Interrupt: %x\n", psxHu32(0x1070) & psxHu32(0x1074));
#endif
			psxException(0x400, 0);
		}
	}

	if ((psxRegs.cycle - psxNextsCounter) >= psxNextCounter)
		psxRcntUpdate();
}

void psxJumpTest() {
	if (Config.HLE) return;

	switch (psxRegs.pc & 0x1fffff) {
		case 0xa0:
#ifdef PSX_LOG
			PSX_LOG("BIOS A0 call: %x\n", psxRegs.GPR.n.t1);
#endif
			psxBiosA0();
			break;

		case 0xb0:
#ifdef PSX_LOG
			PSX_LOG("BIOS B0 call: %x\n", psxRegs.GPR.n.t1);
#endif
			psxBiosB0();
			break;

		case 0xc0:
#ifdef PSX_LOG
			PSX_LOG("BIOS C0 call: %x\n", psxRegs.GPR.n.t1);
#endif
			psxBiosC0();
			break;
	}
}

void psxExecuteBios() {
	while (psxRegs.pc != 0x80030000) {
		psxCpu->ExecuteBlock();
	}
}
