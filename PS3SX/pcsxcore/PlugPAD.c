#include <stdio.h>
#include <stdlib.h>
#include "psxcommon.h"
#include "plugins.h"

long  PadFlags = 0;

long PAD__init(long flags) {
	PadFlags |= flags;
	return 0;
}

long PAD__shutdown(void) {
	return 0;
}

long PAD__open(void)
{

	SysPrintf("start PAD1_open()\n");

	return 0;
}


long PAD__close(void) {
	return 0;
}