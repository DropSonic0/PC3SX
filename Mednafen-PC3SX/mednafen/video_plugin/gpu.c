/*
 * Copyright (C) 2010 Benoit Gschwind
 * Inspired by original author : Pete Bernert
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "gpu.h"
#include "swap.h"

#define INFO_TW        0
#define INFO_DRAWSTART 1
#define INFO_DRAWEND   2
#define INFO_DRAWOFF   3

#define DR_NORMAL        0
#define DR_VRAMTRANSFER  1

//Init Quit functions
long		pkGPUopen			(unsigned long * disp, char * CapText, char * CfgFile)
{
	return 0;
}

long		pkGPUclose			()
{
	return 0;
}

long		pkGPUinit			()
{
	memset(g_gpu.ulStatusControl, 0, 256 * sizeof(uint32_t)); // init save state scontrol field
	g_gpu.psxVSecure.u8 = (uint8_t *) malloc((512 * 2) * 1024 + (1024 * 1024)); // always alloc one extra MB for soft drawing funcs security
	if (!g_gpu.psxVSecure.u8)
	{
		return -1;
	}

	//!!! ATTENTION !!!
	g_gpu.psx_vram.u8 = g_gpu.psxVSecure.u8 + 512 * 1024; // security offset into double sized psx vram!
	g_gpu.psxVuw_eom.u16 = g_gpu.psx_vram.u16 + 1024 * 512; // pre-calc of end of vram

	memset(g_gpu.psxVSecure.s8, 0x00, (512 * 2) * 1024 + (1024 * 1024));
	memset(g_gpu.lGPUInfoVals, 0x00, 16 * sizeof(uint32_t));

	g_gpu.dsp.range.x0 = 0;
	g_gpu.dsp.range.x1 = 0;
	g_gpu.lGPUdataRet = 0x400;

	g_gpu.DataWriteMode = DR_NORMAL;

	// Reset transfer values, to prevent mis-transfer of data
	memset(&g_gpu.VRAMWrite, 0, sizeof(gxv_vram_load_t));
	memset(&g_gpu.VRAMRead, 0, sizeof(gxv_vram_load_t));

	// device initialised already !
	g_gpu.status_reg = 0x14802000;
	return 0;
}

long		pkGPUshutdown		()
{
	free(g_gpu.psxVSecure.s8);
	return 0;
}



void  pkGPUupdateLace(void) // VSYNC
{
	g_gpu.status_reg ^= STATUS_ODDLINES;

	if (g_gpu.status_reg & STATUS_DISPLAYDISABLED)
	{
		return;
	}
}

uint32_t  pkGPUreadStatus(void) // READ STATUS
{
	return g_gpu.status_reg;
}

void  pkGPUwriteStatus(uint32_t gdata)
{
	uint32_t lCommand = (gdata >> 24) & 0xff;
	g_gpu.ulStatusControl[lCommand] = gdata; // store command for freezing
	switch (lCommand) {
	case 0x00:
		/* reset GPU */
		memset(g_gpu.lGPUInfoVals, 0x00, 16 * sizeof(uint32_t));
		/* STATUS_READYFORCOMMANDS | STATUS_IDLE | STATUS_DISPLAYDISABLED | UNKNOW13 */
		g_gpu.status_reg = 0x14802000;
		g_gpu.DataWriteMode = DR_NORMAL;
		g_gpu.DataReadMode = DR_NORMAL;
		g_prim.drawX = 0;
		g_prim.drawY = 0;
		g_prim.drawW = 0;
		g_prim.drawH = 0;
		g_draw.sSetMask = 0;
		g_draw.lSetMask = 0;
		g_draw.bCheckMask = 0;
		g_prim.usMirror = 0;
		g_soft.GlobalTextAddrX = 0;
		g_soft.GlobalTextAddrY = 0;
		g_soft.GlobalTextTP = 0;
		g_soft.GlobalTextABR = 0;
		g_prim.bUsingTWin = 0;
		return;
	case 0x01:
		/* reset command buffer */
		//fprintf(stderr, "Reset command buffer not implemented\n");
		return;
	case 0x02:
		/* reset IRQ */
		//fprintf(stderr, "Reset IRQ not implemented\n");
		return;
	case 0x03:
		/* Enable or disable the display */
		if (gdata & 0x01)
			g_gpu.status_reg |= STATUS_DISPLAYDISABLED;
		else
			g_gpu.status_reg &= ~STATUS_DISPLAYDISABLED;
		return;
	case 0x04:
		/* Set the transfering mode */
		gdata &= 0x03; // Only want the lower two bits
		g_gpu.DataWriteMode = DR_NORMAL;
		g_gpu.DataReadMode = DR_NORMAL;
		if (gdata == 0x02)
			g_gpu.DataWriteMode = DR_VRAMTRANSFER;
		if (gdata == 0x03)
			g_gpu.DataReadMode = DR_VRAMTRANSFER;
		g_gpu.status_reg &= ~STATUS_DMABITS; // Clear the current settings of the DMA bits
		g_gpu.status_reg |= (gdata << 29); // Set the DMA bits according to the received data
		return;
	case 0x05:
		/* setting display position */
		g_gpu.dsp.position.y = (short) ((gdata >> 10) & 0x1ff);
		g_gpu.dsp.position.x = (short) (gdata & 0x3ff);
		//fprintf(stderr, "Update display position X=%d,Y=%d\n",
		//		g_gpu.dsp.position.x, g_gpu.dsp.position.y);
		return;
	case 0x06:
		/* Set width */
		g_gpu.dsp.range.x0 = (short) (gdata & 0x03ff);
		g_gpu.dsp.range.x1 = (short) ((gdata >> 12) & 0x0fff);
		//fprintf(stderr, "Setrange x0 : %d, x1 : %d\n", g_gpu.dsp.range.x0,
		//		g_gpu.dsp.range.x1);
		return;
	case 0x07:
		/* Set height */
		g_gpu.dsp.range.y0 = (short) (gdata & 0x3ff);
		g_gpu.dsp.range.y1 = (short) ((gdata >> 10) & 0x3ff);
		//fprintf(stderr, "Setrange y0 : %d, y1 : %d\n", g_gpu.dsp.range.y0,
		//		g_gpu.dsp.range.y1);
		return;
	case 0x08:
		/* setting display infos */
		//fprintf(stderr, "command %x\n", gdata&0x000043);
		switch (gdata & 0x000043) {
		case 0x00:
			g_gpu.dsp.mode.x = 10;
			break;
		case 0x01:
			g_gpu.dsp.mode.x = 8;
			break;
		case 0x02:
			g_gpu.dsp.mode.x = 5;
			break;
		case 0x03:
			g_gpu.dsp.mode.x = 4;
			break;
		case 0x40:
			g_gpu.dsp.mode.x = 7;
			break;
		default:
			g_gpu.dsp.mode.x = 0;
		}

		/* clear width status */
		g_gpu.status_reg &= ~STATUS_WIDTHBITS;
		/* update status reg */
		g_gpu.status_reg |= ((gdata & 0x00000003) << 17)
				| ((gdata & 0x00000040) << 10);

		switch (gdata & 0x000004) {
		case 0x00:
			g_gpu.status_reg &= ~STATUS_DOUBLEHEIGHT;
			g_gpu.dsp.mode.y = 1;
			break;
		case 0x04:
			g_gpu.status_reg |= STATUS_DOUBLEHEIGHT;
			g_gpu.dsp.mode.y = 2;
			break;
		}

		switch (gdata & 0x08) {
		case 0x00:
			g_gpu.status_reg &= ~STATUS_PAL;
			g_gpu.fps = 60;
			break;
		case 0x08:
			g_gpu.status_reg |= STATUS_PAL;
			g_gpu.fps = 50;
			break;
		}
		//fprintf(stderr, "video mode %dx%d\n", g_gpu.dsp.mode.x, g_gpu.dsp.mode.y);

		if ((gdata & 0x10)) {
			g_gpu.status_reg |= STATUS_RGB24;
		} else {
			g_gpu.status_reg &= ~STATUS_RGB24;
		}

		if ((gdata & 0x20)) {
			g_gpu.status_reg |= STATUS_INTERLACED;
		} else {
			g_gpu.status_reg &= ~STATUS_INTERLACED;
		}
		return;
	case 0x10:
		/* ask about GPU version and other stuff */
		gdata &= 0xff;
		switch (gdata) {
		case 0x02:
			g_gpu.lGPUdataRet = g_gpu.lGPUInfoVals[INFO_TW]; // tw infos
			return;
		case 0x03:
			g_gpu.lGPUdataRet = g_gpu.lGPUInfoVals[INFO_DRAWSTART]; // draw start
			return;
		case 0x04:
			g_gpu.lGPUdataRet = g_gpu.lGPUInfoVals[INFO_DRAWEND]; // draw end
			return;
		case 0x05:
		case 0x06:
			g_gpu.lGPUdataRet = g_gpu.lGPUInfoVals[INFO_DRAWOFF]; // draw offset
			return;
		case 0x07:
			if (0)
				g_gpu.lGPUdataRet = 0x01;
			else
				g_gpu.lGPUdataRet = 0x02; // gpu type
			return;
		case 0x08:
		case 0x0F: // some bios addr?
			g_gpu.lGPUdataRet = 0xBFC03720;
			return;
		}
		return;
	default:
		fprintf(stderr, "Unknow command %02x\n", lCommand);
	}
}

