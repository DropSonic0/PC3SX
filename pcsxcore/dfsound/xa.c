/***************************************************************************
                            xa.c  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
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

#include "stdafx.h"

#define _IN_XA
#include <stdint.h>

// will be included from spu.c
#ifdef _IN_SPU

////////////////////////////////////////////////////////////////////////
// XA GLOBALS
////////////////////////////////////////////////////////////////////////

xa_decode_t   * xapGlobal=0;

unsigned long * XAFeed  = NULL;
unsigned long * XAPlay  = NULL;
unsigned long   XAStart[44100*4] __attribute__((aligned(32)));
unsigned long * XAEnd   = NULL;

unsigned long   XARepeat  = 0;
unsigned long   XALastVal = 0;

unsigned long * CDDAFeed  = NULL;
unsigned long * CDDAPlay  = NULL;
unsigned long * CDDAStart = NULL;
unsigned long * CDDAEnd   = NULL;

int             iLeftXAVol  = 32767;
int             iRightXAVol = 32767;

static int gauss_ptr = 0;
static int gauss_window[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define gvall0 gauss_window[gauss_ptr]
#define gvall(x) gauss_window[(gauss_ptr+x)&3]
#define gvalr0 gauss_window[4+gauss_ptr]
#define gvalr(x) gauss_window[4+((gauss_ptr+x)&3)]

long cdxa_dbuf_ptr;

////////////////////////////////////////////////////////////////////////
// MIX XA & CDDA
////////////////////////////////////////////////////////////////////////

static int lastxa_lc, lastxa_rc;
static int lastcd_lc, lastcd_rc;

INLINE void MixXA(void)
{
	int i;
	unsigned long cdda_l = 0;
    int decoded_xa;
    int decoded_cdda;
	unsigned long XALastVal = 0;
	int leftvol =iLeftXAVol;
	int rightvol=iRightXAVol;
	int *ssuml=SSumL;
	int *ssumr=SSumR;
	
	for(i=0;i<NSSIZE && XAPlay!=XAFeed;i++)
	{
		XALastVal=*XAPlay++;
		if(XAPlay==XAEnd) XAPlay=XAStart;
		(*ssuml++)+=(((short)(XALastVal&0xffff))       * leftvol)/32768;
		(*ssumr++)+=(((short)((XALastVal>>16)&0xffff)) * rightvol)/32768;
	}
	
	if(XAPlay==XAFeed && XARepeat)
	{
		XARepeat--;
		for(;i<NSSIZE;i++)
		{
			(*ssuml++)+=(((short)(XALastVal&0xffff))       * leftvol)/32768;
			(*ssumr++)+=(((short)((XALastVal>>16)&0xffff)) * rightvol)/32768;
		}
	}
}


////////////////////////////////////////////////////////////////////////
// FEED XA 
////////////////////////////////////////////////////////////////////////

INLINE void FeedXA(xa_decode_t *xap)
{
	int sinc,spos,i,iSize;
	
	if(!bSPUIsOpen) return;
	
	xapGlobal = xap;                                      // store info for save states
	XARepeat  = 100;                                      // set up repeat
	
	iSize=((44100*xap->nsamples)/xap->freq);              // get size
	if(!iSize) return;                                    // none? bye
	
	if(XAFeed<XAPlay) {
		if ((XAPlay-XAFeed)==0) return;               // how much space in my buf?
	} else {
		if (((XAEnd-XAFeed) + (XAPlay-XAStart))==0) return;
	}
	
	spos=0x10000L;
	sinc = (xap->nsamples << 16) / iSize;                 // calc freq by num / size
	
	if(xap->stereo)
	{
		unsigned long * pS=(unsigned long *)xap->pcm;
		unsigned long l=0;
		
		for(i=0;i<iSize;i++)
		{
			while(spos>=0x10000L)
			{
				l = *pS++;
				spos -= 0x10000L;
			}
			
			*XAFeed++=l;
			
			if(XAFeed==XAEnd)
				XAFeed=XAStart;
			if(XAFeed==XAPlay) 
			{
				if(XAPlay!=XAStart)
					XAFeed=XAPlay-1;
				break;
			}
			
			spos += sinc;
		}
	}
	else
	{
		unsigned short * pS=(unsigned short *)xap->pcm;
		unsigned long l;short s=0;
		
		for(i=0;i<iSize;i++)
		{
			while(spos>=0x10000L)
			{
				s = *pS++;
				spos -= 0x10000L;
			}
			l=s;
			
			*XAFeed++=(l|(l<<16));
			
			if(XAFeed==XAEnd)
				XAFeed=XAStart;
			if(XAFeed==XAPlay) 
			{
				if(XAPlay!=XAStart)
					XAFeed=XAPlay-1;
				break;
			}
			
			spos += sinc;
		}
	}
}


#endif
