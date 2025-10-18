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

#ifndef __PSXDMA_H__
#define __PSXDMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psxcommon.h"
#include "r3000a.h"
#include "psxhw.h"
#include "psxmem.h"

#define GPUDMA_INT(eCycle) { \
	psxRegs.interrupt |= 0x01000000; \
	psxRegs.intCycle[3+24].cycle = eCycle; \
	psxRegs.intCycle[3+24].sCycle = psxRegs.cycle; \
}

#define MDECOUTDMA_INT(eCycle) { \
	psxRegs.interrupt |= 0x02000000; \
	psxRegs.intCycle[5+24].cycle = eCycle; \
	psxRegs.intCycle[5+24].sCycle = psxRegs.cycle; \
}

#define SPUDMA_INT(eCycle) { \
	psxRegs.interrupt |= 0x03000000; \
	psxRegs.intCycle[6+24].cycle = eCycle; \
	psxRegs.intCycle[6+24].sCycle = psxRegs.cycle; \
}

#define MDECINDMA_INT(eCycle) { \
	psxRegs.interrupt |= 0x04000000; \
	psxRegs.intCycle[7+24].cycle = eCycle; \
	psxRegs.intCycle[7+24].sCycle = psxRegs.cycle; \
}

#define GPUOTCDMA_INT(eCycle) { \
	psxRegs.interrupt |= 0x05000000; \
	psxRegs.intCycle[8+24].cycle = eCycle; \
	psxRegs.intCycle[8+24].sCycle = psxRegs.cycle; \
}

#define CDRDMA_INT(eCycle) { \
	psxRegs.interrupt |= 0x06000000; \
	psxRegs.intCycle[9+24].cycle = eCycle; \
	psxRegs.intCycle[9+24].sCycle = psxRegs.cycle; \
}

void psxDma3(u32 madr, u32 bcr, u32 chcr);
void psxDma4(u32 madr, u32 bcr, u32 chcr);
void psxDma6(u32 madr, u32 bcr, u32 chcr);
void spuInterrupt();
void mdec0Interrupt();
void gpuotcInterrupt();
void cdrDmaInterrupt();

#ifdef __cplusplus
}
#endif
#endif

