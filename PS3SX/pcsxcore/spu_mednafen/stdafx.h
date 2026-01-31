#ifndef STDAFX_PSXSPU_H
#define STDAFX_PSXSPU_H
/***************************************************************************
                           StdAfx.h  -  description
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <math.h>  
#include <assert.h>
#include <stdint.h>
#include "psemuxa.h"

#define CALLBACK

static inline int			CLAMP16				(int x)
{
	return (x > 32767) ? 32767 : ((x < -32768) ? -32768 : x);
}

#endif
