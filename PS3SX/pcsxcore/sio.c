/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
* SIO functions.
*/

#include "sio.h"
#include <sys/stat.h>

// Status Flags
#define TX_RDY		0x0001
#define RX_RDY		0x0002
#define TX_EMPTY	0x0004
#define PARITY_ERR	0x0008
#define RX_OVERRUN	0x0010
#define FRAMING_ERR	0x0020
#define SYNC_DETECT	0x0040
#define DSR			0x0080
#define CTS			0x0100
#define IRQ			0x0200

// Control Flags
#define TX_PERM		0x0001
#define DTR			0x0002
#define RX_PERM		0x0004
#define BREAK		0x0008
#define RESET_ERR	0x0010
#define RTS			0x0020
#define SIO_RESET	0x0040

// *** FOR WORKS ON PADS AND MEMORY CARDS *****

void LoadDongle( char *str );
void SaveDongle( char *str );

#define BUFFER_SIZE 0x1010

static unsigned char buf[ BUFFER_SIZE ];

unsigned char cardh[4] = { 0x00, 0x00, 0x5a, 0x5d };

// Transfer Ready and the Buffer is Empty
static unsigned short StatReg = TX_RDY | TX_EMPTY;
static unsigned short ModeReg;
static unsigned short CtrlReg;
static unsigned short BaudReg;

static unsigned int bufcount;
static unsigned int parp;
static unsigned int mcdst, rdwr;
static unsigned char adrH, adrL;
static unsigned int padst;
static unsigned int gsdonglest;

char Mcd1Data[MCD_SIZE], Mcd2Data[MCD_SIZE];

#define DONGLE_SIZE 0x40 * 0x1000

unsigned int DongleBank;
unsigned char DongleData[ DONGLE_SIZE ];
static int DongleInit;

// PS3SX Adaptation: Use bit 0x80 and intCycle[7]/[8]
#define SIO_INT(eCycle) { \
	if (!Config.Sio) { \
		psxRegs.interrupt |= 0x80; \
		psxRegs.intCycle[7+1] = eCycle; \
		psxRegs.intCycle[7] = psxRegs.cycle; \
	} \
}

// clk cycle byte
// 4us * 8bits = (PSXCLK / 1000000) * 32; (linuzappz)
// TODO: add SioModePrescaler
#define SIO_CYCLES (BaudReg * 8)

unsigned char reverse_8( unsigned char bits )
{
	unsigned char tmp;
	int lcv;

	tmp = 0;
	for( lcv = 0; lcv < 8; lcv++ )
	{
		tmp >>= 1;
		tmp |= (bits & 0x80);

		bits <<= 1;
	}

	return tmp;
}

// remove the leading and trailing spaces in a string
void trim(char *str) {
	int pos = 0;
	char *dest = str;

	// skip leading blanks
	while (str[pos] <= ' ' && str[pos] > 0)
		pos++;

	while (str[pos]) {
		*(dest++) = str[pos];
		pos++;
	}

	*(dest--) = '\0'; // store the null

	// remove trailing blanks
	while (dest >= str && *dest <= ' ' && *dest > 0)
		*(dest--) = '\0';
}

