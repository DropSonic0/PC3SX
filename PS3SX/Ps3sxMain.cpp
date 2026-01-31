#include <stdio.h>
#include "cell.h"
#include <vector>
#include "rom_list.h"
#include <math.h>
#include <sys/paths.h>
#include "ps3video.h"
#include "ps3audio.h"
#include "cellinput.h"
#include "ini.h"
#include <unistd.h> 
#include <pthread.h>
#include <sys/timer.h>
#include <sysutil/sysutil_gamecontent.h>

SYS_PROCESS_PARAM(1001, 0x10000);

PS3Graphics* Graphics;
CellInputFacade* PS3input  = 0;
FileIniConfig Iniconfig;

bool is_running = 0;
int boot_cdrom;
int RomType=0;
int CpuConfig = 0;

#define SAMPLERATE_44KHZ 44100
#define SB_SIZE 3840

static bool runbios = 0;
char rom_path[256];

void InitPS3()
{
	Graphics = new PS3Graphics();
	Graphics->Init();

	// FIXME: Is this necessary?
	if (Graphics->InitCg() != CELL_OK)
	{
		printf("Failed to InitCg: %d\n", __LINE__);
		exit(0);
	}

	PS3input = new CellInputFacade();
	Graphics->InitDbgFont();	
}

extern "C"
{

#include "psxcommon.h"
#include "sio.h"
#include "PlugCD.h"
#include "plugins.h"
#include "misc.h"
#include "r3000a.h"

int NeedReset = 0;
int Running =0;
long LoadCdBios = 0;
int wanna_leave = 0;

//Sound Function
unsigned long SoundGetBytesBuffered(void)
{
		return cellAudioPortWriteAvail();
}

void SoundFeedStreamData(unsigned char *pSound, long lBytes)
{		
	cellAudioPortWrite((const audio_input_t*)pSound,lBytes / 2);
}

void SetupSound(void)
{	
	cellAudioPortInit(SAMPLERATE_44KHZ,SB_SIZE);
}

void RemoveSound(void)
{
	cellAudioPortExit();
	sys_ppu_thread_exit(0);

}

//end Sound 

//Video Output Function
void ps3sxSwapBuffer(unsigned char *pixels,int w,int h)
{
	Graphics->Draw(w,h,pixels);
	Graphics->Swap();
}
//end Video

//Start PAD
long PAD__readPort1(PadDataS* pad)
{
		static unsigned short pad_status = 0xffff;

		PS3input->UpdateDevice(0);
  
		if (PS3input->IsButtonPressed(0,CTRL_CIRCLE))
		{
			pad_status &= ~(1<<13);
		}else{
			pad_status |=  (1<<13);
		}

		if (PS3input->IsButtonPressed(0,CTRL_CROSS))
		{
			pad_status &= ~(1<<14);
		}else{
			pad_status |=  (1<<14);
		}

		if (PS3input->IsButtonPressed(0,CTRL_SELECT))
		{
			pad_status &= ~(1<<2); 
		}else{
			pad_status |=  (1<<2);
		}

		if (PS3input->IsButtonPressed(0,CTRL_START))
		{
			pad_status &= ~(1<<3);
		}else{
			pad_status |=  (1<<3);
		}

		if (PS3input->IsButtonPressed(0,CTRL_DOWN))
		{
			pad_status &= ~(1<<6);
		}else{
			pad_status |=  (1<<6);
		}

		if (PS3input->IsButtonPressed(0,CTRL_UP))
		{
			pad_status &= ~(1<<4);
		}else{
			pad_status |=  (1<<4);
		}

		if (PS3input->IsButtonPressed(0,CTRL_RIGHT))
		{
			pad_status &= ~(1<<5);
		}else{
			pad_status |=  (1<<5);
		}

		if (PS3input->IsButtonPressed(0,CTRL_LEFT))
		{
			pad_status &= ~(1<<7);
		}else{
			pad_status |=  (1<<7);
		}

		if (PS3input->IsButtonPressed(0,CTRL_R1))
		{
			pad_status &= ~(1<<11);
		}else{
			pad_status |=  (1<<11);
		}

		if (PS3input->IsButtonPressed(0,CTRL_L1))
		{
			pad_status &= ~(1<<10);
		}else{
			pad_status |=  (1<<10);
		}

		if (PS3input->IsButtonPressed(0,CTRL_R2))
		{
			pad_status &= ~(1<<8);
		}else{
			pad_status |=  (1<<8);
		}

		if (PS3input->IsButtonPressed(0,CTRL_L2))
		{
			pad_status &= ~(1<<9);
		}else{
			pad_status |=  (1<<9);
		}

		if (PS3input->IsButtonPressed(0,CTRL_TRIANGLE))
		{
			pad_status &= ~(1<<12);
		}else{
			pad_status |=  (1<<12);
		}

		if (PS3input->IsButtonPressed(0,CTRL_SQUARE))
		{
			pad_status &= ~(1<<15);
		}else{
			pad_status |=  (1<<15);
		}


	pad->buttonStatus = pad_status;

	if(Settings.PAD)
		pad->controllerType = PSE_PAD_TYPE_ANALOGPAD; 
	else
		pad->controllerType = PSE_PAD_TYPE_STANDARD;

	return PSE_PAD_ERR_SUCCESS;
}

long PAD__readPort2(PadDataS* pad)
{	
		static unsigned short pad_status = 0xffff;

		PS3input->UpdateDevice(1);
  
		if (PS3input->IsButtonPressed(1,CTRL_CIRCLE))
		{
			pad_status &= ~(1<<13);
		}else{
			pad_status |=  (1<<13);
		}

		if (PS3input->IsButtonPressed(1,CTRL_CROSS))
		{
			pad_status &= ~(1<<14);
		}else{
			pad_status |=  (1<<14);
		}

		if (PS3input->IsButtonPressed(1,CTRL_SELECT))
		{
			pad_status &= ~(1<<2); 
		}else{
			pad_status |=  (1<<2);
		}

		if (PS3input->IsButtonPressed(1,CTRL_START))
		{
			pad_status &= ~(1<<3);
		}else{
			pad_status |=  (1<<3);
		}

		if (PS3input->IsButtonPressed(1,CTRL_DOWN))
		{
			pad_status &= ~(1<<6);
		}else{
			pad_status |=  (1<<6);
		}

		if (PS3input->IsButtonPressed(1,CTRL_UP))
		{
			pad_status &= ~(1<<4);
		}else{
			pad_status |=  (1<<4);
		}

		if (PS3input->IsButtonPressed(1,CTRL_RIGHT))
		{
			pad_status &= ~(1<<5);
		}else{
			pad_status |=  (1<<5);
		}

		if (PS3input->IsButtonPressed(1,CTRL_LEFT))
		{
			pad_status &= ~(1<<7);
		}else{
			pad_status |=  (1<<7);
		}

		if (PS3input->IsButtonPressed(1,CTRL_R1))
		{
			pad_status &= ~(1<<11);
		}else{
			pad_status |=  (1<<11);
		}

		if (PS3input->IsButtonPressed(1,CTRL_L1))
		{
			pad_status &= ~(1<<10);
		}else{
			pad_status |=  (1<<10);
		}

		if (PS3input->IsButtonPressed(1,CTRL_R2))
		{
			pad_status &= ~(1<<8);
		}else{
			pad_status |=  (1<<8);
		}

		if (PS3input->IsButtonPressed(1,CTRL_L2))
		{
			pad_status &= ~(1<<9);
		}else{
			pad_status |=  (1<<9);
		}

		if (PS3input->IsButtonPressed(1,CTRL_TRIANGLE))
		{
			pad_status &= ~(1<<12);
		}else{
			pad_status |=  (1<<12);
		}

		if (PS3input->IsButtonPressed(1,CTRL_SQUARE))
		{
			pad_status &= ~(1<<15);
		}else{
			pad_status |=  (1<<15);
		}


	pad->buttonStatus = pad_status;

	if(Settings.PAD)
		pad->controllerType = PSE_PAD_TYPE_ANALOGPAD; 
	else
		pad->controllerType = PSE_PAD_TYPE_STANDARD;

	return PSE_PAD_ERR_SUCCESS;
}

//end Pad

void InitConfig()
{
	memset(&Config, 0, sizeof(PcsxConfig));

	Config.PsxAuto = 1; //Autodetect
	Config.HLE	   = Settings.HLE; //Use HLE
	Config.Xa      = 1; //enable xa decoding (audio)
	Config.Sio     = 0; //disable sio interrupt ?
	Config.Mdec    = 1; //movie decode
	Config.Cdda    = 1; //enable cdda playback
	Config.SlowBoot = 0;
	
	Config.Cpu	   = Settings.CPU;// interpreter 1 :  dynarec 0

	Config.SpuIrq  = 0;
	Config.RCntFix = 0;//Parasite Eve 2, Vandal Hearts 1/2 Fix
	Config.VSyncWA = 0; // interlaced /non ? something with the display timer
	Config.PsxOut =  0; // on screen debug 
	Config.UseNet = 0;

	strcpy(Config.Net, "Disabled"); 
	strcpy(Config.Net, _("Disabled"));
	
	strcpy(Config.BiosDir, Iniconfig.biospath);
	
	//Set Bios
	strcpy(Config.Bios, "scph1001.bin");

	sprintf(Config.Mcd1, "%s/Mcd001.mcr",Iniconfig.savpath);
	sprintf(Config.Mcd2, "%s/Mcd002.mcr",Iniconfig.savpath);
}

static int sysInited = 0;

int SysInit()
{
	sysInited = 1;

    SysPrintf("start SysInit()\n");

    SysPrintf("psxInit()\n");
	EmuInit();

    SysPrintf("LoadPlugins()\n");
	LoadPlugins();
    SysPrintf("LoadMcds()\n");
	LoadMcds(Config.Mcd1, Config.Mcd2);
	SysPrintf("end SysInit()\n");
	return 0;
}

void SysReset() {
    SysPrintf("start SysReset()\n");
	EmuReset();
	SysPrintf("end SysReset()\n");
}

void SysPrintf(const char *fmt, ...) {
    va_list list;
    char msg[512];

    va_start(list, fmt);
    vsnprintf(msg, 512, fmt, list);
    va_end(list);

	dprintf_console(msg);
	if(emuLog == NULL) emuLog = fopen("/dev_hdd0/emuLog.txt","wb");
	if(emuLog) {
		fputs(msg, emuLog);
		fflush(emuLog);
	}
	printf("%s", msg);
}

void SysMessage(const char *fmt, ...) {
	va_list list;
    char msg[512];

    va_start(list, fmt);
    vsnprintf(msg, 512, fmt, list);
    va_end(list);

	dprintf_console(msg);
	if(emuLog == NULL) emuLog = fopen("/dev_hdd0/emuLog.txt","wb");
	if(emuLog) {
		fputs(msg, emuLog);
		fflush(emuLog);
		fclose(emuLog);
	}
	printf("%s", msg);
}

void *SysLoadLibrary(const char *lib) {
		return (void*)lib;
}

void *SysLoadSym(void *lib, const char *sym) {
	(void)sym;
	return lib; //smhzc
}

const char *SysLibError() {
	return NULL;
}

void SysCloseLibrary(void *lib) {
	(void)lib;
}

// Called periodically from the emu thread
void SysUpdate() {

}

// Returns to the Gui
void SysRunGui()
{

}

// Close mem and plugins
void SysClose() {
	EmuShutdown();
	ReleasePlugins();
}

void OnFile_Exit() {

}

void RunCD(){ // run the cd, no bios
	LoadCdBios = 0;
	SysPrintf("RunCD\n");
	newCD(rom_path); 
	SysReset();
	CheckCdrom();
	if (LoadCdrom() == -1) {
		ClosePlugins();

		exit(0);//epic fail
	}
	psxCpu->Execute();
}

void RunCDBIOS(){ // run the bios on the cd?
	SysPrintf("RunCDBIOS\n");
	LoadCdBios = 1;
	newCD(rom_path); 
	CheckCdrom();
	SysReset();
	psxCpu->Execute();
}

void RunEXE(){
	SysPrintf("RunEXE\n");
	SysReset();
	Load(rom_path);
	psxCpu->Execute();
}

void RunBios(){
	SysPrintf("RunBios\n");
	SysReset();
	SysMessage("Bios done!!!\n");
	psxCpu->Execute();
}

void sysutil_callback (uint64_t status, uint64_t param, void *userdata) {
	(void) param;
	(void) userdata;

	switch (status) {
		case CELL_SYSUTIL_REQUEST_EXITGAME:
			SysPrintf("exit from game\n");
			is_running = 0;
			break;
		case CELL_SYSUTIL_DRAWING_BEGIN:
		case CELL_SYSUTIL_DRAWING_END:
			break;
	}
}

//we parse our ini files
static int handler(void* user, const char* section, const char* name,const char* value)
{
    FileIniConfig* pconfig = (FileIniConfig*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("PS3SX", "version")) {
        pconfig->version = strdup(value);
    } else if (MATCH("psxrom", "rompath")) {
        pconfig->rompath = strdup(value);
    } else if (MATCH("psxsav", "savpath")) {
        pconfig->savpath = strdup(value);
    }else if (MATCH("psxsram", "srampath")) {
        pconfig->sram_path = strdup(value);
    }else if (MATCH("psxbios", "biospath")) {
        pconfig->biospath = strdup(value);
    }
	return 1;
}

void CreatFolder(char* folders)
{
	struct stat st;
	if( stat(folders,&st) == 0) return;
	
	if(mkdir(folders,0777))
	{
		gl_dprintf(0.09f,0.05f,FontSize(),"Error folder cannot be created %s !!\nplease check your GenesisConf.ini\n",folders);
		sys_timer_sleep(5);
		sys_process_exit(0);
	}
}

void RomBrowser()
{
	SysPrintf("aspec ration  0x%X \n",(int)Graphics->GetDeviceAspectRatio());
	//detection 16/9 or 4/3 Anonymous
	if((int)Graphics->GetDeviceAspectRatio() == 0x1) //0x1 == 16:9 
		Graphics->SetAspectRatio(0); // 16:9
	else
		Graphics->SetAspectRatio(1); // 4:3

	//browser with roms folder
	MenuMainLoop(Iniconfig.rompath);
	
	InitConfig();

	if (Config.HLE){
		strcpy(Config.Bios, "HLE");
	}else{
		strcpy(Config.Bios, "scph1001.bin");  	
	}

	SysInit();

	OpenPlugins();

	//clear screen to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}

int main()
{
	emuLog=NULL;
	
    int i, ret;
	
	struct stat st;
	sys_spu_initialize(6, 1); 
	cellSysutilRegisterCallback(0, (CellSysutilCallback)sysutil_callback, NULL); 

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
	if( ret != 0) printf("CELL_SYSMODULE_FS error %X\n", ret);
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_IO);
	if( ret != 0) printf("CELL_SYSMODULE_IO error %X\n", ret);
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_GAME);
	if( ret != 0) printf("CELL_SYSMODULE_SYSUTIL_GAME error %X\n", ret);
	
	is_running = 1;
	InitPS3();
	PS3input->Init();
	
	printf("InitPS3 done\n");
	unsigned int type = 0;
	unsigned int attributes = 0;
	char usrdirPath[255];
	char contentInfoPath[255];
	// we must use cellGameBootCheck before cellGameContentPermit
	ret = cellGameBootCheck(&type, &attributes, NULL, NULL);
	if (ret != CELL_GAME_RET_OK) {
		SysPrintf("cellGameBootCheck Error %X\n",ret);
	}
	
	printf("cellGameContentPermit\n");
	ret = cellGameContentPermit(contentInfoPath, usrdirPath);
	if (ret != CELL_GAME_RET_OK) {
		SysPrintf("cellGameContentPermit failed %X\n",ret);
		strcpy(usrdirPath, "/dev_hdd0/game/PCSX00001/USRDIR");
	}
	
	
	char ConfigPath[255];
	sprintf(ConfigPath, "%s/Ps3sxConf.ini", usrdirPath);
	
	SysPrintf("ini_parse\n");
	//read the ini now 
	if (ini_parse(ConfigPath, handler, &Iniconfig) < 0)
	{
		gl_dprintf(0.09f,0.05f,FontSize(),"Can't load %s\n", ConfigPath);
		sys_timer_sleep(1);
		gl_dprintf(0.09f,0.05f,FontSize(),"Wtf where is the ini!!!!!!!!bye bye try again\nPath: %s", ConfigPath);
		sys_timer_sleep(5);
		sys_process_exit(0);
	}

	
	//main path Check if not present creat all folder and exit
	CreatFolder(Iniconfig.rompath);
	CreatFolder(Iniconfig.savpath);
	CreatFolder(Iniconfig.sram_path);
	CreatFolder(Iniconfig.biospath);
	
	SysPrintf(" version  %s \n",Iniconfig.version);
	SysPrintf(" rompath  %s \n",Iniconfig.rompath);
	SysPrintf(" savpath  %s \n",Iniconfig.savpath);
	SysPrintf(" srampath %s \n",Iniconfig.sram_path);
	SysPrintf(" biospath %s \n",Iniconfig.biospath);
	
	SysPrintf("Run the emulator\n");
	RomBrowser();

	//clear screen to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	switch(RomType){
	 case 1: 
		 RunEXE();
		 break;
	 case 2: 
		 RunCD();
		 break;
	 default:
		 RunBios();
		 break;
	}

	SysPrintf("done \n");
	
	cellSysmoduleUnloadModule(CELL_SYSMODULE_IO);    
	cellSysmoduleUnloadModule(CELL_SYSMODULE_FS); 
	cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_GAME);
	cellSysutilUnregisterCallback(0);  
    
	 return(-1);
}

}

