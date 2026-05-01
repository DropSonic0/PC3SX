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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Plugins.h"
#include "spu.h"
#include "PsxCommon.h"
#include "../plugins/plugins.h"

#ifdef __WIN32__
#pragma warning(disable:4244)
#endif

#define CheckErr(func) \
    err = SysLibError(); \
    if (err != NULL) { SysMessage ("Error loading %s: %s\n",func,err); return -1; }

#define LoadSym(dest, src, name, checkerr) \
    dest = (src) SysLoadSym(drv, name); if (checkerr == 1) CheckErr(name); \
    if (checkerr == 2) { err = SysLibError(); if (err != NULL) errval = 1; }

static char *err;
static int errval;

extern int ReadPsxPad(int PadNbr);

void *hGPUDriver = NULL;

void ConfigurePlugins();

void CALLBACK GPU__displayText(char *pText) {
	SysPrintf("%s\n", pText);
}

void CALLBACK GPUbusy( int ticks )
{
    //printf( "GPUbusy( %i )\n", ticks );
    //fflush( 0 );

    psxRegs.interrupt |= (1 << PSXINT_GPUBUSY);
    psxRegs.intCycle[PSXINT_GPUBUSY].cycle = ticks;
    psxRegs.intCycle[PSXINT_GPUBUSY].sCycle = psxRegs.cycle;
}

long CALLBACK GPU__configure(void) { return 0; }
long CALLBACK GPU__test(void) { return 0; }
void CALLBACK GPU__about(void) {}
void CALLBACK GPU__makeSnapshot(void) {}
void CALLBACK GPU__keypressed(int key) {}
long CALLBACK GPU__getScreenPic(unsigned char *pMem) { return -1; }
long CALLBACK GPU__showScreenPic(unsigned char *pMem) { return -1; }
void CALLBACK GPU__clearDynarec(void (CALLBACK *callback)(void)) {}
void CALLBACK GPU__vBlank(int val) {}
void CALLBACK GPU__registerCallback(void (CALLBACK *callback)(int)) {}
void CALLBACK GPU__idle(void) {}
void CALLBACK GPU__visualVibration(unsigned long iSmall, unsigned long iBig) {}
void CALLBACK GPU__cursor(int player, int x, int y) {}
void CALLBACK GPU__addVertex(short sx,short sy,s64 fx,s64 fy,s64 fz) {}

#define LoadGpuSym1(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, TRUE);

