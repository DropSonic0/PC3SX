//Includes from mednafen
#include <mednafen/mednafen.h>
#include <mednafen/cdrom/cdromif.h>
#include <mednafen/mednafen-driver.h>
#include <mednafen/git.h>
#include <mednafen/general.h>
#include <mednafen/mempatcher.h>
#include <stdarg.h>

#ifdef __CELLOS_LV2__ //math.h must NOT be included in an 'extern "C"' block.
#include <math.h>
#endif

#define MODULENAMESPACE pcsxr
#include <module_helper.h>
using namespace pcsxr;



//Pair of functions to export timing to C files.
//TODO: These shouldn't be needed as the emulator module shouldn't be doing timing.
extern "C" uint32_t MDFNDC_GetTime(){return MDFND_GetTime();}
extern "C" void* MDFNDC_AllocateExec(uint32_t aSize){return MDFND_AllocateExec(aSize);}
extern "C" void MDFNDC_FreeExec(void* aData, uint32_t aSize){MDFND_FreeExec(aData, aSize);}

//Definitions for PCSX
extern "C"
{
	//PCSX Includes
	#include "psxcommon.h"
	#include "plugins.h"
	#include "mednafen/video_plugin/globals.h"
	#include "mednafen/input_plugin/pad.h"

	extern int8_t*		psxM;									//Pointer to the PSX's memory, for cheats
	extern int			wanna_leave;							//Flag indicating that the emulator should return

	//PCSX FUNCTIONS:
	int					OpenPlugins				();				//PCSX Suggested function (called by netError) (in src/mednafen/plugins.c)
	void				ClosePlugins			();				//PCSX Required function (called by netError) (in src/mednafen/plugins.c)

	//PCSX Required function (called on recompiler error)
	void				SysClose				()
	{
		//TODO
		assert(false);
	}

	//PCSX Required function (called on recompiler error)
	void				SysReset				()
	{
		//TODO
		assert(false);
	}

	//PCSX Required function (called every frame)
	void				SysUpdate				()
	{
		wanna_leave = 1;
	}

	//PCSX Required function (called by netError)
	void				SysRunGui				()
	{
		/* Nothing */
	}

	//PCSX Required function
	void				SysPrintf				(const char *fmt, ...)
	{
		char buffer[2048];
		va_list args;
		va_start (args, fmt);
		vsnprintf(buffer, 2048, fmt, args);
		va_end (args);

		MDFND_Message(buffer);
	}

	//PCSX Required function
	void				SysMessage				(const char *fmt, ...)
	{
		char buffer[2048];
		va_list args;
		va_start (args, fmt);
		vsnprintf(buffer, 2048, fmt, args);
		va_end (args);

		MDFND_Message(buffer);
	}

	//A Buffer holding the BIOS
	static uint8_t		BiosBuffer[1024 * 512];

	//Get the bios buffer
	void				MDFNPCSXGetBios			(uint8_t* aBuffer)
	{
		memcpy(aBuffer, BiosBuffer, 1024 * 512);
	}

	//Zlib style implement used for saving states to memory
	typedef void* smFile;
	static StateMem* stateMemory;
	smFile smopen (const char *path , const char *mode )
	{
		if(!stateMemory)
		{
			MDFND_PrintError("pcsxr: Using save state functions without a memory object?\n");
		}

		return (void*)stateMemory;
	}

	int smclose (smFile file )
	{
		if(!stateMemory)
		{
			MDFND_PrintError("pcsxr: Using save state functions without a memory object?\n");
		}

		stateMemory = 0;
	}

	off_t smseek(smFile file, off_t offset, int whence)
	{
		StateMem* mem = (StateMem*)file;
		return smem_seek(mem, offset, whence);
	}

	int smwrite (smFile file, const void* buf, unsigned int len)
	{
		StateMem* mem = (StateMem*)file;
		smem_write(mem, (void*)buf, len);
		return len;
	}

	int smread (smFile file, void* buf, unsigned int len)
	{
		StateMem* mem = (StateMem*)file;
		smem_read(mem, buf, len);
		return len;
	}
}

