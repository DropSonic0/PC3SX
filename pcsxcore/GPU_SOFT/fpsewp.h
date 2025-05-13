/******************************************************************************
 * FPSE gpu wrapper include
 * here are just some protos...
 * lu
 ******************************************************************************/

#ifndef _WINDOWS

long CALLBACK GPUopen(unsigned long *, char *, char *);

#else

long CALLBACK GPUopen(HWND hwndGPU);

#endif

long CALLBACK GPUinit(void);
long CALLBACK GPUshutdown(void);
long CALLBACK GPUclose(void);
void CALLBACK GPUwriteStatus(unsigned long);
void CALLBACK GPUwriteData(unsigned long);
void CALLBACK GPUwriteDataMem(unsigned long *, int);
unsigned long CALLBACK GPUreadStatus(void);
unsigned long CALLBACK GPUreadData(void);
void CALLBACK GPUreadDataMem(unsigned long *, int);
long CALLBACK GPUdmaChain(unsigned long *,unsigned long);
void CALLBACK GPUupdateLace(void);
long CALLBACK GPUconfigure(void);
long CALLBACK GPUtest(void);
void CALLBACK GPUabout(void);
void CALLBACK GPUmakeSnapshot(void);




