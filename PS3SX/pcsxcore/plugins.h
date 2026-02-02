/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2003  Pcsx Team
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

#ifndef __PLUGINS_H__
#define __PLUGINS_H__



typedef void* HWND;
#define CALLBACK
#include "Plugin.h"

#include "PSEmu_Plugin_Defs.h"
#include "Decode_XA.h"


int  LoadPlugins(void);
void ReleasePlugins(void);
void  OpenPlugins(void);
void ClosePlugins(void);
void ResetPlugins(void);


typedef unsigned long (CALLBACK* PSEgetLibType)(void);
typedef unsigned long (CALLBACK* PSEgetLibVersion)(void);
typedef char *(CALLBACK* PSEgetLibName)(void);

///GPU PLUGIN STUFF 
typedef long (CALLBACK* CBGPUopen)(unsigned long *, char *, char *);
typedef long (CALLBACK* CBGPUinit)(void);
typedef long (CALLBACK* CBGPUshutdown)(void);
typedef long (CALLBACK* CBGPUclose)(void);
typedef void (CALLBACK* CBGPUwriteStatus)(unsigned long);
typedef void (CALLBACK* CBGPUwriteData)(unsigned long);
typedef void (CALLBACK* CBGPUwriteDataMem)(unsigned long *, int);
typedef unsigned long (CALLBACK* CBGPUreadStatus)(void);
typedef unsigned long (CALLBACK* CBGPUreadData)(void);
typedef void (CALLBACK* CBGPUreadDataMem)(unsigned long *, int);
typedef long (CALLBACK* CBGPUdmaChain)(unsigned long *,unsigned long);
typedef void (CALLBACK* CBGPUupdateLace)(void);
typedef long (CALLBACK* CBGPUconfigure)(void);
typedef long (CALLBACK* CBGPUtest)(void);
typedef void (CALLBACK* CBGPUabout)(void);
typedef void (CALLBACK* CBGPUmakeSnapshot)(void);
typedef void (CALLBACK* CBGPUkeypressed)(int);
typedef void (CALLBACK* CBGPUdisplayText)(char *);
typedef struct {
	unsigned long ulFreezeVersion;
	unsigned long ulStatus;
	unsigned long ulControl[256];
	unsigned char psxVRam[1024*512*2];
} GPUFreeze_t;
typedef long (CALLBACK* CBGPUfreeze)(unsigned long, GPUFreeze_t *);
typedef long (CALLBACK* CBGPUgetScreenPic)(unsigned char *);
typedef long (CALLBACK* CBGPUshowScreenPic)(unsigned char *);
typedef void (CALLBACK* CBGPUclearDynarec)(void (CALLBACK *callback)(void));

//plugin stuff From Shadow
// *** walking in the valley of your darking soul i realize that i was alone
//Gpu function pointers
extern CBGPUupdateLace    GPU_updateLace;
extern CBGPUinit          GPU_init;
extern CBGPUshutdown      GPU_shutdown;
extern CBGPUconfigure     GPU_configure;
extern CBGPUtest          GPU_test;
extern CBGPUabout         GPU_about;
extern CBGPUopen          GPU_open;
extern CBGPUclose         GPU_close;
extern CBGPUreadStatus    GPU_readStatus;
extern CBGPUreadData      GPU_readData;
extern CBGPUreadDataMem   GPU_readDataMem;
extern CBGPUwriteStatus   GPU_writeStatus;
extern CBGPUwriteData     GPU_writeData;
extern CBGPUwriteDataMem  GPU_writeDataMem;
extern CBGPUdmaChain      GPU_dmaChain;
extern CBGPUkeypressed    GPU_keypressed;
extern CBGPUdisplayText   GPU_displayText;
extern CBGPUmakeSnapshot  GPU_makeSnapshot;
extern CBGPUfreeze        GPU_freeze;
extern CBGPUgetScreenPic  GPU_getScreenPic;
extern CBGPUshowScreenPic GPU_showScreenPic;
extern CBGPUclearDynarec  GPU_clearDynarec;