namespace MODULENAMESPACE
{
	extern MDFNGI							ModuleInfo;

	static const InputDeviceInputInfoStruct	GamepadIDII[] =
	{
		{"select",	"SELECT",	15,	IDIT_BUTTON, NULL},
		{"l3",		"L3",		13,	IDIT_BUTTON, NULL},
		{"r3",		"R3",		10,	IDIT_BUTTON, NULL},
		{"start",	"START",	14,	IDIT_BUTTON, NULL},
		{"up",		"UP",		0,	IDIT_BUTTON, "down"},
		{"right",	"RIGHT",	3,	IDIT_BUTTON, "left"},
		{"down",	"DOWN",		1,	IDIT_BUTTON, "up"},
		{"left",	"LEFT",		2,	IDIT_BUTTON, "right"},
		{"l2",		"L2",		12,	IDIT_BUTTON, NULL},
		{"r2",		"R2",		9,	IDIT_BUTTON, NULL},
		{"l1",		"L1",		11, IDIT_BUTTON, NULL},
		{"r1",		"R1",		8,	IDIT_BUTTON, NULL},
		{"triangle","TRIANGLE",	4,	IDIT_BUTTON, 0},
		{"circle",	"CIRCLE",	5,	IDIT_BUTTON, 0},
		{"cross",	"CROSS",	6,	IDIT_BUTTON, 0},
		{"square",	"SQUARE",	7,	IDIT_BUTTON, 0},
	};

	static InputDeviceInfoStruct 			InputDeviceInfoPSXPort[] =
	{
		{"none",	"none",		NULL,	0,	NULL},
		{"gamepad", "Gamepad",	NULL,	16,	GamepadIDII},
	};


	static const InputPortInfoStruct 		PortInfo[] =
	{
		{0, "port1", "Port 1", 2, InputDeviceInfoPSXPort, "gamepad"},
		{0, "port2", "Port 2", 2, InputDeviceInfoPSXPort, "gamepad"},
	};

	static InputInfoStruct					ModuleInput =
	{
		1,
		PortInfo
	};


	static FileExtensionSpecStruct			ModuleExtensions[] = 
	{
		{".cue", "PSX Cue File"},
		{0, 0}
	};


	static MDFNSetting						ModuleSettings[] =
	{
		{"pcsxr.bios",			MDFNSF_EMU_STATE,	"Path to required PSX BIOS ROM image.",									NULL, MDFNST_STRING,	"scph1001.bin"},
		{"pcsxr.recompiler",	MDFNSF_NOFLAGS,		"Enable the dynamic recompiler. (Need to restart mednafen to change).",	NULL, MDFNST_BOOL,		"0"},
		{"pcsxr.slowboot",		MDFNSF_NOFLAGS,		"Show the BIOS booting screen.",										NULL, MDFNST_BOOL,		"1"},
		{NULL}
	};



	static int								ModuleLoad				()
	{
		//Load the BIOS
		MDFNFILE biosFile;
		if(biosFile.Open(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, MDFN_GetSettingS("pcsxr.bios").c_str()).c_str(), 0))
		{
			if(biosFile.size == 512 * 1024)
			{
				memcpy(BiosBuffer, biosFile.data, 512 * 1024);
			}
			else
			{
				MDFN_printf("pcsxr: BIOS file size incorrect\n");
			}
		}
		else
		{
			MDFN_printf("pcsxr: Failed to load bios\n");
			return 0;
		}
	
		//Setup the config structure
		memset(&Config, 0, sizeof(Config));
		Config.PsxAuto = 1;
		Config.Cpu = MDFN_GetSettingB("pcsxr.recompiler") ? CPU_DYNAREC : CPU_INTERPRETER;
		Config.SlowBoot = MDFN_GetSettingB("pcsxr.slowboot");
		strcpy(Config.PluginsDir, "builtin");
		strcpy(Config.Gpu, "builtin");
		strcpy(Config.Spu, "builtin");
		strcpy(Config.Pad1, "builtin");
		strcpy(Config.Pad2, "builtin");
		strcpy(Config.Cdr, "builtin");
		strcpy(Config.Net, "Disabled");
		strncpy(Config.Mcd1, MDFN_MakeFName(MDFNMKF_SAV, 0, "sav").c_str(), MAXPATHLEN);
		strncpy(Config.Mcd2, MDFN_MakeFName(MDFNMKF_SAV, 0, "sav2").c_str(), MAXPATHLEN);

