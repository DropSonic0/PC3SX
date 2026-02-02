#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "PsxCommon.h"
#include "plugins.h"

#define _PSEMU_PLUGIN_DEFS_H // Prevent re-including if it's already included via plugins.h

// Mednafen PAD definitions
enum {
	DKEY_SELECT = 0,
	DKEY_L3,
	DKEY_R3,
	DKEY_START,
	DKEY_UP,
	DKEY_RIGHT,
	DKEY_DOWN,
	DKEY_LEFT,
	DKEY_L2,
	DKEY_R2,
	DKEY_L1,
	DKEY_R1,
	DKEY_TRIANGLE,
	DKEY_CIRCLE,
	DKEY_CROSS,
	DKEY_SQUARE,

	DKEY_TOTAL
};

enum {
	ANALOG_LEFT = 0,
	ANALOG_RIGHT,

	ANALOG_TOTAL
};

enum { NONE = 0, AXIS, HAT, BUTTON };

typedef struct tagKeyDef {
	uint8_t			JoyEvType;
	union {
		int16_t		d;
		int16_t		Axis;   // positive=axis+, negative=axis-, abs(Axis)-1=axis index
		uint16_t	Hat;	// 8-bit for hat number, 8-bit for direction
		uint16_t	Button; // button number
	} J;
	uint16_t		Key;
} KEYDEF;

enum { ANALOG_XP = 0, ANALOG_XM, ANALOG_YP, ANALOG_YM };

typedef struct tagPadDef {
	int8_t			DevNum;
	uint16_t		Type;
	KEYDEF			KeyDef[DKEY_TOTAL];
	KEYDEF			AnalogDef[ANALOG_TOTAL][4];
} PADDEF;

typedef struct tagConfig {
	uint8_t			Threaded;
	PADDEF			PadDef[2];
} CONFIG;

typedef struct tagPadState {
	uint8_t				PadMode;
	uint8_t				PadID;
	volatile uint16_t	KeyStatus;
	volatile uint16_t	JoyKeyStatus;
	volatile uint8_t	AnalogStatus[ANALOG_TOTAL][2]; // 0-255 where 127 is center position
	volatile uint8_t	AnalogKeyStatus[ANALOG_TOTAL][4];
} PADSTATE;

typedef struct tagGlobalData {
	CONFIG				cfg;

	uint8_t				Opened;
	void				*Disp; //Unused now

	PADSTATE			PadState[2];
	volatile long		KeyLeftOver;
} GLOBALDATA;

GLOBALDATA		g;

enum {
	CMD_READ_DATA_AND_VIBRATE = 0x42,
	CMD_CONFIG_MODE = 0x43,
	CMD_SET_MODE_AND_LOCK = 0x44,
	CMD_QUERY_MODEL_AND_MODE = 0x45,
	CMD_QUERY_ACT = 0x46, // ??
	CMD_QUERY_COMB = 0x47, // ??
	CMD_QUERY_MODE = 0x4C, // QUERY_MODE ??
	CMD_VIBRATION_TOGGLE = 0x4D,
};

extern long SysPAD_readPort1(PadDataS *pad);
extern long SysPAD_readPort2(PadDataS *pad);

static void UpdatePadState(int pad) {
    PadDataS p;
    memset(&p, 0, sizeof(p));
    if (pad == 0) SysPAD_readPort1(&p);
    else SysPAD_readPort2(&p);

    g.cfg.PadDef[pad].Type = p.controllerType;
    g.PadState[pad].KeyStatus = p.buttonStatus;
    g.PadState[pad].JoyKeyStatus = 0xFFFF;
    g.PadState[pad].AnalogStatus[ANALOG_RIGHT][0] = p.rightJoyX;
    g.PadState[pad].AnalogStatus[ANALOG_RIGHT][1] = p.rightJoyY;
    g.PadState[pad].AnalogStatus[ANALOG_LEFT][0] = p.leftJoyX;
    g.PadState[pad].AnalogStatus[ANALOG_LEFT][1] = p.leftJoyY;
}

long CALLBACK PAD__open(unsigned long *Disp) {
	g.KeyLeftOver = 0;
	g.Opened = 1;

	return PSE_PAD_ERR_SUCCESS;
}

long CALLBACK PAD__close(void) {
	g.Opened = 0;

	return PSE_PAD_ERR_SUCCESS;
}

