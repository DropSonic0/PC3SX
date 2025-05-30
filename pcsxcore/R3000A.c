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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.           *
 ***************************************************************************/

/*
* R3000A CPU functions.
*/

#include "R3000A.h"
#include "PsxHw.h"
#include "PsxDma.h"
#include "CdRom.h"
#include "Mdec.h"

R3000Acpu *psxCpu;
psxRegisters psxRegs;

int psxInit() {

	if(Config.Cpu) {
	psxCpu = &psxInt;
	}
#if defined(__x86_64__) || defined(__i386__) || defined(__sh__) || defined(__ppc__) || defined(__BIGENDIAN__)
	if (!Config.Cpu) psxCpu = &psxRec;
#endif

	Log = 0;

	if (psxMemInit() == -1) return -1;

	return psxCpu->Init();
}

void psxReset() {
	psxCpu->Reset();

	psxMemReset();

	memset(&psxRegs, 0, sizeof(psxRegs));

	psxRegs.pc = 0xbfc00000; // Start in bootstrap

	psxRegs.CP0.r[12] = 0x10900000; // COP0 enabled | BEV = 1 | TS = 1
	psxRegs.CP0.r[15] = 0x00000002; // PRevID = Revision ID, same as R3000A

	psxHwReset();
	psxBiosInit();

	if (!Config.HLE)
		psxExecuteBios();

#ifdef EMU_LOG
	EMU_LOG("*BIOS END*\n");
#endif
	Log = 0;
}

void psxShutdown() {
	psxMemShutdown();
	psxBiosShutdown();

	psxCpu->Shutdown();
}

void psxException(u32 code, u32 bd) {
	// Set the Cause
	psxRegs.CP0.n.Cause = code;

	// Set the EPC & PC
	if (bd) {
#ifdef PSXCPU_LOG
		PSXCPU_LOG("bd set!!!\n");
#endif
		printf("bd set!!!\n");
		psxRegs.CP0.n.Cause |= 0x80000000;
		psxRegs.CP0.n.EPC = (psxRegs.pc - 4);
	} else
		psxRegs.CP0.n.EPC = (psxRegs.pc);

	if (psxRegs.CP0.n.Status & 0x400000)
		psxRegs.pc = 0xbfc00180;
	else
		psxRegs.pc = 0x80000080;

	// Set the Status
	psxRegs.CP0.n.Status = (psxRegs.CP0.n.Status &~0x3f) |
						  ((psxRegs.CP0.n.Status & 0xf) << 2);

	if (!Config.HLE && (((PSXMu32(psxRegs.CP0.n.EPC) >> 24) & 0xfe) == 0x4a)) {
		// "hokuto no ken" / "Crash Bandicot 2" ... fix
		PSXMu32ref(psxRegs.CP0.n.EPC)&= SWAPu32(~0x02000000);
	}

	if (Config.HLE) psxBiosException();
}

void psxBranchTest() {
	if ((psxRegs.cycle - psxNextsCounter) >= psxNextCounter)
		psxRcntUpdate();

	if (psxRegs.interrupt) {
		if ((psxRegs.interrupt & 0x80) && (!Config.Sio)) { // sio
			if ((psxRegs.cycle - psxRegs.intCycle[7]) >= psxRegs.intCycle[7+1]) {
				psxRegs.interrupt&=~0x80;
				sioInterrupt();
			}
		}
		if (psxRegs.interrupt & 0x04) { // cdr
			if ((psxRegs.cycle - psxRegs.intCycle[2]) >= psxRegs.intCycle[2+1]) {
				psxRegs.interrupt&=~0x04;
				cdrInterrupt();
			}
		}
		if (psxRegs.interrupt & 0x040000) { // cdr read
			if ((psxRegs.cycle - psxRegs.intCycle[2+16]) >= psxRegs.intCycle[2+16+1]) {
				psxRegs.interrupt&=~0x040000;
				cdrReadInterrupt();
			}
		}
		if (psxRegs.interrupt & 0x01000000) { // gpu dma
			if ((psxRegs.cycle - psxRegs.intCycle[3+24]) >= psxRegs.intCycle[3+24+1]) {
				psxRegs.interrupt&=~0x01000000;
				gpuInterrupt();
			}
		}
		if (psxRegs.interrupt & 0x02000000) { // mdec out dma
			if ((psxRegs.cycle - psxRegs.intCycle[5+24]) >= psxRegs.intCycle[5+24+1]) {
				psxRegs.interrupt&=~0x02000000;
				mdec1Interrupt();
			}
		}

		if (psxRegs.interrupt & 0x80000000) {
			psxRegs.interrupt&=~0x80000000;
			psxTestHWInts();
		}
	}
//	if (psxRegs.cycle > 0xd29c6500) Log=1;
}

void psxTestHWInts() {
	if (psxHu32(0x1070) & psxHu32(0x1074)) {
		if ((psxRegs.CP0.n.Status & 0x401) == 0x401) {
#ifdef PSXCPU_LOG
			PSXCPU_LOG("Interrupt: %x %x\n", psxHu32(0x1070), psxHu32(0x1074));
#endif
//			printf("Interrupt (%x): %x %x\n", psxRegs.cycle, psxHu32(0x1070), psxHu32(0x1074));
			psxException(0x400, 0);
		}
	}
}

void psxJumpTest() {
	if (!Config.HLE && Config.PsxOut) {
		u32 call = psxRegs.GPR.n.t1 & 0xff;
		switch (psxRegs.pc & 0x1fffff) {
			case 0xa0:
				if (biosA0[call])
					biosA0[call]();
				else if (call != 0x28 && call != 0xe) {
#ifdef PSXBIOS_LOG
					PSXBIOS_LOG("Bios call a0: %s (%x) %x,%x,%x,%x\n", biosA0n[call], call,
							psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3);
#endif
			}
				break;
			case 0xb0:
				if (biosB0[call])
					biosB0[call]();
                else if (call != 0x17 && call != 0xb) {
#ifdef PSXBIOS_LOG
					PSXBIOS_LOG("Bios call b0: %s (%x) %x,%x,%x,%x\n", biosB0n[call], call,
							psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3);
#endif
			}
				break;
			case 0xc0:
			if (biosC0[call])
				biosC0[call]();
            else {
#ifdef PSXBIOS_LOG
					PSXBIOS_LOG("Bios call c0: %s (%x) %x,%x,%x,%x\n", biosC0n[call], call,
							psxRegs.GPR.n.a0, psxRegs.GPR.n.a1, psxRegs.GPR.n.a2, psxRegs.GPR.n.a3);
#endif
			}

				break;
		}
	}
}

void psxExecuteBios() {
	while (psxRegs.pc != 0x80030000)
		psxCpu->ExecuteBlock();
}

