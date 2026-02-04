/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "plugins.h"
#include "spu.h"

//extern GPUopen GPU_open;
extern CBGPUopen GPU_open;

unsigned long gpuDisp;

extern CDRplay CDR_play;
extern CDRstop CDR_stop;

// extern SPUopen SPU_open;

extern PADopen PAD1_open;
extern PADreadPort1 PAD1_readPort1;
extern PADopen PAD2_open;
extern PADreadPort2 PAD2_readPort2;

int StatesC = 0;
extern char CdromLabel[33];
extern int UseGui;
int cdOpenCase = 0;


long PAD1__open(void) {
	SysPrintf("returning pad open ");
	return PAD1_open(&gpuDisp);
}

long PAD2__open(void) {
	return PAD2_open(&gpuDisp);
}

void SignalExit(int sig) {
	ClosePlugins();
//	OnFile_Exit();
}

void SPUirq(void);

void OpenPlugins() {
	int ret;

    SysPrintf("start OpenPlugins()\n");

	SysPrintf("CDR_open()\n");
	ret = CDR_open();
	if (ret != 0) { SysMessage ("Error Opening CDR Plugin\n"); exit(1); }
	SysPrintf("SPU_open()\n");
	ret = SPU_open();
	if (ret != 0) { SysMessage ("Error Opening SPU Plugin\n"); exit(1); }
	SPU_registerCallback(SPUirq);
	SysPrintf("GPU_open()\n");
	ret = GPU_open(&gpuDisp, "P©SX", NULL);
	SysPrintf("after GPU_open()\n");
	if (ret != 0) { SysMessage ("Error Opening GPU Plugin\n"); exit(1); }
	SysPrintf("PAD1_open()\n");
	ret = PAD1_open(&gpuDisp);
	if (ret != 0) { SysMessage ("Error Opening PAD1 Plugin\n"); exit(1); }
	ret = PAD2_open(&gpuDisp);
	if (ret < 0) { SysPrintf("Error Opening PAD2 Plugin\n");  }
	SysPrintf("end OpenPlugins()\n");

}

void ClosePlugins() {
	int ret;

	ret = CDR_close();
	if (ret != 0) { SysMessage ("Error Closing CDR Plugin\n"); exit(1); }
	ret = SPU_close();
	if (ret != 0) { SysMessage ("Error Closing SPU Plugin\n"); exit(1); }
	ret = PAD1_close();
	if (ret != 0) { SysMessage ("Error Closing PAD1 Plugin\n"); exit(1); }
	ret = PAD2_close();
	if (ret != 0) { SysMessage ("Error Closing PAD2 Plugin\n"); exit(1); }
	ret = GPU_close();
	if (ret != 0) { SysMessage ("Error Closing GPU Plugin\n"); exit(1); }
}

void ResetPlugins() {
	int ret;

	CDR_shutdown();
	GPU_shutdown();
	SPU_shutdown();
	PAD1_shutdown();
	PAD2_shutdown();

	ret = CDR_init();
	if (ret != 0) { SysMessage ("CDRinit error : %d\n",ret); exit(1); }
	ret = GPU_init();
	if (ret != 0) { SysMessage ("GPUinit error : %d\n",ret); exit(1); }
	ret = SPU_init();
	if (ret != 0) { SysMessage ("SPUinit error : %d\n",ret); exit(1); }
	ret = PAD1_init(1);
	if (ret != 0) { SysMessage ("PAD1init error : %d\n",ret); exit(1); }
	ret = PAD2_init(2);
	if (ret != 0) { SysMessage ("PAD2init error : %d\n",ret); exit(1); }
}
