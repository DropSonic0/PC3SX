#include <stdint.h>
#include <sys/types.h>
#include "plugins.h"

long GPUopen(unsigned long *a, char *b, char *c) { return 0; }
long GPUinit(void) { return 0; }
long GPUshutdown(void) { return 0; }
long GPUclose(void) { return 0; }
void GPUwriteStatus(unsigned long a){}
void GPUwriteData(unsigned long a){}
void GPUwriteDataMem(unsigned long * pMem, int iSize){}
unsigned long GPUreadStatus(void) { return 0; }
unsigned long GPUreadData(void) { return 0; }
void GPUreadDataMem(unsigned long * pMem, int iSize) {  }
long GPUdmaChain(unsigned long *a ,unsigned long b) { return 0; }
void GPUupdateLace(void) { }
