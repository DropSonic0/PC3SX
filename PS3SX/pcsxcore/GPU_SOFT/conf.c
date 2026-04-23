/***************************************************************************
                          cfg.c  -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

//*************************************************************************//
// History of changes:
//
// 2005/04/15 - Pete
// - Changed user frame limit to floating point value
//
// 2003/01/26 - linuzappz
// - rewrote Config/About dialogs using Glade
// - fullscreen modes are get through XF86VM if enabled
//
// 2003/01/04 - Pete
// - added dithering/scale2x options and odd/even game fix
//
// 2002/11/06 - Pete
// - added 2xSai, Super2xSaI, SuperEagle cfg stuff
//
// 2002/09/27 - linuzappz
// - added based on cfg.c for linux gui
//
//*************************************************************************//

#include "stdafx.h"


#define _IN_CFG

//#include <gtk/gtk.h>
//#include "support.h"
//#include "callbacks.h"
//#include "interface.h"
#undef FALSE
#undef TRUE

#ifdef USE_XF86VM

#include <X11/extensions/xf86vmode.h>
static XF86VidModeModeInfo **modes=0;

#endif

#include "externals.h"
#include "cfg.h"
#include "gpu.h"


int            iResX;
int            iResY;
int32_t           GlobalTextAddrX,GlobalTextAddrY,GlobalTextTP;
int32_t           GlobalTextREST,GlobalTextABR,GlobalTextPAGE;
short          ly0,lx0,ly1,lx1,ly2,lx2,ly3,lx3;
long           lLowerpart;
BOOL           bIsFirstFrame;
int            iWinSize;
BOOL           bCheckMask;
unsigned short sSetMask;
unsigned long  lSetMask;
BOOL           bDeviceOK;
short          g_m1;
short          g_m2;
short          g_m3;
short          DrawSemiTrans;
int            iUseGammaVal;
int            iUseScanLines;
int            iUseDither;
int            iDesktopCol;
int            iUseNoStretchBlt;
int            iShowFPS;
int            iFastFwd;


BOOL           bUsingTWin;
TWin_t         TWin;
unsigned long  clutid;
void (*primTableJ[256])(unsigned char *);
void (*primTableSkip[256])(unsigned char *);
unsigned short  usMirror;
uint32_t  dwCfgFixes;
uint32_t  dwActFixes;
int            iUseFixes;
BOOL           bDoVSyncUpdate;
int32_t           drawX;
int32_t           drawY;
int32_t           drawW;
int32_t           drawH;

VRAMLoad_t     VRAMWrite;
VRAMLoad_t     VRAMRead;
DATAREGISTERMODES DataWriteMode;
DATAREGISTERMODES DataReadMode;
int            iColDepth;
int            iWindowMode;
//char           szDispBuf[];
//char           szMenuBuf[];
//char           szDebugText[];
//short          sDispWidths[];
BOOL           bDebugText;
//unsigned int   iMaxDMACommandCounter;
//unsigned long  dwDMAChainStop;
PSXDisplay_t   PSXDisplay;
PSXDisplay_t   PreviousPSXDisplay;
BOOL           bSkipNextFrame;
long           lGPUstatusRet;
long           drawingLines;
unsigned char  * psxVub;
signed char    * psxVsb;
unsigned short * psxVuw;  
signed short   * psxVsw;
uint32_t  * psxVul;
int32_t    * psxVsl;
BOOL           bChangeWinMode;
long           lSelectedSlot;
BOOL           bInitCap;


uint32_t dwCoreFlags;

unsigned long  ulKeybits;


int            UseFrameLimit;
int            UseFrameSkip;
float          fFrameRate;
int            iFrameLimit;
float          fFrameRateHz;
float          fps_skip;
float          fps_cur;

char           modesl[64][32];

extern char * pConfigFile;


#if 0 
////////////////////////////////////////////////////////////////////////
// gtk linux stuff
////////////////////////////////////////////////////////////////////////

