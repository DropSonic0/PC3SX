/***************************************************************************
                        externals.h -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#include "gpu.h"
#include "prim.h"
#include "soft.h"

typedef struct {
    long lLowerpart;
    int bCheckMask;
    uint16_t sSetMask;
    uint32_t lSetMask;
} gxv_draw_t;

extern char * g_file_name;
extern gxv_gpu_t g_gpu;
extern gxv_draw_t g_draw;

#define GPUIsBusy (lGPUstatusRet &= ~GPUSTATUS_IDLE)
#define GPUIsIdle (lGPUstatusRet |= GPUSTATUS_IDLE)

#define GPUIsNotReadyForCommands (lGPUstatusRet &= ~GPUSTATUS_READYFORCOMMANDS)
#define GPUIsReadyForCommands (lGPUstatusRet |= GPUSTATUS_READYFORCOMMANDS)

//X11 render
#define __inline inline
#define CALLBACK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

// linux defines for some windows stuff

#define FALSE 0
#define TRUE 1
#define BOOL unsigned short
#define LOWORD(l)           ((unsigned short)(l))
#define HIWORD(l)           ((unsigned short)(((uint32_t)(l) >> 16) & 0xFFFF))
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define DWORD uint32_t
#define __int64 long long int

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

// draw.c

#ifndef _IN_DRAW

extern char *         pCaptionText;

extern int            iResX;
extern int            iResY;
extern int32_t           GlobalTextAddrX,GlobalTextAddrY,GlobalTextTP;
extern int32_t           GlobalTextREST,GlobalTextABR,GlobalTextPAGE;
extern short          ly0,lx0,ly1,lx1,ly2,lx2,ly3,lx3;
extern long           lLowerpart;
extern BOOL           bIsFirstFrame;
extern int            iWinSize;
extern BOOL           bCheckMask;
extern unsigned short sSetMask;
extern unsigned long  lSetMask;
extern BOOL           bDeviceOK;
extern short          g_m1;
extern short          g_m2;
extern short          g_m3;
extern short          DrawSemiTrans;
extern int            iUseGammaVal;
extern int            iMaintainAspect;
extern int            iDesktopCol;
extern int            iUseNoStretchBlt;
extern int            iShowFPS;
extern int            iFastFwd;
extern int            iDebugMode;
extern int            iFVDisplay;
extern PSXPoint_t     ptCursorPoint[];
extern unsigned short usCursorActive;


#endif

// prim.c

#ifndef _IN_PRIMDRAW

extern BOOL           bUsingTWin;
extern TWin_t         TWin;
//extern unsigned long  clutid;
extern void (*primTableJ[256])(unsigned char *);
extern unsigned short  usMirror;
extern int            iDither;
extern uint32_t  dwCfgFixes;
extern uint32_t  dwActFixes;
extern uint32_t  dwEmuFixes;
extern int            iUseFixes;
extern int            iUseDither;
extern BOOL           bDoVSyncUpdate;
extern int32_t           drawX;
extern int32_t           drawY;
extern int32_t           drawW;
extern int32_t           drawH;

#endif

// gpu.c

#ifndef _IN_GPU

extern gxv_vram_load_t     VRAMWrite;
extern gxv_vram_load_t     VRAMRead;
extern uint16_t DataWriteMode;
extern uint16_t DataReadMode;
extern int            iColDepth;
extern int            iWindowMode;
extern char           szDispBuf[];
extern char           szMenuBuf[];
extern char           szDebugText[];
extern short          sDispWidths[];
extern int            bDebugText;
//extern unsigned int   iMaxDMACommandCounter;
//extern unsigned long  dwDMAChainStop;
extern gxv_gpu_t g_gpu;
extern gxv_gpu_t   PreviousPSXDisplay;
extern BOOL           bSkipNextFrame;
extern long           lGPUstatusRet;
//extern long           drawingLines;
extern unsigned char  * psxVSecure;
extern unsigned char  * psxVub;
extern signed char    * psxVsb;
extern unsigned short * psxVuw;
extern signed short   * psxVsw;
extern uint32_t  * psxVul;
extern int32_t    * psxVsl;
extern unsigned short * psxVuw_eom;
extern BOOL           bChangeWinMode;
extern long           lSelectedSlot;
extern BOOL           bInitCap;
extern DWORD          dwLaceCnt;
extern uint32_t lGPUInfoVals[16];
extern uint32_t ulStatusControl[256];
extern uint32_t  vBlank;
extern int            iRumbleVal;
extern int            iRumbleTime;

#endif

// menu.c

#ifndef _IN_MENU

extern uint32_t dwCoreFlags;

#endif

// key.c

#ifndef _IN_KEY

extern unsigned long  ulKeybits;

#endif

// fps.c

#ifndef _IN_FPS

extern int            UseFrameLimit;
extern int            UseFrameSkip;
extern float          fFrameRate;
extern int            iFrameLimit;
extern float          fFrameRateHz;
extern float          fps_skip;
extern float          fps_cur;

#endif

// key.c

#ifndef _IN_KEY

#endif

// cfg.c

#ifndef _IN_CFG

extern char * pConfigFile;

#endif

// zn.c

#ifndef _IN_ZN

extern uint32_t dwGPUVersion;
extern int           iGPUHeight;
extern int           iGPUHeightMask;
extern int           GlobalTextIL;
extern int           iTileCheat;

#endif


