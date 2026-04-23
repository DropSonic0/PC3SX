/******************************************************************************
 * FPSE gpu Wrapping functions for P.e.op.s. SDL plugins
 * thit software is under the GPL licence
 * FPSE Team & P.e.Op.s. Team (lu_zero)
 * Memento: that will work for sure on FPSE0.09 and FPSE0.08
 * for Linux,BeOS and QNX6
 *****************************************************************************/

#include "stdafx.h"

#ifndef _WINDOWS
#include "type.h"	    //some typedefs
#include "sdk.h"     	//FPSE0.09 SDK
#else
#include "fpse\type.h"	//some typedefs
#include "fpse\sdk.h" 	//FPSE0.09 SDK
#endif

#ifndef _WINDOWS
#ifdef __BEOS__
#include "gnudef.h"
#else //linux
#include "linuxdef.h"
#endif
#else //win32
#include "fpse\win32def.h"
//something more to be added

#endif
#include "fpsewp.h" 	//psemu protos
#include "externals.h"  
#define VERSION		0x0105  // Version must be in BCD format
#ifdef _SDL //to be placed in the include
#include <SDL/SDL.h>
extern SDL_Surface *display;
extern Uint32 sdl_mask; 
extern int depth;
#endif

#ifdef _WINDOWS
static FPSEWin32About info = 
{
    FPSE_GPU,
    VERSION & 0xFF,
    VERSION >> 8,
    FPSE_OK, 
    "Pete Bernert and the P.E.Op.S. team",
    "GPU P.E.Op.S",
    "That GPU plugin is the result of some hacks on the Peops X11 plugin.\x00d\x00a"
    "If you have some question or you've found a bug send an email to:\x00d\x00a"
    "<lu_zero@brsk.virtualave.net>\x00d\x00a"
    "\x00d\x00a"
    "P.E.Op.S. team:\x00d\x00a"
    "- Pete Bernert\x00d\x00a"
    "- Lewpy\x00d\x00a"
    "- lu_zero\x00d\x00a"
    "- linuzappz\x00d\x00a"
    "- syo\x00d\x00a"
    "- Darko Matesic\x00d\x00a"
    "P.E.Op.S. page:https://sourceforge.net/projects/peops"    
};

#else
static FPSElinuxAbout info = 
{   
    FPSE_GPU,
    VERSION & 0xFF,
    VERSION >> 8,
    FPSE_OK, 
    "Pete Bernert and the P.E.Op.S. team",
    "GPU P.E.Op.S SDL",
    "That GPU plugin is the result \n"
    "of some hacks on the Peops X11 plugin.\n"
    "If you have some question or \n"
    "you've found a bug send an email to:\n"
    "<lu_zero@brsk.virtualave.net>\n"
    "\n"
    "P.E.Op.S. team:\n"
    "- Pete Bernert\n"
    "- Lewpy\n"
    "- lu_zero\n"
    "- Syo\n"
    "- linuzappz\n"
    "- Darko Matesic\n"
    "P.E.Op.S. page:\n"
    "http://sourceforge.net/projects/peops"    
};
#endif

void (*Flush)();
void GPUkeypressed(int);
static UINT8 *ram;

#ifndef _WINDOWS
#ifdef __BEOS__
static FPSEbeos *g;
#else
static FPSElinux *g;
#endif
#else
static FPSEWin32 *g;
#endif

void GPU_Update(void)
{
#ifndef _WINDOWS
#ifdef _SDL
Uint8 *keystate;
SDL_PumpEvents();
GPUkeypressed(0);
keystate=SDL_GetKeyState(NULL);

    if(keystate[SDLK_F10]){
	iWindowMode=!iWindowMode;
	SDL_WM_ToggleFullScreen(display);
    }
#endif
#endif
    GPUupdateLace();
}

UINT32 GP1_Read(void)
{
    return GPUreadStatus();
}

UINT32 GP0_Read(void)
{
    return GPUreadData();
}