GtkWidget *ConfDlg;
GtkWidget *FullBtn,*WinBtn;
//GtkWidget *StretchBtn;
GtkWidget *ShowFpsBtn,*FpsLimitBtn,*FrameSkipBtn,*AutoFpsBtn;
GtkWidget *ScanLinesBtn,*FixesBtn;
GtkWidget *EditW,*EditH,*EditF;
GtkWidget *ComboModes, *StretchComboModes, *DitherComboModes;
GtkWidget *FixesDlg;
GtkWidget *FixesBtns[32];
GList *Modes;
GList *SModes;
GList *DModes;

void OnCfgFixes_Ok()
{
 int i;

 for (i=0; i<32; i++)
  if (FixesBtns[i] != NULL)
  {
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FixesBtns[i]))) dwCfgFixes |= 1<<i;
   else dwCfgFixes &= ~(1<<i);
  }
 gtk_widget_hide(FixesDlg);
}

void OnCfgFixes_Cancel()
{
 gtk_widget_hide(FixesDlg);
}

#define AddCfgFixBtn(name, desc) \
 BBox = gtk_hbox_new(0, 5); \
  gtk_box_pack_start(GTK_BOX(Box), BBox, FALSE, FALSE, 0); \
  \
 sprintf(Text, "0x%4.4x:", (1<<i)); \
 Label = gtk_label_new(Text); \
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0); \
 \
 FixesBtns[i] = gtk_check_button_new_with_label(name); \
 gtk_widget_set_usize(FixesBtns[i], 180, 25); \
 gtk_box_pack_start(GTK_BOX(BBox), FixesBtns[i], FALSE, FALSE, 0); \
 \
 Label = gtk_label_new(desc); \
 gtk_box_pack_start(GTK_BOX(BBox), Label, FALSE, FALSE, 0); \
 \
 i++;

void OnCfgFixes(GtkButton *button, gpointer user_data)
{
 GtkWidget *DlgPacker;
 GtkWidget *Label;
 GtkWidget *Ok,*Cancel;
 GtkWidget *Box,*BBox;
 int i;
 char Text[32];

 FixesDlg = gtk_window_new(GTK_WINDOW_DIALOG);
// gtk_widget_set_usize(FixesDlg, 500, 400);
 gtk_window_set_title(GTK_WINDOW(FixesDlg), "Special game fixes...");
 gtk_window_set_position(GTK_WINDOW(FixesDlg), GTK_WIN_POS_CENTER);
 gtk_container_set_border_width(GTK_CONTAINER(FixesDlg), 5);

 DlgPacker = gtk_packer_new();
 gtk_container_add(GTK_CONTAINER(FixesDlg), DlgPacker);

 Box = gtk_vbox_new(0, 5);
 gtk_container_set_border_width(GTK_CONTAINER(Box), 5);
 gtk_packer_add(GTK_PACKER(DlgPacker), Box, GTK_SIDE_TOP, GTK_ANCHOR_CENTER,
                GTK_FILL_X, 0, 8, 8, 0, 0);

 Label = gtk_label_new("Activate the following options only if you want to play one of the listed games\n(or if your game is showing similar glitches)!");
 gtk_box_pack_start(GTK_BOX(Box), Label, FALSE, FALSE, 0);

 for (i=0; i<32; i++) FixesBtns[i] = NULL;

 i = 0;
 AddCfgFixBtn("Odd/even bit hack"        , "epsxe 1.5.2 - ChronoCross");
 AddCfgFixBtn("Expand screen width"      , "2D Capcom fighters");
 AddCfgFixBtn("Ignore brightness color"  , "Black screens in Lunar");
 AddCfgFixBtn("Disable coord check"      , "Old compatibility mode");
 AddCfgFixBtn("Unused"                   , "");
 AddCfgFixBtn("Use PC fps calculation"   , "Better fps limitation with some games");
 AddCfgFixBtn("Lazy screen updates"      , "Maybe faster, needed for Pandemonium2");
 AddCfgFixBtn("Use old frame skipping"   , "Skips only every second frame");
 AddCfgFixBtn("Repeated flat tex triangles"   , "Needed by Dark Forces");
 AddCfgFixBtn("Draw quads with triangles"   , "better g-colors, worse textures");

 for (i=0; i<32; i++)
  if (FixesBtns[i] != NULL)
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FixesBtns[i]), dwCfgFixes & (1<<i));

 BBox = gtk_hbutton_box_new();
 gtk_packer_add(GTK_PACKER(DlgPacker), BBox, GTK_SIDE_BOTTOM, GTK_ANCHOR_S,
                GTK_FILL_X | GTK_FILL_Y | GTK_EXPAND, 0, 8, 8, 0, 0);

 Ok = gtk_button_new_with_label("Ok");
 gtk_signal_connect(GTK_OBJECT(Ok), "clicked", GTK_SIGNAL_FUNC(OnCfgFixes_Ok), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Ok, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Ok, GTK_CAN_DEFAULT);

 Cancel = gtk_button_new_with_label("Cancel");
 gtk_signal_connect(GTK_OBJECT(Cancel), "clicked", GTK_SIGNAL_FUNC(OnCfgFixes_Cancel), NULL);
 gtk_box_pack_start(GTK_BOX(BBox), Cancel, FALSE, FALSE, 0);
 GTK_WIDGET_SET_FLAGS(Cancel, GTK_CAN_DEFAULT);

 gtk_widget_show_all(FixesDlg);
}