inline void FinishedVRAMWrite(void)
{
	// Set register to NORMAL operation
	g_gpu.DataWriteMode = DR_NORMAL;
	// Reset transfer values, to prevent mis-transfer of data
	g_gpu.VRAMWrite.x = 0;
	g_gpu.VRAMWrite.y = 0;
	g_gpu.VRAMWrite.Width = 0;
	g_gpu.VRAMWrite.Height = 0;
	g_gpu.VRAMWrite.ColsRemaining = 0;
	g_gpu.VRAMWrite.RowsRemaining = 0;
}

inline void FinishedVRAMRead(void)
{
	// Set register to NORMAL operation
	g_gpu.DataReadMode = DR_NORMAL;
	// Reset transfer values, to prevent mis-transfer of data
	g_gpu.VRAMRead.x = 0;
	g_gpu.VRAMRead.y = 0;
	g_gpu.VRAMRead.Width = 0;
	g_gpu.VRAMRead.Height = 0;
	g_gpu.VRAMRead.ColsRemaining = 0;
	g_gpu.VRAMRead.RowsRemaining = 0;

	// Indicate GPU is no longer ready for VRAM data in the STATUS REGISTER
	g_gpu.status_reg &= ~STATUS_READYFORVRAM;
}

////////////////////////////////////////////////////////////////////////
// core read from vram
////////////////////////////////////////////////////////////////////////

