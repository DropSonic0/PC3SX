/*
 * PSEmu linux Null Sound routines.
 *
 * By: linuzappz <linuzappz@hotmail.com>
 *
 * Modified for Dreamcast
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "plugins.h"

//typedef char HWND;

#include "PSEmu_Plugin_Defs.h"
#include "decode_xa.h"

static char *LibName = "LinuzAppz SpuNull Driver";
/*
const unsigned char version = 1; // PSEmu 1.x library
const unsigned char revision = 1;
const unsigned char build = 0;
*/
static int spu_sbaddr;
static short spureg[(0x1e00-0x1c00)/2];
static short *spumem;

/*
char *PSEgetLibName(void) {
    return LibName;
}

unsigned long PSEgetLibType(void) {
    return PSE_LT_SPU;
}

unsigned long PSEgetLibVersion(void) {
    return version<<16|revision<<8|build;
}
*/

long SPU_d_init(void) {
    spumem = (short *)malloc(512*1024);
    if (spumem == NULL) return -1;

    return 0;
}

long SPU_d_shutdown(void) {
    if (spumem != NULL) {
	free(spumem);
	spumem = NULL;
    }

    return 0;
}

long SPU_d_open(void) {
	SysPrintf("start SPU_open()\n");

	SysPrintf("end SPU_open()\n");
    return 0;
}

long SPU_d_close(void) {
    return 0;
}

// New Interface

void SPU_d_writeRegister(unsigned long reg, unsigned short val) {
    spureg[(reg-0x1f801c00)/2] = val;
    switch(reg) {
	case 0x1f801da6: // spu sbaddr
    	    spu_sbaddr = val * 8;
    	    break;
	case 0x1f801da8: // spu data
	    spumem[spu_sbaddr/2] = (short)val;
	    spu_sbaddr+=2;
	    if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
    	    break;
    }
}

unsigned short SPU_d_readRegister(unsigned long reg) {
    switch (reg){
	case 0x1f801da6: // spu sbaddr
    	    return spu_sbaddr / 8;
	case 0x1f801da8: // spu data
	    {
	    int ret = spumem[spu_sbaddr/2];
	    spu_sbaddr+=2;
	    if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
	    return ret;
	    }
	default:
	    return spureg[(reg-0x1f801c00)/2];
    }
    return 0;
}

unsigned short SPU_d_readDMA(void) {
    int ret = spumem[spu_sbaddr/2];
    spu_sbaddr+=2;
    if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
    return ret;
}

void SPU_d_writeDMA(unsigned short val) {
    spumem[spu_sbaddr/2] = (short)val;
    spu_sbaddr+=2;
    if (spu_sbaddr > 0x7ffff) spu_sbaddr = 0;
}

void SPU_d_playADPCMchannel(xa_decode_t *xap) {
}
// Old Interface

unsigned short SPU_d_getOne(unsigned long val) {
    if (val > 0x7ffff) return 0;
    return spumem[val/2];
}

void SPU_d_putOne(unsigned long val, unsigned short data) {
    if (val > 0x7ffff) return;
    spumem[val/2] = data;
}

void SPU_d_setAddr(unsigned char ch, unsigned short waddr) {
}

void SPU_d_setPitch(unsigned char ch, unsigned short pitch) {
}

void SPU_d_setVolumeL(unsigned char ch, short vol) {
}

void SPU_d_setVolumeR(unsigned char ch, short vol) {
}

void SPU_d_startChannels1(unsigned short channels) {
}

void SPU_d_startChannels2(unsigned short channels) {
}

void SPU_d_stopChannels1(unsigned short channels) {
}

void SPU_d_stopChannels2(unsigned short channels) {
}


long SPU_d_test(void) {
    return 0;
}

long SPU_d_configure(void) {
    return 0;
}

void SPU_d_about(void) {
}

void SPU_d_async(void) {
}