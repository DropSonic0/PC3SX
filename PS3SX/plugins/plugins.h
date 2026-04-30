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


long GPU__init(void);
long GPU__shutdown(void);
long GPU__open(unsigned long *, char *, char *);
long GPU__close(void);
void GPU__writeStatus(unsigned long);
void GPU__writeData(unsigned long);
unsigned long GPU__readStatus(void);
unsigned long GPU__readData(void);
long GPU__dmaChain(unsigned long *,unsigned long);
void GPU__updateLace(void);

long PAD__init(long);
long PAD__shutdown(void);	
long PAD__open(void);
long PAD__close(void);
long PAD__readPort1(PadDataS*);
long PAD__readPort2(PadDataS*);

#endif