		//Init psx cpu emulator and memory mapping
		EmuInit();

		//Open and initialize all of the plugins
		OpenPlugins();

		//Load memory cards
		LoadMcds(Config.Mcd1, Config.Mcd2);

		//Get cdrom ID and init PPF support...
		CheckCdrom();

		//Reset the emulated CPU and Memory
		EmuReset();

		//Prepares the game to run, either runs the CPU thru the bios or finds and loads the game EXE if using the emulated bios
		LoadCdrom();

		//TODO: Set the clock if the machine is PAL

		//Just say two 1M pages, for fun not profit
		MDFNMP_Init(1024 * 1024, 2);
		MDFNMP_AddRAM(1024 * 1024 * 2, 0, (uint8_t*)psxM);

		return 1;
	}

	static bool								ModuleTestMagic			()
	{
		//TODO: Does this work in all cases?
		uint8_t Buffer[4000];
		CDIF_ReadRawSector(Buffer, 4);
		return Buffer[56] == 'S' && Buffer[57] == 'o' && Buffer[58] == 'n' && Buffer[59] == 'y';
	}

	static void								ModuleCloseGame			()
	{
		EmuShutdown();
		ClosePlugins();

		//Kill resampler
		Resampler::Kill();

		//Close the cheat engine
		MDFNMP_Kill();
	}


	static int								ModuleStateAction		(StateMem *sm, int load, int data_only)
	{
		//TODO:
		if(!load)
		{
			stateMemory = sm;
			int result = SaveState("");
			stateMemory = 0;
			return result == 0;
		}
		else
		{
			stateMemory = sm;
			int result = LoadState("");
			stateMemory = 0;
			return result == 0;
		}
	}

	static void								ModuleEmulate			(EmulateSpecStruct *espec)
	{
		//AUDIO PREP
		Resampler::Init(espec, 44100.0);

		//INPUT
		g.PadState[0].JoyKeyStatus = ~Input::GetPort<0, 2>();
		g.PadState[0].KeyStatus = ~Input::GetPort<0, 2>();
		g.PadState[1].JoyKeyStatus = ~Input::GetPort<1, 2>();
		g.PadState[1].KeyStatus = ~Input::GetPort<1, 2>();

		//CHEATS
		MDFNMP_ApplyPeriodicCheats();

		//EMULATE
		psxCpu->Execute();

		//VIDEO
		#define RED(x) (x & 0xff)
		#define BLUE(x) ((x>>16) & 0xff)
		#define GREEN(x) ((x>>8) & 0xff)
		#define COLOR(x) (x & 0xffffff)

		Video::SetDisplayRect(espec, 0, 0, 320, 240);

		if((g_gpu.dsp.mode.x && g_gpu.dsp.mode.y) && (!(g_gpu.status_reg & STATUS_DISPLAYDISABLED)) && !espec->skip)
		{
			Video::SetDisplayRect(espec, 0, 0, (g_gpu.dsp.range.x1 - g_gpu.dsp.range.x0) / g_gpu.dsp.mode.x, (g_gpu.dsp.range.y1 - g_gpu.dsp.range.y0) * g_gpu.dsp.mode.y);

			uint32_t* pixels = espec->surface->pixels;

			if(g_gpu.status_reg & STATUS_RGB24)
			{
				for(int i = 0; i != espec->DisplayRect.h; i++)
				{
					int startxy = ((1024) * (i + g_gpu.dsp.position.y)) + g_gpu.dsp.position.x;
					unsigned char* pD = (unsigned char *)&g_gpu.psx_vram.u16[startxy];
					uint32_t* destpix = (uint32_t *)(pixels + (i * espec->surface->pitchinpix));
					for(int j = 0; j != espec->DisplayRect.w; j++)
					{
						uint32_t lu = SWAP32(*((uint32_t *)pD));
						destpix[j] = 0xff000000 | (RED(lu) << 16) | (GREEN(lu) << 8) | (BLUE(lu));
						pD += 3;
					}
				}
			}
			else
			{
				Video::BlitRGB15<0, 1, 2, 2, 1, 0, -1>(espec, &g_gpu.psx_vram.u16[1024 * g_gpu.dsp.position.y + g_gpu.dsp.position.x], espec->DisplayRect.w, espec->DisplayRect.h, 1024);
			}
		}
		else if(g_gpu.status_reg & STATUS_DISPLAYDISABLED)
		{
			Video::Clear<uint32_t>(espec, 320, 240);
		}

		//AUDIO
		Resampler::Fetch(espec);

		//Update timing
		espec->MasterCycles = 1LL * 100;
	}

	static void								ModuleSetInput			(int port, const char *type, void *ptr)
	{
		Input::SetPort(port, (uint8_t*)ptr);
	}

	static void								ModuleDoSimpleCommand	(int cmd)
	{
		if(cmd == MDFN_MSC_RESET)
		{
			EmuReset();
		}
		else if(cmd == MDFN_MSC_POWER)
		{
			EmuReset();
		}
	}

	MDFNGI									ModuleInfo = 
	{
	/*	shortname:			*/	"pcsxr",
	/*	fullname:			*/	"Sony Playstation (PCSX-Reloaded)",
	/*	FileExtensions:		*/	ModuleExtensions,
	/*	ModulePriority:		*/	MODPRIO_EXTERNAL_HIGH,
	/*	Debugger:			*/	0,
	/*	InputInfo:			*/	&ModuleInput,

	/*	Load:				*/	0,
	/*	TestMagic:			*/	0,
	/*	LoadCD:				*/	ModuleLoad,
	/*	TestMagicCD:		*/	ModuleTestMagic,
	/*	CloseGame:			*/	ModuleCloseGame,
	/*	ToggleLayer:		*/	0,
	/*	LayerNames:			*/	0,
	/*	InstallReadPatch:	*/	0,
	/*	RemoveReadPatches:	*/	0,
	/*	MemRead:			*/	0,
	/*	StateAction:		*/	ModuleStateAction,
	/*	Emulate:			*/	ModuleEmulate,
	/*	SetInput:			*/	ModuleSetInput,
	/*	DoSimpleCommand:	*/	ModuleDoSimpleCommand,
	/*	Settings:			*/	ModuleSettings,
	/*	MasterClock:		*/	MDFN_MASTERCLOCK_FIXED(6000),
	/*	fps:				*/	0,
	/*	multires:			*/	true,
	/*	lcm_width:			*/	1280,	//?
	/*	lcm_height:			*/	960,	//?
	/*	dummy_separator:	*/	0,
	/*	nominal_width:		*/	640,	//?
	/*	nominal_height:		*/	480,	//?
	/*	fb_width:			*/	1024,
	/*	fb_height:			*/	512,
	/*	soundchan:			*/	2
	};
}

#ifdef MLDLL
#define VERSION_FUNC GetVersion
#define GETEMU_FUNC GetEmulator
#ifdef __WIN32__
#define DLL_PUBLIC __attribute__((dllexport))
#else
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#endif
#else
#define VERSION_FUNC pcsxGetVersion
#define GETEMU_FUNC pcsxGetEmulator
#define	DLL_PUBLIC
#endif

extern "C" DLL_PUBLIC	uint32_t		VERSION_FUNC()
{
	return 0x918;
//	return MEDNAFEN_VERSION_NUMERIC;
}
	
extern "C" DLL_PUBLIC	MDFNGI*			GETEMU_FUNC(uint32_t aIndex)
{
	return (aIndex == 0) ? &MODULENAMESPACE::ModuleInfo : 0;
}

