#include <stdio.h>
#include <spu_printf.h>

int main(unsigned long long arg)
{
	(void)arg;
	spu_printf("Hello from the SPU!\n");
	return 0;
}
