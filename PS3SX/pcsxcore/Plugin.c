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
#include "Spu.h"

unsigned long gpuDisp;

int StatesC = 0;
extern int UseGui;
int cdOpenCase = 0;

int OpenPlugins(void) {
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
	ret = GPU_open(&gpuDisp, (char*)"PSX", NULL);
	SysPrintf("after GPU_open()\n");
	if (ret != 0) { SysMessage ("Error Opening GPU Plugin\n"); exit(1); }
	SysPrintf("PAD1_open()\n");
	ret = PAD1_open(&gpuDisp);
	if (ret != 0) { SysMessage ("Error Opening PAD1 Plugin\n"); exit(1); }
	ret = PAD2_open(&gpuDisp);
	if (ret < 0) { SysPrintf("Error Opening PAD2 Plugin\n");  }
	SysPrintf("end OpenPlugins()\n");

	return 0;
}

void ClosePlugins(void) {
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
