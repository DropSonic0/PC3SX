#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "Decode_XA.h"
#include "PSEmu_Plugin_Defs.h"

long CDR__init(void);
long CDR__shutdown(void);
long CDR__open(void);
long CDR__close(void);
long CDR__getTN(unsigned char *);
long CDR__getTD(unsigned char , unsigned char *);
long CDR__readTrack(unsigned char *);
unsigned char *CDR__getBuffer(void);

long SPU_d_init(void);				
long SPU_d_shutdown(void);	
long SPU_d_close(void);			
long SPU_d_open(void);			
void SPU_d_playSample(unsigned char);		
void SPU_d_startChannels1(unsigned short);	
void SPU_d_startChannels2(unsigned short);
void SPU_d_stopChannels1(unsigned short);	
void SPU_d_stopChannels2(unsigned short);	
void SPU_d_putOne(unsigned long,unsigned short);			
unsigned short SPU_d_getOne(unsigned long);			
void SPU_d_setAddr(unsigned char, unsigned short);			
void SPU_d_setPitch(unsigned char, unsigned short);		
void SPU_d_setVolumeL(unsigned char, short );		
void SPU_d_setVolumeR(unsigned char, short );		

long SPU_d_test(void);
long SPU_d_configure(void);
void SPU_d_about(void);
void SPU_d_playADPCMchannel(xa_decode_t *xap);

void SPU_d_writeDMA(unsigned short val);
unsigned short SPU_d_readDMA(void);
unsigned short SPU_d_readRegister(unsigned long reg);
void SPU_d_writeRegister(unsigned long reg, unsigned short val);


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
long PAD__open(unsigned long *);
long PAD__close(void);
long PAD__query(void);
long PAD__readPort1(PadDataS*);
long PAD__readPort2(PadDataS*);
long PAD__keypressed(void);
unsigned char PAD__startPoll(int);
unsigned char PAD__poll(unsigned char);
long PAD__configure(void);
void PAD__about(void);
long PAD__test(void);
void PAD__setSensitive(int);
void PAD__registerVibration(void (CALLBACK *callback)(unsigned long, unsigned long));
void PAD__registerCursor(void (CALLBACK *callback)(int, int, int));

#endif
