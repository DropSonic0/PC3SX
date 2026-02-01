#ifndef __GPU_H__
#define __GPU_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "psxcommon.h"

int gpuReadStatus(void);

void psxDma2(u32 madr, u32 bcr, u32 chcr);
void gpuInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif
