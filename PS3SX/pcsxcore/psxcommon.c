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

#include "psxcommon.h"
#include "R3000A.h"
#include "PsxBios.h"

#include "cheat.h"
#include "ppf.h"
#include "cdriso.h"
#include "debug.h"

PcsxConfig Config;
boolean NetOpened = FALSE;

int Log = 0;
FILE *emuLog = NULL;
boolean hleSoftCall = FALSE;

int EmuInit(void) {
	cdrIsoInit();
	if (Config.Debug) StartDebugger();
	return psxInit();
}

void EmuReset(void) {
	FreeCheatSearchResults();
	FreeCheatSearchMem();

	psxReset();
}

void EmuShutdown(void) {
	StopDebugger();

	ClearAllCheats();
	FreeCheatSearchResults();
	FreeCheatSearchMem();

	FreePPFCache();

	psxShutdown();
}

void EmuUpdate(void) {
	// Do not allow hotkeys inside a softcall from HLE BIOS
	if (!Config.HLE || !hleSoftCall)
		SysUpdate();

	ApplyCheats();
}

void __Log(char *fmt, ...) {
	va_list list;
	char tmp[1024];

	va_start(list, fmt);
	if (emuLog != NULL) {
		va_list list2;
		va_copy(list2, list);
		vfprintf(emuLog, fmt, list2);
		va_end(list2);
	}
	vsnprintf(tmp, sizeof(tmp), fmt, list);
	SysPrintf("%s", tmp);
	va_end(list);
}
