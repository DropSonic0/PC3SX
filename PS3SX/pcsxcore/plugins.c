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

/*
* Plugin library callback/access functions.
*/

#include "plugins.h"

static char IsoFile[MAXPATHLEN] = "";
static s64 cdOpenCaseTime = 0;

GPUupdateLace         GPU_updateLace;
GPUinit               GPU_init;
GPUshutdown           GPU_shutdown;
GPUconfigure          GPU_configure;
GPUtest               GPU_test;
GPUabout              GPU_about;
GPUopen               GPU_open;
GPUclose              GPU_close;
GPUreadStatus         GPU_readStatus;
GPUreadData           GPU_readData;
GPUreadDataMem        GPU_readDataMem;
GPUwriteStatus        GPU_writeStatus;
GPUwriteData          GPU_writeData;
GPUwriteDataMem       GPU_writeDataMem;
GPUdmaChain           GPU_dmaChain;
GPUkeypressed         GPU_keypressed;
GPUdisplayText        GPU_displayText;
GPUmakeSnapshot       GPU_makeSnapshot;
GPUfreeze             GPU_freeze;
GPUgetScreenPic       GPU_getScreenPic;
GPUshowScreenPic      GPU_showScreenPic;
GPUclearDynarec       GPU_clearDynarec;
GPUvBlank             GPU_vBlank;
GPUregisterCallback   GPU_registerCallback;
GPUidle               GPU_idle;
GPUvisualVibration    GPU_visualVibration;
GPUcursor             GPU_cursor;
GPUaddVertex          GPU_addVertex;

CDRinit               CDR_init;
CDRshutdown           CDR_shutdown;
CDRopen               CDR_open;
CDRclose              CDR_close;
CDRtest               CDR_test;
CDRgetTN              CDR_getTN;
CDRgetTD              CDR_getTD;
CDRreadTrack          CDR_readTrack;
CDRgetBuffer          CDR_getBuffer;
CDRplay               CDR_play;
CDRstop               CDR_stop;
CDRgetStatus          CDR_getStatus;
CDRgetDriveLetter     CDR_getDriveLetter;
CDRgetBufferSub       CDR_getBufferSub;
CDRconfigure          CDR_configure;
CDRabout              CDR_about;
CDRsetfilename        CDR_setfilename;
CDRreadCDDA           CDR_readCDDA;
CDRgetTE              CDR_getTE;

SPUconfigure          SPU_configure;
SPUabout              SPU_about;
SPUinit               SPU_init;
SPUshutdown           SPU_shutdown;
SPUtest               SPU_test;
SPUopen               SPU_open;
SPUclose              SPU_close;
SPUplaySample         SPU_playSample;
SPUwriteRegister      SPU_writeRegister;
SPUreadRegister       SPU_readRegister;
SPUwriteDMA           SPU_writeDMA;
SPUreadDMA            SPU_readDMA;
SPUwriteDMAMem        SPU_writeDMAMem;
SPUreadDMAMem         SPU_readDMAMem;
SPUplayADPCMchannel   SPU_playADPCMchannel;
SPUfreeze             SPU_freeze;
SPUregisterCallback   SPU_registerCallback;
SPUasync              SPU_async;
SPUplayCDDAchannel    SPU_playCDDAchannel;

PADconfigure          PAD1_configure;
PADabout              PAD1_about;
PADinit               PAD1_init;
PADshutdown           PAD1_shutdown;
PADtest               PAD1_test;
PADopen               PAD1_open;
PADclose              PAD1_close;
PADquery              PAD1_query;
PADreadPort1          PAD1_readPort1;
PADkeypressed         PAD1_keypressed;
PADstartPoll          PAD1_startPoll;
PADpoll               PAD1_poll;
PADsetSensitive       PAD1_setSensitive;
PADregisterVibration  PAD1_registerVibration;
PADregisterCursor     PAD1_registerCursor;

PADconfigure          PAD2_configure;
PADabout              PAD2_about;
PADinit               PAD2_init;
PADshutdown           PAD2_shutdown;
PADtest               PAD2_test;
PADopen               PAD2_open;
PADclose              PAD2_close;
PADquery              PAD2_query;
PADreadPort2          PAD2_readPort2;
PADkeypressed         PAD2_keypressed;
PADstartPoll          PAD2_startPoll;
PADpoll               PAD2_poll;
PADsetSensitive       PAD2_setSensitive;
PADregisterVibration  PAD2_registerVibration;
PADregisterCursor     PAD2_registerCursor;

NETinit               NET_init;
NETshutdown           NET_shutdown;
NETopen               NET_open;
NETclose              NET_close;
NETtest               NET_test;
NETconfigure          NET_configure;
NETabout              NET_about;
NETpause              NET_pause;
NETresume             NET_resume;
NETqueryPlayer        NET_queryPlayer;
NETsendData           NET_sendData;
NETrecvData           NET_recvData;
NETsendPadData        NET_sendPadData;
NETrecvPadData        NET_recvPadData;
NETsetInfo            NET_setInfo;
NETkeypressed         NET_keypressed;

#ifdef ENABLE_SIO1API
SIO1init              SIO1_init;
SIO1shutdown          SIO1_shutdown;
// ...
#endif

// Prototypes for static plugins
extern long CALLBACK SOFTGPUinit(void);
extern long CALLBACK SOFTGPUopen(unsigned long *, char *, char *);
extern long CALLBACK SOFTGPUshutdown(void);
extern long CALLBACK SOFTGPUclose(void);
extern void CALLBACK SOFTGPUwriteStatus(uint32_t);
extern void CALLBACK SOFTGPUwriteData(uint32_t);
extern void CALLBACK SOFTGPUwriteDataMem(uint32_t *, int);
extern uint32_t CALLBACK SOFTGPUreadStatus(void);
extern uint32_t CALLBACK SOFTGPUreadData(void);
extern void CALLBACK SOFTGPUreadDataMem(uint32_t *, int);
extern long CALLBACK SOFTGPUdmaChain(uint32_t *,uint32_t);
extern void CALLBACK SOFTGPUupdateLace(void);
extern long CALLBACK SOFTGPUconfigure(void);
extern long CALLBACK SOFTGPUtest(void);
extern void CALLBACK SOFTGPUabout(void);
extern void CALLBACK SOFTGPUmakeSnapshot(void);
extern long CALLBACK SOFTGPUfreeze(uint32_t, GPUFreeze_t *);

extern long CDR__open(void);
extern long CDR__init(void);
extern long CDR__shutdown(void);
extern long CDR__close(void);
extern long CDR__getTN(unsigned char *);
extern long CDR__getTD(unsigned char, unsigned char *);
extern long CDR__readTrack(unsigned char *);
extern unsigned char *CDR__getBuffer(void);

extern long MDFNSPUinit(void);
extern long MDFNSPUshutdown(void);
extern long MDFNSPUopen(void);
extern long MDFNSPUclose(void);
extern void MDFNSPUwriteRegister(unsigned long, unsigned short);
extern unsigned short MDFNSPUreadRegister(unsigned long);
extern void MDFNSPUwriteDMA(unsigned short);
extern unsigned short MDFNSPUreadDMA(void);
extern void MDFNSPUwriteDMAMem(unsigned short *, int);
extern void MDFNSPUreadDMAMem(unsigned short *, int);
extern void MDFNSPUplayADPCMchannel(xa_decode_t *);
extern void MDFNSPUplayCDDAchannel(short *, int);
extern void MDFNSPUasync(uint32_t);
extern void MDFNSPUregisterCallback(void (CALLBACK *callback)(void));
extern long MDFNSPUfreeze(uint32_t, SPUFreeze_t *);

extern long PAD__readPort1(PadDataS*);
extern long PAD__readPort2(PadDataS*);

void CALLBACK GPU__displayText(char *pText) {
	SysPrintf("%s\n", pText);
}

void CALLBACK GPUbusy( int ticks )
{
    psxRegs.interrupt |= (1 << PSXINT_GPUBUSY);
    psxRegs.intCycle[PSXINT_GPUBUSY].cycle = ticks;
    psxRegs.intCycle[PSXINT_GPUBUSY].sCycle = psxRegs.cycle;
}

void CALLBACK clearDynarec(void) {
	psxCpu->Reset();
}

