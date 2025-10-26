#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sys.h>
#include <kammy.h>
#include <common/kammy_lv2.h>

#include "debug.h"

#include "ethdebug.bin.h"

#include <stdio.h>

static u64 LoadModule(const u8* bin, const char* name, u64 param1 = 0, u64 param2 = 0, u64 param3 = 0, u64 param4 = 0, u64 param5 = 0, u64 param6 = 0)
{
	u64 ret = KAMMY_ERROR;
	printf("\tLoading %s into lv2... ", name);
	const Lv2Module* module = Kammy_Load(bin);
	if (!module->Verify()) {
		printf("Failed! %s is corrupt.\n", name);
		return ret;
	}
	if (!module->Execute(&ret, param1, param2, param3, param4, param5, param6))
		printf("Failed! Kammy was unable to load %s.\n", name);
	else
		printf("Done! Result returned from %s in lv2: 0x%08x%08x\n", name, (u32)(ret >> 32), (u32)ret);
	return ret;
}

static bool Init()
{
	if (Kammy_IsInitialised())
		printf("\tAlready loaded!\n");
	else {
		printf("\tInitializing... ");
		if (Kammy_Initialise())
			printf("Done! Kammy injected into lv2 successfully.\n");
		else {
			printf("Failed! Syscalls could not be injected. Does this payload have peek/poke?\n");
			return false;
		}
	}

	return true;
}

#define PCSX_PROG        ("/dev_hdd0/game/PCSX00001/EMU/Ps3sx.bin")

extern "C" int main(int argc, char** argv)
{
	printf("Kammy v" LIBKAMMY_VERSION "\n");
	
	if (!Init()) {
		return 0;
	}

	LoadModule(ethdebug_bin, "ethdebug.bin");
	printf("This should be a TTY_WRITE test.\n");

	printf("Aaand we're done for tonight folks.\n");

	u64 lol = Lv2Syscall(363);
	printf("lolpsn: %016llx\n", lol);

	printf("This is main program.\n");

	sys_game_process_exitspawn2(PCSX_PROG, NULL, NULL,NULL, 0, 0,
									   SYS_PROCESS_PRIMARY_STACK_SIZE_1M);

	return 0;
}