//char szStretchModes[15][32]={"0: Full size stretch","1: No stretching","2: 2xSaI stretching","3: 2xSaI unstretched","4: Super2xSaI stretching","5: Super2xSaI unstretched","6: SuperEagle stretching","7: SuperEagle unstretched","8: Scale2x stretching","9: Scale2x unstretched","10: Scale3x stretching","11: Scale3x unstretched","12: HQ2X unstretched","13: HQ2X stretched","14: HQ3X unstretched"};
char szStretchModes[10][32]={"0: Full size stretch","1: No stretching","2: 2xSaI stretching","3: 2xSaI unstretched","4: Super2xSaI stretching","5: Super2xSaI unstretched","6: SuperEagle stretching","7: SuperEagle unstretched","8: Scale2x stretching","9: Scale2x unstretched"};
char szDitherModes[3][32]={"0: No dithering (fastest)","1: Game dependend","2: Always dither"};


void OnDefFast(GtkButton *button, gpointer user_data)
{
 char Text[32];

 //iResX = 320; iResY = 240;
//iResX=362;iResY=272;  // 362x272
iResX=640;iResY=480;  // 
 if (!iWindowMode)
 {
  sprintf (Text, "%dx%d", iResX, iResY);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry), Text);
 }
 sprintf(Text,"%d", iResX);
 gtk_entry_set_text(GTK_ENTRY(EditW), Text);
 sprintf(Text,"%d", iResY);
 gtk_entry_set_text(GTK_ENTRY(EditH), Text);
 sprintf(Text,"%d", 200);
 gtk_entry_set_text(GTK_ENTRY(EditF), Text);

 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FullBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WinBtn), 0);
// gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(StretchBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ShowFpsBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FpsLimitBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FrameSkipBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(AutoFpsBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ScanLinesBtn), 0);

 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry),(char *)&szStretchModes[1][0]);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry),(char *)&szDitherModes[0][0]);
}

void OnDefNice(GtkButton *button, gpointer user_data)
{
 char Text[32];

 iResX = 640; iResY = 480;
 if (!iWindowMode)
 {
  sprintf (Text, "%dx%d", iResX, iResY);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry), Text);
 }
 sprintf(Text,"%d", iResX);
 gtk_entry_set_text(GTK_ENTRY(EditW), Text);
 sprintf(Text,"%d", iResY);
 gtk_entry_set_text(GTK_ENTRY(EditH), Text);
 sprintf(Text,"%d", 200);
 gtk_entry_set_text(GTK_ENTRY(EditF), Text);

 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FullBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WinBtn), 0);
 //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(StretchBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ShowFpsBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FpsLimitBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FrameSkipBtn), 0);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(AutoFpsBtn), 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ScanLinesBtn), 0);

 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry),(char *)&szStretchModes);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry),(char *)&szDitherModes[1][0]);
}

