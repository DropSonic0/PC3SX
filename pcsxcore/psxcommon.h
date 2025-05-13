/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *   schultz.ryan@gmail.com, http://rschultz.ath.cx/code.php               *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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

/* Local includes */
#include "system.h"
#include "debug.h"

/* Ryan TODO WTF is this? */
#if defined (__LINUX__) || defined (__MACOSX__)|| defined (__ppc__)
#define strnicmp strncasecmp
#endif
#define __inline inline

/* Enables NLS/internationalization if active */
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
	char Gpu[MAXPATHLEN];
	char Spu[MAXPATHLEN];
	char Cdr[MAXPATHLEN];
	char Pad1[MAXPATHLEN];
	char Pad2[MAXPATHLEN];
	char Net[MAXPATHLEN];
    char Sio1[MAXPATHLEN];
	char Mcd1[MAXPATHLEN];
	char Mcd2[MAXPATHLEN];
	char Bios[MAXPATHLEN];
	char BiosDir[MAXPATHLEN];
	char PluginsDir[MAXPATHLEN];
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
// FIXME: Count the proper cycle and get rid of this.
// PCSX4ALL team notes about cpu BIAS
// The higher values are faster as the CPU is underclocked but if the game needs more CPU power the game will be slowed down.
// Lower values can be selected for compatibility but the emulator will be very slow.

//#define BIAS	2 //standart pcsx reloaded value.stable.(tekken 2,3, front mission 3,and heavy cpu intensive games works very well(correct speed) with this value).
//#define BIAS	3 //should be ok for the majority of the games. If the game needs more CPU power(tekken 2,3, front mission 3,etc)the game will be slowed down.
//#define BIAS  4 //can be used with some 2D games to gain speed.
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

int EmuInit();
void EmuReset();
void EmuShutdown();
void EmuUpdate();

#if 0
#define malloc	balloc
#define free	bfree
#endif

#ifdef __cplusplus
}
#endif
#endif