void GP0_Write(UINT32 data)
{
    GPUwriteData(data);
}
void GP1_Write(UINT32 code)
{
    GPUwriteStatus(code);
}

#ifndef _WINDOWS
void WriteConfig(void)
{
char buf[12];
    sprintf(buf,"%i",iResX);
    g->WriteCfg("GpuPeopsSDL","ResX", buf);
    sprintf(buf,"%i",iResY);
    g->WriteCfg("GpuPeopsSDL","ResY", buf);
    sprintf(buf,"%i",iColDepth);
    g->WriteCfg("GpuPeopsSDL","ColDepth",buf);
    sprintf(buf,"%i",iWindowMode);
    g->WriteCfg("GpuPeopsSDL","WindowMode",buf);
    sprintf(buf,"%i",iUseScanLines);
    g->WriteCfg("GpuPeopsSDL","iUseScanLines",buf);
    sprintf(buf,"%i",iShowFPS);
    g->WriteCfg("GpuPeopsSDL","ShowFPS", buf);
    sprintf(buf,"%i",UseFrameLimit);
    g->WriteCfg("GpuPeopsSDL","UseFrameLimit",buf);
    sprintf(buf,"%i",UseFrameSkip);
    g->WriteCfg("GpuPeopsSDL","UseFrameSkip",buf);
    sprintf(buf,"%i",iFrameLimit);
    g->WriteCfg("GpuPeopsSDL","FrameLimit", buf);
    sprintf(buf,"%.1f",fFrameRate);
    g->WriteCfg("GpuPeopsSDL","FrameRate", buf);
    sprintf(buf,"%li",dwCfgFixes);
    g->WriteCfg("GpuPeopsSDL","dwCfgFixes",buf);
    sprintf(buf,"%i",iUseFixes);
    g->WriteCfg("GpuPeopsSDL","iUseFixes",buf);
    sprintf(buf,"%i",iUseNoStretchBlt);
    g->WriteCfg("GpuPeopsSDL","iUseNoStretchBlt", buf);
}