void OnOk(GtkButton *button, gpointer user_data)
{
 char *tmp;

 iWindowMode     = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FullBtn));
// iUseNoStretchBlt = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(StretchBtn));
 iShowFPS        = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ShowFpsBtn));
 UseFrameLimit   = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FpsLimitBtn));
 UseFrameSkip    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FrameSkipBtn));
 iFrameLimit     = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(AutoFpsBtn))+1;
 iUseScanLines   = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ScanLinesBtn));
 iUseFixes       = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(FixesBtn));

 iUseNoStretchBlt =atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry)));

 iUseDither =atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry)));

 tmp = gtk_entry_get_text(GTK_ENTRY(EditW));
 iResX = atoi(tmp);
 tmp = gtk_entry_get_text(GTK_ENTRY(EditH));
 iResY = atoi(tmp);
 if (!iWindowMode)
 {
  tmp = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry));
  sscanf(tmp, "%dx%d", &iResX, &iResY);
 }
 tmp = gtk_entry_get_text(GTK_ENTRY(EditF));
 fFrameRate = (float)atof(tmp);
 gtk_widget_destroy(ConfDlg);
/* moved by lu_zero
 gtk_main_quit();
 */
 WriteConfig();
}

void OnCancel(GtkButton *button, gpointer user_data)
{
 gtk_widget_destroy(ConfDlg);
/* moved by lu_zero
 gtk_main_quit();
 */
}

void SoftDlgProc(void)
{
 char Text[32];

 ReadConfig();

 ConfDlg = create_Config();
#ifdef _SDL
 gtk_window_set_title(GTK_WINDOW(ConfDlg), "Configure the P.E.Op.S. PSX SDL Soft Renderer...");
#endif

 FullBtn = lookup_widget(ConfDlg, "GtkRadioButton_Fullscreen");
 WinBtn  = lookup_widget(ConfDlg, "GtkRadioButton_Windowed");
 EditW   = lookup_widget(ConfDlg, "GtkEntry_Width");
 EditH   = lookup_widget(ConfDlg, "GtkEntry_Height");
 EditF   = lookup_widget(ConfDlg, "GtkEntry_FPS");
 StretchComboModes = lookup_widget(ConfDlg, "GtkCombo_Stretching");
 DitherComboModes  = lookup_widget(ConfDlg, "GtkCombo_Dithering");
 ShowFpsBtn   = lookup_widget(ConfDlg, "GtkCheckButton_FPSdisplay");
 FpsLimitBtn  = lookup_widget(ConfDlg, "GtkCheckButton_FPSlimit");
 FrameSkipBtn = lookup_widget(ConfDlg, "GtkCheckButton_FPSskip");
 AutoFpsBtn   = lookup_widget(ConfDlg, "GtkCheckButton_FPSauto");
 ScanLinesBtn = lookup_widget(ConfDlg, "GtkCheckButton_Scanlines");
 FixesBtn     = lookup_widget(ConfDlg, "GtkCheckButton_Fixes");

 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(FullBtn), iWindowMode != 1);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WinBtn),  iWindowMode != 0);

 Modes = NULL;

#if !defined(_SDL) && defined(USE_XF86VM)
 {
  Display *display;
  int myscreen;

  // Open display
  display=XOpenDisplay(NULL);

  if(display)
   {
    int nmodes,iC;

    myscreen=DefaultScreen(display);
    XF86VidModeGetAllModeLines(display,myscreen,      // -> enum all mode infos
                                &nmodes,&modes);
    if(modes)                                         // -> infos got?
     {
      for(iC=0;iC<nmodes;++iC)                        // -> loop modes
       {
	    sprintf(modesl[iC], "%dx%d", modes[iC]->hdisplay, modes[iC]->vdisplay);
        Modes = g_list_append(Modes, modesl[iC]);
       }
     }
   }
 }