//cd rom plugin ;)
typedef long (CALLBACK* CDRinit)(void);
typedef long (CALLBACK* CDRshutdown)(void);
typedef long (CALLBACK* CDRopen)(void);
typedef long (CALLBACK* CDRclose)(void);
typedef long (CALLBACK* CDRgetTN)(unsigned char *);
typedef long (CALLBACK* CDRgetTD)(unsigned char , unsigned char *);
typedef long (CALLBACK* CDRreadTrack)(unsigned char *);
typedef unsigned char * (CALLBACK* CDRgetBuffer)(void);
typedef long (CALLBACK* CDRconfigure)(void);
typedef long (CALLBACK* CDRtest)(void);
typedef void (CALLBACK* CDRabout)(void);
typedef long (CALLBACK* CDRplay)(unsigned char *);
typedef long (CALLBACK* CDRstop)(void);
typedef long (CALLBACK* CDRreadCDDA)(unsigned char, unsigned char, unsigned char, unsigned char *);
struct CdrStat {
	unsigned long Type;
	unsigned long Status;
	unsigned char Time[3];
};
typedef long (CALLBACK* CDRgetStatus)(struct CdrStat *);
typedef long (CALLBACK* CDRsetfilename)(char *);
typedef char* (CALLBACK* CDRgetDriveLetter)(void);
struct SubQ {
	char res0[11];
	unsigned char ControlAndADR;
	unsigned char TrackNumber;
	unsigned char IndexNumber;
	unsigned char TrackRelativeAddress[3];
	unsigned char Filler;
	unsigned char AbsoluteAddress[3];
	char res1[72];
};
typedef unsigned char* (CALLBACK* CDRgetBufferSub)(void);

//cd rom function pointers 
extern CDRinit               CDR_init;
extern CDRshutdown           CDR_shutdown;
extern CDRopen               CDR_open;
extern CDRclose              CDR_close; 
extern CDRtest               CDR_test;
extern CDRgetTN              CDR_getTN;
extern CDRgetTD              CDR_getTD;
extern CDRreadTrack          CDR_readTrack;
extern CDRgetBuffer          CDR_getBuffer;
extern CDRplay               CDR_play;
extern CDRstop               CDR_stop;
extern CDRgetStatus          CDR_getStatus;
extern CDRsetfilename        CDR_setfilename;
extern CDRgetDriveLetter     CDR_getDriveLetter;
extern CDRgetBufferSub       CDR_getBufferSub;
extern CDRconfigure          CDR_configure;
extern CDRabout              CDR_about;
extern CDRreadCDDA           CDR_readCDDA;

// spu plugin
typedef long (CALLBACK* SPUopen)(void);
typedef long (CALLBACK* SPUinit)(void);				
typedef long (CALLBACK* SPUshutdown)(void);	
typedef long (CALLBACK* SPUclose)(void);			
typedef void (CALLBACK* SPUplaySample)(unsigned char);		
typedef void (CALLBACK* SPUstartChannels1)(unsigned short);	
typedef void (CALLBACK* SPUstartChannels2)(unsigned short);
typedef void (CALLBACK* SPUstopChannels1)(unsigned short);	
typedef void (CALLBACK* SPUstopChannels2)(unsigned short);	
typedef void (CALLBACK* SPUputOne)(unsigned long,unsigned short);			
typedef unsigned short (CALLBACK* SPUgetOne)(unsigned long);			
typedef void (CALLBACK* SPUsetAddr)(unsigned char, unsigned short);			
typedef void (CALLBACK* SPUsetPitch)(unsigned char, unsigned short);		
typedef void (CALLBACK* SPUsetVolumeL)(unsigned char, short );		
typedef void (CALLBACK* SPUsetVolumeR)(unsigned char, short );		
//psemu pro 2 functions from now..
typedef void (CALLBACK* SPUwriteRegister)(unsigned long, unsigned short);	
typedef unsigned short (CALLBACK* SPUreadRegister)(unsigned long);		
typedef void (CALLBACK* SPUwriteDMA)(unsigned short);
typedef unsigned short (CALLBACK* SPUreadDMA)(void);
typedef void (CALLBACK* SPUwriteDMAMem)(unsigned short *, int);
typedef void (CALLBACK* SPUreadDMAMem)(unsigned short *, int);
typedef void (CALLBACK* SPUplayADPCMchannel)(xa_decode_t *);
typedef void (CALLBACK* SPUplayCDDAchannel)(short *, int);
typedef void (CALLBACK* SPUregisterCallback)(void (CALLBACK *callback)(void));
typedef long (CALLBACK* SPUconfigure)(void);
typedef long (CALLBACK* SPUtest)(void);			
typedef void (CALLBACK* SPUabout)(void);

