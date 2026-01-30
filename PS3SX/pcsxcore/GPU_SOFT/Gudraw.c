/***************************************************************************
                          Gudraw.c  -  description
                             -------------------
    copyright            : (C) 2006 by Yoshihiro
 
 
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

#include "stdafx.h"

#define _IN_DRAW

#include "externals.h"
#include "gpu.h"
#include "draw.h"
#include "prim.h"
#include "menu.h"
#include "interp.h"
#include "swap.h"

int Gpu_updated = 0;

////////////////////////////////////////////////////////////////////////////////////
// misc globals
////////////////////////////////////////////////////////////////////////////////////

int            iResX;
int            iResY;
long           lLowerpart;
BOOL           bIsFirstFrame = TRUE;
BOOL           bCheckMask=FALSE;
unsigned short sSetMask=0;
unsigned long  lSetMask=0;
int            iDesktopCol=32;
int            iShowFPS=0;
int            iWinSize; 
int            iUseScanLines=0;
int            iUseNoStretchBlt=0;
int            iFastFwd=0;
int            iDebugMode=0;
int            iFVDisplay=0;
PSXPoint_t     ptCursorPoint[8];
unsigned short usCursorActive=0;

#define RESX_MAX 640	//Vmem width
#define RESY_MAX 480	//Vmem height

int		iResX_Max=RESX_MAX;
int		iResY_Max=RESY_MAX;
int		finalw,finalh;

int iFPSEInterface = 0;

unsigned int   LUT16to32[65536];
unsigned int   RGBtoYUV[65536];
char *               pCaptionText;
static unsigned char	GXtexture[RESX_MAX*RESY_MAX*2];
//char *	Xpixels;
static unsigned char	Xpixels[RESX_MAX*RESY_MAX*2];


void init_video()
{
}


void DestroyDisplay(void)
{
}


void clear_screen(uint16_t color)
{

}

void DoClearScreenBuffer(void) {

}

void DoClearFrontBuffer(void)   
{

}


////////////////////////////////////////////////////////////////////////
// generic 2xSaI helpers
////////////////////////////////////////////////////////////////////////

void *         pSaISmallBuff=NULL;
void *         pSaIBigBuff=NULL;

#define GET_RESULT(A, B, C, D) ((A != C || A != D) - (B != C || B != D))

static __inline int GetResult1(DWORD A, DWORD B, DWORD C, DWORD D, DWORD E)
{
 int x = 0;
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r+=1; 
 if (y <= 1) r-=1;
 return r;
}

static __inline int GetResult2(DWORD A, DWORD B, DWORD C, DWORD D, DWORD E) 
{
 int x = 0; 
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r-=1; 
 if (y <= 1) r+=1;
 return r;
}

#define colorMask8     0x00FEFEFE
#define lowPixelMask8  0x00010101
#define qcolorMask8    0x00FCFCFC
#define qlowpixelMask8 0x00030303

#define INTERPOLATE8(A, B) ((((A & colorMask8) >> 1) + ((B & colorMask8) >> 1) + (A & B & lowPixelMask8)))
#define Q_INTERPOLATE8(A, B, C, D) (((((A & qcolorMask8) >> 2) + ((B & qcolorMask8) >> 2) + ((C & qcolorMask8) >> 2) + ((D & qcolorMask8) >> 2) \
	+ ((((A & qlowpixelMask8) + (B & qlowpixelMask8) + (C & qlowpixelMask8) + (D & qlowpixelMask8)) >> 2) & qlowpixelMask8))))


void Super2xSaI_ex8(unsigned char *srcPtr, DWORD srcPitch,
	            unsigned char  *dstBitmap, int width, int height)
{
 DWORD dstPitch        = srcPitch<<1;
 DWORD srcPitchHalf    = srcPitch>>1;
 int   finWidth        = srcPitch>>2;
 DWORD line;
 DWORD *dP;
 DWORD *bP;
 int iXA,iXB,iXC,iYA,iYB,iYC,finish;
 DWORD color4, color5, color6;
 DWORD color1, color2, color3;
 DWORD colorA0, colorA1, colorA2, colorA3,
       colorB0, colorB1, colorB2, colorB3,
       colorS1, colorS2;
 DWORD product1a, product1b,
       product2a, product2b;

 finalw=width<<1;
 finalh=height<<1;

 line = 0;

  {
   for (; height; height-=1)
	{
     bP = (DWORD *)srcPtr;
	 dP = (DWORD *)(dstBitmap + line*dstPitch);
     for (finish = width; finish; finish -= 1 )
      {
//---------------------------------------    B1 B2
//                                         4  5  6 S2
//                                         1  2  3 S1
//                                           A1 A2
       if(finish==finWidth) iXA=0;
       else                 iXA=1;
       if(finish>4) {iXB=1;iXC=2;}
       else
       if(finish>3) {iXB=1;iXC=1;}
       else         {iXB=0;iXC=0;}
       if(line==0)  {iYA=0;}
       else         {iYA=finWidth;}
       if(height>4) {iYB=finWidth;iYC=srcPitchHalf;}
       else
       if(height>3) {iYB=finWidth;iYC=finWidth;}
       else         {iYB=0;iYC=0;}

       colorB0 = *(bP- iYA - iXA);
       colorB1 = *(bP- iYA);
       colorB2 = *(bP- iYA + iXB);
       colorB3 = *(bP- iYA + iXC);

       color4 = *(bP  - iXA);
       color5 = *(bP);
       color6 = *(bP  + iXB);
       colorS2 = *(bP + iXC);

       color1 = *(bP  + iYB  - iXA);
       color2 = *(bP  + iYB);
       color3 = *(bP  + iYB  + iXB);
       colorS1= *(bP  + iYB  + iXC);

       colorA0 = *(bP + iYC - iXA);
       colorA1 = *(bP + iYC);
       colorA2 = *(bP + iYC + iXB);
       colorA3 = *(bP + iYC + iXC);

       if (color2 == color6 && color5 != color3)
        {
         product2b = product1b = color2;
        }
       else
       if (color5 == color3 && color2 != color6)
        {
         product2b = product1b = color5;
        }
       else
       if (color5 == color3 && color2 == color6)
        {
         register int r = 0;

         r += GET_RESULT ((color6&0x00ffffff), (color5&0x00ffffff), (color1&0x00ffffff),  (colorA1&0x00ffffff));
         r += GET_RESULT ((color6&0x00ffffff), (color5&0x00ffffff), (color4&0x00ffffff),  (colorB1&0x00ffffff));
         r += GET_RESULT ((color6&0x00ffffff), (color5&0x00ffffff), (colorA2&0x00ffffff), (colorS1&0x00ffffff));
         r += GET_RESULT ((color6&0x00ffffff), (color5&0x00ffffff), (colorB2&0x00ffffff), (colorS2&0x00ffffff));

         if (r > 0)
          product2b = product1b = color6;
         else
         if (r < 0)
          product2b = product1b = color5;
         else
          {
           product2b = product1b = INTERPOLATE8(color5, color6);
          }
        }
       else
        {
         if (color6 == color3 && color3 == colorA1 && color2 != colorA2 && color3 != colorA0)
             product2b = Q_INTERPOLATE8 (color3, color3, color3, color2);
         else
         if (color5 == color2 && color2 == colorA2 && colorA1 != color3 && color2 != colorA3)
             product2b = Q_INTERPOLATE8 (color2, color2, color2, color3);
         else
             product2b = INTERPOLATE8 (color2, color3);

         if (color6 == color3 && color6 == colorB1 && color5 != colorB2 && color6 != colorB0)
             product1b = Q_INTERPOLATE8 (color6, color6, color6, color5);
         else
         if (color5 == color2 && color5 == colorB2 && colorB1 != color6 && color5 != colorB3)
             product1b = Q_INTERPOLATE8 (color6, color5, color5, color5);
         else
             product1b = INTERPOLATE8 (color5, color6);
        }

       if (color5 == color3 && color2 != color6 && color4 == color5 && color5 != colorA2)
        product2a = INTERPOLATE8(color2, color5);
       else
       if (color5 == color1 && color6 == color5 && color4 != color2 && color5 != colorA0)
        product2a = INTERPOLATE8(color2, color5);
       else
        product2a = color2;

       if (color2 == color6 && color5 != color3 && color1 == color2 && color2 != colorB2)
        product1a = INTERPOLATE8(color2, color5);
       else
       if (color4 == color2 && color3 == color2 && color1 != color5 && color2 != colorB0)
        product1a = INTERPOLATE8(color2, color5);
       else
        product1a = color5;

       *dP=product1a;
       *(dP+1)=product1b;
       *(dP+(srcPitchHalf))=product2a;
       *(dP+1+(srcPitchHalf))=product2b;

       bP += 1;
       dP += 2;
      }//end of for ( finish= width etc..)

     line += 2;
     srcPtr += srcPitch;
	}; //endof: for (; height; height--)
  }
}

void BlitScreen32(unsigned char *surf, int32_t x, int32_t y)
{
 unsigned char *pD;
 unsigned int startxy;
 uint32_t lu;
 unsigned short s;
 unsigned short row, column;
 unsigned short dx = PreviousPSXDisplay.Range.x1;
 unsigned short dy = PreviousPSXDisplay.DisplayMode.y;

 int32_t lPitch = PSXDisplay.DisplayMode.x << 2;

 uint32_t *destpix;

 if (PreviousPSXDisplay.Range.y0) // centering needed?
  {
   memset(surf, 0, (PreviousPSXDisplay.Range.y0 >> 1) * lPitch);

   dy -= PreviousPSXDisplay.Range.y0;
   surf += (PreviousPSXDisplay.Range.y0 >> 1) * lPitch;

   memset(surf + dy * lPitch,
          0, ((PreviousPSXDisplay.Range.y0 + 1) >> 1) * lPitch);
  }

 if (PreviousPSXDisplay.Range.x0)
  {
   for (column = 0; column < dy; column++)
    {
     destpix = (uint32_t *)(surf + (column * lPitch));
     memset(destpix, 0, PreviousPSXDisplay.Range.x0 << 2);
    }
   surf += PreviousPSXDisplay.Range.x0 << 2;
  }

 if (PSXDisplay.RGB24)
  {
   for (column = 0; column < dy; column++)
    {
     startxy = ((1024) * (column + y)) + x;
     pD = (unsigned char *)&psxVuw[startxy];
     destpix = (uint32_t *)(surf + (column * lPitch));
     for (row = 0; row < dx; row++)
      {
       lu = *((uint32_t *)pD);
       destpix[row] = 
          0xff000000 | (RED(lu) << 16) | (GREEN(lu) << 8) | (BLUE(lu));
       pD += 3;
      }
    }
  }
 else
  {
   for (column = 0;column<dy;column++)
    {
     startxy = (1024 * (column + y)) + x;
     destpix = (uint32_t *)(surf + (column * lPitch));
     for (row = 0; row < dx; row++)
      {
       s = GETLE16(&psxVuw[startxy++]);
       destpix[row] = 
          (((s << 19) & 0xf80000) | ((s << 6) & 0xf800) | ((s >> 7) & 0xf8)) | 0xff000000;
      }
    }
  }
}


void BlitScreenNS_GX(unsigned char * surf,long x,long y, short dx, short dy)
{
 unsigned long lu;
 unsigned short row,column;
// unsigned short dx=PreviousPSXDisplay.Range.x1;
// unsigned short dy=PreviousPSXDisplay.DisplayMode.y;
 unsigned short LineOffset,SurfOffset;
 long lPitch=iResX_Max<<1;
// long lPitch=iResX<<1;
#ifdef USE_DGA2
 int DGA2fix;
 int dga2Fix;
 if (!iWindowMode)
  {
   DGA2fix = (char*)surf == Xpixels;
   dga2Fix = dgaDev->mode.imageWidth - dgaDev->mode.viewportWidth;
  } else DGA2fix = dga2Fix = 0;
#endif

 if(PreviousPSXDisplay.Range.y0)                       // centering needed?
  {
   surf+=PreviousPSXDisplay.Range.y0*lPitch;
   dy-=PreviousPSXDisplay.Range.y0;
  }

 if(PSXDisplay.RGB24)
  {
   unsigned char * pD;unsigned int startxy;

   surf+=PreviousPSXDisplay.Range.x0<<1;
#ifdef USE_DGA2
   if (DGA2fix) lPitch+= dga2Fix*2;
#endif

   for(column=0;column<dy;column++)
    {
     startxy=((1024)*(column+y))+x;

     pD=(unsigned char *)&psxVuw[startxy];

     for(row=0;row<dx;row++)
      {
       lu=*((unsigned long *)pD);
       *((unsigned short *)((surf)+(column*lPitch)+(row<<1)))=
         ((RED(lu)<<8)&0xf800)|((GREEN(lu)<<3)&0x7e0)|(BLUE(lu)>>3);
       pD+=3;
      }
    }
  }
 else
  {
   unsigned long * SRCPtr = (unsigned long *)(psxVuw +
                             (y<<10) + x);

   unsigned long * DSTPtr =
    ((unsigned long *)surf)+(PreviousPSXDisplay.Range.x0>>1);

#ifdef USE_DGA2
   dga2Fix/=2;
#endif
   dx>>=1;

   LineOffset = 512 - dx;
   SurfOffset = (lPitch>>2) - dx;

   for(column=0;column<dy;column++)
    {
     for(row=0;row<dx;row++)
      {
       lu=GETLE16D(SRCPtr++);

       *DSTPtr++=
        ((lu<<11)&0xf800f800)|((lu<<1)&0x7c007c0)|((lu>>10)&0x1f001f);
      }
     SRCPtr += LineOffset;
     DSTPtr += SurfOffset;
#ifdef USE_DGA2
     if (DGA2fix) DSTPtr+= dga2Fix;
#endif
    }
  }
}


void Vid_Flip(short width, short height,unsigned char * buffer)
{
	int h, w;
	long long int *dst = (long long int *) GXtexture;
	long long int *src = (long long int *) buffer;
	char *ra = NULL;


	if((width == 0) || (height == 0))
		return;

	ps3sxSwapBuffer(buffer,width,height);

}

void DoBufferSwap(void)
{
	static int iOldDX=0;
	static int iOldDY=0;
	long x = PSXDisplay.DisplayPosition.x;
	long y = PSXDisplay.DisplayPosition.y;
	short iDX = PreviousPSXDisplay.Range.x1;
	short iDY = PreviousPSXDisplay.DisplayMode.y;

	if(iOldDX!=iDX || iOldDY!=iDY)
	{
		memset(Xpixels,0,iResY_Max*iResX_Max*4);
		iOldDX=iDX;iOldDY=iDY;
	}

	//BlitScreenNS_GX((unsigned char *)Xpixels, x, y, iDX, iDY);
	BlitScreen32((unsigned char *)Xpixels, x, y);
	Gpu_updated = 1;
//	GPUmakeSnapshot(); 
	Vid_Flip(iDX, iDY,(unsigned char *) Xpixels);

}

unsigned long ulInitDisplay(void)
{
 
	memset(Xpixels,0,iResX_Max*iResY_Max*2);
	memset(GXtexture,0,iResX_Max*iResY_Max*2);
 
 return (unsigned long)Xpixels;
}

void CloseDisplay(void)
{

}

void CreatePic(unsigned char * pMem)
{
}

void DestroyPic(void)
{
}

void DisplayPic(void)
{
}

void ShowGpuPic(void){

}

void ShowTextGpuPic(void)
{
}