#else
 Modes = g_list_append(Modes, "320x240");
 Modes = g_list_append(Modes, "640x480");
 Modes = g_list_append(Modes, "800x600");
 Modes = g_list_append(Modes, "1024x768");
#endif

 ComboModes = lookup_widget(ConfDlg, "GtkCombo_Modes");
 gtk_combo_set_popdown_strings(GTK_COMBO(ComboModes), Modes);
 if (!iWindowMode)
 {
  sprintf (Text, "%dx%d", iResX, iResY);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ComboModes)->entry), Text);
 }

 sprintf(Text,"%d", iResX);
 gtk_entry_set_text(GTK_ENTRY(EditW), Text);

 sprintf(Text,"%d", iResY);
 gtk_entry_set_text(GTK_ENTRY(EditH), Text);

 /////////
 SModes = NULL;
 SModes = g_list_append(SModes, (char *)&szStretchModes[0][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[1][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[2][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[3][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[4][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[5][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[6][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[7][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[8][0]);
 SModes = g_list_append(SModes, (char *)&szStretchModes[9][0]);
// SModes = g_list_append(SModes, (char *)&szStretchModes[10][0]);
// SModes = g_list_append(SModes, (char *)&szStretchModes[11][0]);
// SModes = g_list_append(SModes, (char *)&szStretchModes[12][0]);
// SModes = g_list_append(SModes, (char *)&szStretchModes[13][0]);
// SModes = g_list_append(SModes, (char *)&szStretchModes[14][0]);

 gtk_combo_set_popdown_strings(GTK_COMBO(StretchComboModes), SModes);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(StretchComboModes)->entry), (char *)&szStretchModes[iUseNoStretchBlt][0]);

 DModes = NULL;
 DModes = g_list_append(DModes, (char *)&szDitherModes[0][0]);
 DModes = g_list_append(DModes, (char *)&szDitherModes[1][0]);
 DModes = g_list_append(DModes, (char *)&szDitherModes[2][0]);

 gtk_combo_set_popdown_strings(GTK_COMBO(DitherComboModes), DModes);
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(DitherComboModes)->entry), (char *)&szDitherModes[iUseDither][0]);

 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(ShowFpsBtn),   iShowFPS);
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(FpsLimitBtn),  UseFrameLimit);
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(FrameSkipBtn), UseFrameSkip);
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(AutoFpsBtn),   iFrameLimit-1);
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(ScanLinesBtn), iUseScanLines);
 gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(FixesBtn),     iUseFixes);

 sprintf(Text,"%.1f", fFrameRate);
 gtk_entry_set_text(GTK_ENTRY(EditF), Text);

 gtk_widget_show_all(ConfDlg);
 gtk_main ();
}
#ifndef _FPSE

GtkWidget *AboutDlg;

void OnAbout_Ok(GtkButton *button, gpointer user_data)
{
 gtk_widget_destroy(AboutDlg);
}

unsigned char revision;
unsigned char build;
char RELEASE_DATE[256];

void AboutDlgProc(void)
{
 GtkWidget *Label;
 char Version[256];

 AboutDlg = create_About();

 sprintf(Version, "%d.%d", revision, build);

 Label = lookup_widget(AboutDlg, "GtkLabel_Version");
 gtk_label_set_text(GTK_LABEL(Label), Version);
 Label = lookup_widget(AboutDlg, "GtkLabel_Date");
 gtk_label_set_text(GTK_LABEL(Label), RELEASE_DATE);

 gtk_widget_show_all(AboutDlg);
 gtk_main ();
}


////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>

/* file helpers */