int LoadPlugins(void) {
	// Static loading for PS3
	GPU_init = (GPUinit) SOFTGPUinit;
	GPU_open = (GPUopen) SOFTGPUopen;
	GPU_shutdown = (GPUshutdown) SOFTGPUshutdown;
	GPU_close = (GPUclose) SOFTGPUclose;
	GPU_readData = (GPUreadData) SOFTGPUreadData;
	GPU_readDataMem = (GPUreadDataMem) SOFTGPUreadDataMem;
	GPU_readStatus = (GPUreadStatus) SOFTGPUreadStatus;
	GPU_writeData = (GPUwriteData) SOFTGPUwriteData;
	GPU_writeDataMem = (GPUwriteDataMem) SOFTGPUwriteDataMem;
	GPU_writeStatus = (GPUwriteStatus) SOFTGPUwriteStatus;
	GPU_dmaChain = (GPUdmaChain) SOFTGPUdmaChain;
	GPU_updateLace = (GPUupdateLace) SOFTGPUupdateLace;
	GPU_displayText = (GPUdisplayText) GPU__displayText;
	GPU_configure = (GPUconfigure) SOFTGPUconfigure;
	GPU_test = (GPUtest) SOFTGPUtest;
	GPU_about = (GPUabout) SOFTGPUabout;
	GPU_makeSnapshot = (GPUmakeSnapshot) SOFTGPUmakeSnapshot;
    GPU_freeze = (GPUfreeze) SOFTGPUfreeze;

	CDR_init = (CDRinit) CDR__init;
	CDR_shutdown = (CDRshutdown) CDR__shutdown;
	CDR_open = (CDRopen) CDR__open;
	CDR_close = (CDRclose) CDR__close;
	CDR_getTN = (CDRgetTN) CDR__getTN;
	CDR_getTD = (CDRgetTD) CDR__getTD;
	CDR_readTrack = (CDRreadTrack) CDR__readTrack;
	CDR_getBuffer = (CDRgetBuffer) CDR__getBuffer;

	SPU_init = (SPUinit) MDFNSPUinit;
	SPU_shutdown = (SPUshutdown) MDFNSPUshutdown;
	SPU_open = (SPUopen) MDFNSPUopen;
	SPU_close = (SPUclose) MDFNSPUclose;
	SPU_writeRegister = (SPUwriteRegister) MDFNSPUwriteRegister;
	SPU_readRegister = (SPUreadRegister) MDFNSPUreadRegister;
	SPU_writeDMA = (SPUwriteDMA) MDFNSPUwriteDMA;
	SPU_readDMA = (SPUreadDMA) MDFNSPUreadDMA;
	SPU_writeDMAMem = (SPUwriteDMAMem) MDFNSPUwriteDMAMem;
	SPU_readDMAMem = (SPUreadDMAMem) MDFNSPUreadDMAMem;
	SPU_playADPCMchannel = (SPUplayADPCMchannel) MDFNSPUplayADPCMchannel;
	SPU_playCDDAchannel = (SPUplayCDDAchannel) MDFNSPUplayCDDAchannel;
	SPU_registerCallback = (SPUregisterCallback) MDFNSPUregisterCallback;
    SPU_async = (SPUasync) MDFNSPUasync;
    SPU_freeze = (SPUfreeze) MDFNSPUfreeze;

	PAD1_readPort1 = (PADreadPort1) PAD__readPort1;
	PAD2_readPort2 = (PADreadPort2) PAD__readPort2;

	CDR_init();
	GPU_init();
	SPU_init();
	
	// PAD initialization if needed
	
	return 0;
}

void ReleasePlugins(void) {
	CDR_shutdown();
	GPU_shutdown();
	SPU_shutdown();
}

void SetIsoFile(const char *filename) {
	if (filename == NULL) {
		IsoFile[0] = '\0';
		return;
	}
	strncpy(IsoFile, filename, MAXPATHLEN);
}

const char *GetIsoFile(void) {
	return IsoFile;
}

boolean UsingIso(void) {
	return (IsoFile[0] != '\0');
}

void SetCdOpenCaseTime(s64 time_val) {
	cdOpenCaseTime = time_val;
}
