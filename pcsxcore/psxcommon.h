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
* This file contains common definitions and includes for all parts of the 
* emulator core.
*/

#ifndef __PSXCOMMON_H__
#define __PSXCOMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
//#include "config.h"

/* System includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <assert.h>
#include "zlib/zlib.h"

#define MAXPATHLEN 512

// Define types
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef intptr_t sptr;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uptr;

typedef uint8_t boolean;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// Local includes
#include "system.h"
#include "debug.h"

#if defined (__LINUX__) || defined (__MACOSX__)|| defined (__ppc__)
#define strnicmp strncasecmp
#endif
#define __inline inline

// Enables NLS/internationalization if active
#ifdef ENABLE_NLS

#include <libintl.h>

#undef _
#define _(String) gettext(String)
#ifdef gettext_noop
#  define N_(String) gettext_noop (String)
#else
#  define N_(String) (String)
#endif

#else

#define _(msgid) msgid
#define N_(msgid) msgid

#endif

extern int Log;
void __Log(char *fmt, ...);

void __Log(char *fmt, ...);

typedef struct {
	char Gpu[256];
	char Spu[256];
	char Cdr[256];
	char Pad1[256];
	char Pad2[256];
	char Net[256];
	char Sio1[256];
	char Mcd1[256];
	char Mcd2[256];
	char Bios[256];
	char BiosDir[MAXPATHLEN];
	char PluginsDir[MAXPATHLEN];
	char PatchesDir[MAXPATHLEN];
	char IsoImgDir[MAXPATHLEN];
	long Debug;
	long Xa;
	long Sio;
	long Mdec;
	long PsxAuto;
	long PsxType;		/* NTSC or PAL */
	long Cdda;
	long HLE;
	long Cpu;
	long Dbg;
	long PsxOut;
	long SpuIrq;
	long RCntFix;
	long UseNet;
	long VSyncWA;
	long PsxStock;
	long PsxClock;
	long GPUEnaFPSLimit; // To store EnableFPSLimit
	float GPUUserFPS;   // To store UserFPS
} PcsxConfig;

PcsxConfig Config;

extern long LoadCdBios;
extern int StatesC;
extern int cdOpenCase;
extern int NetOpened;


#define gzfreeze(ptr, size) { \
	if (Mode == 1) gzwrite(f, ptr, size); \
	if (Mode == 0) gzread(f, ptr, size); \
}

#define gzfreezel(ptr) gzfreeze(ptr, sizeof(ptr))

// Make the timing events trigger faster as we are currently assuming everything
// takes one cycle, which is not the case on real hardware.
// FIXME: Count the proper cycle and get rid of this
#define BIAS	2
#define PSXCLK	33868800	/* 33.8688 MHz */

enum {
	PSX_TYPE_NTSC = 0,
	PSX_TYPE_PAL
}; // PSX Types

enum {
	CPU_DYNAREC = 0,
	CPU_INTERPRETER
}; // CPU Types

enum {
	CDDA_ENABLED_LE = 0,
	CDDA_DISABLED,
	CDDA_ENABLED_BE
}; // CDDA Types

int EmuInit();
void EmuReset();
void EmuShutdown();
void EmuUpdate();

#ifdef __cplusplus
}
#endif
#endif