void sioWrite8(unsigned char value) {
#ifdef PAD_LOG
	PAD_LOG("sio write8 %x\n", value);
#endif
	switch (padst) {
		case 1: SIO_INT(SIO_CYCLES);
			if ((value & 0x40) == 0x40) {
				padst = 2; parp = 1;
				if (!Config.UseNet) {
					switch (CtrlReg & 0x2002) {
						case 0x0002:
							buf[parp] = PAD1_poll(value);
							break;
						case 0x2002:
							buf[parp] = PAD2_poll(value);
							break;
					}
				}

				if (!(buf[parp] & 0x0f)) {
					bufcount = 2 + 32;
				} else {
					bufcount = 2 + (buf[parp] & 0x0f) * 2;
				}

				// Digital / Dual Shock Controller
				if (buf[parp] == 0x41) {
					switch (value) {
						// enter config mode
						case 0x43:
							buf[1] = 0x43;
							break;

						// get status
						case 0x45:
							buf[1] = 0xf3;
							break;
					}
				}

				// NegCon - Wipeout 3
				if( buf[parp] == 0x23 ) {
					switch (value) {
						// enter config mode
						case 0x43:
							buf[1] = 0x79;
							break;

						// get status
						case 0x45:
							buf[1] = 0xf3;
							break;
					}
				}
			}
			else padst = 0;
			return;
		case 2:
			parp++;
			if (!Config.UseNet) {
				switch (CtrlReg & 0x2002) {
					case 0x0002: buf[parp] = PAD1_poll(value); break;
					case 0x2002: buf[parp] = PAD2_poll(value); break;
				}
			}

			if (parp == bufcount) { padst = 0; return; }
			SIO_INT(SIO_CYCLES);
			return;
	}

	switch (mcdst) {
		case 1:
			SIO_INT(SIO_CYCLES);
			if (rdwr) { parp++; return; }
			parp = 1;
			switch (value) {
				case 0x52: rdwr = 1; break;
				case 0x57: rdwr = 2; break;
				default: mcdst = 0;
			}
			return;
		case 2: // address H
			SIO_INT(SIO_CYCLES);
			adrH = value;
			*buf = 0;
			parp = 0;
			bufcount = 1;
			mcdst = 3;
			return;
		case 3: // address L
			SIO_INT(SIO_CYCLES);
			adrL = value;
			*buf = adrH;
			parp = 0;
			bufcount = 1;
			mcdst = 4;
			return;
		case 4:
			SIO_INT(SIO_CYCLES);
			parp = 0;
			switch (rdwr) {
				case 1: // read
					buf[0] = 0x5c;
					buf[1] = 0x5d;
					buf[2] = adrH;
					buf[3] = adrL;
					switch (CtrlReg & 0x2002) {
						case 0x0002:
							memcpy(&buf[4], Mcd1Data + (adrL | (adrH << 8)) * 128, 128);
							break;
						case 0x2002:
							memcpy(&buf[4], Mcd2Data + (adrL | (adrH << 8)) * 128, 128);
							break;
					}
					{
					char xor = 0;
					int i;
					for (i = 2; i < 128 + 4; i++)
						xor ^= buf[i];
					buf[132] = xor;
					}
					buf[133] = 0x47;
					bufcount = 133;
					break;
				case 2: // write
					buf[0] = adrL;
					buf[1] = value;
					buf[129] = 0x5c;
					buf[130] = 0x5d;
					buf[131] = 0x47;
					bufcount = 131;
					break;
			}
			mcdst = 5;
			return;
		case 5:
			parp++;
			if (rdwr == 2) {
				if (parp < 128) buf[parp + 1] = value;
			}
			SIO_INT(SIO_CYCLES);
			return;
	}

	switch (gsdonglest) {
		// main command loop
		case 1:
			SIO_INT( SIO_CYCLES );
			if( value == 0xae )
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
			}
			else if( value == 0xbe )
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = reverse_8( 0xde );
			}
			else if( value == 0xef )
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = reverse_8( 0xad );
			}
			else if( value == 0x01 )
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = 0x00;
				gsdonglest = 2;
			}
			else if( value == 0x02 )
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = 0x00;
				gsdonglest = 3;
			}
			else if( value == 0x03 )
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = 0x00;
				gsdonglest = 4;
			}
			else if( value == 0x04 )
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = 0x00;
				gsdonglest = 5;
			}
			else
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = 0xff;
				gsdonglest = 0;
			}
			return;

		case 2: {
			unsigned char checksum;
			unsigned int lcv;
			SIO_INT( SIO_CYCLES );
			StatReg |= RX_RDY;
			DongleBank = buf[ 0 ];
			checksum = 0;
			for( lcv = 0; lcv < 0x1000; lcv++ )
			{
				unsigned char data;
				data = DongleData[ DongleBank * 0x1000 + lcv ];
				buf[ lcv+1 ] = reverse_8( data );
				checksum += data;
			}
			parp = 0;
			bufcount = 0x1001;
			buf[ 0x1001 ] = reverse_8( checksum );
			gsdonglest = 255;
			return;
		}

		case 3:
			SIO_INT( SIO_CYCLES );
			StatReg |= RX_RDY;
			if( parp < 0x1000+1 )
			{
				buf[ parp ] = value;
				parp++;
			}
			if( parp == 0x1001 )
			{
				unsigned char checksum;
				unsigned int lcv;
				DongleBank = buf[0];
				memcpy( DongleData + DongleBank * 0x1000, buf+1, 0x1000 );
				SaveDongle( "memcards/CDX_Dongle.bin" );
				checksum = 0;
				for( lcv = 1; lcv < 0x1001; lcv++ )
				{
					checksum += buf[ lcv ];
				}
				parp = 0;
				bufcount = 1;
				buf[1] = reverse_8( checksum );
				gsdonglest = 255;
			}
			return;

		case 4:
			SIO_INT( SIO_CYCLES );
			StatReg |= RX_RDY;
			if( parp < 8 )
			{
				buf[ parp ] = value;
				parp++;
			}
			if( parp == 8 )
			{
				parp = 8;
				bufcount = 12;
				if( buf[2] == 0x12 && buf[3] == 0x34 &&
						buf[4] == 0x56 && buf[5] == 0x78 )
				{
					buf[9] = reverse_8( 0x3e );
					buf[10] = reverse_8( 0xa0 );
					buf[11] = reverse_8( 0x40 );
					buf[12] = reverse_8( 0x29 );
				}
				else if( buf[2] == 0x1f && buf[3] == 0xe3 &&
								 buf[4] == 0x45 && buf[5] == 0x60 )
				{
					buf[9] = reverse_8( 0xee );
					buf[10] = reverse_8( 0xdd );
					buf[11] = reverse_8( 0x71 );
					buf[12] = reverse_8( 0xa8 );
				}
				else if( buf[2] == 0x1f && buf[3] == 0xe3 &&
								 buf[4] == 0x72 && buf[5] == 0xe3 )
				{
					buf[9] = reverse_8( 0xfa );
					buf[10] = reverse_8( 0xde );
					buf[11] = reverse_8( 0x21 );
					buf[12] = reverse_8( 0x97 );
				}
				else if( buf[2] == 0x1f && buf[3] == 0xe3 &&
								 buf[4] == 0x85 && buf[5] == 0xae )
				{
					buf[9] = reverse_8( 0xee );
					buf[10] = reverse_8( 0xdd );
					buf[11] = reverse_8( 0x7d );
					buf[12] = reverse_8( 0x44 );
				}
				else if( buf[2] == 0x17 && buf[3] == 0xe3 &&
								 buf[4] == 0xb5 && buf[5] == 0x60 )
				{
					buf[9] = reverse_8( 0xee );
					buf[10] = reverse_8( 0xdd );
					buf[11] = reverse_8( 0x7e );
					buf[12] = reverse_8( 0xa8 );
				}
				gsdonglest = 255;
			}
			return;

		case 5:
			if( value == 0x00 )
			{
				SIO_INT( SIO_CYCLES );
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[ 0 ] = reverse_8( DONGLE_SIZE / 0x1000 );
				gsdonglest = 0;
			}
			return;

		case 255:
			if( value == 0x00 )
			{
				SIO_INT(1);
				StatReg |= RX_RDY;
				parp++;
				if( parp == bufcount )
				{
					gsdonglest = 0;
				}
			}
			else
			{
				StatReg |= RX_RDY;
				parp = 0;
				bufcount = parp;
				buf[0] = 0xff;
				gsdonglest = 0;
			}
			return;
	}

	switch (value) {
		case 0x01: // start pad
			StatReg |= RX_RDY;
			if (!Config.UseNet) {
				switch (CtrlReg & 0x2002) {
					case 0x0002: buf[0] = PAD1_startPoll(1); break;
					case 0x2002: buf[0] = PAD2_startPoll(2); break;
				}
			} else {
				if ((CtrlReg & 0x2002) == 0x0002) {
					int i, j;
					PAD1_startPoll(1);
					buf[0] = 0;
					buf[1] = PAD1_poll(0x42);
					if (!(buf[1] & 0x0f)) {
						bufcount = 32;
					} else {
						bufcount = (buf[1] & 0x0f) * 2;
					}
					buf[2] = PAD1_poll(0);
					i = 3;
					j = bufcount;
					while (j--) {
						buf[i++] = PAD1_poll(0);
					}
					bufcount+= 3;
					if (NET_sendPadData(buf, bufcount) == -1)
						netError();
					if (NET_recvPadData(buf, 1) == -1)
						netError();
					if (NET_recvPadData(buf + 128, 2) == -1)
						netError();
				} else {
					memcpy(buf, buf + 128, 32);
				}
			}
			bufcount = 2;
			parp = 0;
			padst = 1;
			SIO_INT(SIO_CYCLES);
			return;
		case 0x81: // start memcard
			StatReg |= RX_RDY;
			memcpy(buf, cardh, 4);
			parp = 0;
			bufcount = 3;
			mcdst = 1;
			rdwr = 0;
			SIO_INT(SIO_CYCLES);
			return;
		case 0xae: // GameShark CDX - start dongle
			StatReg |= RX_RDY;
			gsdonglest = 1;
			parp = 0;
			bufcount = parp;
			if( !DongleInit )
			{
				LoadDongle( "memcards/CDX_Dongle.bin" );
				DongleInit = 1;
			}
			SIO_INT( SIO_CYCLES );
			return;
		default: // no hardware found
			StatReg |= RX_RDY;
			return;
	}
}

