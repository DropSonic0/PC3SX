/*
 * menu.cpp
 *
 *  Created on: Oct 10, 2010
 *      Author: halsafar
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stack>
#include <vector>

#include "PS3Video.h"
#include "rom_list.h"
#include "input/Cellinput.h"
#include "fileio/FileBrowser.h"
#include <cell/pad.h>

#include <cell/audio.h>
#include <cell/sysmodule.h>
#include <cell/cell_fs.h>
#include <cell/dbgfont.h>
#include <sysutil/sysutil_sysparam.h>

extern int is_running;
extern int RomType;

SSettings	Settings;

#define stricmp strcasecmp

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define NUM_ENTRY_PER_PAGE 24

extern char rom_path[256];

extern PS3Graphics* Graphics;
extern CellInputFacade* PS3input;

// is the menu running
bool menuRunning = false;

// menu to render
typedef void (*curMenuPtr)();

//
std::stack<curMenuPtr> menuStack;

// main file browser->for rom browser
FileBrowser* browser = NULL;

// tmp file browser->for everything else
FileBrowser* tmpBrowser = NULL;

void MenuStop()
{
	menuRunning = false;

}

float FontSize()
{
	return 100/100.0;
}

bool MenuIsRunning()
{
	return menuRunning;
}

void UpdateBrowser(FileBrowser* b)
{
	if (PS3input->WasButtonPressed(0,CTRL_DOWN) | PS3input->IsAnalogPressedDown(0,CTRL_LSTICK))
	{
		b->IncrementEntry();
	}
	if (PS3input->WasButtonPressed(0,CTRL_UP) | PS3input->IsAnalogPressedUp(0,CTRL_LSTICK))
	{
		b->DecrementEntry();
	}
	if (PS3input->WasButtonPressed(0,CTRL_RIGHT) | PS3input->IsAnalogPressedRight(0,CTRL_LSTICK))
	{
		b->GotoEntry(MIN(b->GetCurrentEntryIndex()+5, b->GetCurrentDirectoryInfo().numEntries-1));
	}
	if (PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->IsAnalogPressedLeft(0,CTRL_LSTICK))
	{
		if (b->GetCurrentEntryIndex() <= 5)
		{
			b->GotoEntry(0);
		}
		else
		{
			b->GotoEntry(b->GetCurrentEntryIndex()-5);
		}
	}
	if (PS3input->WasButtonPressed(0,CTRL_R1))
	{
		b->GotoEntry(MIN(b->GetCurrentEntryIndex()+NUM_ENTRY_PER_PAGE, b->GetCurrentDirectoryInfo().numEntries-1));
	}
	if (PS3input->WasButtonPressed(0,CTRL_L1))
	{
		if (b->GetCurrentEntryIndex() <= NUM_ENTRY_PER_PAGE)
		{
			b->GotoEntry(0);
		}
		else
		{
			b->GotoEntry(b->GetCurrentEntryIndex()-NUM_ENTRY_PER_PAGE);
		}
	}

	if (PS3input->WasButtonPressed(0, CTRL_CIRCLE))
	{
		// don't let people back out past root
		if (b->DirectoryStackCount() > 1)
		{
			b->PopDirectory();
		}
	}
}

void RenderBrowser(FileBrowser* b)
{
	uint32_t file_count = b->GetCurrentDirectoryInfo().numEntries;
	int current_index = b->GetCurrentEntryIndex();

	if (file_count <= 0)
	{
		printf("1: filecount <= 0");
		cellDbgFontPuts		(0.09f,	0.05f,	FontSize(),	RED,	"No Roms founds!!!\n");
	}
	else if (current_index > file_count)
	{
		printf("2: current_index >= file_count");
	}
	else
	{
		int page_number = current_index / NUM_ENTRY_PER_PAGE;
		int page_base = page_number * NUM_ENTRY_PER_PAGE;
		float currentX = 0.05f;
		float currentY = 0.00f;
		float ySpacing = 0.035f;
		for (int i = page_base; i < file_count && i < page_base + NUM_ENTRY_PER_PAGE; ++i)
		{
			currentY = currentY + ySpacing;
			cellDbgFontPuts(currentX, currentY, FontSize(),
					i == current_index ? RED : (*b)[i]->d_type == CELL_FS_TYPE_DIRECTORY ? GREEN : WHITE,
					(*b)[i]->d_name);
			Graphics->FlushDbgFont();
		}
	}
	Graphics->FlushDbgFont();
}

void do_shaderChoice()
{
	if (tmpBrowser == NULL)
	{
		tmpBrowser = new FileBrowser("/dev_hdd0/game/PCSX00001/USRDIR/shaders/\0");
	}

	string path;

	if (PS3input->UpdateDevice(0) == CELL_PAD_OK)
	{
		UpdateBrowser(tmpBrowser);

		if (PS3input->WasButtonPressed(0,CTRL_CROSS))
		{
			if(tmpBrowser->IsCurrentADirectory())
			{
				tmpBrowser->PushDirectory(	tmpBrowser->GetCurrentDirectoryInfo().dir + "/" + tmpBrowser->GetCurrentEntry()->d_name,
						CELL_FS_TYPE_REGULAR | CELL_FS_TYPE_DIRECTORY, "cg");
			}
			else if (tmpBrowser->IsCurrentAFile())
			{
				path = tmpBrowser->GetCurrentDirectoryInfo().dir + "/" + tmpBrowser->GetCurrentEntry()->d_name;

				//load shader
				Graphics->LoadFragmentShader(path);
				Graphics->SetSmooth(false);
				menuStack.pop();
			}
		}

		if (PS3input->WasButtonHeld(0, CTRL_TRIANGLE))
		{
			menuStack.pop();
		}
	}

	cellDbgFontPuts(0.09f, 0.88f, FontSize(), YELLOW, "X - Select shader");
	cellDbgFontPuts(0.09f, 0.92f, FontSize(), PURPLE, "Triangle - return to settings");
	Graphics->FlushDbgFont();

	RenderBrowser(tmpBrowser);
}

int currently_selected_setting = 0;

void do_general_settings()
{
	if(PS3input->UpdateDevice(0) == CELL_PAD_OK)
	{
			// back to ROM menu if CIRCLE is pressed
			if (PS3input->WasButtonPressed(0, CTRL_CIRCLE))
			{
				menuStack.pop();
				return;
			}

			if (PS3input->WasButtonPressed(0, CTRL_DOWN) | PS3input->WasAnalogPressedDown(0, CTRL_LSTICK))	// down to next setting
			{
				currently_selected_setting++;
				if (currently_selected_setting >= MAX_NO_OF_SETTINGS)
				{
					currently_selected_setting = 0;
				}
			}

			if (PS3input->WasButtonPressed(0, CTRL_UP) | PS3input->WasAnalogPressedUp(0, CTRL_LSTICK))	// up to previous setting
			{
					currently_selected_setting--;
					if (currently_selected_setting < 0)
					{
						currently_selected_setting = MAX_NO_OF_SETTINGS-1;
					}
			}
					switch(currently_selected_setting)
					{
					case SETTING_CHANGE_RESOLUTION:
						   if(PS3input->WasButtonPressed(0, CTRL_RIGHT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK))
						   {
							   Graphics->NextResolution();
						   }
						   if(PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK))
						   {
							   Graphics->PreviousResolution();
						   }
						   if(PS3input->WasButtonPressed(0, CTRL_CROSS))
						   {
							   Graphics->SwitchResolution(Graphics->GetCurrentResolution(), Settings.PS3PALTemporalMode60Hz);
						   }
						   if(PS3input->IsButtonPressed(0, CTRL_START))
						   {
							   Graphics->SwitchResolution(Graphics->GetInitialResolution(), Settings.PS3PALTemporalMode60Hz);
						   }
						   break;
				case SETTING_PAL60_MODE:
						   if(PS3input->WasButtonPressed(0, CTRL_RIGHT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0,CTRL_CROSS) | PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK))
						   {
							   if (Graphics->GetCurrentResolution() == CELL_VIDEO_OUT_RESOLUTION_576)
							   {
								   if(Graphics->CheckResolution(CELL_VIDEO_OUT_RESOLUTION_576))
								   {
									   Settings.PS3PALTemporalMode60Hz = !Settings.PS3PALTemporalMode60Hz;
									   Graphics->SetPAL60Hz(Settings.PS3PALTemporalMode60Hz);
									   Graphics->SwitchResolution(Graphics->GetCurrentResolution(), Settings.PS3PALTemporalMode60Hz);
								   }
							   }

						   }
						   break;
				case SETTING_SHADER:
						if(PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->WasAnalogPressedLeft(0, CTRL_LSTICK) | PS3input->WasButtonPressed(0, CTRL_RIGHT) | PS3input->WasAnalogPressedRight(0, CTRL_LSTICK) | PS3input->WasButtonPressed(0, CTRL_CROSS))
						{
							menuStack.push(do_shaderChoice);
							tmpBrowser = NULL;
						}
						break;
				case SETTING_FONT_SIZE:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0,CTRL_CROSS))
					{
						if(Settings.PS3FontSize > -100)
						{
							Settings.PS3FontSize--;
						}
					}
					if(PS3input->WasButtonPressed(0, CTRL_RIGHT) | PS3input->WasAnalogPressedRight(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0,CTRL_CROSS))
					{
						if((Settings.PS3OverscanAmount < 100))
						{
							Settings.PS3FontSize++;
						}
					}
					if(PS3input->IsButtonPressed(0, CTRL_START))
					{
						Settings.PS3FontSize = 100;
					}
					break;
				case SETTING_KEEP_ASPECT_RATIO:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0, CTRL_RIGHT) | PS3input->WasAnalogPressedRight(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0,CTRL_CROSS))
					{
						Settings.PS3KeepAspect = !Settings.PS3KeepAspect;
						Graphics->SetAspectRatio(Settings.PS3KeepAspect);
					}
					if(PS3input->IsButtonPressed(0, CTRL_START))
					{
						Settings.PS3KeepAspect = true;
						Graphics->SetAspectRatio(Settings.PS3KeepAspect);
					}
					break;
				case SETTING_HW_TEXTURE_FILTER:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0, CTRL_RIGHT) | PS3input->WasAnalogPressedRight(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0,CTRL_CROSS))
					{
						Settings.PS3Smooth = !Settings.PS3Smooth;
						Graphics->SetSmooth(Settings.PS3Smooth);
					}
					if(PS3input->IsButtonPressed(0, CTRL_START))
					{
						Settings.PS3Smooth = true;
						Graphics->SetSmooth(Settings.PS3Smooth);
					}
					break;
				case SETTING_PAD:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) )
					{
						Settings.PAD = false;
					}
					if(PS3input->WasButtonPressed(0, CTRL_RIGHT))
					{
						Settings.PAD = true;
					}
					break;
				case SETTING_FPS:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) )
					{
						Settings.Fps = false;
					}
					if(PS3input->WasButtonPressed(0, CTRL_RIGHT))
					{
						Settings.Fps = true;
					}
					break;
				case SETTING_CPU:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) )
					{
						Settings.CPU = false;
					}
					if(PS3input->WasButtonPressed(0, CTRL_RIGHT))
					{
						Settings.CPU = true;
					}
					break;
				case SETTING_HLE:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) )
					{
						Settings.HLE = false;
					}
					if(PS3input->WasButtonPressed(0, CTRL_RIGHT))
					{
						Settings.HLE = true;
					}
					break;
				case SETTING_DEFAULT_ALL:
					if(PS3input->WasButtonPressed(0, CTRL_LEFT) | PS3input->WasAnalogPressedLeft(0,CTRL_LSTICK) | PS3input->WasButtonPressed(0, CTRL_RIGHT) | PS3input->WasAnalogPressedRight(0,CTRL_LSTICK) | PS3input->IsButtonPressed(0, CTRL_START) | PS3input->WasButtonPressed(0, CTRL_CROSS))
					{
						Settings.PS3KeepAspect = true;
						Settings.PS3Smooth = true;
						Settings.CPU = false;
						Settings.HLE = false;
						Settings.Fps = false;
						Graphics->SetAspectRatio(Settings.PS3KeepAspect);
						Graphics->SetSmooth(Settings.PS3Smooth);
						Settings.PAD = false;
						Settings.PS3PALTemporalMode60Hz = false;
						Graphics->SetPAL60Hz(Settings.PS3PALTemporalMode60Hz);
					}
					break;
				default:
					break;
			} // end of switch
	}

	float yPos = 0.09;
	float ySpacing = 0.04;

	cellDbgFontPuts		(0.09f,	0.05f,	FontSize(),	RED,	"GENERAL Setting");
	yPos += ySpacing;
	cellDbgFontPuts		(0.09f,	yPos,	FontSize(),	RED,	"PS3SX");

	yPos += ySpacing;

	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_CHANGE_RESOLUTION ? YELLOW : WHITE, "Resolution");

	switch(Graphics->GetCurrentResolution())
	{
		case CELL_VIDEO_OUT_RESOLUTION_480:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_480 ? GREEN : RED, "720x480 (480p)");
			break;
		case CELL_VIDEO_OUT_RESOLUTION_720:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_720 ? GREEN : RED, "1280x720 (720p)");
			break;
		case CELL_VIDEO_OUT_RESOLUTION_1080:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_1080 ? GREEN : RED, "1920x1080 (1080p)");
			break;
		case CELL_VIDEO_OUT_RESOLUTION_576:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_576 ? GREEN : RED, "720x576 (576p)");
			break;
		case CELL_VIDEO_OUT_RESOLUTION_1600x1080:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_1600x1080 ? GREEN : RED, "1600x1080");
			break;
		case CELL_VIDEO_OUT_RESOLUTION_1440x1080:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_1440x1080 ? GREEN : RED, "1440x1080");
			break;
		case CELL_VIDEO_OUT_RESOLUTION_1280x1080:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_1280x1080 ? GREEN : RED, "1280x1080");
			break;
		case CELL_VIDEO_OUT_RESOLUTION_960x1080:
			cellDbgFontPrintf(0.5f, yPos, FontSize(), Graphics->GetInitialResolution() == CELL_VIDEO_OUT_RESOLUTION_960x1080 ? GREEN : RED, "960x1080");
			break;
	}
	Graphics->FlushDbgFont();

	yPos += ySpacing;
	cellDbgFontPuts		(0.09f,	yPos,	FontSize(),	currently_selected_setting == SETTING_PAL60_MODE ? YELLOW : WHITE,	"PAL60 Mode (576p only)");
	cellDbgFontPrintf	(0.5f,	yPos,	FontSize(),	Settings.PS3PALTemporalMode60Hz == true ? RED : GREEN, Settings.PS3PALTemporalMode60Hz == true ? "ON" : "OFF");

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_SHADER ? YELLOW : WHITE, "Selected shader");
	cellDbgFontPrintf(0.5f, yPos, FontSize(), 
			GREEN, 
			"%s", Graphics->GetFragmentShaderPath().substr(Graphics->GetFragmentShaderPath().find_last_of('/')).c_str());

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_FONT_SIZE ? YELLOW : WHITE, "Font size");
	cellDbgFontPrintf(0.5f,	yPos,	FontSize(),	Settings.PS3FontSize == 100 ? GREEN : RED, "%f", FontSize());

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_KEEP_ASPECT_RATIO ? YELLOW : WHITE, "Aspect Ratio");
	cellDbgFontPrintf(0.5f, yPos, FontSize(), Settings.PS3KeepAspect == true ? GREEN : RED, "%s", Settings.PS3KeepAspect == true ? "Scaled" : "Stretched");
	Graphics->FlushDbgFont();

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_HW_TEXTURE_FILTER ? YELLOW : WHITE, "Hardware Filtering");
	cellDbgFontPrintf(0.5f, yPos, FontSize(), Settings.PS3Smooth == true ? GREEN : RED,
			"%s", Settings.PS3Smooth == true ? "Linear interpolation" : "Point filtering");

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_PAD ? YELLOW : WHITE, "PSX Dualshock PAD");
	cellDbgFontPrintf(0.5f, yPos, FontSize(), Settings.PAD == true ? GREEN : RED,
			"%s", Settings.PAD == true ? "Enable" : "Disable");

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_FPS ? YELLOW : WHITE, "Display FPS");
	cellDbgFontPrintf(0.5f, yPos, FontSize(), Settings.Fps == true ? GREEN : RED,
			"%s", Settings.Fps == true ? "Yes" : "No ");

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_CPU ? YELLOW : WHITE, "Use CPU interpreter");
	cellDbgFontPrintf(0.5f, yPos, FontSize(),Settings.CPU == true ? GREEN : RED,
			"%s", Settings.CPU == true ? "Yes" : "No");

	yPos += ySpacing;
	cellDbgFontPuts(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_HLE ? YELLOW : WHITE, "Use HLE BIOS");
	cellDbgFontPrintf(0.5f, yPos, FontSize(),Settings.HLE == true ? GREEN : RED,
			"%s", Settings.HLE == true ? "Yes" : "No");

	yPos += ySpacing;
	cellDbgFontPrintf(0.09f, yPos, FontSize(), currently_selected_setting == SETTING_DEFAULT_ALL ? YELLOW : GREEN, "DEFAULT");
	Graphics->FlushDbgFont();

	if(currently_selected_setting == SETTING_CPU)
	cellDbgFontPuts(0.09f, 0.80f, FontSize(), WHITE, "Try to use Interpreter more compatible but Slow");

	cellDbgFontPuts(0.09f, 0.88f, FontSize(), YELLOW, "UP/DOWN - select,  X/LEFT/RIGHT - change,  START - default");
	cellDbgFontPuts(0.09f, 0.92f, FontSize(), YELLOW, "CIRCLE - return to Roms menu");
	Graphics->FlushDbgFont();
}

void GetExtension(const char *srcfile,char *outext)
{
		strcpy(outext,srcfile + strlen(srcfile) - 3);
}

void do_ROMMenu()
{
	string path;
	char ext[4];

	if (PS3input->UpdateDevice(0) == CELL_PAD_OK)
	{
		UpdateBrowser(browser);

		if (PS3input->WasButtonPressed(0,CTRL_CROSS))
		{
			if(browser->IsCurrentADirectory())
			{
				browser->PushDirectory( browser->GetCurrentDirectoryInfo().dir + "/" + browser->GetCurrentEntry()->d_name,
						CELL_FS_TYPE_REGULAR | CELL_FS_TYPE_DIRECTORY,
						"iso|img|bin|ISO|BIN|IMG");
			}
			else if (browser->IsCurrentAFile())
			{
				// load game (standard controls), go back to main loop
				path = browser->GetCurrentDirectoryInfo().dir + "/" + browser->GetCurrentEntry()->d_name;

				MenuStop();


				sprintf(rom_path,"%s",path.c_str());


				GetExtension(path.c_str(),ext);
			
				if(!stricmp(ext,"psx")||!stricmp(ext,"PSX")||!stricmp(ext,"exe")||!stricmp(ext,"EXE"))
					RomType = 1;
				else
					RomType = 2;

				return;
			}
		}
		if (PS3input->WasButtonPressed(0,CTRL_CIRCLE))
		{
			menuStack.push(do_general_settings);
			tmpBrowser = NULL;
		}
		if (PS3input->IsButtonPressed(0,CTRL_L2) && PS3input->IsButtonPressed(0,CTRL_R2))
		{
			
			return;
		}
	}

	cellDbgFontPuts(0.09f, 0.88f, FontSize(), YELLOW, "CROSS - Enter directory/Load game");
	cellDbgFontPuts(0.09f, 0.92f, FontSize(), YELLOW, "CIRCLE - Settings screen");
	Graphics->FlushDbgFont();

	RenderBrowser(browser);
}

void MenuMainLoop(char* path)
{
	// create file browser->if null
	if (browser == NULL)
	{
		browser = new FileBrowser(path);//"/dev_usb000/genplus/roms");
	}


	// FIXME: could always just return to last menu item... don't pop on resume kinda thing
	if (menuStack.empty())
	{
		menuStack.push(do_ROMMenu);
	}

	// menu loop
	menuRunning = true;
	while (!menuStack.empty() && menuRunning)
	{
		Graphics->Clear();

		menuStack.top()();

		Graphics->Swap();

		cellSysutilCheckCallback();
		if(!is_running)
		{
		   //BYE BYE
		   sys_process_exit(0);
		}
	}
}

