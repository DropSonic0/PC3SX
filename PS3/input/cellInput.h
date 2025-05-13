/*
 * cellinput.h
 *
 *  Created on: October 27, 2010
 *      Author: Halsafar
 */


#ifndef __CELL_INPUT_H__
#define __CELL_INPUT_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cell/pad.h>

#include <sys/cdefs.h>

#define MAX_PADS 4

#define CTRL_SELECT	( 0x0200	|	0x01 )
#define CTRL_L3		( 0x0200	|	0x02 )
#define CTRL_R3		( 0x0200	|	0x04 )
#define CTRL_START	( 0x0200	|	0x08 )
#define CTRL_UP		( 0x0200	|	0x10 )
#define CTRL_RIGHT	( 0x0200	|	0x20 )
#define CTRL_DOWN	( 0x0200	|	0x40 )
#define CTRL_LEFT	( 0x0200	|	0x80 )

#define CTRL_L2		( 0x0300	|	0x01 )
#define CTRL_R2		( 0x0300	|	0x02 )
#define CTRL_L1		( 0x0300	|	0x04 )
#define CTRL_R1		( 0x0300	|	0x08 )
#define CTRL_TRIANGLE	( 0x0300	|	0x10 )
#define CTRL_CIRCLE	( 0x0300	|	0x20 )
#define CTRL_CROSS	( 0x0300	|	0x40 )
#define CTRL_SQUARE	( 0x0300	|	0x80 )

#define CTRL_RSTICK	( 0x0400	|	0xFF )
#define CTRL_LSTICK	( 0x0600	|	0xFF )

#define GET_SHIFT_BIT( x ) (( x >> 8 ) & 0xFF )
#define GET_TARGET_BIT( x ) ( x & 0xFF )

typedef struct
{
	uint8_t x;
	uint8_t y;
} CellPadUtilAxis;


class CellInputFacade
{
public:
	CellInputFacade();
	~CellInputFacade();

	int32_t Init( void );
	int32_t Deinit( void );

	int32_t UpdateDevice(uint8_t i);
	uint32_t NumberPadsConnected();

	static inline unsigned int cellPadUtilGetData( CellPadData* pData, unsigned int button)
	{
		return pData->button[GET_SHIFT_BIT(button)];
	}

	// buttons
	bool WasButtonPressed(uint8_t pad, unsigned int button);
	bool WasButtonReleased(uint8_t pad, unsigned int button);
	bool IsButtonPressed(uint8_t pad, unsigned int button);
	bool IsButtonReleased(uint8_t pad, unsigned int button);
	bool WasButtonHeld(uint8_t pad, unsigned int button);

	// axis
	CellPadUtilAxis GetNewAxisValue(uint8_t pad, unsigned int axis);
	CellPadUtilAxis GetOldAxisValue(uint8_t pad, unsigned int axis);

	bool IsAnalogPressedLeft(uint8_t pad, unsigned int axis);
	bool IsAnalogPressedRight(uint8_t pad, unsigned int axis);
	bool IsAnalogPressedDown(uint8_t pad, unsigned int axis);
	bool IsAnalogPressedUp(uint8_t pad, unsigned int axis);

	bool WasAnalogPressedLeft(uint8_t pad, unsigned int axis);
	bool WasAnalogPressedRight(uint8_t pad, unsigned int axis);
	bool WasAnalogPressedDown(uint8_t pad, unsigned int axis);
	bool WasAnalogPressedUp(uint8_t pad, unsigned int axis);

	// pad data
	CellPadData* GetNewPadData(uint8_t pad);
	CellPadData* GetOldPadData(uint8_t pad);

private:
	CellPadData _oldPadData[MAX_PADS];
	CellPadData _newPadData[MAX_PADS];

#ifdef PS3_SDK_3_41
	CellPadInfo2 PadInfo;
#else
	CellPadInfo PadInfo;
#endif
};
extern CellInputFacade* PS3input;

#endif /* __CELL_INPUT_H__ */