#define LoadGpuSym0(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, FALSE); \
	if (GPU_##dest == NULL) GPU_##dest = (GPU##dest) GPU__##dest;

#define LoadGpuSymN(dest, name) \
	LoadSym(GPU_##dest, GPU##dest, name, FALSE);

#if defined(__ppc__)
#define PS3LoadGpuSym(dest) \
	GPU_##dest = (GPU##dest) pkGPU##dest;

#define PS3LoadGpuSym0(dest) \
	GPU_##dest = (GPU##dest) GPU__##dest;
#endif

static int LoadGPUplugin(const char *GPUdll) {
#if defined(__ppc__)
	PS3LoadGpuSym(init);
	PS3LoadGpuSym(shutdown);
	PS3LoadGpuSym(open);
	PS3LoadGpuSym(close);
	PS3LoadGpuSym(readData);
	PS3LoadGpuSym(readDataMem);
	PS3LoadGpuSym(readStatus);
	PS3LoadGpuSym(writeData);
	PS3LoadGpuSym(writeDataMem);
	PS3LoadGpuSym(writeStatus);
	PS3LoadGpuSym(dmaChain);
	PS3LoadGpuSym(updateLace);
	PS3LoadGpuSym(keypressed);
	PS3LoadGpuSym0(displayText);
	PS3LoadGpuSym(makeSnapshot);
	PS3LoadGpuSym(freeze);
	PS3LoadGpuSym(getScreenPic);
	PS3LoadGpuSym(showScreenPic);
	PS3LoadGpuSym0(clearDynarec);
	PS3LoadGpuSym(vBlank);
	PS3LoadGpuSym0(registerCallback);
	PS3LoadGpuSym0(idle);
	PS3LoadGpuSym0(visualVibration);
	PS3LoadGpuSym(cursor);
	PS3LoadGpuSym0(addVertex);
	PS3LoadGpuSym(configure);
	PS3LoadGpuSym(test);
	PS3LoadGpuSym(about);

	hGPUDriver = (void*)GPUdll;
#else
	void *drv;

	hGPUDriver = SysLoadLibrary(GPUdll);
	if (hGPUDriver == NULL) {
		GPU_configure = NULL;
		SysMessage (_("Could not load GPU plugin %s!"), GPUdll); return -1;
	}
	drv = hGPUDriver;
	LoadGpuSym1(init, "GPUinit");
	LoadGpuSym1(shutdown, "GPUshutdown");
	LoadGpuSym1(open, "GPUopen");
	LoadGpuSym1(close, "GPUclose");
	LoadGpuSym1(readData, "GPUreadData");
	LoadGpuSym1(readDataMem, "GPUreadDataMem");
	LoadGpuSym1(readStatus, "GPUreadStatus");
	LoadGpuSym1(writeData, "GPUwriteData");
	LoadGpuSym1(writeDataMem, "GPUwriteDataMem");
	LoadGpuSym1(writeStatus, "GPUwriteStatus");
	LoadGpuSym1(dmaChain, "GPUdmaChain");
	LoadGpuSym1(updateLace, "GPUupdateLace");
	LoadGpuSym0(keypressed, "GPUkeypressed");
	LoadGpuSym0(displayText, "GPUdisplayText");
	LoadGpuSym0(makeSnapshot, "GPUmakeSnapshot");
	LoadGpuSym1(freeze, "GPUfreeze");
	LoadGpuSym0(getScreenPic, "GPUgetScreenPic");
	LoadGpuSym0(showScreenPic, "GPUshowScreenPic");
	LoadGpuSym0(clearDynarec, "GPUclearDynarec");
    LoadGpuSym0(vBlank, "GPUvBlank");
    LoadGpuSym0(registerCallback, "GPUregisterCallback");
    LoadGpuSym0(idle, "GPUidle");
    LoadGpuSym0(visualVibration, "GPUvisualVibration");
    LoadGpuSym0(cursor, "GPUcursor");
	LoadGpuSym0(addVertex, "GPUaddVertex");
	LoadGpuSym0(configure, "GPUconfigure");
	LoadGpuSym0(test, "GPUtest");
	LoadGpuSym0(about, "GPUabout");
#endif

	return 0;
}

void *hCDRDriver;

long CALLBACK CDR__play(unsigned char *sector) { return 0; }
long CALLBACK CDR__stop(void) { return 0; }

long CALLBACK CDR__getStatus(struct CdrStat *stat) {
    if (cdOpenCase) stat->Status = 0x10;
    else stat->Status = 0;
    return 0;
}

char* CALLBACK CDR__getDriveLetter(void) { return NULL; }
unsigned char* CALLBACK CDR__getBufferSub(void) { return NULL; }
long CALLBACK CDR__configure(void) { return 0; }
long CALLBACK CDR__test(void) { return 0; }
void CALLBACK CDR__about(void) {}

#if defined(__ppc__)
#define LoadCdrSym0(dest, name) CDR_##dest = (CDR##dest) CDR__##dest;
#define LoadCdrSym1(dest, name) CDR_##dest = (CDR##dest) CDR__##dest;
#else
#define LoadCdrSym1(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, 1);

#define LoadCdrSym0(dest, name) \
	LoadSym(CDR_##dest, CDR##dest, name, 0); \
	if (CDR_##dest == NULL) CDR_##dest = (CDR##dest) CDR__##dest;
#endif

int LoadCDRplugin(char *CDRdll) {
	void *drv;

	hCDRDriver = SysLoadLibrary(CDRdll);
	if (hCDRDriver == NULL) { SysMessage ("Could Not load CDR plugin %s\n",CDRdll);  return -1; }
	drv = hCDRDriver;
	LoadCdrSym1(init, "CDRinit");
	LoadCdrSym1(shutdown, "CDRshutdown");
	LoadCdrSym1(open, "CDRopen");
	LoadCdrSym1(close, "CDRclose");
	LoadCdrSym1(getTN, "CDRgetTN");
	LoadCdrSym1(getTD, "CDRgetTD");
	LoadCdrSym1(readTrack, "CDRreadTrack");
	LoadCdrSym1(getBuffer, "CDRgetBuffer");
	LoadCdrSym0(play, "CDRplay");
	LoadCdrSym0(stop, "CDRstop");
	LoadCdrSym0(getStatus, "CDRgetStatus");
	LoadCdrSym0(getDriveLetter, "CDRgetDriveLetter");
	LoadCdrSym0(getBufferSub, "CDRgetBufferSub");
	LoadCdrSym0(configure, "CDRconfigure");
	LoadCdrSym0(test, "CDRtest");
	LoadCdrSym0(about, "CDRabout");

	return 0;
}

void *hSPUDriver = NULL;

long CALLBACK SPU__configure(void) { return 0; }
void CALLBACK SPU__about(void) {}
long CALLBACK SPU__test(void) { return 0; }

#if defined(__ppc__)
#define PS3LoadSpuSym(dest) \
	SPU_##dest = (SPU##dest) pkSPU##dest;

#define PS3LoadSpuSym1(dest) \
	SPU_##dest = (SPU##dest) SPU__##dest;
#else
#define LoadSpuSym1(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, TRUE);

#define LoadSpuSym0(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, FALSE); \
	if (SPU_##dest == NULL) SPU_##dest = (SPU##dest) SPU__##dest;

#define LoadSpuSymN(dest, name) \
	LoadSym(SPU_##dest, SPU##dest, name, FALSE);
#endif

static int LoadSPUplugin(const char *SPUdll) {
#if defined(__ppc__)
	PS3LoadSpuSym(init);
	PS3LoadSpuSym(shutdown);
	PS3LoadSpuSym(open);
	PS3LoadSpuSym(close);
	PS3LoadSpuSym(configure);
	PS3LoadSpuSym(about);
	PS3LoadSpuSym(test);
	PS3LoadSpuSym(writeRegister);
	PS3LoadSpuSym(readRegister);
	PS3LoadSpuSym(writeDMA);
	PS3LoadSpuSym(readDMA);
	PS3LoadSpuSym(writeDMAMem);
	PS3LoadSpuSym(readDMAMem);
	PS3LoadSpuSym(playADPCMchannel);
	PS3LoadSpuSym(freeze);
	PS3LoadSpuSym(registerCallback);
	PS3LoadSpuSym(async);
	PS3LoadSpuSym(playCDDAchannel);

	hSPUDriver = (void*)SPUdll;
#else
	void *drv;

	hSPUDriver = SysLoadLibrary(SPUdll);
	if (hSPUDriver == NULL) {
		SPU_configure = NULL;
		SysMessage (_("Could not load SPU plugin %s!"), SPUdll); return -1;
	}
	drv = hSPUDriver;
	LoadSpuSym1(init, "SPUinit");
	LoadSpuSym1(shutdown, "SPUshutdown");
	LoadSpuSym1(open, "SPUopen");
	LoadSpuSym1(close, "SPUclose");
	LoadSpuSym0(configure, "SPUconfigure");
	LoadSpuSym0(about, "SPUabout");
	LoadSpuSym0(test, "SPUtest");
	LoadSpuSym1(writeRegister, "SPUwriteRegister");
	LoadSpuSym1(readRegister, "SPUreadRegister");
	LoadSpuSym1(writeDMA, "SPUwriteDMA");
	LoadSpuSym1(readDMA, "SPUreadDMA");
	LoadSpuSym1(writeDMAMem, "SPUwriteDMAMem");
	LoadSpuSym1(readDMAMem, "SPUreadDMAMem");
	LoadSpuSym1(playADPCMchannel, "SPUplayADPCMchannel");
	LoadSpuSym1(freeze, "SPUfreeze");
	LoadSpuSym1(registerCallback, "SPUregisterCallback");
	LoadSpuSymN(async, "SPUasync");
	LoadSpuSymN(playCDDAchannel, "SPUplayCDDAchannel");
#endif

	return 0;
}

void *hPAD1Driver;
void *hPAD2Driver;

static unsigned char buf[256];
unsigned char stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char mousepar[8] = { 0x00, 0x12, 0x5a, 0xff, 0xff, 0xff, 0xff };
unsigned char analogpar[9] = { 0x00, 0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int bufcount, bufc;

PadDataS padd1, padd2;

unsigned char _PADstartPoll(PadDataS *pad) {
	bufc = 0;

	switch (pad->controllerType)
	{
		case PSE_PAD_TYPE_MOUSE:
			mousepar[3] = pad->buttonStatus & 0xff;
			mousepar[4] = pad->buttonStatus >> 8;
			mousepar[5] = pad->moveX;
			mousepar[6] = pad->moveY;

			memcpy(buf, mousepar, 7);
			bufcount = 6;
			break;
		case PSE_PAD_TYPE_NEGCON: // npc101/npc104(slph00001/slph00069)
			analogpar[1] = 0x23;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_ANALOGPAD: // scph1150
			analogpar[1] = 0x73;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_ANALOGJOY: // scph1110
			analogpar[1] = 0x53;
			analogpar[3] = pad->buttonStatus & 0xff;
			analogpar[4] = pad->buttonStatus >> 8;
			analogpar[5] = pad->rightJoyX;
			analogpar[6] = pad->rightJoyY;
			analogpar[7] = pad->leftJoyX;
			analogpar[8] = pad->leftJoyY;

			memcpy(buf, analogpar, 9);
			bufcount = 8;
			break;
		case PSE_PAD_TYPE_STANDARD:
		default:
			stdpar[3] = pad->buttonStatus & 0xff;
			stdpar[4] = pad->buttonStatus >> 8;

			memcpy(buf, stdpar, 5);
			bufcount = 4;
			break;
	}

	return buf[bufc++];
}

unsigned char _PADpoll(unsigned char value) {
	if (bufc > bufcount) return 0;
	return buf[bufc++];
}

unsigned char CALLBACK PAD1__startPoll(int pad) {
	PadDataS padd;

	PAD1_readPort1(&padd);

	return _PADstartPoll(&padd);
}

unsigned char CALLBACK PAD1__poll(unsigned char value) {
	return _PADpoll(value);
}


long CALLBACK PAD1__configure(void) { return 0; }
void CALLBACK PAD1__about(void) {}
long CALLBACK PAD1__test(void) { return 0; }
long CALLBACK PAD1__query(void) { return 3; }
long CALLBACK PAD1__keypressed() { return 0; }

#if defined(__ppc__)
#define LoadPad1Sym0(dest, name) PAD1_##dest = (PAD##dest) PAD1__##dest;
#define LoadPad1Sym1(dest, name) PAD1_##dest = (PAD##dest) PAD__##dest;
#else
#define LoadPad1Sym1(dest, name) \
	LoadSym(PAD1_##dest, PAD##dest, name, 1);

#define LoadPad1Sym0(dest, name) \
	LoadSym(PAD1_##dest, PAD##dest, name, 0); \
	if (PAD1_##dest == NULL) PAD1_##dest = (PAD##dest) PAD1__##dest;
#endif

int LoadPAD1plugin(char *PAD1dll) {
	void *drv;

	hPAD1Driver = SysLoadLibrary(PAD1dll);
	if (hPAD1Driver == NULL) { SysMessage ("Could Not load PAD1 plugin %s\n",PAD1dll);  return -1; }
	drv = hPAD1Driver;
	LoadPad1Sym1(init, "PADinit");
	LoadPad1Sym1(shutdown, "PADshutdown");
	LoadPad1Sym1(open, "PADopen");
	LoadPad1Sym1(close, "PADclose");
	LoadPad1Sym0(query, "PADquery");
	LoadPad1Sym1(readPort1, "PADreadPort1");
	LoadPad1Sym0(configure, "PADconfigure");
	LoadPad1Sym0(test, "PADtest");
	LoadPad1Sym0(about, "PADabout");
	LoadPad1Sym0(keypressed, "PADkeypressed");
	LoadPad1Sym0(startPoll, "PADstartPoll");
	LoadPad1Sym0(poll, "PADpoll");

	return 0;
}

unsigned char CALLBACK PAD2__startPoll(int pad) {
	PadDataS padd;

	PAD2_readPort2(&padd);
	
	return _PADstartPoll(&padd);
}

unsigned char CALLBACK PAD2__poll(unsigned char value) {
	return _PADpoll(value);
}

long CALLBACK PAD2__configure(void) { return 0; }
void CALLBACK PAD2__about(void) {}
long CALLBACK PAD2__test(void) { return 0; }
long CALLBACK PAD2__query(void) { return 3; }
long CALLBACK PAD2__keypressed() { return 0; }

#if defined(__ppc__)
#define LoadPad2Sym0(dest, name) PAD2_##dest = (PAD##dest) PAD2__##dest;
#define LoadPad2Sym1(dest, name) PAD2_##dest = (PAD##dest) PAD__##dest;
#else
#define LoadPad2Sym1(dest, name) \
	LoadSym(PAD2_##dest, PAD##dest, name, 1);

#define LoadPad2Sym0(dest, name) \
	LoadSym(PAD2_##dest, PAD##dest, name, 0); \
	if (PAD2_##dest == NULL) PAD2_##dest = (PAD##dest) PAD2__##dest;
#endif

int LoadPAD2plugin(char *PAD2dll) {
	void *drv;

	hPAD2Driver = SysLoadLibrary(PAD2dll);
	if (hPAD2Driver == NULL) { SysMessage ("Could Not load PAD plugin %s\n",PAD2dll);  return -1; }
	drv = hPAD2Driver;
	LoadPad2Sym1(init, "PADinit");
	LoadPad2Sym1(shutdown, "PADshutdown");
	LoadPad2Sym1(open, "PADopen");
	LoadPad2Sym1(close, "PADclose");
	LoadPad2Sym0(query, "PADquery");
	LoadPad2Sym1(readPort2, "PADreadPort2");
	LoadPad2Sym0(configure, "PADconfigure");
	LoadPad2Sym0(test, "PADtest");
	LoadPad2Sym0(about, "PADabout");
	LoadPad2Sym0(keypressed, "PADkeypressed");
	LoadPad2Sym0(startPoll, "PADstartPoll");
	LoadPad2Sym0(poll, "PADpoll");

	return 0;
}


/*long PAD2__readBuffer() {
	return PAD2_readBuffer();
}*/

int LoadPlugins() {
	int ret;
	char Plugin[256];

	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Cdr);
	if (LoadCDRplugin(Plugin) == -1) return -1;
	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Gpu);
	if (LoadGPUplugin(Plugin) == -1) return -1;
	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Spu);
	if (LoadSPUplugin(Plugin) == -1) return -1;

	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Pad1);
	if (LoadPAD1plugin(Plugin) == -1) return -1;
	sprintf(Plugin, "%s%s", Config.PluginsDir, Config.Pad2);
	if (LoadPAD2plugin(Plugin) == -1) return -1;
	ret = CDR_init();
	if (ret != 0) { SysMessage ("CDRinit error : %d\n",ret); return -1; }
	ret = GPU_init();
	if (ret != 0) { SysMessage ("GPUinit error : %d\n",ret); return -1; }
	ret = SPU_init();
	if (ret != 0) { SysMessage ("SPUinit error : %d\n",ret); return -1; }
	ret = PAD1_init(1);
	if (ret != 0) { SysMessage ("PAD1init error : %d\n",ret); return -1; }
	ret = PAD2_init(2);
	if (ret != 0) { SysMessage ("PAD2init error : %d\n",ret); return -1; }

	return 0;
}

void ReleasePlugins() {
	if (hCDRDriver  == NULL || hGPUDriver  == NULL || hSPUDriver == NULL ||
		hPAD1Driver == NULL || hPAD2Driver == NULL) return;
	CDR_shutdown();
	GPU_shutdown();
	SPU_shutdown();
	PAD1_shutdown();
	PAD2_shutdown();
	SysCloseLibrary(hCDRDriver); hCDRDriver = NULL;
	SysCloseLibrary(hGPUDriver); hGPUDriver = NULL;
	SysCloseLibrary(hSPUDriver); hSPUDriver = NULL;
	SysCloseLibrary(hPAD1Driver); hPAD1Driver = NULL;
	SysCloseLibrary(hPAD2Driver); hPAD2Driver = NULL;
}