#define GetValue(name, var) \
 p = strstr(pB, name); \
 if (p != NULL) { \
  p+=strlen(name); \
  while ((*p == ' ') || (*p == '=')) p++; \
  if (*p != '\n') var = atoi(p); \
 }

#define GetFloatValue(name, var) \
 p = strstr(pB, name); \
 if (p != NULL) { \
  p+=strlen(name); \
  while ((*p == ' ') || (*p == '=')) p++; \
  if (*p != '\n') var = (float)atof(p); \
 }

#define SetValue(name, var) \
 p = strstr(pB, name); \
 if (p != NULL) { \
  p+=strlen(name); \
  while ((*p == ' ') || (*p == '=')) p++; \
  if (*p != '\n') { \
   len = sprintf(t1, "%d", var); \
   strncpy(p, t1, len); \
   if (p[len] != ' ' && p[len] != '\n' && p[len] != 0) p[len] = ' '; \
  } \
 } \
 else { \
  size+=sprintf(pB+size, "%s = %d\n", name, var); \
 }

#define SetFloatValue(name, var) \
 p = strstr(pB, name); \
 if (p != NULL) { \
  p+=strlen(name); \
  while ((*p == ' ') || (*p == '=')) p++; \
  if (*p != '\n') { \
   len = sprintf(t1, "%.1f", (double)var); \
   strncpy(p, t1, len); \
   if (p[len] != ' ' && p[len] != '\n' && p[len] != 0) p[len] = ' '; \
  } \
 } \
 else { \
  size+=sprintf(pB+size, "%s = %.1f\n", name, (double)var); \
 }

void ReadConfigFile()
{

 struct stat buf;
 FILE *in;char t[256];int len, size;
 char * pB, * p;

 if(pConfigFile) 
      strcpy(t,pConfigFile);
 else 
  {
   strcpy(t,"cfg/gpuPeopsSoftX.cfg");
   in = fopen(t,"rb");
   if (!in)
    {
     strcpy(t,"gpuPeopsSoftX.cfg");
     in = fopen(t,"rb");
     if(!in) sprintf(t,"%s/gpuPeopsSoftX.cfg",getenv("HOME"));
     else    fclose(in);
    }
   else     fclose(in);
  }

 if (stat(t, &buf) == -1) return;
 size = buf.st_size;

 in = fopen(t,"rb");
 if (!in) return;

 pB=(char *)malloc(size);
 memset(pB,0,size);

 len = fread(pB, 1, size, in);
 fclose(in);

 GetValue("ResX", iResX);
 if(iResX<20) iResX=20;
 iResX=(iResX/4)*4;

 GetValue("ResY", iResY);
 if(iResY<20) iResY=20;
 iResY=(iResY/4)*4;

 GetValue("NoStretch", iUseNoStretchBlt);

 GetValue("FullScreen", iWindowMode);
 if(iWindowMode!=0) iWindowMode=0;
 else               iWindowMode=1;

 GetValue("ShowFPS", iShowFPS);
 if(iShowFPS<0) iShowFPS=0;
 if(iShowFPS>1) iShowFPS=1;

 GetValue("ScanLines", iUseScanLines);
 if(iUseScanLines<0) iUseScanLines=0;
 if(iUseScanLines>1) iUseScanLines=1;

 GetValue("UseDither", iUseDither);
 if(iUseDither<0) iUseDither=0;
 if(iUseDither>2) iUseDither=2;

 GetValue("UseFrameLimit", UseFrameLimit);
 if(UseFrameLimit<0) UseFrameLimit=0;
 if(UseFrameLimit>1) UseFrameLimit=1;

 GetValue("UseFrameSkip", UseFrameSkip);
 if(UseFrameSkip<0) UseFrameSkip=0;
 if(UseFrameSkip>1) UseFrameSkip=1;

 GetValue("FPSDetection", iFrameLimit);
 if(iFrameLimit<1) iFrameLimit=1;
 if(iFrameLimit>2) iFrameLimit=2;

 GetFloatValue("FrameRate", fFrameRate);
 if(fFrameRate<10.0f)   fFrameRate=10.0f;
 if(fFrameRate>1000.0f) fFrameRate=1000.0f;

 GetValue("CfgFixes", dwCfgFixes);

 GetValue("UseFixes", iUseFixes);
 if(iUseFixes<0) iUseFixes=0;
 if(iUseFixes>1) iUseFixes=1;

 free(pB);

}