void  pkGPUreadDataMem(uint32_t * pMem, int iSize)
{
	int i;

	if (g_gpu.DataReadMode != DR_VRAMTRANSFER)
	{
		return;
	}

	g_gpu.status_reg &= ~STATUS_IDLE;

	// adjust read ptr, if necessary

	while (g_gpu.VRAMRead.ImagePtr >= g_gpu.psxVuw_eom.u16)
		g_gpu.VRAMRead.ImagePtr -= 512 * 1024;
	while (g_gpu.VRAMRead.ImagePtr < g_gpu.psx_vram.u16)
		g_gpu.VRAMRead.ImagePtr += 512 * 1024;

	for (i = 0; i < iSize; i++) {
		// do 2 seperate 16bit reads for compatibility (wrap issues)
		if ((g_gpu.VRAMRead.ColsRemaining > 0) && (g_gpu.VRAMRead.RowsRemaining
				> 0)) {
			// lower 16 bit
			g_gpu.lGPUdataRet = (uint32_t) GETLE16(g_gpu.VRAMRead.ImagePtr);

			g_gpu.VRAMRead.ImagePtr++;
			if (g_gpu.VRAMRead.ImagePtr >= g_gpu.psxVuw_eom.u16)
				g_gpu.VRAMRead.ImagePtr -= 512 * 1024;
			g_gpu.VRAMRead.RowsRemaining--;

			if (g_gpu.VRAMRead.RowsRemaining <= 0) {
				g_gpu.VRAMRead.RowsRemaining = g_gpu.VRAMRead.Width;
				g_gpu.VRAMRead.ColsRemaining--;
				g_gpu.VRAMRead.ImagePtr += 1024 - g_gpu.VRAMRead.Width;
				if (g_gpu.VRAMRead.ImagePtr >= g_gpu.psxVuw_eom.u16)
					g_gpu.VRAMRead.ImagePtr -= 512 * 1024;
			}

			// higher 16 bit (always, even if it's an odd width)
			g_gpu.lGPUdataRet |= (uint32_t) GETLE16(g_gpu.VRAMRead.ImagePtr)
					<< 16;
			PUTLE32(pMem, g_gpu.lGPUdataRet);
			pMem++;

			if (g_gpu.VRAMRead.ColsRemaining <= 0) {
				FinishedVRAMRead();
				goto ENDREAD;
			}

			g_gpu.VRAMRead.ImagePtr++;
			if (g_gpu.VRAMRead.ImagePtr >= g_gpu.psxVuw_eom.u16)
				g_gpu.VRAMRead.ImagePtr -= 512 * 1024;
			g_gpu.VRAMRead.RowsRemaining--;
			if (g_gpu.VRAMRead.RowsRemaining <= 0) {
				g_gpu.VRAMRead.RowsRemaining = g_gpu.VRAMRead.Width;
				g_gpu.VRAMRead.ColsRemaining--;
				g_gpu.VRAMRead.ImagePtr += 1024 - g_gpu.VRAMRead.Width;
				if (g_gpu.VRAMRead.ImagePtr >= g_gpu.psxVuw_eom.u16)
					g_gpu.VRAMRead.ImagePtr -= 512 * 1024;
			}
			if (g_gpu.VRAMRead.ColsRemaining <= 0) {
				FinishedVRAMRead();
				goto ENDREAD;
			}
		} else {
			FinishedVRAMRead();
			goto ENDREAD;
		}
	}

	ENDREAD: g_gpu.status_reg |= STATUS_IDLE;
}

