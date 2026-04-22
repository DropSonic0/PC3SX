/***************************************************************************
                            zn.c  -  description
                            --------------------
    begin                : Wed April 23 2004
    copyright            : (C) 2004 by Pete Bernert
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

//*************************************************************************//
// History of changes:
//
// 2004/04/23 - Pete
// - added ZINC zn interface
//
//*************************************************************************//

#include "stdafx.h"
#include "xa.h"

//*************************************************************************//
// global marker, if ZINC is running

int iZincEmu=0;

//-------------------------------------------------------------------------// 
// not used by ZINC

void CALLBACK SPU_d_async(unsigned long cycle);

void CALLBACK ZN_SPUupdate(void) 
{
 SPU_d_async(0);
}

//-------------------------------------------------------------------------// 

void CALLBACK SPU__writeRegister(unsigned long reg, unsigned short val);

void CALLBACK ZN_SPUwriteRegister(unsigned long reg, unsigned short val)
{ 
 SPU__writeRegister(reg,val);
}

//-------------------------------------------------------------------------// 

unsigned short CALLBACK SPU__readRegister(unsigned long reg);

unsigned short CALLBACK ZN_SPUreadRegister(unsigned long reg)
{
 return SPU__readRegister(reg);
}

//-------------------------------------------------------------------------// 
// not used by ZINC

unsigned short CALLBACK SPU__readDMA(void);

unsigned short CALLBACK ZN_SPUreadDMA(void)
{
 return SPU__readDMA();
}

//-------------------------------------------------------------------------// 
// not used by ZINC

void CALLBACK SPU__writeDMA(unsigned short val);

void CALLBACK ZN_SPUwriteDMA(unsigned short val)
{
 SPU__writeDMA(val);
}

//-------------------------------------------------------------------------// 
// not used by ZINC

void CALLBACK SPU__writeDMAMem(unsigned short * pusPSXMem,int iSize);

void CALLBACK ZN_SPUwriteDMAMem(unsigned short * pusPSXMem,int iSize)
{
 SPU__writeDMAMem(pusPSXMem,iSize);
}

//-------------------------------------------------------------------------// 
// not used by ZINC

void CALLBACK SPU__readDMAMem(unsigned short * pusPSXMem,int iSize);

void CALLBACK ZN_SPUreadDMAMem(unsigned short * pusPSXMem,int iSize)
{
 SPU__readDMAMem(pusPSXMem,iSize);
}

//-------------------------------------------------------------------------// 
// not used by ZINC

void CALLBACK SPU_d_playADPCMchannel(xa_decode_t *xap);

void CALLBACK ZN_SPUplayADPCMchannel(xa_decode_t *xap)
{
 SPU_d_playADPCMchannel(xap);
}

//-------------------------------------------------------------------------// 
// attention: no separate SPUInit/Shutdown funcs in ZN interface

long CALLBACK SPU_d_init(void);

#ifdef _WINDOWS

long CALLBACK SPU_d_open(HWND hW);

long CALLBACK ZN_SPUopen(HWND hW)                          
{
 iZincEmu=1;
 SPU_d_init();
 return SPU_d_open(hW);
}

#else

long SPU_d_open(void);

long ZN_SPUopen(void)
{
 iZincEmu=1;
 SPU_d_init();
 return SPU_d_open();
}

#endif

//-------------------------------------------------------------------------// 

long CALLBACK SPU_d_shutdown(void);
long CALLBACK SPU_d_close(void);

long CALLBACK ZN_SPUclose(void)
{
 long lret=SPU_d_close();
 SPU_d_shutdown();
 return lret;
}

//-------------------------------------------------------------------------// 
// not used by ZINC

void CALLBACK SPU_d_registerCallback(void (CALLBACK *callback)(void));

void CALLBACK ZN_SPUregisterCallback(void (CALLBACK *callback)(void))
{
 SPU_d_registerCallback(callback);
}

//-------------------------------------------------------------------------// 
// not used by ZINC

long CALLBACK SPU_d_freeze(unsigned long ulFreezeMode,void * pF);

long CALLBACK ZN_SPUfreeze(unsigned long ulFreezeMode,void * pF)
{
 return SPU_d_freeze(ulFreezeMode,pF);
}

//-------------------------------------------------------------------------// 

extern void (CALLBACK *irqQSound)(unsigned char *,long *,long);      

void CALLBACK ZN_SPUqsound(void (CALLBACK *callback)(unsigned char *,long *,long))
{
 irqQSound = callback;
}

//-------------------------------------------------------------------------// 