long CALLBACK PAD__init(long flags) {
    g.cfg.PadDef[0].Type = PSE_PAD_TYPE_STANDARD;
    g.cfg.PadDef[1].Type = PSE_PAD_TYPE_STANDARD;
	g.PadState[0].PadMode = 0;
	g.PadState[0].PadID = 0x41;
	g.PadState[0].KeyStatus = 0xffff;
	g.PadState[0].JoyKeyStatus = 0xffff;
	g.PadState[1].PadMode = 0;
	g.PadState[1].PadID = 0x41;
	g.PadState[1].KeyStatus = 0xffff;
	g.PadState[1].JoyKeyStatus = 0xffff;

	return PSE_PAD_ERR_SUCCESS;
}

long CALLBACK PAD__shutdown(void) {
	PAD__close();
	return PSE_PAD_ERR_SUCCESS;
}

long CALLBACK PAD__query(void) {
	return PSE_PAD_USE_PORT1 | PSE_PAD_USE_PORT2;
}

static uint8_t stdpar[2][8] = {
	{0xFF, 0x5A, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80},
	{0xFF, 0x5A, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80}
};

static uint8_t unk46[2][8] = {
	{0xFF, 0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A},
	{0xFF, 0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A}
};

static uint8_t unk47[2][8] = {
	{0xFF, 0x5A, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00},
	{0xFF, 0x5A, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00}
};

