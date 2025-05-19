/***************************************************************************
                            cfg.c  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
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

#include "stdafx.h"

#define _IN_CFG

#include "externals.h"

////////////////////////////////////////////////////////////////////////
// WINDOWS CONFIG/ABOUT HANDLING
////////////////////////////////////////////////////////////////////////
        
#include <unistd.h>

////////////////////////////////////////////////////////////////////////
// START EXTERNAL CFG TOOL
////////////////////////////////////////////////////////////////////////

void StartCfgTool(char * pCmdLine)
{

}

/////////////////////////////////////////////////////////
// READ LINUX CONFIG FILE
/////////////////////////////////////////////////////////

void ReadConfigFile(void)
{
}

/////////////////////////////////////////////////////////
// READ CONFIG called by spu funcs
/////////////////////////////////////////////////////////

void ReadConfig(void)             
{
#ifndef __GX__
 iVolume=2; 
#endif //!__GX__
 iUseXA=1; 
 iXAPitch=0;
 iSPUIRQWait=1;  
 iUseTimer=2;
 iUseReverb=2;
 iUseInterpolation=2;
 iDisStereo=0;
 iUseDBufIrq=0;

 ReadConfigFile();

}


