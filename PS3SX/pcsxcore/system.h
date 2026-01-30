/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *   schultz.ryan@gmail.com, http://rschultz.ath.cx/code.php               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

int SysInit(void);							// Init mem and plugins
void SysReset(void);						// Resets mem
void SysPrintf(const char *fmt, ...);			// Printf used by bios syscalls
void SysMessage(const char *fmt, ...);		// Message used to print msg to users
void *SysLoadLibrary(const char *lib);		// Loads Library
void *SysLoadSym(void *lib, const char *sym);	// Loads Symbol from Library
const char *SysLibError(void);				// Gets previous error loading sysbols
void SysCloseLibrary(void *lib);		// Closes Library
void SysUpdate(void);						// Called on VBlank (to update i.e. pads)
void SysRunGui(void);						// Returns to the Gui
void SysClose(void);						// Close mem and plugins

#endif /* __SYSTEM_H__ */
