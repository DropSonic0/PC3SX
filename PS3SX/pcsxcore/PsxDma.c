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
 * Handles PSX DMA functions.
 */

#include "PsxDma.h"
#include "plugins.h"

// Dma0/1 in Mdec.c
// Dma3 in CdRom.c

void spuInterrupt(void) {
	HW_DMA4_CHCR &= SWAP32(~0x01000000);
	DMA_INTERRUPT(4);
}

void psxDma4(u32 madr, u32 bcr, u32 chcr) { // SPU
	u16 *ptr;
	u32 size;

	switch (chcr) {
		case 0x01000201: //cpu to spu transfer
#ifdef PSXDMA_LOG
			PSXDMA_LOG("*** DMA4 SPU - mem2spu *** %x addr = %x size = %x\n", chcr, madr, bcr);
#endif
			ptr = (u16 *)PSXM(madr);
			if (ptr == NULL) {
#ifdef CPU_LOG
				CPU_LOG("** DMA4 SPU - mem2spu *** NULL Pointer!!!\n");
#endif
				break;
			}
			SPU_writeDMAMem(ptr, (bcr >> 16) * (bcr & 0xffff) * 2);

			// Jungle Book - 0-0.333x DMA
			SPUDMA_INT((bcr >> 16) * (bcr & 0xffff) / 3);
			return;

		case 0x01000200: //spu to cpu transfer
#ifdef PSXDMA_LOG
			PSXDMA_LOG("*** DMA4 SPU - spu2mem *** %x addr = %x size = %x\n", chcr, madr, bcr);
#endif
			ptr = (u16 *)PSXM(madr);
			if (ptr == NULL) {
#ifdef CPU_LOG
				CPU_LOG("** DMA4 SPU - spu2mem *** NULL Pointer!!!\n");
#endif
				break;
			}
			size = (bcr >> 16) * (bcr & 0xffff) * 2;
			SPU_readDMAMem(ptr, size);
			psxCpu->Clear(madr, size);

#if 1
			SPUDMA_INT((bcr >> 16) * (bcr & 0xffff) / 2);
#else
			// Experimental burst dma transfer (0.333x max)
			SPUDMA_INT((bcr >> 16) * (bcr & 0xffff) / 3);
#endif
			return;

#ifdef PSXDMA_LOG
		default:
			PSXDMA_LOG("*** DMA4 SPU - unknown *** %x addr = %x size = %x\n", chcr, madr, bcr);
			break;
#endif
	}

	HW_DMA4_CHCR &= SWAP32(~0x01000000);
	DMA_INTERRUPT(4);
}

void psxDma6(u32 madr, u32 bcr, u32 chcr) {
	u32 size;
	u32 *mem = (u32 *)PSXM(madr);

#ifdef PSXDMA_LOG
	PSXDMA_LOG("*** DMA6 OT *** %x addr = %x size = %x\n", chcr, madr, bcr);
#endif

	if (chcr == 0x11000002) {
		if (mem == NULL) {
#ifdef CPU_LOG
			CPU_LOG("*** DMA6 OT *** NULL Pointer!!!\n");
#endif
			HW_DMA6_CHCR &= SWAP32(~0x01000000);
			DMA_INTERRUPT(6);
			return;
		}

		// already 32-bit size
		size = bcr;

		while (bcr--) {
			*mem-- = SWAP32((madr - 4) & 0xffffff);
			madr -= 4;
		}
		mem++;
		*mem = SWAP32(0xffffff);

#if 1
		GPUOTCDMA_INT(size);
#else
		// Experimental burst dma transfer (0.333x max)
		GPUOTCDMA_INT(size / 3);
#endif
		return;
	}
#ifdef PSXDMA_LOG
	else {
		// Unknown option
		PSXDMA_LOG("*** DMA6 OT - unknown *** %x addr = %x size = %x\n", chcr, madr, bcr);
	}
#endif

	HW_DMA6_CHCR &= SWAP32(~0x01000000);
	DMA_INTERRUPT(6);
}

void gpuotcInterrupt(void) {
	HW_DMA6_CHCR &= SWAP32(~0x01000000);
	DMA_INTERRUPT(6);
}

// Logic from Mednafen's gpu.c for psxDma2

static u32 lUsedAddr[3];

