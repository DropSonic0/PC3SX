#ifndef MENU_H_
#define MENU_H_

#include <string>

#include "colors.h"

#define MAX_PATH 1024

struct SSettings
{
	//BEGIN OF Setting PS3
	int		PS3KeepAspect;
	int		PS3Smooth;
	int		PAD;
	int		CPU;
	int		HLE;
	int		Fps;
	bool		AnalogToDPad;
	std::string	PS3CurrentShader;
	std::string	PS3PathROMDirectory;
	int			PS3OverscanEnabled;
	int			PS3OverscanAmount;
	int			PS3PALTemporalMode60Hz;
	int			PS3FontSize;
};

extern struct SSettings			Settings;

// if you add more settings to the screen, remember to change this value to the correct number
// PATH - Total amount of Path settings
#define MAX_NO_OF_PATH_SETTINGS 4
//SNES9x - Total amount of SNES9x settings
#define MAX_NO_OF_SNES9X_SETTINGS      12
//GENERAL - Total amount of settings
#define MAX_NO_OF_SETTINGS      12

//GENERAL - setting constants
#define SETTING_CHANGE_RESOLUTION 0
#define SETTING_PAL60_MODE 1
#define SETTING_SHADER 2
#define SETTING_FONT_SIZE 3
#define SETTING_KEEP_ASPECT_RATIO 4
#define SETTING_HW_TEXTURE_FILTER 5
#define SETTING_PAD 6
#define SETTING_ANALOG_TO_DPAD 7
#define SETTING_FPS 8
#define SETTING_CPU 9
#define SETTING_HLE 10
#define SETTING_DEFAULT_ALL 11

//PATH - setting constants
#define SETTING_PATH_DEFAULT_ROM_DIRECTORY 0
#define SETTING_PATH_SAVESTATES_DIRECTORY 1
#define SETTING_PATH_SRAM_DIRECTORY 2
#define SETTING_PATH_CHEATS 3
#define SETTING_PATH_DEFAULT_ALL 4

void MenuMainLoop(char* path);
float FontSize();
void MenuStop();
bool MenuIsRunning();
char* MenuGetSelectedROM();
char* MenuGetCurrentPath();
char* do_pathmenu(uint16_t is_for_shader_or_dir_selection, const char * pathname = "/");

#endif /* MENU_H_ */