////////////////////////////////////////////////////////////////////////

uint32_t  pkGPUreadData(void)
{
	uint32_t l;
	pkGPUreadDataMem(&l, 1);
	return g_gpu.lGPUdataRet;
}

////////////////////////////////////////////////////////////////////////
// processes data send to GPU data register
// extra table entries for fixing polyline troubles
////////////////////////////////////////////////////////////////////////

const unsigned char primTableCX[256] = {
// 00
		0, 0, 3, 0, 0, 0, 0, 0,
		// 08
		0, 0, 0, 0, 0, 0, 0, 0,
		// 10
		0, 0, 0, 0, 0, 0, 0, 0,
		// 18
		0, 0, 0, 0, 0, 0, 0, 0,
		// 20
		4, 4, 4, 4, 7, 7, 7, 7,
		// 28
		5, 5, 5, 5, 9, 9, 9, 9,
		// 30
		6, 6, 6, 6, 9, 9, 9, 9,
		// 38
		8, 8, 8, 8, 12, 12, 12, 12,
		// 40
		3, 3, 3, 3, 0, 0, 0, 0,
		// 48
		//  5,5,5,5,6,6,6,6,    // FLINE
		254, 254, 254, 254, 254, 254, 254, 254,
		// 50
		4, 4, 4, 4, 0, 0, 0, 0,
		// 58
		//  7,7,7,7,9,9,9,9,    // GLINE
		255, 255, 255, 255, 255, 255, 255, 255,
		// 60
		3, 3, 3, 3, 4, 4, 4, 4,
		// 68
		2, 2, 2, 2, 3, 3, 3, 3, // 3=SPRITE1???
		// 70
		2, 2, 2, 2, 3, 3, 3, 3,
		// 78
		2, 2, 2, 2, 3, 3, 3, 3,
		// 80
		4, 0, 0, 0, 0, 0, 0, 0,
		// 88
		0, 0, 0, 0, 0, 0, 0, 0,
		// 90
		0, 0, 0, 0, 0, 0, 0, 0,
		// 98
		0, 0, 0, 0, 0, 0, 0, 0,
		// a0
		3, 0, 0, 0, 0, 0, 0, 0,
		// a8
		0, 0, 0, 0, 0, 0, 0, 0,
		// b0
		0, 0, 0, 0, 0, 0, 0, 0,
		// b8
		0, 0, 0, 0, 0, 0, 0, 0,
		// c0
		3, 0, 0, 0, 0, 0, 0, 0,
		// c8
		0, 0, 0, 0, 0, 0, 0, 0,
		// d0
		0, 0, 0, 0, 0, 0, 0, 0,
		// d8
		0, 0, 0, 0, 0, 0, 0, 0,
		// e0
		0, 1, 1, 1, 1, 1, 1, 0,
		// e8
		0, 0, 0, 0, 0, 0, 0, 0,
		// f0
		0, 0, 0, 0, 0, 0, 0, 0,
		// f8
		0, 0, 0, 0, 0, 0, 0, 0 };