void sioWriteStat16(unsigned short value) {
}

void sioWriteMode16(unsigned short value) {
	ModeReg = value;
}

void sioWriteCtrl16(unsigned short value) {
	CtrlReg = value & ~RESET_ERR;
	if (value & RESET_ERR) StatReg &= ~IRQ;
	if ((CtrlReg & SIO_RESET) || (!CtrlReg)) {
		padst = 0; mcdst = 0; parp = 0;
		StatReg = TX_RDY | TX_EMPTY;
		// PS3SX Adaptation: Clear bit 0x80
		psxRegs.interrupt &= ~0x80;
	}
}

void sioWriteBaud16(unsigned short value) {
	BaudReg = value;
}

unsigned char sioRead8() {
	unsigned char ret = 0;
	if ((StatReg & RX_RDY)) {
		ret = buf[parp];
		if (parp == bufcount) {
			StatReg &= ~RX_RDY;
			if (mcdst == 5) {
				mcdst = 0;
				if (rdwr == 2) {
					switch (CtrlReg & 0x2002) {
						case 0x0002:
							memcpy(Mcd1Data + (adrL | (adrH << 8)) * 128, &buf[1], 128);
							SaveMcd(Config.Mcd1, Mcd1Data, (adrL | (adrH << 8)) * 128, 128);
							break;
						case 0x2002:
							memcpy(Mcd2Data + (adrL | (adrH << 8)) * 128, &buf[1], 128);
							SaveMcd(Config.Mcd2, Mcd2Data, (adrL | (adrH << 8)) * 128, 128);
							break;
					}
				}
			}
			if (padst == 2) padst = 0;
			if (mcdst == 1) {
				mcdst = 2;
				StatReg|= RX_RDY;
			}
		}
	}
#ifdef PAD_LOG
	PAD_LOG("sio read8 ;ret = %x\n", ret);
#endif
	return ret;
}

