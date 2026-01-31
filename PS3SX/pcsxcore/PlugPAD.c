#include <stdio.h>
#include <stdlib.h>
#include "PsxCommon.h"
#include "plugins.h"

long  PadFlags = 0;

long PAD__init(long flags) {
	PadFlags |= flags;
	return 0;
}

long PAD__shutdown(void) {
	return 0;
}

long PAD__open(unsigned long *disp)
{
	(void)disp;
	SysPrintf("start PAD1_open()\n");

	return 0;
}


long PAD__close(void) {
	return 0;
}