void  pkGPUwriteDataMem(uint32_t * pMem, int iSize)
{
	unsigned char command;
	uint32_t gdata = 0;
	int i = 0;

	g_gpu.status_reg &= ~STATUS_IDLE;
	g_gpu.status_reg &= ~STATUS_READYFORCOMMANDS;

	STARTVRAM:

	if (g_gpu.DataWriteMode == DR_VRAMTRANSFER) {
		char bFinished = 0;

		// make sure we are in vram
		while (g_gpu.VRAMWrite.ImagePtr >= g_gpu.psxVuw_eom.u16)
			g_gpu.VRAMWrite.ImagePtr -= 512 * 1024;
		while (g_gpu.VRAMWrite.ImagePtr < g_gpu.psx_vram.u16)
			g_gpu.VRAMWrite.ImagePtr += 512 * 1024;

		// now do the loop
		while (g_gpu.VRAMWrite.ColsRemaining > 0) {
			while (g_gpu.VRAMWrite.RowsRemaining > 0) {
				if (i >= iSize) {
					goto ENDVRAM;
				}
				i++;

				gdata = GETLE32(pMem);
				pMem++;

				PUTLE16(g_gpu.VRAMWrite.ImagePtr, (unsigned short)gdata);
				g_gpu.VRAMWrite.ImagePtr++;
				if (g_gpu.VRAMWrite.ImagePtr >= g_gpu.psxVuw_eom.u16)
					g_gpu.VRAMWrite.ImagePtr -= 512 * 1024;
				g_gpu.VRAMWrite.RowsRemaining--;

				if (g_gpu.VRAMWrite.RowsRemaining <= 0) {
					g_gpu.VRAMWrite.ColsRemaining--;
					if (g_gpu.VRAMWrite.ColsRemaining <= 0) // last pixel is odd width
					{
						gdata
								= (gdata & 0xFFFF)
										| (((uint32_t) GETLE16(g_gpu.VRAMWrite.ImagePtr))
												<< 16);
						FinishedVRAMWrite();
						goto ENDVRAM;
					}
					g_gpu.VRAMWrite.RowsRemaining = g_gpu.VRAMWrite.Width;
					g_gpu.VRAMWrite.ImagePtr += 1024 - g_gpu.VRAMWrite.Width;
				}

				PUTLE16(g_gpu.VRAMWrite.ImagePtr, (unsigned short)(gdata>>16));
				g_gpu.VRAMWrite.ImagePtr++;
				if (g_gpu.VRAMWrite.ImagePtr >= g_gpu.psxVuw_eom.u16)
					g_gpu.VRAMWrite.ImagePtr -= 512 * 1024;
				g_gpu.VRAMWrite.RowsRemaining--;
			}

			g_gpu.VRAMWrite.RowsRemaining = g_gpu.VRAMWrite.Width;
			g_gpu.VRAMWrite.ColsRemaining--;
			g_gpu.VRAMWrite.ImagePtr += 1024 - g_gpu.VRAMWrite.Width;
			bFinished = 1;
		}

		FinishedVRAMWrite();
	}

	ENDVRAM:

	if (g_gpu.DataWriteMode == DR_NORMAL) {
		void (* *primFunc)(unsigned char *);
		primFunc = primTableJ;

		for (; i < iSize;) {
			if (g_gpu.DataWriteMode == DR_VRAMTRANSFER)
				goto STARTVRAM;

			gdata = GETLE32(pMem);
			pMem++;
			i++;

			if (g_gpu.gpuDataC == 0) {
				command = (unsigned char) ((gdata >> 24) & 0xff);

				//if(command>=0xb0 && command<0xc0) auxprintf("b0 %x!!!!!!!!!\n",command);

				if (primTableCX[command]) {
					g_gpu.gpuDataC = primTableCX[command];
					g_gpu.gpuCommand = command;
					PUTLE32(&g_gpu.gpuDataM[0], gdata);
					g_gpu.gpuDataP = 1;
				} else {
					//fprintf(stderr, "unknow command %02x \n", command);
					continue;
				}
			} else {
				PUTLE32(&g_gpu.gpuDataM[g_gpu.gpuDataP], gdata);
				if (g_gpu.gpuDataC > 128) {
					if ((g_gpu.gpuDataC == 254 && g_gpu.gpuDataP >= 3)
							|| (g_gpu.gpuDataC == 255 && g_gpu.gpuDataP >= 4
									&& !(g_gpu.gpuDataP & 1))) {
						if ((g_gpu.gpuDataM[g_gpu.gpuDataP] & 0xF000F000)
								== 0x50005000)
							g_gpu.gpuDataP = g_gpu.gpuDataC - 1;
					}
				}
				g_gpu.gpuDataP++;
			}

			if (g_gpu.gpuDataP == g_gpu.gpuDataC) {
				g_gpu.gpuDataC = g_gpu.gpuDataP = 0;
				primFunc[g_gpu.gpuCommand]((unsigned char *) g_gpu.gpuDataM);
			}
		}
	}

	g_gpu.lGPUdataRet = gdata;

	g_gpu.status_reg |= STATUS_READYFORCOMMANDS;
	g_gpu.status_reg |= STATUS_IDLE;
}