static uint8_t unk4c[2][8] = {
	{0xFF, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0xFF, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

static uint8_t unk4d[2][8] = {
	{0xFF, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0xFF, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

static uint8_t stdcfg[2][8]   = {
	{0xFF, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0xFF, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

static uint8_t stdmode[2][8]  = {
	{0xFF, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0xFF, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

static uint8_t stdmodel[2][8] = {
	{0xFF,
	 0x5A,
	 0x01, // 03 - dualshock2, 01 - dualshock
	 0x02, // number of modes
	 0x01, // current mode: 01 - analog, 00 - digital
	 0x02,
	 0x01,
	 0x00},
	{0xFF,
	 0x5A,
	 0x01, // 03 - dualshock2, 01 - dualshock
	 0x02, // number of modes
	 0x01, // current mode: 01 - analog, 00 - digital
	 0x02,
	 0x01,
	 0x00}
};

static uint8_t CurPad = 0;
static uint8_t CurByte[2] = {0, 0};
static uint8_t CurCmd[2] = {0, 0};
static uint8_t CmdLen[2] = {0, 0};

unsigned char CALLBACK PAD__startPoll(int pad) {
	CurPad = pad - 1;
	CurByte[CurPad] = 0;

    UpdatePadState(CurPad);

	return 0xFF;
}

unsigned char CALLBACK PAD__poll(unsigned char value) {
	static uint8_t		*buf[2] = {NULL, NULL};
	uint16_t			n;

	if (CurByte[CurPad] == 0) {
		CurByte[CurPad]++;

		// Don't enable Analog/Vibration for a standard pad
		if (g.cfg.PadDef[CurPad].Type != PSE_PAD_TYPE_ANALOGPAD) {
			CurCmd[CurPad] = CMD_READ_DATA_AND_VIBRATE;
		} else {
			CurCmd[CurPad] = value;
		}

		switch (CurCmd[CurPad]) {
			case CMD_CONFIG_MODE:
				CmdLen[CurPad] = 8;
				buf[CurPad] = stdcfg[CurPad];
				if (stdcfg[CurPad][3] == 0xFF) return 0xF3;
				else return g.PadState[CurPad].PadID;

			case CMD_SET_MODE_AND_LOCK:
				CmdLen[CurPad] = 8;
				buf[CurPad] = stdmode[CurPad];
				return 0xF3;

			case CMD_QUERY_MODEL_AND_MODE:
				CmdLen[CurPad] = 8;
				buf[CurPad] = stdmodel[CurPad];
				buf[CurPad][4] = g.PadState[CurPad].PadMode;
				return 0xF3;

			case CMD_QUERY_ACT:
				CmdLen[CurPad] = 8;
				buf[CurPad] = unk46[CurPad];
				return 0xF3;

			case CMD_QUERY_COMB:
				CmdLen[CurPad] = 8;
				buf[CurPad] = unk47[CurPad];
				return 0xF3;

			case CMD_QUERY_MODE:
				CmdLen[CurPad] = 8;
				buf[CurPad] = unk4c[CurPad];
				return 0xF3;

			case CMD_VIBRATION_TOGGLE:
				CmdLen[CurPad] = 8;
				buf[CurPad] = unk4d[CurPad];
				return 0xF3;

			case CMD_READ_DATA_AND_VIBRATE:
			default:
				n = g.PadState[CurPad].KeyStatus;
				n &= g.PadState[CurPad].JoyKeyStatus;

				stdpar[CurPad][2] = n & 0xFF;
				stdpar[CurPad][3] = n >> 8;

				if (g.PadState[CurPad].PadMode == 1) {
					CmdLen[CurPad] = 8;

					stdpar[CurPad][4] = g.PadState[CurPad].AnalogStatus[ANALOG_RIGHT][0];
					stdpar[CurPad][5] = g.PadState[CurPad].AnalogStatus[ANALOG_RIGHT][1];
					stdpar[CurPad][6] = g.PadState[CurPad].AnalogStatus[ANALOG_LEFT][0];
					stdpar[CurPad][7] = g.PadState[CurPad].AnalogStatus[ANALOG_LEFT][1];
				} else {
					CmdLen[CurPad] = 4;
				}

				buf[CurPad] = stdpar[CurPad];
				return g.PadState[CurPad].PadID;
		}
	}

	switch (CurCmd[CurPad]) {
		case CMD_CONFIG_MODE:
			if (CurByte[CurPad] == 2) {
				switch (value) {
					case 0:
						buf[CurPad][2] = 0;
						buf[CurPad][3] = 0;
						break;

					case 1:
						buf[CurPad][2] = 0xFF;
						buf[CurPad][3] = 0xFF;
						break;
			}
			}
			break;

		case CMD_SET_MODE_AND_LOCK:
			if (CurByte[CurPad] == 2) {
				g.PadState[CurPad].PadMode = value;
				g.PadState[CurPad].PadID = value ? 0x73 : 0x41;
			}
			break;

		case CMD_QUERY_ACT:
			if (CurByte[CurPad] == 2) {
				switch (value) {
					case 0: // default
						buf[CurPad][5] = 0x02;
						buf[CurPad][6] = 0x00;
						buf[CurPad][7] = 0x0A;
						break;

					case 1: // Param std conf change
						buf[CurPad][5] = 0x01;
						buf[CurPad][6] = 0x01;
						buf[CurPad][7] = 0x14;
						break;
				}
			}
			break;

		case CMD_QUERY_MODE:
			if (CurByte[CurPad] == 2) {
				switch (value) {
					case 0: // mode 0 - digital mode
						buf[CurPad][5] = PSE_PAD_TYPE_STANDARD;
						break;

					case 1: // mode 1 - analog mode
						buf[CurPad][5] = PSE_PAD_TYPE_ANALOGPAD;
						break;
				}
			}
			break;
	}

	if (CurByte[CurPad] >= CmdLen[CurPad]) return 0;
	return buf[CurPad][CurByte[CurPad]++];
}

long CALLBACK PAD__readPort(int num, PadDataS *pad) {
    UpdatePadState(num);
	pad->buttonStatus = (g.PadState[num].KeyStatus & g.PadState[num].JoyKeyStatus);

	// ePSXe different from pcsx, swap bytes
	pad->buttonStatus = (pad->buttonStatus >> 8) | (pad->buttonStatus << 8);

	switch (g.PadState[num].PadMode) {
		case 1: // Analog Mode
			pad->controllerType = PSE_PAD_TYPE_ANALOGPAD;
			pad->rightJoyX = g.PadState[num].AnalogStatus[ANALOG_RIGHT][0];
			pad->rightJoyY = g.PadState[num].AnalogStatus[ANALOG_RIGHT][1];
			pad->leftJoyX = g.PadState[num].AnalogStatus[ANALOG_LEFT][0];
			pad->leftJoyY = g.PadState[num].AnalogStatus[ANALOG_LEFT][1];
			break;

		case PSE_PAD_TYPE_STANDARD: // Standard Pad SCPH-1080, SCPH-1150
		default:
			pad->controllerType = PSE_PAD_TYPE_STANDARD;
			break;
	}

	return PSE_PAD_ERR_SUCCESS;
}

long CALLBACK PAD__readPort1(PadDataS *pad) {
	return PAD__readPort(0, pad);
}

long CALLBACK PAD__readPort2(PadDataS *pad) {
	return PAD__readPort(1, pad);
}

long CALLBACK PAD__keypressed(void) {
	long s;

	s = g.KeyLeftOver;
	g.KeyLeftOver = 0;

	return s;
}

long CALLBACK PAD__configure(void) {
	return PSE_PAD_ERR_SUCCESS;
}

void CALLBACK PAD__about(void) {
}

long CALLBACK PAD__test(void) {
	return PSE_PAD_ERR_SUCCESS;
}

void CALLBACK PAD__setSensitive(int pad) {
}

void CALLBACK PAD__registerVibration(void (CALLBACK *callback)(unsigned long, unsigned long)) {
}

void CALLBACK PAD__registerCursor(void (CALLBACK *callback)(int, int, int)) {
}