unsigned short sioReadStat16() {
	return StatReg;
}

unsigned short sioReadMode16() {
	return ModeReg;
}

unsigned short sioReadCtrl16() {
	return CtrlReg;
}

unsigned short sioReadBaud16() {
	return BaudReg;
}

void netError() {
	ClosePlugins();
	SysMessage(_("Connection closed!\n"));
	CdromId[0] = '\0';
	CdromLabel[0] = '\0';
	SysRunGui();
}

void sioInterrupt() {
#ifdef PAD_LOG
	PAD_LOG("Sio Interrupt (CP0.Status = %x)\n", psxRegs.CP0.n.Status);
#endif
	StatReg |= IRQ;
	psxHu32ref(0x1070) |= SWAPu32(0x80);
	psxRegs.interrupt |= 0x80000000;
}

void LoadMcd(int mcd, char *str) {
	FILE *f;
	char *data = NULL;
	if (mcd == 1) data = Mcd1Data;
	if (mcd == 2) data = Mcd2Data;
	if (*str == 0) {
		sprintf(str, "memcards/card%d.mcd", mcd);
		SysPrintf(_("No memory card value was specified - creating a default card %s\n"), str);
	}
	f = fopen(str, "rb");
	if (f == NULL) {
		SysPrintf(_("The memory card %s doesn't exist - creating it\n"), str);
		CreateMcd(str);
		f = fopen(str, "rb");
		if (f != NULL) {
			struct stat buf;
			if (stat(str, &buf) != -1) {
				if (buf.st_size == MCD_SIZE + 64)
					fseek(f, 64, SEEK_SET);
				else if(buf.st_size == MCD_SIZE + 3904)
					fseek(f, 3904, SEEK_SET);
			}
			fread(data, 1, MCD_SIZE, f);
			fclose(f);
		}
		else
			SysMessage(_("Memory card %s failed to load!\n"), str);
	}
	else {
		struct stat buf;
		SysPrintf(_("Loading memory card %s\n"), str);
		if (stat(str, &buf) != -1) {
			if (buf.st_size == MCD_SIZE + 64)
				fseek(f, 64, SEEK_SET);
			else if(buf.st_size == MCD_SIZE + 3904)
				fseek(f, 3904, SEEK_SET);
		}
		fread(data, 1, MCD_SIZE, f);
		fclose(f);
	}
}