// Mednafen compatibility functions
extern "C" {
	void* MDFNDC_AllocateExec(uint32_t aSize) {
		return memalign(128, aSize);
	}

	void MDFNDC_FreeExec(void* aData, uint32_t aSize) {
		(void)aSize;
		free(aData);
	}

	uint32_t MDFNDC_GetTime() {
		sys_time_sec_t sec;
		sys_time_nsec_t nsec;
		sys_time_get_current_time(&sec, &nsec);
		return (uint32_t)((sec * 1000) + (nsec / 1000000));
	}

	void MDFND_Sleep(uint32_t ms) {
		sys_timer_usleep(ms * 1000);
	}

	void MDFND_Message(const char *msg) {
		SysPrintf("%s", msg);
	}

	void MDFN_printf(const char *fmt, ...) {
		va_list list;
		char msg[512];
		va_start(list, fmt);
		vsnprintf(msg, 512, fmt, list);
		va_end(list);
		SysPrintf("%s", msg);
	}

	void MDFND_PrintError(const char *fmt, ...) {
		va_list list;
		char msg[512];
		va_start(list, fmt);
		vsnprintf(msg, 512, fmt, list);
		va_end(list);
		SysPrintf("Error: %s", msg);
	}

	// Memory based file interface for savestates (Satisfying MDFNPS3 requirements)
	// smFile is defined as void* in psxcommon.h

	void* smopen(const char *path, const char *mode) {
		return (void*)fopen(path, mode);
	}

	off_t smseek(void* file, off_t offset, int whence) {
		return (off_t)fseek((FILE*)file, (long)offset, whence);
	}

	int smclose(void* file) {
		return fclose((FILE*)file);
	}

	int smwrite(void* file, const void* buf, unsigned int len) {
		return (int)fwrite(buf, 1, len, (FILE*)file);
	}

	int smread(void* file, void* buf, unsigned int len) {
		return (int)fread(buf, 1, len, (FILE*)file);
	}

	//A Buffer holding the BIOS
	static uint8_t BiosBuffer[1024 * 512];
	static bool BiosLoaded = false;

	//Get the bios buffer
	void MDFNPCSXGetBios(uint8_t* aBuffer)
	{
		if (!BiosLoaded) {
			char biosPath[256];
			sprintf(biosPath, "%s/%s", Iniconfig.biospath, "scph1001.bin");
			FILE* f = fopen(biosPath, "rb");
			if (f) {
				fread(BiosBuffer, 1, 512 * 1024, f);
				fclose(f);
				BiosLoaded = true;
			} else {
				SysPrintf("Error: Could not load BIOS from %s\n", biosPath);
				memset(BiosBuffer, 0, 512 * 1024);
			}
		}
		memcpy(aBuffer, BiosBuffer, 1024 * 512);
	}
}