typedef struct {
	unsigned char PluginName[8];
	unsigned long PluginVersion;
	unsigned long Size;
	unsigned char SPUPorts[0x200];
	unsigned char SPURam[0x80000];
	xa_decode_t xa;
	unsigned char *SPUInfo;
} SPUFreeze_t;
typedef long (CALLBACK* SPUfreeze)(unsigned long, SPUFreeze_t *);
typedef void (CALLBACK* SPUasync)(unsigned long);

//SPU POINTERS
extern SPUconfigure        SPU_configure;
extern SPUabout            SPU_about;
extern SPUinit             SPU_init;
extern SPUshutdown         SPU_shutdown;
extern SPUtest             SPU_test;
extern SPUopen             SPU_open;
extern SPUclose            SPU_close;
extern SPUplaySample       SPU_playSample;
extern SPUstartChannels1   SPU_startChannels1;
extern SPUstartChannels2   SPU_startChannels2;
extern SPUstopChannels1    SPU_stopChannels1;
extern SPUstopChannels2    SPU_stopChannels2;
extern SPUputOne           SPU_putOne;
extern SPUgetOne           SPU_getOne;
extern SPUsetAddr          SPU_setAddr;
extern SPUsetPitch         SPU_setPitch;
extern SPUsetVolumeL       SPU_setVolumeL;
extern SPUsetVolumeR       SPU_setVolumeR;
extern SPUwriteRegister    SPU_writeRegister;
extern SPUreadRegister     SPU_readRegister;
extern SPUwriteDMA         SPU_writeDMA;
extern SPUreadDMA          SPU_readDMA;
extern SPUwriteDMAMem      SPU_writeDMAMem;
extern SPUreadDMAMem       SPU_readDMAMem;
extern SPUplayADPCMchannel SPU_playADPCMchannel;
extern SPUplayCDDAchannel  SPU_playCDDAchannel;
extern SPUfreeze           SPU_freeze;
extern SPUregisterCallback SPU_registerCallback;
extern SPUasync            SPU_async;

// PAD Functions
typedef long (CALLBACK* PADopen)(unsigned long *);
typedef long (CALLBACK* PADconfigure)(void);
typedef void (CALLBACK* PADabout)(void);
typedef long (CALLBACK* PADinit)(long);
typedef long (CALLBACK* PADshutdown)(void);	
typedef long (CALLBACK* PADtest)(void);		
typedef long (CALLBACK* PADclose)(void);
typedef long (CALLBACK* PADquery)(void);
typedef long (CALLBACK*	PADreadPort1)(PadDataS*);
typedef long (CALLBACK* PADreadPort2)(PadDataS*);
typedef long (CALLBACK* PADkeypressed)(void);
typedef unsigned char (CALLBACK* PADstartPoll)(int);
typedef unsigned char (CALLBACK* PADpoll)(unsigned char);
typedef void (CALLBACK* PADsetSensitive)(int);