void LoadMcds(char *mcd1, char *mcd2) {
	LoadMcd(1, mcd1);
	LoadMcd(2, mcd2);
}

void SaveMcd(char *mcd, char *data, uint32_t adr, int size) {
	FILE *f;
	f = fopen(mcd, "r+b");
	if (f != NULL) {
		struct stat buf;
		if (stat(mcd, &buf) != -1) {
			if (buf.st_size == MCD_SIZE + 64)
				fseek(f, adr + 64, SEEK_SET);
			else if (buf.st_size == MCD_SIZE + 3904)
				fseek(f, adr + 3904, SEEK_SET);
			else
				fseek(f, adr, SEEK_SET);
		} else
			fseek(f, adr, SEEK_SET);
		fwrite(data + adr, 1, size, f);
		fclose(f);
		return;
	}
	ConvertMcd(mcd, data);
}

void CreateMcd(char *mcd) {
	FILE *f;
	struct stat buf;
	int s = MCD_SIZE;
	int i = 0, j;
	f = fopen(mcd, "wb");
	if (f == NULL)
		return;
	if (stat(mcd, &buf) != -1) {
		if ((buf.st_size == MCD_SIZE + 3904) || strstr(mcd, ".gme")) {
			s = s + 3904;
			fputc('1', f); s--; fputc('2', f); s--; fputc('3', f); s--; fputc('-', f); s--;
			fputc('4', f); s--; fputc('5', f); s--; fputc('6', f); s--; fputc('-', f); s--;
			fputc('S', f); s--; fputc('T', f); s--; fputc('D', f); s--;
			for (i = 0; i < 7; i++) { fputc(0, f); s--; }
			fputc(1, f); s--; fputc(0, f); s--; fputc(1, f); s--;
			fputc('M', f); s--; fputc('Q', f); s--;
			for (i = 0; i < 14; i++) { fputc(0xa0, f); s--; }
			fputc(0, f); s--; fputc(0xff, f);
			while (s-- > (MCD_SIZE + 1)) fputc(0, f);
		} else if ((buf.st_size == MCD_SIZE + 64) || strstr(mcd, ".mem") || strstr(mcd, ".vgs")) {
			s = s + 64;
			fputc('V', f); s--; fputc('g', f); s--; fputc('s', f); s--; fputc('M', f); s--;
			for (i = 0; i < 3; i++) { fputc(1, f); s--; fputc(0, f); s--; fputc(0, f); s--; fputc(0, f); s--; }
			fputc(0, f); s--; fputc(2, f);
			while (s-- > (MCD_SIZE + 1)) fputc(0, f);
		}
	}
	fputc('M', f); s--; fputc('C', f); s--;
	while (s-- > (MCD_SIZE - 127)) fputc(0, f);
	fputc(0xe, f); s--;
	for (i = 0; i < 15; i++) { // 15 blocks
		fputc(0xa0, f); s--; fputc(0x00, f); s--; fputc(0x00, f); s--; fputc(0x00, f); s--; fputc(0x00, f); s--;
		fputc(0x00, f); s--; fputc(0x00, f); s--; fputc(0x00, f); s--; fputc(0xff, f); s--; fputc(0xff, f); s--;
		for (j = 0; j < 117; j++) { fputc(0x00, f); s--; }
		fputc(0xa0, f); s--;
	}
	for (i = 0; i < 20; i++) {
		fputc(0xff, f); s--; fputc(0xff, f); s--; fputc(0xff, f); s--; fputc(0xff, f); s--;
		fputc(0x00, f); s--; fputc(0x00, f); s--; fputc(0x00, f); s--; fputc(0x00, f); s--;
		fputc(0xff, f); s--; fputc(0xff, f); s--;
		for (j = 0; j < 118; j++) { fputc(0x00, f); s--; }
	}
	while ((s--) >= 0) fputc(0, f);
	fclose(f);
}