int ReadConfig(void){
    
    char myvalue[256];
    
    if (g->ReadCfg("GpuPeopsSDL","ResX", myvalue) == FPSE_OK && strlen(myvalue)) {
		iResX=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","ResY", myvalue) == FPSE_OK && strlen(myvalue)) {
		iResY=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","ColDepth", myvalue) == FPSE_OK && strlen(myvalue)) {
		iColDepth=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","WindowMode", myvalue) == FPSE_OK && strlen(myvalue)) {
		iWindowMode=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","iUseScanLines", myvalue) == FPSE_OK && strlen(myvalue)) {
		iUseScanLines=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","ShowFPS", myvalue) == FPSE_OK && strlen(myvalue)) {
		iShowFPS=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","UseFrameLimit", myvalue) == FPSE_OK && strlen(myvalue)) {
		UseFrameLimit=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","UseFrameSkip", myvalue) == FPSE_OK && strlen(myvalue)) {
		UseFrameSkip=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","FrameLimit", myvalue) == FPSE_OK && strlen(myvalue)) {
		iFrameLimit=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","FrameRate", myvalue) == FPSE_OK && strlen(myvalue)) {
		fFrameRate=(float)atof(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","dwCfgFixes", myvalue) == FPSE_OK && strlen(myvalue)) {
		dwCfgFixes=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","iUseFixes", myvalue) == FPSE_OK && strlen(myvalue)) {
		iUseFixes=atoi(myvalue);}
	    else return FPSE_ERR;
    if (g->ReadCfg("GpuPeopsSDL","iUseNoStretchBlt", myvalue) == FPSE_OK && strlen(myvalue)) {
		iUseNoStretchBlt=atoi(myvalue);}
	    else return FPSE_ERR;
    return FPSE_OK;
}
#endif

int GPU_Open(UINT32 *par)
{
#ifndef _WINDOWS
#ifdef __BEOS__
    g = (FPSEbeos *)par;
#else
    g = (FPSElinux *)par;
#endif
#else
    g = (FPSEWin32 *)par;
#endif
    
#ifndef _WINDOWS    
    printf("(.) Gpu : GpuPeopsSDL %d.%d \n",VERSION >>8,VERSION & 0xFF);
    if (ReadConfig()!=FPSE_OK) {
     printf("->(!) Gpu not configured, using defaults.\n");
    // iResX=320;iResY=240;
//	iResX=362;iResY=272;  // 362x272
iResX=640;iResY=480;  // 
     iColDepth=16;
     iWindowMode=1;
     iUseScanLines=0;
     UseFrameLimit=0;
     UseFrameSkip=1; //smhzc
     iFrameLimit=2;
     fFrameRate=200.0f;
     dwCfgFixes=0;
     iUseFixes=0;
     iUseNoStretchBlt=1;
     iShowFPS=0;
     WriteConfig();
    }
    printf("->(.) Gpu Configuration loaded\n");
#else
    iFPSEInterface=1;
#endif

    //isconfigured=1;
    GPUinit();
#ifndef _WINDOWS
    GPUopen(NULL,NULL,NULL);
#else
    GPUopen(g->HWnd);
#endif
    ram = g->SystemRam;
    Flush = g->FlushRec;

#ifdef _WINDOWS
//    g->HInstance;
#endif

    return FPSE_OK;
}

void GPU_Close(void)
{
    GPUclose();
    GPUshutdown();

}

void GPU_ScreenShot(char *path)
{
    GPUmakeSnapshot();
}	    

void GPU_DmaExec(UINT32 adr,UINT32 bcr,UINT32 chcr)
{
    int size;
    UINT32 *ram32 = (UINT32 *)(ram+(adr & 0x1FFFFF));

    size = (bcr>>16)*(bcr&0xffff);

    switch(chcr){
    case 0x01000200:
        Flush(adr,adr+size*4);
	GPUreadDataMem(ram32, size);
        break;
    case 0x01000201:
	GPUwriteDataMem(ram32, size);
        break;
    case 0x01000401:
        GPUdmaChain((unsigned long*)ram,adr&0x1fffff);
        break;
    case 0x00000200:
    case 0x00000201:
    case 0x00000401: // disable dma
        break;
#ifndef _WINDOWS
    default: printf("gpudma unknown %08x\n",(int)chcr);
#endif
    }
}



int GPU_Configure(UINT32 *par)
{
#ifndef _WINDOWS
#ifdef __BEOS__
g = (FPSEbeos *)par;
#endif
g = (FPSElinux *)par;
#else
g = (FPSEWin32 *)par;
#endif

#ifdef _WINDOWS
GPUconfigure();
#else
//     iResX=320;iResY=240;  // 362x272
//     iResX=362;iResY=272;  // 362x272
iResX=640;iResY=480;  // 
     iColDepth=16;
     iWindowMode=1;
     iUseScanLines=0;
     UseFrameLimit=0;
     UseFrameSkip=1; //smhzc 0 
     iFrameLimit=2;
     fFrameRate=200.0f;
     dwCfgFixes=0;
     iUseFixes=0;
     iUseNoStretchBlt=0;
     iShowFPS=0;
if (ReadConfig()!=FPSE_OK) {
     printf("->(!) Gpu not configured, using defaults.\n");
}
GPUconfigure();
// WriteConfig(); really needed?
#endif
    return FPSE_OK;
}


void GPU_About(UINT32 *par)
{
#ifndef _WINDOWS
#ifdef __BEOS__
    memcpy( par, &info, sizeof(FPSEbeosAbout) );
#endif
    memcpy( par, &info, sizeof(FPSElinuxAbout) );
#else
    memcpy( par, &info, sizeof(FPSEWin32About) );
#endif
}

void GPU_LoadState(GPU_State *state)
{}

void GPU_SaveState(GPU_State *state)
{}