////////////////////////////////////////////////////////////////////////

void ReadConfig(void)
{

 // defaults psp setting :=)
 iResX=480;iResY=272;  // 
 iWinSize=MAKELONG(iResX,iResY);
 iColDepth=16;
 iWindowMode=1;
 iUseScanLines=0;
 UseFrameLimit=0;
 UseFrameSkip=1;
 iFrameLimit=0;
 fFrameRate=100.0f;
 dwCfgFixes=0;
 iUseFixes=0;
 iUseNoStretchBlt=0;
 iUseDither=0;
 iShowFPS=0;

 // read sets
 ReadConfigFile();

 // additional checks
 if(!iColDepth)       iColDepth=32;
 if(iUseFixes)        dwActFixes=dwCfgFixes;
}

void WriteConfig(void) {

 struct stat buf;
 FILE *out;char t[256];int len, size;
 char * pB, * p; char t1[8];

 if(pConfigFile) 
      strcpy(t,pConfigFile);
 else 
  {
   strcpy(t,"cfg/gpuPeopsSoftX.cfg");
   out = fopen(t,"rb");
   if (!out) 
    {
     strcpy(t,"gpuPeopsSoftX.cfg");
     out = fopen(t,"rb");
     if(!out) sprintf(t,"%s/gpuPeopsSoftX.cfg",getenv("HOME"));
     else     fclose(out);
    }
   else     fclose(out);
  }

 if (stat(t, &buf) != -1) size = buf.st_size;
 else size = 0;

 out = fopen(t,"rb");
 if (!out) {
  // defaults
//  iResX=320;iResY=240;
//iResX=362;iResY=272;  // 362x272
iResX=640;iResY=480;  // 
  iColDepth=16;
  iWindowMode=1;
  iUseScanLines=0;
  iUseDither=0;
  UseFrameLimit=0;
  UseFrameSkip=0;
  iFrameLimit=2;
  fFrameRate=200.0f;
  dwCfgFixes=0;
  iUseFixes=0;
  iUseNoStretchBlt=1;
  iShowFPS=0;

  size = 0;
  pB=(char *)malloc(4096);
  memset(pB,0,4096);
 }
 else {
  pB=(char *)malloc(size+4096);
  memset(pB,0,size+4096);

  len = fread(pB, 1, size, out);
  fclose(out);
 }

 SetValue("ResX", iResX);
 SetValue("ResY", iResY);
 SetValue("NoStretch", iUseNoStretchBlt);
 SetValue("UseDither", iUseDither);
 SetValue("FullScreen", !iWindowMode);
 SetValue("ShowFPS", iShowFPS);
 SetValue("ScanLines", iUseScanLines);
 SetValue("UseFrameLimit", UseFrameLimit);
 SetValue("UseFrameSkip", UseFrameSkip);
 SetValue("FPSDetection", iFrameLimit);
 SetFloatValue("FrameRate", fFrameRate);
 SetValue("CfgFixes", (unsigned int)dwCfgFixes);
 SetValue("UseFixes", iUseFixes);

 out = fopen(t,"wb");
 if (!out) return;

 len = fwrite(pB, 1, size, out);
 fclose(out);

 free(pB);

}
#endif
/*
int main(int argc, char *argv[]) {
	if (argc < 2) return 0;

	gtk_init(NULL, NULL);

	if (!strcmp(argv[1], "configure")) {
		SoftDlgProc();
	} else {
		revision = atol(argv[2]);
		build = atol(argv[3]);
		strcpy(RELEASE_DATE, argv[4]);
		AboutDlgProc();
	}

	return 0;
}
*/




#endif 