void ConvertMcd(char *mcd, char *data) {
	FILE *f;
	int i = 0;
	int s = MCD_SIZE;
	if (strstr(mcd, ".gme")) {
		f = fopen(mcd, "wb");
		if (f != NULL) { fwrite(data - 3904, 1, MCD_SIZE + 3904, f); fclose(f); }
		f = fopen(mcd, "r+");
		s = s + 3904;
		fputc('1', f); s--; fputc('2', f); s--; fputc('3', f); s--; fputc('-', f); s--;
		fputc('4', f); s--; fputc('5', f); s--; fputc('6', f); s--; fputc('-', f); s--;
		fputc('S', f); s--; fputc('T', f); s--; fputc('D', f); s--;
		for (i = 0; i < 7; i++) { fputc(0, f); s--; }
		fputc(1, f); s--; fputc(0, f); s--; fputc(1, f); s--; fputc('M', f); s--; fputc('Q', f); s--;
		for(i=0;i<14;i++) { fputc(0xa0, f); s--; }
		fputc(0, f); s--; fputc(0xff, f);
		while (s-- > (MCD_SIZE+1)) fputc(0, f);
		fclose(f);
	} else if(strstr(mcd, ".mem") || strstr(mcd,".vgs")) {
		f = fopen(mcd, "wb");
		if (f != NULL) { fwrite(data-64, 1, MCD_SIZE+64, f); fclose(f); }
		f = fopen(mcd, "r+");
		s = s + 64;
		fputc('V', f); s--; fputc('g', f); s--; fputc('s', f); s--; fputc('M', f); s--;
		for(i=0;i<3;i++) { fputc(1, f); s--; fputc(0, f); s--; fputc(0, f); s--; fputc(0, f); s--; }
		fputc(0, f); s--; fputc(2, f);
		while (s-- > (MCD_SIZE+1)) fputc(0, f);
		fclose(f);
	} else {
		f = fopen(mcd, "wb");
		if (f != NULL) { fwrite(data, 1, MCD_SIZE, f); fclose(f); }
	}
}

