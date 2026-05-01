#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "../pcsxcore/Decode_XA.h"
#include "../pcsxcore/PSEmu_Plugin_Defs.h"

long CDR__open(void);
long CDR__init(void);
long CDR__shutdown(void);
long CDR__open(void);
long CDR__close(void);
long CDR__getTN(unsigned char *);
long CDR__getTD(unsigned char , unsigned char *);
long CDR__readTrack(unsigned char *);
unsigned char *CDR__getBuffer(void);

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


void			pkGPUinit				();
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
void			pkGPUshowScreenPic		(unsigned char * pMem);
void			pkGPUvBlank				(int val);
void			pkGPUkeypressed			(int keycode);
long			pkGPUtest				();

long PAD__init(long);
long PAD__shutdown(void);	
long PAD__open(void);
long PAD__close(void);
long PAD__readPort1(PadDataS*);
long PAD__readPort2(PadDataS*);

#endif