////////////////////////////////////////////////////////////////////////

void  pkGPUwriteData(uint32_t gdata)
{
	PUTLE32(&gdata, gdata);
	pkGPUwriteDataMem(&gdata, 1);
}

////////////////////////////////////////////////////////////////////////
// this functions will be removed soon (or 'soonish')... not really needed, but some emus want them
////////////////////////////////////////////////////////////////////////


long  pkGPUgetMode(void)
{
	long iT = 0;

	if (g_gpu.DataWriteMode == DR_VRAMTRANSFER)
		iT |= 0x1;
	if (g_gpu.DataReadMode == DR_VRAMTRANSFER)
		iT |= 0x2;
	return iT;
}

unsigned long lUsedAddr[3];

inline char CheckForEndlessLoop(unsigned long laddr)
{
	if (laddr == lUsedAddr[1])
	{
		return 1;
	}
	if (laddr == lUsedAddr[2])
	{return 1;}


	if (laddr < lUsedAddr[0])
		lUsedAddr[1] = laddr;
	else
		lUsedAddr[2] = laddr;
	lUsedAddr[0] = laddr;
	return 0;
}

long  pkGPUdmaChain(uint32_t * baseAddrL, uint32_t addr)
{
	uint32_t dmaMem;
	unsigned char * baseAddrB;
	short count;
	unsigned int DMACommandCounter = 0;

	g_gpu.status_reg &= ~STATUS_IDLE;

	lUsedAddr[0] = lUsedAddr[1] = lUsedAddr[2] = 0xffffff;

	baseAddrB = (unsigned char*) baseAddrL;

	do {
		if (1)
			addr &= 0x1FFFFC;
		if (DMACommandCounter++ > 2000000)
			break;
		if (CheckForEndlessLoop(addr))
			break;

		count = baseAddrB[addr + 3];

		dmaMem = addr + 4;

		if (count > 0)
			pkGPUwriteDataMem(&baseAddrL[dmaMem >> 2], count);

		addr = GETLE32(&baseAddrL[addr>>2]) & 0xffffff;
	} while (addr != 0xffffff);

	g_gpu.status_reg |= STATUS_IDLE;
	return 0;
}