static inline boolean CheckForEndlessLoop(u32 laddr) {
	if (laddr == lUsedAddr[1]) return TRUE;
	if (laddr == lUsedAddr[2]) return TRUE;

	if (laddr < lUsedAddr[0]) lUsedAddr[1] = laddr;
	else lUsedAddr[2] = laddr;

	lUsedAddr[0] = laddr;

	return FALSE;
}

static u32 gpuDmaChainSize(u32 addr) {
	u32 size;
	u32 DMACommandCounter = 0;

	lUsedAddr[0] = lUsedAddr[1] = lUsedAddr[2] = 0xffffff;

	// initial linked list ptr (word)
	size = 1;

	do {
		addr &= 0x1ffffc;

		if (DMACommandCounter++ > 2000000) break;
		if (CheckForEndlessLoop(addr)) break;


		// # 32-bit blocks to transfer
		size += psxMu8( addr + 3 );


		// next 32-bit pointer
		addr = psxMu32( addr & ~0x3 ) & 0xffffff;
		size += 1;
	} while (addr != 0xffffff);


	return size;
}

void psxDma2(u32 madr, u32 bcr, u32 chcr) { // GPU
	u32 *ptr;
	u32 size;

	switch (chcr) {
		case 0x01000200: // vram2mem
#ifdef PSXDMA_LOG
			PSXDMA_LOG("*** DMA2 GPU - vram2mem *** %x addr = %x size = %x\n", chcr, madr, bcr);
#endif
			ptr = (u32 *)PSXM(madr);
			if (ptr == NULL) {
#ifdef CPU_LOG
				CPU_LOG("*** DMA2 GPU - vram2mem *** NULL Pointer!!!\n");
#endif
				break;
			}
			// BA blocks * BS words (word = 32-bits)
			size = (bcr >> 16) * (bcr & 0xffff);
			GPU_readDataMem((unsigned long *)ptr, size);
			psxCpu->Clear(madr, size);

#if 1
			// already 32-bit word size ((size * 4) / 4)
			GPUDMA_INT(size);
#else
			// Experimental burst dma transfer (0.333x max)
			GPUDMA_INT(size/3);
#endif
			return;

		case 0x01000201: // mem2vram
#ifdef PSXDMA_LOG
			PSXDMA_LOG("*** DMA 2 - GPU mem2vram *** %x addr = %x size = %x\n", chcr, madr, bcr);
#endif
			ptr = (u32 *)PSXM(madr);
			if (ptr == NULL) {
#ifdef CPU_LOG
				CPU_LOG("*** DMA2 GPU - mem2vram *** NULL Pointer!!!\n");
#endif
				break;
			}
			// BA blocks * BS words (word = 32-bits)
			size = (bcr >> 16) * (bcr & 0xffff);
			GPU_writeDataMem((unsigned long *)ptr, size);

#if 1
			// already 32-bit word size ((size * 4) / 4)
			GPUDMA_INT(size);
#else
			// Experimental burst dma transfer
			// - X-Files = 0.333333x max for videos
			GPUDMA_INT( size / 3 );
#endif
			return;

		case 0x01000401: // dma chain
#ifdef PSXDMA_LOG
			PSXDMA_LOG("*** DMA 2 - GPU dma chain *** %x addr = %x size = %x\n", chcr, madr, bcr);
#endif

			size = gpuDmaChainSize(madr);
			GPU_dmaChain((unsigned long *)psxM, madr & 0x1fffff);

			// Tekken 3 = use 1.0 only (not 1.5x)

			// Einhander = parse linked list in pieces (todo)
			// Final Fantasy 4 = internal vram time (todo)
			// Rebel Assault 2 = parse linked list in pieces (todo)
			// Vampire Hunter D = allow edits to linked list (todo)
			GPUDMA_INT(size);
			return;

#ifdef PSXDMA_LOG
		default:
			PSXDMA_LOG("*** DMA 2 - GPU unknown *** %x addr = %x size = %x\n", chcr, madr, bcr);
			break;
#endif
	}

	HW_DMA2_CHCR &= SWAP32(~0x01000000);
	DMA_INTERRUPT(2);
}

void gpuInterrupt(void) {
	HW_DMA2_CHCR &= SWAP32(~0x01000000);
	DMA_INTERRUPT(2);
}