//PAD POINTERS
extern PADconfigure        PAD1_configure;
extern PADabout            PAD1_about;
extern PADinit             PAD1_init;
extern PADshutdown         PAD1_shutdown;
extern PADtest             PAD1_test;
extern PADopen             PAD1_open;
extern PADclose            PAD1_close;
extern PADquery			PAD1_query;
extern PADreadPort1		PAD1_readPort1;
extern PADkeypressed		PAD1_keypressed;
extern PADstartPoll        PAD1_startPoll;
extern PADpoll             PAD1_poll;
extern PADsetSensitive     PAD1_setSensitive;

extern PADconfigure        PAD2_configure;
extern PADabout            PAD2_about;
extern PADinit             PAD2_init;
extern PADshutdown         PAD2_shutdown;
extern PADtest             PAD2_test;
extern PADopen             PAD2_open;
extern PADclose            PAD2_close;
extern PADquery            PAD2_query;
extern PADreadPort2		PAD2_readPort2;
extern PADkeypressed		PAD2_keypressed;
extern PADstartPoll        PAD2_startPoll;
extern PADpoll             PAD2_poll;
extern PADsetSensitive     PAD2_setSensitive;

// NET plugin

typedef long (CALLBACK* NETinit)(void);
typedef long (CALLBACK* NETshutdown)(void);
typedef long (CALLBACK* NETclose)(void);
typedef long (CALLBACK* NETconfigure)(void);
typedef long (CALLBACK* NETtest)(void);
typedef void (CALLBACK* NETabout)(void);
typedef void (CALLBACK* NETpause)(void);
typedef void (CALLBACK* NETresume)(void);
typedef long (CALLBACK* NETqueryPlayer)(void);
typedef long (CALLBACK* NETsendData)(void *, int, int);
typedef long (CALLBACK* NETrecvData)(void *, int, int);
typedef long (CALLBACK* NETsendPadData)(void *, int);
typedef long (CALLBACK* NETrecvPadData)(void *, int);

typedef struct {
	char EmuName[32];
	char CdromID[9];	// ie. 'SCPH12345', no \0 trailing character
	char CdromLabel[11];
	void *psxMem;
	CBGPUshowScreenPic GPU_showScreenPic;
	CBGPUdisplayText GPU_displayText;
	PADsetSensitive PAD_setSensitive;
	char GPUpath[256];	// paths must be absolute
	char SPUpath[256];
	char CDRpath[256];
	char MCD1path[256];
	char MCD2path[256];
	char BIOSpath[256];	// 'HLE' for internal bios
	char Unused[1024];
} netInfo;

typedef long (CALLBACK* NETsetInfo)(netInfo *);
typedef long (CALLBACK* NETkeypressed)(int);


// NET function pointers 
extern NETinit               NET_init;
extern NETshutdown           NET_shutdown;
//extern NETopen               NET_open;
extern NETclose              NET_close; 
extern NETtest               NET_test;
extern NETconfigure          NET_configure;
extern NETabout              NET_about;
extern NETpause              NET_pause;
extern NETresume             NET_resume;
extern NETqueryPlayer        NET_queryPlayer;
extern NETsendData           NET_sendData;
extern NETrecvData           NET_recvData;
extern NETsendPadData        NET_sendPadData;
extern NETrecvPadData        NET_recvPadData;
extern NETsetInfo            NET_setInfo;
extern NETkeypressed         NET_keypressed;

const char *GetIsoFile(void);
int LoadCDRplugin(char *CDRdll);
int LoadGPUplugin(char *GPUdll);
int LoadSPUplugin(char *SPUdll);
int LoadPAD1plugin(char *PAD1dll);
int LoadPAD2plugin(char *PAD2dll);
int LoadNETplugin(char *NETdll);

void CALLBACK clearDynarec(void);

#endif /* __PLUGINS_H__ */