typedef struct GPUFREEZETAG {
	uint32_t ulFreezeVersion; // should be always 1 for now (set by main emu)
	uint32_t ulStatus; // current gpu status
	uint32_t ulControl[256]; // latest control register values
	unsigned char psxVRam[1024 * 1024 * 2]; // current VRam image (full 2 MB for ZN)
} GPUFreeze_t;

////////////////////////////////////////////////////////////////////////

long  pkGPUfreeze(uint32_t ulGetFreezeData, GPUFreeze_t * pF) {
	//----------------------------------------------------//
	if (ulGetFreezeData == 2) // 2: info, which save slot is selected? (just for display)
	{
		long lSlotNum = *((long *) pF);
		if (lSlotNum < 0)
			return 0;
		if (lSlotNum > 8)
			return 0;
		//g_gpu.lSelectedSlot = lSlotNum + 1;
		//BuildDispMenu(0);
		return 1;
	}
	//----------------------------------------------------//
	if (!pF)
		return 0; // some checks
	if (pF->ulFreezeVersion != 1)
		return 0;

	if (ulGetFreezeData == 1) // 1: get data
	{
		pF->ulStatus = g_gpu.status_reg;
		memcpy(pF->ulControl, g_gpu.ulStatusControl, 256 * sizeof(uint32_t));
		memcpy(pF->psxVRam, g_gpu.psx_vram.u8, 1024 * 512 * 2);

		return 1;
	}

	if (ulGetFreezeData != 0)
		return 0; // 0: set data

	g_gpu.status_reg = pF->ulStatus;
	memcpy(g_gpu.ulStatusControl, pF->ulControl, 256 * sizeof(uint32_t));
	memcpy(g_gpu.psx_vram.u8, pF->psxVRam, 1024 * 512 * 2);

	// RESET TEXTURE STORE HERE, IF YOU USE SOMETHING LIKE THAT

	pkGPUwriteStatus(g_gpu.ulStatusControl[0]);
	pkGPUwriteStatus(g_gpu.ulStatusControl[1]);
	pkGPUwriteStatus(g_gpu.ulStatusControl[2]);
	pkGPUwriteStatus(g_gpu.ulStatusControl[3]);
	pkGPUwriteStatus(g_gpu.ulStatusControl[8]); // try to repair things
	pkGPUwriteStatus(g_gpu.ulStatusControl[6]);
	pkGPUwriteStatus(g_gpu.ulStatusControl[7]);
	pkGPUwriteStatus(g_gpu.ulStatusControl[5]);
	pkGPUwriteStatus(g_gpu.ulStatusControl[4]);

	return 1;
}

//Simple functions
void		pkGPUcursor			(int iPlayer, int x, int y)
{
}

void		pkGPUmakeSnapshot	(void)
{
}

void		pkGPUgetScreenPic		(unsigned char * pMem)
{
}

void		pkGPUshowScreenPic		(unsigned char * pMem)
{
}

void		pkGPUsetfix				(uint32_t dwFixBits)
{
	g_prim.dwEmuFixes = dwFixBits;
}

void		pkGPUsetMode			(unsigned long gdata)
{
}

void		pkGPUvBlank				(int val)
{
}

void		pkGPUabout				(void)
{
	return;
}

long		pkGPUtest				(void)
{
	return 0;
}

long		pkGPUconfigure			(void)
{
	return 0;
}

