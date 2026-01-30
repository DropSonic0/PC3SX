/*
 * cellinput.cpp
 *
 *  Created on: Oct 27, 2010
 *      Author: Halsafar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cellInput.h"



CellInputFacade::CellInputFacade()
{
	memset(_oldPadData, 0, sizeof(CellPadData) * MAX_PADS);
	memset(_newPadData, 0, sizeof(CellPadData) * MAX_PADS);
}


CellInputFacade::~CellInputFacade()
{

}


int32_t CellInputFacade::Init()
{
	return cellPadInit( MAX_PADS );
}


int32_t CellInputFacade::Deinit()
{
	return cellPadEnd();
}


CellPadData* CellInputFacade::GetNewPadData(uint8_t pad)
{
	if(pad >= NumberPadsConnected())
	{
		return 0;
	}

	return &_newPadData[pad];
}


CellPadData* CellInputFacade::GetOldPadData(uint8_t pad)
{
	if(pad >= NumberPadsConnected())
	{
		return 0;
	}

	return &_oldPadData[pad];
}


int32_t CellInputFacade::UpdateDevice(uint8_t id)
{
	int32_t ret = 0;

	// get pad info
#ifdef PS3_SDK_3_41
	ret = cellPadGetInfo2 ( &PadInfo);
#else
	ret = cellPadGetInfo ( &PadInfo);
#endif

	if(ret != CELL_PAD_OK )
	{
		return false;
	}

#ifdef PS3_SDK_3_41
	ret = PadInfo.port_status[id];
#else
	ret = PadInfo.status[id];
#endif
	if (ret == CELL_PAD_STATUS_CONNECTED)
	{
		// backup the old data
		memcpy(&_oldPadData[id], &_newPadData[id], sizeof(CellPadData));

		// get new pad data
		ret = cellPadGetData( id, &_newPadData[id] );
	}

	return ret;
}


uint32_t CellInputFacade::NumberPadsConnected()
{
#ifdef PS3_SDK_3_41
	cellPadGetInfo2 ( &PadInfo);
#else
	cellPadGetInfo ( &PadInfo);
#endif
	return PadInfo.now_connect;
}


bool CellInputFacade::WasButtonPressed(uint8_t pad, unsigned int button)
{
 	return 	((cellPadUtilGetData(&_oldPadData[pad], button) & GET_TARGET_BIT(button)) == 0) &&
 			((cellPadUtilGetData(&_newPadData[pad], button) & GET_TARGET_BIT(button)) != 0);
}


bool CellInputFacade::WasButtonReleased(uint8_t pad, unsigned int button)
{
 	return 	((cellPadUtilGetData(&_oldPadData[pad], button) & GET_TARGET_BIT(button)) != 0) &&
 			((cellPadUtilGetData(&_newPadData[pad], button) & GET_TARGET_BIT(button)) == 0);
}


bool CellInputFacade::IsButtonPressed(uint8_t pad, unsigned int button)
{
	return (cellPadUtilGetData(&_newPadData[pad], button) & GET_TARGET_BIT(button)) != 0;
}


bool CellInputFacade::IsButtonReleased(uint8_t pad, unsigned int button)
{
	return (cellPadUtilGetData(&_newPadData[pad], button) & GET_TARGET_BIT(button)) == 0;
}


bool CellInputFacade::WasButtonHeld(uint8_t pad, unsigned int button)
{
 	return 	((cellPadUtilGetData(&_oldPadData[pad], button) & GET_TARGET_BIT(button)) != 0) &&
 			((cellPadUtilGetData(&_newPadData[pad], button) & GET_TARGET_BIT(button)) != 0);
}


CellPadUtilAxis CellInputFacade::GetNewAxisValue(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis ret;

	ret.x = cellPadUtilGetData( &_newPadData[pad], axis);
	axis = ((GET_SHIFT_BIT( axis ) + 0x1) << 8) | 0xFF;
	ret.y = cellPadUtilGetData( &_newPadData[pad], axis);
	return ret;
}

CellPadUtilAxis CellInputFacade::GetOldAxisValue(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis ret;

	ret.x = cellPadUtilGetData( &_oldPadData[pad], axis);
	axis = ((GET_SHIFT_BIT( axis ) + 0x1) << 8) | 0xFF;
	ret.y = cellPadUtilGetData( &_oldPadData[pad], axis);
	return ret;
}


// is
bool CellInputFacade::IsAnalogPressedLeft(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis padaxis = GetNewAxisValue(pad,axis);
	return (padaxis.x <= 55);
}


bool CellInputFacade::IsAnalogPressedRight(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis padaxis = GetNewAxisValue(pad,axis);
	return (padaxis.x >= 210);
}


bool CellInputFacade::IsAnalogPressedDown(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis padaxis = GetNewAxisValue(pad,axis);
	return (padaxis.y >= 210);
}


bool CellInputFacade::IsAnalogPressedUp(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis padaxis = GetNewAxisValue(pad,axis);
	return (padaxis.y <= 55);
}


// was
bool CellInputFacade::WasAnalogPressedLeft(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis oldpadaxis = GetOldAxisValue(pad,axis);
	return (!(oldpadaxis.x <= 55) && IsAnalogPressedLeft(pad, axis));
}


bool CellInputFacade::WasAnalogPressedRight(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis oldpadaxis = GetOldAxisValue(pad,axis);
	return (!(oldpadaxis.x >= 210) && IsAnalogPressedRight(pad, axis));
}


bool CellInputFacade::WasAnalogPressedDown(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis oldpadaxis = GetOldAxisValue(pad,axis);
	return (!(oldpadaxis.y >= 210) && IsAnalogPressedDown(pad, axis));
}


bool CellInputFacade::WasAnalogPressedUp(uint8_t pad, unsigned int axis)
{
	CellPadUtilAxis oldpadaxis = GetOldAxisValue(pad,axis);
	return (!(oldpadaxis.y <= 55) && IsAnalogPressedUp(pad, axis));
}










