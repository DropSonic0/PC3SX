#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "../pcsxcore/Decode_XA.h"
#include "../pcsxcore/PSEmu_Plugin_Defs.h"

long            pkCDRinit               (void);
long            pkCDRshutdown           (void);
long            pkCDRopen               (void);
long            pkCDRclose              (void);
long            pkCDRgetTN              (unsigned char * aBuffer);
long            pkCDRgetTD              (unsigned char aTrack, unsigned char* aBuffer);
long            pkCDRreadTrack          (unsigned char* aPosition);
unsigned char*  pkCDRgetBuffer          (void);
unsigned char*  pkCDRgetBufferSub       (void);
long            pkCDRconfigure          (void);
long            pkCDRtest               (void);
void            pkCDRabout              (void);
long            pkCDRplay               (unsigned char* aTime);
long            pkCDRstop               (void);
long            pkCDRsetfilename        (char* aFileName);
long            pkCDRgetStatus          (struct CdrStat* aStatus);
long            pkCDRreadCDDA           (unsigned char aMinutes, unsigned char aSeconds, unsigned char aFrames, unsigned char* aBuffer);

long			pkSPUopen				(void);
long			pkSPUinit				(void);
long			pkSPUshutdown			(void);
long			pkSPUclose				(void);
void			pkSPUplaySample			(unsigned char u);
void			pkSPUwriteRegister		(unsigned long u, unsigned short y);
unsigned short	pkSPUreadRegister		(unsigned long u);
void			pkSPUwriteDMA			(unsigned short u);
unsigned short	pkSPUreadDMA			(void);
void			pkSPUwriteDMAMem		(unsigned short *i, int l);
void			pkSPUreadDMAMem			(unsigned short *i, int l);
void			pkSPUplayADPCMchannel	(xa_decode_t *n);
void			pkSPUregisterCallback	(void (*callback)(void));
long			pkSPUconfigure			(void);
long			pkSPUtest				(void);
void			pkSPUabout				(void);
long			pkSPUfreeze				(uint32_t i, struct SPUFreeze_t *j);
void			pkSPUasync				(uint32_t o);
void			pkSPUplayCDDAchannel	(short * m, int i);


long			pkGPUinit				();
long			pkGPUopen				(unsigned long * disp, char * CapText, char * CfgFile);
long			pkGPUclose				();
long			pkGPUshutdown			();
void			pkGPUupdateLace			(void); // VSYNC
uint32_t		pkGPUreadStatus			(void); // READ STATUS
void			pkGPUwriteStatus		(uint32_t gdata);
void			pkGPUreadDataMem		(uint32_t * pMem, int iSize);
uint32_t		pkGPUreadData			(void);
void			pkGPUwriteDataMem		(uint32_t * pMem, int iSize);
void			pkGPUwriteData			(uint32_t gdata);
long			pkGPUconfigure			(void);
long			pkGPUdmaChain			(uint32_t * baseAddrL, uint32_t addr);
void			pkGPUabout				(void); // ABOUT
long			pkGPUfreeze				(uint32_t ulGetFreezeData, struct GPUFreeze_t * pF);
void			pkGPUgetScreenPic		(unsigned char * pMem);
void			pkGPUshowScreenPic		(unsigned char * pMem);
void			pkGPUmakeSnapshot		(void);
void			pkGPUvBlank				(int val);
void			pkGPUkeypressed			(int keycode);
void			pkGPUcursor				(int iPlayer, int x, int y);
long			pkGPUtest				();

long PAD__init(long);
long PAD__shutdown(void);	
long PAD__open(void);
long PAD__close(void);
long PAD__readPort1(PadDataS*);
long PAD__readPort2(PadDataS*);

#endif