void GetMcdBlockInfo(int mcd, int block, McdBlock *Info) {
	unsigned char *data = NULL, *ptr, *str, *sstr;
	unsigned short clut[16];
	unsigned short c;
	int i, x;
	memset(Info, 0, sizeof(McdBlock));
	if (mcd == 1) data = Mcd1Data;
	if (mcd == 2) data = Mcd2Data;
	ptr = data + block * 8192 + 2;
	Info->IconCount = *ptr & 0x3;
	ptr += 2;
	x = 0;
	str = Info->Title;
	sstr = Info->sTitle;
	for (i = 0; i < 48; i++) {
		c = *(ptr) << 8;
		c |= *(ptr + 1);
		if (!c) break;
		if (c >= 0x8281 && c <= 0x829A)
			c = (c - 0x8281) + 'a';
		else if (c >= 0x824F && c <= 0x827A)
			c = (c - 0x824F) + '0';
		else if (c == 0x8140) c = ' ';
		else if (c == 0x8143) c = ',';
		else if (c == 0x8144) c = '.';
		else if (c == 0x8146) c = ':';
		else if (c == 0x8147) c = ';';
		else if (c == 0x8148) c = '?';
		else if (c == 0x8149) c = '!';
		else if (c == 0x815E) c = '/';
		else if (c == 0x8168) c = '"';
		else if (c == 0x8169) c = '(';
		else if (c == 0x816A) c = ')';
		else if (c == 0x816D) c = '[';
		else if (c == 0x816E) c = ']';
		else if (c == 0x817C) c = '-';
		else {
			str[i] = ' ';
			sstr[x++] = *ptr++; sstr[x++] = *ptr++;
			continue;
		}
		str[i] = sstr[x++] = c;
		ptr += 2;
	}
	trim(str);
	trim(sstr);
	ptr = data + block * 8192 + 0x60; // icon palette data
	for (i = 0; i < 16; i++) {
		clut[i] = *((unsigned short *)ptr);
		ptr += 2;
	}
	for (i = 0; i < Info->IconCount; i++) {
		short *icon = &Info->Icon[i * 16 * 16];
		ptr = data + block * 8192 + 128 + 128 * i; // icon data
		for (x = 0; x < 16 * 16; x++) {
			icon[x++] = clut[*ptr & 0xf];
			icon[x] = clut[*ptr >> 4];
			ptr++;
		}
	}
	ptr = data + block * 128;
	Info->Flags = *ptr;
	ptr += 0xa;
	strncpy(Info->ID, ptr, 12);
	ptr += 12;
	strncpy(Info->Name, ptr, 16);
}

int sioFreeze(gzFile f, int Mode) {
	gzfreeze(buf, sizeof(buf));
	gzfreeze(&StatReg, sizeof(StatReg));
	gzfreeze(&ModeReg, sizeof(ModeReg));
	gzfreeze(&CtrlReg, sizeof(CtrlReg));
	gzfreeze(&BaudReg, sizeof(BaudReg));
	gzfreeze(&bufcount, sizeof(bufcount));
	gzfreeze(&parp, sizeof(parp));
	gzfreeze(&mcdst, sizeof(mcdst));
	gzfreeze(&rdwr, sizeof(rdwr));
	gzfreeze(&adrH, sizeof(adrH));
	gzfreeze(&adrL, sizeof(adrL));
	gzfreeze(&padst, sizeof(padst));
	return 0;
}

void LoadDongle( char *str )
{
	FILE *f;
	f = fopen(str, "r+b");
	if (f != NULL) {
		fread( DongleData, 1, DONGLE_SIZE, f );
		fclose( f );
	}
	else {
		u32 *ptr, lcv;
		ptr = (unsigned int *) DongleData;
		ptr[0] = (u32) 0x02015447;
		ptr[1] = (u32) 7;
		ptr[2] = (u32) 1;
		ptr[3] = (u32) 0;
		for( lcv=4; lcv<0x6c / 4; lcv++ )
		{
			ptr[ lcv ] = 0;
		}
		ptr[ lcv ] = (u32) 0x02000100;
		lcv++;
		while( lcv < 0x1000/4 )
		{
			ptr[ lcv ] = (u32) 0xffffffff;
			lcv++;
		}
	}
}

void SaveDongle( char *str )
{
	FILE *f;
	f = fopen(str, "wb");
	if (f != NULL) {
		fwrite( DongleData, 1, DONGLE_SIZE, f );
		fclose( f );
	}
}
