#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sys.h>
#include <kammy.h>
#include <common/kammy_lv2.h>

#define SONIC3AIR_PROG        ("/dev_hdd0/game/SNC300AIR/USRDIR/sonic3air.self")

// Step 3: Pure launcher test.
// This program logs that it's about to launch the emulator, then tries to launch it.

extern "C" int main(int argc, char** argv)
{

	sys_game_process_exitspawn2(SONIC3AIR_PROG, NULL, NULL,NULL, 0, 0,
									   SYS_PROCESS_PRIMARY_STACK_SIZE_512K);

	return 0;
}
