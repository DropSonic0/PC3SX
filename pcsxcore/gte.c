/***************************************************************************
 *   PCSX-Revolution - PlayStation Emulator for Nintendo Wii               *
 *   Copyright (C) 2009-2010  PCSX-Revolution Dev Team                     *
 *   <http://code.google.com/p/pcsx-revolution/>                           *
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
* GTE functions.
*/

#include "gte.h"
#include "psxmem.h"

#define VX(n) (n < 3 ? psxRegs.CP2D.p[n << 1].sw.l : psxRegs.CP2D.p[9].sw.l)
#define VY(n) (n < 3 ? psxRegs.CP2D.p[n << 1].sw.h : psxRegs.CP2D.p[10].sw.l)
#define VZ(n) (n < 3 ? psxRegs.CP2D.p[(n << 1) + 1].sw.l : psxRegs.CP2D.p[11].sw.l)
#define MX11(n) (n < 3 ? psxRegs.CP2C.p[(n << 3)].sw.l : 0)
#define MX12(n) (n < 3 ? psxRegs.CP2C.p[(n << 3)].sw.h : 0)
#define MX13(n) (n < 3 ? psxRegs.CP2C.p[(n << 3) + 1].sw.l : 0)
#define MX21(n) (n < 3 ? psxRegs.CP2C.p[(n << 3) + 1].sw.h : 0)
#define MX22(n) (n < 3 ? psxRegs.CP2C.p[(n << 3) + 2].sw.l : 0)
#define MX23(n) (n < 3 ? psxRegs.CP2C.p[(n << 3) + 2].sw.h : 0)
#define MX31(n) (n < 3 ? psxRegs.CP2C.p[(n << 3) + 3].sw.l : 0)
#define MX32(n) (n < 3 ? psxRegs.CP2C.p[(n << 3) + 3].sw.h : 0)
#define MX33(n) (n < 3 ? psxRegs.CP2C.p[(n << 3) + 4].sw.l : 0)
#define CV1(n) (n < 3 ? (s32)psxRegs.CP2C.r[(n << 3) + 5] : 0)
#define CV2(n) (n < 3 ? (s32)psxRegs.CP2C.r[(n << 3) + 6] : 0)
#define CV3(n) (n < 3 ? (s32)psxRegs.CP2C.r[(n << 3) + 7] : 0)

#define fSX(n) ((psxRegs.CP2D.p)[((n) + 12)].sw.l)
#define fSY(n) ((psxRegs.CP2D.p)[((n) + 12)].sw.h)
#define fSZ(n) ((psxRegs.CP2D.p)[((n) + 17)].w.l) /* (n == 0) => SZ1; */

#ifdef GTE_DUMP
#define G_OP(name,delay) fprintf(gteLog, "* : %08X : %02d : %s\n", psxRegs.code, delay, name);
#define G_SD(reg)  fprintf(gteLog, "+D%02d : %08X\n", reg, psxRegs.CP2D.r[reg]);
#define G_SC(reg)  fprintf(gteLog, "+C%02d : %08X\n", reg, psxRegs.CP2C.r[reg]);
#define G_GD(reg)  fprintf(gteLog, "-D%02d : %08X\n", reg, psxRegs.CP2D.r[reg]);
#define G_GC(reg)  fprintf(gteLog, "-C%02d : %08X\n", reg, psxRegs.CP2C.r[reg]);
#else
#define G_OP(name,delay)
#define G_SD(reg)
#define G_SC(reg)
#define G_GD(reg)
#define G_GC(reg)
#endif

#define SUM_FLAG if(gteFLAG & 0x7F87E000) gteFLAG |= 0x80000000;

#if defined(__BIGENDIAN__)
#define SEL16(n) ((n)^1)
#define SEL8(n) ((n)^3)
#else
#define SEL16(n) (n)
#define SEL8(n) (n)
#endif

#define gteVXY0 (psxRegs.CP2D.r[0])
#define gteVX0  (psxRegs.CP2D.p[0].sw.l)
#define gteVY0  (psxRegs.CP2D.p[0].sw.h)
#define gteVZ0  (psxRegs.CP2D.p[1].sw.l)
#define gteVXY1 (psxRegs.CP2D.r[2])
#define gteVX1  (psxRegs.CP2D.p[2].sw.l)
#define gteVY1  (psxRegs.CP2D.p[2].sw.h)
#define gteVZ1  (psxRegs.CP2D.p[3].sw.l)
#define gteVXY2 (psxRegs.CP2D.r[4])
#define gteVX2  (psxRegs.CP2D.p[4].sw.l)
#define gteVY2  (psxRegs.CP2D.p[4].sw.h)
#define gteVZ2  (psxRegs.CP2D.p[5].sw.l)
#define gteRGB  (psxRegs.CP2D.r[6])
#define gteR    (psxRegs.CP2D.p[6].b.l)
#define gteG    (psxRegs.CP2D.p[6].b.h)
#define gteB    (psxRegs.CP2D.p[6].b.h2)
#define gteCODE (psxRegs.CP2D.p[6].b.h3)
#define gteOTZ  (psxRegs.CP2D.p[7].w.l)
#define gteIR0  (psxRegs.CP2D.p[8].sw.l)
#define gteIR1  (psxRegs.CP2D.p[9].sw.l)
#define gteIR2  (psxRegs.CP2D.p[10].sw.l)
#define gteIR3  (psxRegs.CP2D.p[11].sw.l)
#define gteSXY0 (psxRegs.CP2D.r[12])
#define gteSX0  (psxRegs.CP2D.p[12].sw.l)
#define gteSY0  (psxRegs.CP2D.p[12].sw.h)
#define gteSXY1 (psxRegs.CP2D.r[13])
#define gteSX1  (psxRegs.CP2D.p[13].sw.l)
#define gteSY1  (psxRegs.CP2D.p[13].sw.h)
#define gteSXY2 (psxRegs.CP2D.r[14])
#define gteSX2  (psxRegs.CP2D.p[14].sw.l)
#define gteSY2  (psxRegs.CP2D.p[14].sw.h)
#define gteSXYP (psxRegs.CP2D.r[15])
#define gteSXP  (psxRegs.CP2D.p[15].sw.l)
#define gteSYP  (psxRegs.CP2D.p[15].sw.h)
#define gteSZ0  (psxRegs.CP2D.p[16].w.l)
#define gteSZ1  (psxRegs.CP2D.p[17].w.l)
#define gteSZ2  (psxRegs.CP2D.p[18].w.l)
#define gteSZ3  (psxRegs.CP2D.p[19].w.l)
#define gteRGB0  (psxRegs.CP2D.r[20])
#define gteR0    (psxRegs.CP2D.p[20].b.l)
#define gteG0    (psxRegs.CP2D.p[20].b.h)
#define gteB0    (psxRegs.CP2D.p[20].b.h2)
#define gteCODE0 (psxRegs.CP2D.p[20].b.h3)
#define gteRGB1  (psxRegs.CP2D.r[21])
#define gteR1    (psxRegs.CP2D.p[21].b.l)
#define gteG1    (psxRegs.CP2D.p[21].b.h)
#define gteB1    (psxRegs.CP2D.p[21].b.h2)
#define gteCODE1 (psxRegs.CP2D.p[21].b.h3)
#define gteRGB2  (psxRegs.CP2D.r[22])
#define gteR2    (psxRegs.CP2D.p[22].b.l)
#define gteG2    (psxRegs.CP2D.p[22].b.h)
#define gteB2    (psxRegs.CP2D.p[22].b.h2)
#define gteCODE2 (psxRegs.CP2D.p[22].b.h3)
#define gteRES1  (psxRegs.CP2D.r[23])
#define gteMAC0  (((s32 *)psxRegs.CP2D.r)[24])
#define gteMAC1  (((s32 *)psxRegs.CP2D.r)[25])
#define gteMAC2  (((s32 *)psxRegs.CP2D.r)[26])
#define gteMAC3  (((s32 *)psxRegs.CP2D.r)[27])
#define gteIRGB  (psxRegs.CP2D.r[28])
#define gteORGB  (psxRegs.CP2D.r[29])
#define gteLZCS  (psxRegs.CP2D.r[30])
#define gteLZCR  (psxRegs.CP2D.r[31])

#define gteR11R12 (((s32 *)psxRegs.CP2C.r)[0])
#define gteR22R23 (((s32 *)psxRegs.CP2C.r)[2])
#define gteR11 (psxRegs.CP2C.p[0].sw.l)
#define gteR12 (psxRegs.CP2C.p[0].sw.h)
#define gteR13 (psxRegs.CP2C.p[1].sw.l)
#define gteR21 (psxRegs.CP2C.p[1].sw.h)
#define gteR22 (psxRegs.CP2C.p[2].sw.l)
#define gteR23 (psxRegs.CP2C.p[2].sw.h)
#define gteR31 (psxRegs.CP2C.p[3].sw.l)
#define gteR32 (psxRegs.CP2C.p[3].sw.h)
#define gteR33 (psxRegs.CP2C.p[4].sw.l)
#define gteTRX (((s32 *)psxRegs.CP2C.r)[5])
#define gteTRY (((s32 *)psxRegs.CP2C.r)[6])
#define gteTRZ (((s32 *)psxRegs.CP2C.r)[7])
#define gteL11 (psxRegs.CP2C.p[8].sw.l)
#define gteL12 (psxRegs.CP2C.p[8].sw.h)
#define gteL13 (psxRegs.CP2C.p[9].sw.l)
#define gteL21 (psxRegs.CP2C.p[9].sw.h)
#define gteL22 (psxRegs.CP2C.p[10].sw.l)
#define gteL23 (psxRegs.CP2C.p[10].sw.h)
#define gteL31 (psxRegs.CP2C.p[11].sw.l)
#define gteL32 (psxRegs.CP2C.p[11].sw.h)
#define gteL33 (psxRegs.CP2C.p[12].sw.l)
#define gteRBK (((s32 *)psxRegs.CP2C.r)[13])
#define gteGBK (((s32 *)psxRegs.CP2C.r)[14])
#define gteBBK (((s32 *)psxRegs.CP2C.r)[15])
#define gteLR1 (psxRegs.CP2C.p[16].sw.l)
#define gteLR2 (psxRegs.CP2C.p[16].sw.h)
#define gteLR3 (psxRegs.CP2C.p[17].sw.l)
#define gteLG1 (psxRegs.CP2C.p[17].sw.h)
#define gteLG2 (psxRegs.CP2C.p[18].sw.l)
#define gteLG3 (psxRegs.CP2C.p[18].sw.h)
#define gteLB1 (psxRegs.CP2C.p[19].sw.l)
#define gteLB2 (psxRegs.CP2C.p[19].sw.h)
#define gteLB3 (psxRegs.CP2C.p[20].sw.l)
#define gteRFC (((s32 *)psxRegs.CP2C.r)[21])
#define gteGFC (((s32 *)psxRegs.CP2C.r)[22])
#define gteBFC (((s32 *)psxRegs.CP2C.r)[23])
#define gteOFX (((s32 *)psxRegs.CP2C.r)[24])
#define gteOFY (((s32 *)psxRegs.CP2C.r)[25])
#define gteH   (psxRegs.CP2C.p[26].sw.l)
#define gteDQA (psxRegs.CP2C.p[27].sw.l)
#define gteDQB (((s32 *)psxRegs.CP2C.r)[28])
#define gteZSF3 (psxRegs.CP2C.p[29].sw.l)
#define gteZSF4 (psxRegs.CP2C.p[30].sw.l)
#define gteFLAG (psxRegs.CP2C.r[31])

#define gteSZx     ((u16*)psxRegs.CP2D.r)[SEL16(16*2)]

#define gteC       gteCODE
#define gteC0      gteCODE0
#define gteC1      gteCODE1
#define gteC2      gteCODE2

#define GTE_OP(op) ((op >> 20) & 31)
#define GTE_SF(op) ((op >> 19) & 1)
#define GTE_MX(op) ((op >> 17) & 3)
#define GTE_V(op) ((op >> 15) & 3)
#define GTE_CV(op) ((op >> 13) & 3)
#define GTE_CD(op) ((op >> 11) & 3) /* not used */
#define GTE_LM(op) ((op >> 10) & 1)
#define GTE_CT(op) ((op >> 6) & 15) /* not used */
#define GTE_FUNCT(op) (op & 63)

#define gteop (psxRegs.code & 0x1ffffff)

static inline s64 BOUNDS(s64 n_value, s64 n_max, int n_maxflag, s64 n_min, int n_minflag) {
	if (n_value > n_max) {
		gteFLAG |= n_maxflag;
	} else if (n_value < n_min) {
		gteFLAG |= n_minflag;
	}
	return n_value;
}

static inline s32 LIM(s32 value, s32 max, s32 min, u32 flag) {
	s32 ret = value;
	if (value > max) {
		gteFLAG |= flag;
		ret = max;
	} else if (value < min) {
		gteFLAG |= flag;
		ret = min;
	}
	return ret;
}

#define A1(a) BOUNDS((a), 0x7fffffff, (1 << 30), -(s64)0x80000000, (1 << 31) | (1 << 27))
#define A2(a) BOUNDS((a), 0x7fffffff, (1 << 29), -(s64)0x80000000, (1 << 31) | (1 << 26))
#define A3(a) BOUNDS((a), 0x7fffffff, (1 << 28), -(s64)0x80000000, (1 << 31) | (1 << 25))
#define limB1(a, l) LIM((a), 0x7fff, -0x8000 * !l, (1 << 31) | (1 << 24))
#define limB2(a, l) LIM((a), 0x7fff, -0x8000 * !l, (1 << 31) | (1 << 23))
#define limB3(a, l) LIM((a), 0x7fff, -0x8000 * !l, (1 << 22))
#define limC1(a) LIM((a), 0x00ff, 0x0000, (1 << 21))
#define limC2(a) LIM((a), 0x00ff, 0x0000, (1 << 20))
#define limC3(a) LIM((a), 0x00ff, 0x0000, (1 << 19))
#define limD(a) LIM((a), 0xffff, 0x0000, (1 << 31) | (1 << 18))

#define limD(a) LIM((a), 0xffff, 0x0000, (1 << 31) | (1 << 18))

static inline u32 limE(u32 result) {
	if (result > 0x1ffff) {
		gteFLAG |= (1 << 31) | (1 << 17);
		return 0x1ffff;
	}
	return result;
}

static inline float flimE(float result) {
	if (result > 0x1ffff) {
		gteFLAG |= (1 << 31) | (1 << 17);
		return 0x1ffff;
	}
	return result;
}

#define F(a) BOUNDS((a), 0x7fffffff, (1 << 31) | (1 << 16), -(s64)0x80000000, (1 << 31) | (1 << 15))
#define limG1(a) LIM((a), 0x3ff, -0x400, (1 << 31) | (1 << 14))
#define limG2(a) LIM((a), 0x3ff, -0x400, (1 << 31) | (1 << 13))
#define limH(a) LIM((a), 0x1000, 0x0000, (1 << 12))

#define limG1_ia(a) LIM((a), 0x3ffffff, -0x4000000, (1 << 31) | (1 << 14))
#define limG2_ia(a) LIM((a), 0x3ffffff, -0x4000000, (1 << 31) | (1 << 13))

#include "gte_divider.h"

static inline u32 MFC2(int reg) {
	switch (reg) {
		case 1:
		case 3:
		case 5:
		case 8:
		case 9:
		case 10:
		case 11:
			psxRegs.CP2D.r[reg] = (s32)psxRegs.CP2D.p[reg].sw.l;
			break;

		case 7:
		case 16:
		case 17:
		case 18:
		case 19:
			psxRegs.CP2D.r[reg] = (u32)psxRegs.CP2D.p[reg].w.l;
			break;

		case 15:
			psxRegs.CP2D.r[reg] = gteSXY2;
			break;

		case 28:
		case 29:
			psxRegs.CP2D.r[reg] = LIM(gteIR1 >> 7, 0x1f, 0, 0) |
									(LIM(gteIR2 >> 7, 0x1f, 0, 0) << 5) |
									(LIM(gteIR3 >> 7, 0x1f, 0, 0) << 10);
			break;
	}
	return psxRegs.CP2D.r[reg];
}

static inline void MTC2(u32 value, int reg) {
	switch (reg) {
		case 15:
			gteSXY0 = gteSXY1;
			gteSXY1 = gteSXY2;
			gteSXY2 = value;
			gteSXYP = value;
			break;

		case 28:
			gteIRGB = value;
			gteIR1 = (value & 0x1f) << 7;
			gteIR2 = (value & 0x3e0) << 2;
			gteIR3 = (value & 0x7c00) >> 3;
			break;

		case 30:
			{
#if 1
				int a;
				gteLZCS = value;

				// non-MAME code
				a = gteLZCS;
				if (a > 0) {
					int i;
					for (i = 31; (a & (1 << i)) == 0 && i >= 0; i--);
					gteLZCR = 31 - i;
				} else if (a < 0) {
					int i;
					a ^= 0xffffffff;
					for (i = 31; (a & (1 << i)) == 0 && i >= 0; i--);
					gteLZCR = 31 - i;
				} else {
					gteLZCR = 32;
				}
#else
				// MAME
				u32 lzcs = value;
				u32 lzcr = 0;

				gteLZCS = value;

				if( ( lzcs & 0x80000000 ) == 0 )
				{
					lzcs = ~lzcs;
				}
				while( ( lzcs & 0x80000000 ) != 0 )
				{
					lzcr++;
					lzcs <<= 1;
				}
				gteLZCR = lzcr;
#endif
			}
			break;

#if 1
		case 31:
			return;
#else
		case 31:
			value = psxRegs.CP2D.r[reg];
			break;
#endif

		default:
			psxRegs.CP2D.r[reg] = value;
	}
}

static inline void CTC2(u32 value, int reg) {
	switch (reg) {
		case 4:
		case 12:
		case 20:
		case 26:
		case 27:
		case 29:
		case 30:
			value = (s32)(s16)value;
			break;

		case 31:
			value = value & 0x7ffff000;
			if (value & 0x7f87e000) value |= 0x80000000;
			break;
	}

	psxRegs.CP2C.r[reg] = value;
}

void gteMFC2() {
	if (!_Rt_) return;
	psxRegs.GPR.r[_Rt_] = MFC2(_Rd_);
}

void gteCFC2() {
	if (!_Rt_) return;
	psxRegs.GPR.r[_Rt_] = psxRegs.CP2C.r[_Rd_];
}

void gteMTC2() {
	MTC2(psxRegs.GPR.r[_Rt_], _Rd_);
}

void gteCTC2() {
	CTC2(psxRegs.GPR.r[_Rt_], _Rd_);
}

#define _oB_ (psxRegs.GPR.r[_Rs_] + _Imm_)

void gteLWC2() {
	MTC2(psxMemRead32(_oB_), _Rt_);
}

void gteSWC2() {
	psxMemWrite32(_oB_, MFC2(_Rt_));
}

void gteRTPS() {
	int quotient;
    float fquotient;

#ifdef GTE_LOG
	GTE_LOG("GTE RTPS\n");
#endif
	gteFLAG = 0;

	gteMAC1 = A1((((s64)gteTRX << 12) + (gteR11 * gteVX0) + (gteR12 * gteVY0) + (gteR13 * gteVZ0)) >> 12);
	gteMAC2 = A2((((s64)gteTRY << 12) + (gteR21 * gteVX0) + (gteR22 * gteVY0) + (gteR23 * gteVZ0)) >> 12);
	gteMAC3 = A3((((s64)gteTRZ << 12) + (gteR31 * gteVX0) + (gteR32 * gteVY0) + (gteR33 * gteVZ0)) >> 12);
	gteIR1 = limB1(gteMAC1, 0);
	gteIR2 = limB2(gteMAC2, 0);
	gteIR3 = limB3(gteMAC3, 0);
	gteSZ0 = gteSZ1;
	gteSZ1 = gteSZ2;
	gteSZ2 = gteSZ3;
	gteSZ3 = limD(gteMAC3);
	quotient = limE(DIVIDE(gteH, gteSZ3));
	gteSXY0 = gteSXY1;
	gteSXY1 = gteSXY2;
	gteSX2 = limG1(F((s64)gteOFX + ((s64)gteIR1 * quotient)) >> 16);
	gteSY2 = limG2(F((s64)gteOFY + ((s64)gteIR2 * quotient)) >> 16);

    fquotient = flimE((float)(gteH << 16) / (float)gteSZ3);

	gteMAC0 = F((s64)(gteDQB + ((s64)gteDQA * quotient)) >> 12);
	gteIR0 = limH(gteMAC0);
}

void gteRTPT() {
	int quotient;
    float fquotient;
	int v;
	s32 vx, vy, vz;

#ifdef GTE_LOG
	GTE_LOG("GTE RTPT\n");
#endif
	gteFLAG = 0;

	gteSZ0 = gteSZ3;
	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		gteMAC1 = A1((((s64)gteTRX << 12) + (gteR11 * vx) + (gteR12 * vy) + (gteR13 * vz)) >> 12);
		gteMAC2 = A2((((s64)gteTRY << 12) + (gteR21 * vx) + (gteR22 * vy) + (gteR23 * vz)) >> 12);
		gteMAC3 = A3((((s64)gteTRZ << 12) + (gteR31 * vx) + (gteR32 * vy) + (gteR33 * vz)) >> 12);
		gteIR1 = limB1(gteMAC1, 0);
		gteIR2 = limB2(gteMAC2, 0);
		gteIR3 = limB3(gteMAC3, 0);
		fSZ(v) = limD(gteMAC3);
		quotient = limE(DIVIDE(gteH, fSZ(v)));
		fSX(v) = limG1(F((s64)gteOFX + ((s64)gteIR1 * quotient)) >> 16);
		fSY(v) = limG2(F((s64)gteOFY + ((s64)gteIR2 * quotient)) >> 16);

        fquotient = flimE((float)(gteH << 16) / (float)fSZ(v));
	}
	gteMAC0 = F((s64)(gteDQB + ((s64)gteDQA * quotient)) >> 12);
	gteIR0 = limH(gteMAC0);
}

void gteMVMVA() {
	int shift = 12 * GTE_SF(gteop);
	int mx = GTE_MX(gteop);
	int v = GTE_V(gteop);
	int cv = GTE_CV(gteop);
	int lm = GTE_LM(gteop);
	s32 vx = VX(v);
	s32 vy = VY(v);
	s32 vz = VZ(v);

#ifdef GTE_LOG
	GTE_LOG("GTE MVMVA\n");
#endif
	gteFLAG = 0;

	gteMAC1 = A1((((s64)CV1(cv) << 12) + (MX11(mx) * vx) + (MX12(mx) * vy) + (MX13(mx) * vz)) >> shift);
	gteMAC2 = A2((((s64)CV2(cv) << 12) + (MX21(mx) * vx) + (MX22(mx) * vy) + (MX23(mx) * vz)) >> shift);
	gteMAC3 = A3((((s64)CV3(cv) << 12) + (MX31(mx) * vx) + (MX32(mx) * vy) + (MX33(mx) * vz)) >> shift);

	gteIR1 = limB1(gteMAC1, lm);
	gteIR2 = limB2(gteMAC2, lm);
	gteIR3 = limB3(gteMAC3, lm);
}

void gteNCLIP() {
#ifdef GTE_LOG
	GTE_LOG("GTE NCLIP\n");
#endif
	gteFLAG = 0;

	gteMAC0 = F((s64)gteSX0 * (gteSY1 - gteSY2) +
				gteSX1 * (gteSY2 - gteSY0) +
				gteSX2 * (gteSY0 - gteSY1));
}

void gteAVSZ3() {
#ifdef GTE_LOG
	GTE_LOG("GTE AVSZ3\n");
#endif
	gteFLAG = 0;

	gteMAC0 = F((s64)(gteZSF3 * gteSZ1) + (gteZSF3 * gteSZ2) + (gteZSF3 * gteSZ3));
	gteOTZ = limD(gteMAC0 >> 12);
}

void gteAVSZ4() {
#ifdef GTE_LOG
	GTE_LOG("GTE AVSZ4\n");
#endif
	gteFLAG = 0;

	gteMAC0 = F((s64)(gteZSF4 * (gteSZ0 + gteSZ1 + gteSZ2 + gteSZ3)));
	gteOTZ = limD(gteMAC0 >> 12);
}

__inline double NC_OVERFLOW1(double x) {
	if (x<-2147483648.0) {gteFLAG |= 1<<29;}	
	else if (x> 2147483647.0) {gteFLAG |= 1<<26;}

	return x;
}

__inline double NC_OVERFLOW2(double x) {
	if (x<-2147483648.0) {gteFLAG |= 1<<28;}	
	else if (x> 2147483647.0) {gteFLAG |= 1<<25;}
	
	return x;
}

__inline double NC_OVERFLOW3(double x) {
	if (x<-2147483648.0) {gteFLAG |= 1<<27;}	
	else if (x> 2147483647.0) {gteFLAG |= 1<<24;}
	
	return x;
}

__inline double NC_OVERFLOW4(double x) {
	if (x<-2147483648.0) {gteFLAG |= 1<<16;}	
	else if (x> 2147483647.0) {gteFLAG |= 1<<15;}
	
	return x;
}

__inline s32 FNC_OVERFLOW1(s64 x) {
	if (x< (s64)0xffffffff80000000LL) {gteFLAG |= 1<<29;}	
	else if (x> 2147483647) {gteFLAG |= 1<<26;}

	return (s32)x;
}

__inline s32 FNC_OVERFLOW2(s64 x) {
	if (x< (s64)0xffffffff80000000LL) {gteFLAG |= 1<<28;}	
	else if (x> 2147483647) {gteFLAG |= 1<<25;}
	
	return (s32)x;
}

__inline s32 FNC_OVERFLOW3(s64 x) {
	if (x< (s64)0xffffffff80000000LL) {gteFLAG |= 1<<27;}	
	else if (x> 2147483647) {gteFLAG |= 1<<24;}
	
	return (s32)x;
}

__inline s32 FNC_OVERFLOW4(s64 x) {
	if (x< (s64)0xffffffff80000000LL) {gteFLAG |= 1<<16;}	
	else if (x> 2147483647) {gteFLAG |= 1<<15;}
	
	return (s32)x;
}

#define _LIMX(negv, posv, flagb) { \
	if (x < (negv)) { x = (negv); gteFLAG |= (1<<flagb); } else \
	if (x > (posv)) { x = (posv); gteFLAG |= (1<<flagb); } return (x); \
}

__inline double limA1S(double x) { _LIMX(-32768.0, 32767.0, 24); }
__inline double limA2S(double x) { _LIMX(-32768.0, 32767.0, 23); }
__inline double limA3S(double x) { _LIMX(-32768.0, 32767.0, 22); }
__inline double limA1U(double x) { _LIMX(0.0, 32767.0, 24); }
__inline double limA2U(double x) { _LIMX(0.0, 32767.0, 23); }
__inline double limA3U(double x) { _LIMX(0.0, 32767.0, 22); }
__inline double limC  (double x) { _LIMX(0.0, 65535.0, 18); }
__inline double limD1 (double x) { _LIMX(-1024.0, 1023.0, 14); }
__inline double limD2 (double x) { _LIMX(-1024.0, 1023.0, 13); }

__inline s32 F12limA1S(s64 x) { _LIMX(-32768<<12, 32767<<12, 24); }
__inline s32 F12limA2S(s64 x) { _LIMX(-32768<<12, 32767<<12, 23); }
__inline s32 F12limA3S(s64 x) { _LIMX(-32768<<12, 32767<<12, 22); }
__inline s32 F12limA1U(s64 x) { _LIMX(0, 32767<<12, 24); }
__inline s32 F12limA2U(s64 x) { _LIMX(0, 32767<<12, 23); }
__inline s32 F12limA3U(s64 x) { _LIMX(0, 32767<<12, 22); }

__inline s16 FlimA1S(s32 x) { _LIMX(-32768, 32767, 24); }
__inline s16 FlimA2S(s32 x) { _LIMX(-32768, 32767, 23); }
__inline s16 FlimA3S(s32 x) { _LIMX(-32768, 32767, 22); }
__inline s16 FlimA1U(s32 x) { _LIMX(0, 32767, 24); }
__inline s16 FlimA2U(s32 x) { _LIMX(0, 32767, 23); }
__inline s16 FlimA3U(s32 x) { _LIMX(0, 32767, 22); }
__inline u8  FlimB1 (s32 x) { _LIMX(0, 255, 21); }
__inline u8  FlimB2 (s32 x) { _LIMX(0, 255, 20); }
__inline u8  FlimB3 (s32 x) { _LIMX(0, 255, 19); }
__inline u16 FlimC  (s32 x) { _LIMX(0, 65535, 18); }
__inline s32 FlimD1 (s32 x) { _LIMX(-1024, 1023, 14); }
__inline s32 FlimD2 (s32 x) { _LIMX(-1024, 1023, 13); }
__inline s32 FlimE  (s32 x) { _LIMX(0, 65535, 12); }
//__inline s32 FlimE  (s32 x) { _LIMX(0, 4095, 12); }

__inline s32 FlimG1(s64 x) {
	if (x > 2147483647) { gteFLAG |= (1<<16); } else
	if (x < (s64)0xffffffff80000000LL) { gteFLAG |= (1<<15); }

	if (x >       1023) { x =  1023; gteFLAG |= (1<<14); } else
	if (x <      -1024) { x = -1024; gteFLAG |= (1<<14); } return (x);
}

__inline s32 FlimG2(s64 x) {
	if (x > 2147483647) { gteFLAG |= (1<<16); } else
	if (x < (s64)0xffffffff80000000LL) { gteFLAG |= (1<<15); }

	if (x >       1023) { x =  1023; gteFLAG |= (1<<13); } else
	if (x <      -1024) { x = -1024; gteFLAG |= (1<<13); } return (x);
}

#define MAC2IR() { \
	if (gteMAC1 < (long)(-32768)) { gteIR1=(long)(-32768); gteFLAG|=1<<24;} \
	else \
	if (gteMAC1 > (long)( 32767)) { gteIR1=(long)( 32767); gteFLAG|=1<<24;} \
	else gteIR1=(long)gteMAC1; \
	if (gteMAC2 < (long)(-32768)) { gteIR2=(long)(-32768); gteFLAG|=1<<23;} \
	else \
	if (gteMAC2 > (long)( 32767)) { gteIR2=(long)( 32767); gteFLAG|=1<<23;} \
	else gteIR2=(long)gteMAC2; \
	if (gteMAC3 < (long)(-32768)) { gteIR3=(long)(-32768); gteFLAG|=1<<22;} \
	else \
	if (gteMAC3 > (long)( 32767)) { gteIR3=(long)( 32767); gteFLAG|=1<<22;} \
	else gteIR3=(long)gteMAC3; \
}


#define MAC2IR1() {           \
	if (gteMAC1 < (long)0) { gteIR1=(long)0; gteFLAG|=1<<24;}  \
	else if (gteMAC1 > (long)(32767)) { gteIR1=(long)(32767); gteFLAG|=1<<24;} \
	else gteIR1=(long)gteMAC1;                                                         \
	if (gteMAC2 < (long)0) { gteIR2=(long)0; gteFLAG|=1<<23;}      \
	else if (gteMAC2 > (long)(32767)) { gteIR2=(long)(32767); gteFLAG|=1<<23;}    \
	else gteIR2=(long)gteMAC2;                                                            \
	if (gteMAC3 < (long)0) { gteIR3=(long)0; gteFLAG|=1<<22;}         \
	else if (gteMAC3 > (long)(32767)) { gteIR3=(long)(32767); gteFLAG|=1<<22;}       \
	else gteIR3=(long)gteMAC3; \
}

//********END OF LIMITATIONS**********************************/

#define GTE_RTPS1(vn) { \
	gteMAC1 = FNC_OVERFLOW1(((signed long)(gteR11*gteVX##vn + gteR12*gteVY##vn + gteR13*gteVZ##vn)>>12) + gteTRX); \
	gteMAC2 = FNC_OVERFLOW2(((signed long)(gteR21*gteVX##vn + gteR22*gteVY##vn + gteR23*gteVZ##vn)>>12) + gteTRY); \
	gteMAC3 = FNC_OVERFLOW3(((signed long)(gteR31*gteVX##vn + gteR32*gteVY##vn + gteR33*gteVZ##vn)>>12) + gteTRZ); \
}

/*	gteMAC1 = NC_OVERFLOW1(((signed long)(gteR11*gteVX0 + gteR12*gteVY0 + gteR13*gteVZ0)>>12) + gteTRX);
	gteMAC2 = NC_OVERFLOW2(((signed long)(gteR21*gteVX0 + gteR22*gteVY0 + gteR23*gteVZ0)>>12) + gteTRY);
	gteMAC3 = NC_OVERFLOW3(((signed long)(gteR31*gteVX0 + gteR32*gteVY0 + gteR33*gteVZ0)>>12) + gteTRZ);*/

#if 0

#define GTE_RTPS2(vn) { \
	if (gteSZ##vn == 0) { \
		DSZ = 2.0f; gteFLAG |= 1<<17; \
	} else { \
		DSZ = (double)gteH / gteSZ##vn; \
		if (DSZ > 2.0) { DSZ = 2.0f; gteFLAG |= 1<<17; } \
/*		if (DSZ > 2147483647.0) { DSZ = 2.0f; gteFLAG |= 1<<17; }*/ \
	} \
 \
/*	gteSX##vn = limG1(gteOFX/65536.0 + (limA1S(gteMAC1) * DSZ));*/ \
/*	gteSY##vn = limG2(gteOFY/65536.0 + (limA2S(gteMAC2) * DSZ));*/ \
	gteSX##vn = FlimG1(gteOFX/65536.0 + (gteIR1 * DSZ)); \
	gteSY##vn = FlimG2(gteOFY/65536.0 + (gteIR2 * DSZ)); \
}

#define GTE_RTPS3() { \
	DSZ = gteDQB/16777216.0 + (gteDQA/256.0) * DSZ; \
	gteMAC0 =      DSZ * 16777216.0; \
	gteIR0  = limE(DSZ * 4096.0f); \
printf("zero %x, %x\n", gteMAC0, gteIR0); \
}
#endif
//#if 0
#define GTE_RTPS2(vn) { \
	if (gteSZ##vn == 0) { \
		FDSZ = 2 << 16; gteFLAG |= 1<<17; \
	} else { \
		FDSZ = ((u64)gteH << 32) / ((u64)gteSZ##vn << 16); \
		if ((u64)FDSZ > (2 << 16)) { FDSZ = 2 << 16; gteFLAG |= 1<<17; } \
	} \
 \
	gteSX##vn = FlimG1((gteOFX + (((s64)((s64)gteIR1 << 16) * FDSZ) >> 16)) >> 16); \
	gteSY##vn = FlimG2((gteOFY + (((s64)((s64)gteIR2 << 16) * FDSZ) >> 16)) >> 16); \
}

#define GTE_RTPS3() { \
	FDSZ = (s64)((s64)gteDQB + (((s64)((s64)gteDQA << 8) * FDSZ) >> 8)); \
	gteMAC0 = FDSZ; \
	gteIR0  = FlimE(FDSZ >> 12); \
}
//#endif
//	gteMAC0 =      (gteDQB/16777216.0 + (gteDQA/256.0) * DSZ) * 16777216.0;
//	gteIR0  = limE((gteDQB/16777216.0 + (gteDQA/256.0) * DSZ) * 4096.0);
//	gteMAC0 =       ((gteDQB >> 24) + (gteDQA >> 8) * DSZ) * 16777216.0;
//	gteIR0  = FlimE(((gteDQB >> 24) + (gteDQA >> 8) * DSZ) * 4096.0);

#define gte_C11 gteLR1
#define gte_C12 gteLR2
#define gte_C13 gteLR3
#define gte_C21 gteLG1
#define gte_C22 gteLG2
#define gte_C23 gteLG3
#define gte_C31 gteLB1
#define gte_C32 gteLB2
#define gte_C33 gteLB3

#define _MVMVA_FUNC(_v0, _v1, _v2, mx) { \
	SSX = (_v0) * mx##11 + (_v1) * mx##12 + (_v2) * mx##13; \
	SSY = (_v0) * mx##21 + (_v1) * mx##22 + (_v2) * mx##23; \
	SSZ = (_v0) * mx##31 + (_v1) * mx##32 + (_v2) * mx##33; \
}

void gteSQR() {
	int shift = 12 * GTE_SF(gteop);
	int lm = GTE_LM(gteop);

#ifdef GTE_LOG
	GTE_LOG("GTE SQR\n");
#endif
	gteFLAG = 0;

	gteMAC1 = A1((gteIR1 * gteIR1) >> shift);
	gteMAC2 = A2((gteIR2 * gteIR2) >> shift);
	gteMAC3 = A3((gteIR3 * gteIR3) >> shift);
	gteIR1 = limB1(gteMAC1, lm);
	gteIR2 = limB2(gteMAC2, lm);
	gteIR3 = limB3(gteMAC3, lm);
}

void gteNCCS() {
#ifdef GTE_LOG
	GTE_LOG("GTE NCCS\n");
#endif
	gteFLAG = 0;

	gteMAC1 = A1((((s64)gteL11 * gteVX0) + (gteL12 * gteVY0) + (gteL13 * gteVZ0)) >> 12);
	gteMAC2 = A2((((s64)gteL21 * gteVX0) + (gteL22 * gteVY0) + (gteL23 * gteVZ0)) >> 12);
	gteMAC3 = A3((((s64)gteL31 * gteVX0) + (gteL32 * gteVY0) + (gteL33 * gteVZ0)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1((((s64)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
	gteMAC2 = A2((((s64)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
	gteMAC3 = A3((((s64)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1(((s64)gteR * gteIR1) >> 8);
	gteMAC2 = A2(((s64)gteG * gteIR2) >> 8);
	gteMAC3 = A3(((s64)gteB * gteIR3) >> 8);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteNCCT() {
	int v;
	s32 vx, vy, vz;

#ifdef GTE_LOG
	GTE_LOG("GTE NCCT\n");
#endif
	gteFLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		gteMAC1 = A1((((s64)gteL11 * vx) + (gteL12 * vy) + (gteL13 * vz)) >> 12);
		gteMAC2 = A2((((s64)gteL21 * vx) + (gteL22 * vy) + (gteL23 * vz)) >> 12);
		gteMAC3 = A3((((s64)gteL31 * vx) + (gteL32 * vy) + (gteL33 * vz)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1((((s64)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
		gteMAC2 = A2((((s64)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
		gteMAC3 = A3((((s64)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1(((s64)gteR * gteIR1) >> 8);
		gteMAC2 = A2(((s64)gteG * gteIR2) >> 8);
		gteMAC3 = A3(((s64)gteB * gteIR3) >> 8);

		gteRGB0 = gteRGB1;
		gteRGB1 = gteRGB2;
		gteCODE2 = gteCODE;
		gteR2 = limC1(gteMAC1 >> 4);
		gteG2 = limC2(gteMAC2 >> 4);
		gteB2 = limC3(gteMAC3 >> 4);
	}
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
}

void gteNCDS() {
#ifdef GTE_LOG
	GTE_LOG("GTE NCDS\n");
#endif
	gteFLAG = 0;

	gteMAC1 = A1((((s64)gteL11 * gteVX0) + (gteL12 * gteVY0) + (gteL13 * gteVZ0)) >> 12);
	gteMAC2 = A2((((s64)gteL21 * gteVX0) + (gteL22 * gteVY0) + (gteL23 * gteVZ0)) >> 12);
	gteMAC3 = A3((((s64)gteL31 * gteVX0) + (gteL32 * gteVY0) + (gteL33 * gteVZ0)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1((((s64)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
	gteMAC2 = A2((((s64)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
	gteMAC3 = A3((((s64)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1(((((s64)gteR << 4) * gteIR1) + (gteIR0 * limB1(gteRFC - ((gteR * gteIR1) >> 8), 0))) >> 12);
	gteMAC2 = A2(((((s64)gteG << 4) * gteIR2) + (gteIR0 * limB2(gteGFC - ((gteG * gteIR2) >> 8), 0))) >> 12);
	gteMAC3 = A3(((((s64)gteB << 4) * gteIR3) + (gteIR0 * limB3(gteBFC - ((gteB * gteIR3) >> 8), 0))) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteNCDT() {
	int v;
	s32 vx, vy, vz;

#ifdef GTE_LOG
	GTE_LOG("GTE NCDT\n");
#endif
	gteFLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		gteMAC1 = A1((((s64)gteL11 * vx) + (gteL12 * vy) + (gteL13 * vz)) >> 12);
		gteMAC2 = A2((((s64)gteL21 * vx) + (gteL22 * vy) + (gteL23 * vz)) >> 12);
		gteMAC3 = A3((((s64)gteL31 * vx) + (gteL32 * vy) + (gteL33 * vz)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1((((s64)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
		gteMAC2 = A2((((s64)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
		gteMAC3 = A3((((s64)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1(((((s64)gteR << 4) * gteIR1) + (gteIR0 * limB1(gteRFC - ((gteR * gteIR1) >> 8), 0))) >> 12);
		gteMAC2 = A2(((((s64)gteG << 4) * gteIR2) + (gteIR0 * limB2(gteGFC - ((gteG * gteIR2) >> 8), 0))) >> 12);
		gteMAC3 = A3(((((s64)gteB << 4) * gteIR3) + (gteIR0 * limB3(gteBFC - ((gteB * gteIR3) >> 8), 0))) >> 12);

		gteRGB0 = gteRGB1;
		gteRGB1 = gteRGB2;
		gteCODE2 = gteCODE;
		gteR2 = limC1(gteMAC1 >> 4);
		gteG2 = limC2(gteMAC2 >> 4);
		gteB2 = limC3(gteMAC3 >> 4);
	}
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
}

#define GTE_NCCS(vn) \
	gte_LL1 = F12limA1U((gteL11*gteVX##vn + gteL12*gteVY##vn + gteL13*gteVZ##vn) >> 12); \
	gte_LL2 = F12limA2U((gteL21*gteVX##vn + gteL22*gteVY##vn + gteL23*gteVZ##vn) >> 12); \
	gte_LL3 = F12limA3U((gteL31*gteVX##vn + gteL32*gteVY##vn + gteL33*gteVZ##vn) >> 12); \
	gte_RRLT= F12limA1U(gteRBK + ((gteLR1*gte_LL1 + gteLR2*gte_LL2 + gteLR3*gte_LL3) >> 12)); \
	gte_GGLT= F12limA2U(gteGBK + ((gteLG1*gte_LL1 + gteLG2*gte_LL2 + gteLG3*gte_LL3) >> 12)); \
	gte_BBLT= F12limA3U(gteBBK + ((gteLB1*gte_LL1 + gteLB2*gte_LL2 + gteLB3*gte_LL3) >> 12)); \
 \
	gteMAC1 = (long)(((s64)((u32)gteR<<12)*gte_RRLT) >> 20);\
	gteMAC2 = (long)(((s64)((u32)gteG<<12)*gte_GGLT) >> 20);\
	gteMAC3 = (long)(((s64)((u32)gteB<<12)*gte_BBLT) >> 20);

#define GTE_NCDS(vn) \
	gte_LL1 = F12limA1U((gteL11*gteVX##vn + gteL12*gteVY##vn + gteL13*gteVZ##vn) >> 12); \
	gte_LL2 = F12limA2U((gteL21*gteVX##vn + gteL22*gteVY##vn + gteL23*gteVZ##vn) >> 12); \
	gte_LL3 = F12limA3U((gteL31*gteVX##vn + gteL32*gteVY##vn + gteL33*gteVZ##vn) >> 12); \
	gte_RRLT= F12limA1U(gteRBK + ((gteLR1*gte_LL1 + gteLR2*gte_LL2 + gteLR3*gte_LL3) >> 12)); \
	gte_GGLT= F12limA2U(gteGBK + ((gteLG1*gte_LL1 + gteLG2*gte_LL2 + gteLG3*gte_LL3) >> 12)); \
	gte_BBLT= F12limA3U(gteBBK + ((gteLB1*gte_LL1 + gteLB2*gte_LL2 + gteLB3*gte_LL3) >> 12)); \
 \
	gte_RR0 = (long)(((s64)((u32)gteR<<12)*gte_RRLT) >> 12);\
	gte_GG0 = (long)(((s64)((u32)gteG<<12)*gte_GGLT) >> 12);\
	gte_BB0 = (long)(((s64)((u32)gteB<<12)*gte_BBLT) >> 12);\
	gteMAC1 = (long)((gte_RR0 + (((s64)gteIR0 * F12limA1S((s64)(gteRFC << 8) - gte_RR0)) >> 12)) >> 8);\
	gteMAC2 = (long)((gte_GG0 + (((s64)gteIR0 * F12limA2S((s64)(gteGFC << 8) - gte_GG0)) >> 12)) >> 8);\
	gteMAC3 = (long)((gte_BB0 + (((s64)gteIR0 * F12limA3S((s64)(gteBFC << 8) - gte_BB0)) >> 12)) >> 8);

#define	gteD1	(*(short *)&gteR11)
#define	gteD2	(*(short *)&gteR22)
#define	gteD3	(*(short *)&gteR33)

void gteOP() {
//	double SSX0=0,SSY0=0,SSZ0=0;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif

#ifdef GTE_LOG
	GTE_LOG("GTE_OP %lx\n", psxRegs.code & 0x1ffffff);
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("OP", 6);
		G_SD(9);
		G_SD(10);
		G_SD(11);

		G_SC(0);
		G_SC(2);
		G_SC(4);
	}
#endif
/*	gteFLAG=0;

	switch (psxRegs.code & 0x1ffffff) {
		case 0x178000C://op12
			SSX0 = EDETEC1((gteR22*(short)gteIR3 - gteR33*(short)gteIR2)/(double)4096);
			SSY0 = EDETEC2((gteR33*(short)gteIR1 - gteR11*(short)gteIR3)/(double)4096);
			SSZ0 = EDETEC3((gteR11*(short)gteIR2 - gteR22*(short)gteIR1)/(double)4096);	
			break;
		case 0x170000C:
			SSX0 = EDETEC1((gteR22*(short)gteIR3 - gteR33*(short)gteIR2));
			SSY0 = EDETEC2((gteR33*(short)gteIR1 - gteR11*(short)gteIR3));
			SSZ0 = EDETEC3((gteR11*(short)gteIR2 - gteR22*(short)gteIR1));
			break;
	}

	gteMAC1 = (long)float2int(SSX0);
	gteMAC2 = (long)float2int(SSY0);
	gteMAC3 = (long)float2int(SSZ0);
	
	MAC2IR();
	
	if (gteIR1<0) gteIR1=0;
	if (gteIR2<0) gteIR2=0;
	if (gteIR3<0) gteIR3=0;

    if (gteFLAG & 0x7f87e000) gteFLAG|=0x80000000;*/
	gteFLAG = 0;
	
/*	if (psxRegs.code  & 0x80000) {
		
		gteMAC1 = NC_OVERFLOW1((gteD2 * gteIR3 - gteD3 * gteIR2) / 4096.0f);
		gteMAC2 = NC_OVERFLOW2((gteD3 * gteIR1 - gteD1 * gteIR3) / 4096.0f);
        gteMAC3 = NC_OVERFLOW3((gteD1 * gteIR2 - gteD2 * gteIR1) / 4096.0f);
	} else {
		
		gteMAC1 = NC_OVERFLOW1(gteD2 * gteIR3 - gteD3 * gteIR2);
		gteMAC2 = NC_OVERFLOW2(gteD3 * gteIR1 - gteD1 * gteIR3);
        gteMAC3 = NC_OVERFLOW3(gteD1 * gteIR2 - gteD2 * gteIR1);
	}*/
	if (psxRegs.code  & 0x80000) {
		gteMAC1 = FNC_OVERFLOW1((gteD2 * gteIR3 - gteD3 * gteIR2) >> 12);
		gteMAC2 = FNC_OVERFLOW2((gteD3 * gteIR1 - gteD1 * gteIR3) >> 12);
        gteMAC3 = FNC_OVERFLOW3((gteD1 * gteIR2 - gteD2 * gteIR1) >> 12);
	} else {
		gteMAC1 = FNC_OVERFLOW1(gteD2 * gteIR3 - gteD3 * gteIR2);
		gteMAC2 = FNC_OVERFLOW2(gteD3 * gteIR1 - gteD1 * gteIR3);
        gteMAC3 = FNC_OVERFLOW3(gteD1 * gteIR2 - gteD2 * gteIR1);
	}

	/* NC: old
	MAC2IR1();
	*/
	MAC2IR();

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteDCPL() {
//	unsigned long C,R,G,B;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif
#ifdef GTE_LOG
	GTE_LOG("GTE_DCPL\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("DCPL", 8);
		G_SD(6);
		G_SD(8);
		G_SD(9);
		G_SD(10);
		G_SD(11);

		G_SC(21);
		G_SC(22);
		G_SC(23);
	}
#endif
/*	R = ((gteRGB)&0xff);
	G = ((gteRGB>> 8)&0xff);
	B = ((gteRGB>>16)&0xff);
	C = ((gteRGB>>24)&0xff);

	gteMAC1 = (signed long)((double)(R*gteIR1) + (double)(gteIR0*LimitAS(gteRFC-(double)(R*gteIR1),24))/4096.0);
	gteMAC2 = (signed long)((double)(G*gteIR2) + (double)(gteIR0*LimitAS(gteGFC-(double)(G*gteIR2),23))/4096.0);
	gteMAC3 = (signed long)((double)(B*gteIR3) + (double)(gteIR0*LimitAS(gteBFC-(double)(B*gteIR3),22))/4096.0);

	MAC2IR()
		
	R = (unsigned long)LimitB(gteMAC1,21); if (R>255) R=255; else if (R<0) R=0;
	G = (unsigned long)LimitB(gteMAC2,20); if (G>255) G=255; else if (G<0) G=0;
	B = (unsigned long)LimitB(gteMAC3,19); if (B>255) B=255; else if (B<0) B=0;

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteRGB2 = R|(G<<8)|(B<<16)|(C<<24);
	
	if (gteFLAG & 0x7f87e000) gteFLAG|=0x80000000;*/

/*	gteFLAG = 0;
	
	gteMAC1 = NC_OVERFLOW1((gteR * gteIR1) / 256.0f + (gteIR0 * limA1S(gteRFC - ((gteR * gteIR1) / 256.0f))) / 4096.0f);
	gteMAC2 = NC_OVERFLOW2((gteG * gteIR1) / 256.0f + (gteIR0 * limA2S(gteGFC - ((gteG * gteIR1) / 256.0f))) / 4096.0f);
	gteMAC3 = NC_OVERFLOW3((gteB * gteIR1) / 256.0f + (gteIR0 * limA3S(gteBFC - ((gteB * gteIR1) / 256.0f))) / 4096.0f);
	*/
/*	gteMAC1 = ( (signed long)(gteR)*gteIR1 + (gteIR0*(signed short)limA1S(gteRFC - ((gteR*gteIR1)>>12) )) ) >>6;
	gteMAC2 = ( (signed long)(gteG)*gteIR2 + (gteIR0*(signed short)limA2S(gteGFC - ((gteG*gteIR2)>>12) )) ) >>6;
	gteMAC3 = ( (signed long)(gteB)*gteIR3 + (gteIR0*(signed short)limA3S(gteBFC - ((gteB*gteIR3)>>12) )) ) >>6;*/

/*	gteMAC1 = ( (signed long)(gteR)*gteIR1 + (gteIR0*(signed short)limA1S(gteRFC - ((gteR*gteIR1)>>12) )) ) >>8;
	gteMAC2 = ( (signed long)(gteG)*gteIR2 + (gteIR0*(signed short)limA2S(gteGFC - ((gteG*gteIR2)>>12) )) ) >>8;
	gteMAC3 = ( (signed long)(gteB)*gteIR3 + (gteIR0*(signed short)limA3S(gteBFC - ((gteB*gteIR3)>>12) )) ) >>8;*/
	gteMAC1 = ( (signed long)(gteR)*gteIR1 + (gteIR0*(signed short)FlimA1S(gteRFC - ((gteR*gteIR1)>>12) )) ) >>8;
	gteMAC2 = ( (signed long)(gteG)*gteIR2 + (gteIR0*(signed short)FlimA2S(gteGFC - ((gteG*gteIR2)>>12) )) ) >>8;
	gteMAC3 = ( (signed long)(gteB)*gteIR3 + (gteIR0*(signed short)FlimA3S(gteBFC - ((gteB*gteIR3)>>12) )) ) >>8;

	gteFLAG=0;
	MAC2IR();
 
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
 
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteGPF() {
//	double ipx, ipy, ipz;
//	s32 ipx, ipy, ipz;

#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif
#ifdef GTE_LOG
	GTE_LOG("GTE_GPF %lx\n", psxRegs.code & 0x1ffffff);
#endif
#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("GPF", 5);
		G_SD(6);
		G_SD(8);
		G_SD(9);
		G_SD(10);
		G_SD(11);
	}
#endif
/*	gteFLAG = 0;

	ipx = (double)((short)gteIR0) * ((short)gteIR1);
	ipy = (double)((short)gteIR0) * ((short)gteIR2);
	ipz = (double)((short)gteIR0) * ((short)gteIR3);

	// same as mvmva
	if (psxRegs.code & 0x80000) {
		ipx /= 4096.0; ipy /= 4096.0; ipz /= 4096.0;
	}

	gteMAC1 = (long)ipx;
	gteMAC2 = (long)ipy;
	gteMAC3 = (long)ipz;

	gteIR1 = (long)LimitAS(ipx,24);
	gteIR2 = (long)LimitAS(ipy,23);
	gteIR3 = (long)LimitAS(ipz,22);
	
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteC2 = gteCODE;
	gteR2 = (unsigned char)LimitB(ipx,21);
	gteG2 = (unsigned char)LimitB(ipy,20);
	gteB2 = (unsigned char)LimitB(ipz,19);*/

	gteFLAG = 0;

/*	if (psxRegs.code & 0x80000) {
		gteMAC1 = NC_OVERFLOW1((gteIR0 * gteIR1) / 4096.0f);
		gteMAC2 = NC_OVERFLOW2((gteIR0 * gteIR2) / 4096.0f);
		gteMAC3 = NC_OVERFLOW3((gteIR0 * gteIR3) / 4096.0f);
	} else {
		gteMAC1 = NC_OVERFLOW1(gteIR0 * gteIR1);
		gteMAC2 = NC_OVERFLOW2(gteIR0 * gteIR2);
        gteMAC3 = NC_OVERFLOW3(gteIR0 * gteIR3);
	}*/
	if (psxRegs.code & 0x80000) {
		gteMAC1 = FNC_OVERFLOW1((gteIR0 * gteIR1) >> 12);
		gteMAC2 = FNC_OVERFLOW2((gteIR0 * gteIR2) >> 12);
		gteMAC3 = FNC_OVERFLOW3((gteIR0 * gteIR3) >> 12);
	} else {
		gteMAC1 = FNC_OVERFLOW1(gteIR0 * gteIR1);
		gteMAC2 = FNC_OVERFLOW2(gteIR0 * gteIR2);
        gteMAC3 = FNC_OVERFLOW3(gteIR0 * gteIR3);
	}
	MAC2IR();
	
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteGPL() {
 //   double IPX=0,IPY=0,IPZ=0;
//    unsigned long C,R,G,B;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif
#ifdef GTE_LOG
	GTE_LOG("GTE_GPL %lx\n", psxRegs.code & 0x1ffffff);
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("GPL", 5);
		G_SD(6);
		G_SD(8);
		G_SD(9);
		G_SD(10);
		G_SD(11);

		G_SD(25);
		G_SD(26);
		G_SD(27);
	}
#endif

/*	gteFLAG=0;
	switch(psxRegs.code & 0x1ffffff) {
		case 0x1A8003E:
			IPX = EDETEC1((double)gteMAC1 + ((double)gteIR0*(double)gteIR1)/4096.0f);
	        IPY = EDETEC2((double)gteMAC2 + ((double)gteIR0*(double)gteIR2)/4096.0f);
	        IPZ = EDETEC3((double)gteMAC3 + ((double)gteIR0*(double)gteIR3)/4096.0f);
			break;

		case 0x1A0003E:
	       IPX = EDETEC1((double)gteMAC1 + ((double)gteIR0*(double)gteIR1));
           IPY = EDETEC2((double)gteMAC2 + ((double)gteIR0*(double)gteIR2));
	       IPZ = EDETEC3((double)gteMAC3 + ((double)gteIR0*(double)gteIR3));
			break;
	}
	gteIR1  = (short)float2int(LimitAS(IPX,24));
	gteIR2  = (short)float2int(LimitAS(IPY,23));
	gteIR3  = (short)float2int(LimitAS(IPZ,22));

	gteMAC1 = (int)float2int(IPX);
	gteMAC2 = (int)float2int(IPY);
	gteMAC3 = (int)float2int(IPZ);

	C = gteRGB & 0xff000000;
	R = float2int(ALIMIT(IPX,0,255));
	G = float2int(ALIMIT(IPY,0,255));
	B = float2int(ALIMIT(IPZ,0,255));

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteRGB2 = C|R|(G<<8)|(B<<16);*/
	gteFLAG = 0;
	
/*	if (psxRegs.code & 0x80000) {
		gteMAC1 = NC_OVERFLOW1(gteMAC1 + (gteIR0 * gteIR1) / 4096.0f);
		gteMAC2 = NC_OVERFLOW2(gteMAC2 + (gteIR0 * gteIR2) / 4096.0f);
        gteMAC3 = NC_OVERFLOW3(gteMAC3 + (gteIR0 * gteIR3) / 4096.0f);
	} else {
		gteMAC1 = NC_OVERFLOW1(gteMAC1 + (gteIR0 * gteIR1));
		gteMAC2 = NC_OVERFLOW2(gteMAC2 + (gteIR0 * gteIR2));
        gteMAC3 = NC_OVERFLOW3(gteMAC3 + (gteIR0 * gteIR3));
	}*/
	if (psxRegs.code & 0x80000) {
		gteMAC1 = FNC_OVERFLOW1(gteMAC1 + ((gteIR0 * gteIR1) >> 12));
		gteMAC2 = FNC_OVERFLOW2(gteMAC2 + ((gteIR0 * gteIR2) >> 12));
        gteMAC3 = FNC_OVERFLOW3(gteMAC3 + ((gteIR0 * gteIR3) >> 12));
	} else {
		gteMAC1 = FNC_OVERFLOW1(gteMAC1 + (gteIR0 * gteIR1));
		gteMAC2 = FNC_OVERFLOW2(gteMAC2 + (gteIR0 * gteIR2));
        gteMAC3 = FNC_OVERFLOW3(gteMAC3 + (gteIR0 * gteIR3));
	}
	MAC2IR();
	
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

/*
#define GTE_DPCS() { \
	RR0 = (double)R + (gteIR0*LimitAS((double)(gteRFC - R),24))/4096.0; \
	GG0 = (double)G + (gteIR0*LimitAS((double)(gteGFC - G),23))/4096.0; \
	BB0 = (double)B + (gteIR0*LimitAS((double)(gteBFC - B),22))/4096.0; \
 \
	gteIR1 = (long)LimitAS(RR0,24); \
	gteIR2 = (long)LimitAS(GG0,23); \
	gteIR3 = (long)LimitAS(BB0,22); \
 \
	gteRGB0 = gteRGB1; \
	gteRGB1 = gteRGB2; \
	gteC2 = C; \
	gteR2 = (unsigned char)LimitB(RR0/16.0,21); \
	gteG2 = (unsigned char)LimitB(GG0/16.0,20); \
	gteB2 = (unsigned char)LimitB(BB0/16.0,19); \
 \
	gteMAC1 = (long)RR0; \
	gteMAC2 = (long)GG0; \
	gteMAC3 = (long)BB0; \
}
*/
void gteDPCS() {
//	unsigned long C,R,G,B;
//	double RR0,GG0,BB0;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif
#ifdef GTE_LOG
	GTE_LOG("GTE_DPCS\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("DPCS", 8);
		G_SD(6);
		G_SD(8);

		G_SC(21);
		G_SC(22);
		G_SC(23);
	}
#endif

/*	gteFLAG = 0;

	C = gteCODE;
	R = gteR * 16.0;
	G = gteG * 16.0;
	B = gteB * 16.0;

	GTE_DPCS();

	if (gteFLAG & 0x7f87e000) gteFLAG|=0x80000000;*/
/*	gteFLAG = 0;
	
	gteMAC1 = NC_OVERFLOW1((gteR * 16.0f) + (gteIR0 * limA1S(gteRFC - (gteR * 16.0f))) / 4096.0f);
	gteMAC2 = NC_OVERFLOW2((gteG * 16.0f) + (gteIR0 * limA2S(gteGFC - (gteG * 16.0f))) / 4096.0f);
	gteMAC3 = NC_OVERFLOW3((gteB * 16.0f) + (gteIR0 * limA3S(gteBFC - (gteB * 16.0f))) / 4096.0f);
	*/
/*	gteMAC1 = (gteR<<4) + ( (gteIR0*(signed short)limA1S(gteRFC-(gteR<<4)) ) >>12);
	gteMAC2 = (gteG<<4) + ( (gteIR0*(signed short)limA2S(gteGFC-(gteG<<4)) ) >>12);
	gteMAC3 = (gteB<<4) + ( (gteIR0*(signed short)limA3S(gteBFC-(gteB<<4)) ) >>12);*/
	gteMAC1 = (gteR<<4) + ( (gteIR0*(signed short)FlimA1S(gteRFC-(gteR<<4)) ) >>12);
	gteMAC2 = (gteG<<4) + ( (gteIR0*(signed short)FlimA2S(gteGFC-(gteG<<4)) ) >>12);
	gteMAC3 = (gteB<<4) + ( (gteIR0*(signed short)FlimA3S(gteBFC-(gteB<<4)) ) >>12);

	gteFLAG = 0;
	MAC2IR();
	
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteDPCT() {
//	unsigned long C,R,G,B;	
//	double RR0,GG0,BB0;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif
#ifdef GTE_LOG
	GTE_LOG("GTE_DPCT\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("DPCT", 17);
		G_SD(8);

		G_SD(20);
		G_SD(21);
		G_SD(22);

		G_SC(21);
		G_SC(22);
		G_SC(23);
	}
#endif
/*	gteFLAG = 0;

	C = gteCODE0;
	R = gteR0 * 16.0;
	G = gteG0 * 16.0;
	B = gteB0 * 16.0;

	GTE_DPCS();

	C = gteCODE0;
	R = gteR0 * 16.0;
	G = gteG0 * 16.0;
	B = gteB0 * 16.0;

	GTE_DPCS();

	C = gteCODE0;
	R = gteR0 * 16.0;
	G = gteG0 * 16.0;
	B = gteB0 * 16.0;

	GTE_DPCS();

	if (gteFLAG & 0x7f87e000) gteFLAG|=0x80000000;*/
/*	gteFLAG = 0;

	gteMAC1 = NC_OVERFLOW1((gteR0 * 16.0f) + gteIR0 * limA1S(gteRFC - (gteR0 * 16.0f)));
	gteMAC2 = NC_OVERFLOW2((gteG0 * 16.0f) + gteIR0 * limA2S(gteGFC - (gteG0 * 16.0f)));
	gteMAC3 = NC_OVERFLOW3((gteB0 * 16.0f) + gteIR0 * limA3S(gteBFC - (gteB0 * 16.0f)));
	*/
/*	gteMAC1 = (gteR0<<4) + ( (gteIR0*(signed short)limA1S(gteRFC-(gteR0<<4)) ) >>12);
	gteMAC2 = (gteG0<<4) + ( (gteIR0*(signed short)limA2S(gteGFC-(gteG0<<4)) ) >>12);
	gteMAC3 = (gteB0<<4) + ( (gteIR0*(signed short)limA3S(gteBFC-(gteB0<<4)) ) >>12);*/
	gteMAC1 = (gteR0<<4) + ( (gteIR0*(signed short)FlimA1S(gteRFC-(gteR0<<4)) ) >>12);
	gteMAC2 = (gteG0<<4) + ( (gteIR0*(signed short)FlimA2S(gteGFC-(gteG0<<4)) ) >>12);
	gteMAC3 = (gteB0<<4) + ( (gteIR0*(signed short)FlimA3S(gteBFC-(gteB0<<4)) ) >>12);
//	MAC2IR();
	
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

/*	gteMAC1 = (gteR0<<4) + ( (gteIR0*(signed short)limA1S(gteRFC-(gteR0<<4)) ) >>12);
	gteMAC2 = (gteG0<<4) + ( (gteIR0*(signed short)limA2S(gteGFC-(gteG0<<4)) ) >>12);
	gteMAC3 = (gteB0<<4) + ( (gteIR0*(signed short)limA3S(gteBFC-(gteB0<<4)) ) >>12);*/
	gteMAC1 = (gteR0<<4) + ( (gteIR0*(signed short)FlimA1S(gteRFC-(gteR0<<4)) ) >>12);
	gteMAC2 = (gteG0<<4) + ( (gteIR0*(signed short)FlimA2S(gteGFC-(gteG0<<4)) ) >>12);
	gteMAC3 = (gteB0<<4) + ( (gteIR0*(signed short)FlimA3S(gteBFC-(gteB0<<4)) ) >>12);
//	MAC2IR();
    gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

/*	gteMAC1 = (gteR0<<4) + ( (gteIR0*(signed short)limA1S(gteRFC-(gteR0<<4)) ) >>12);
	gteMAC2 = (gteG0<<4) + ( (gteIR0*(signed short)limA2S(gteGFC-(gteG0<<4)) ) >>12);
	gteMAC3 = (gteB0<<4) + ( (gteIR0*(signed short)limA3S(gteBFC-(gteB0<<4)) ) >>12);*/
	gteMAC1 = (gteR0<<4) + ( (gteIR0*(signed short)FlimA1S(gteRFC-(gteR0<<4)) ) >>12);
	gteMAC2 = (gteG0<<4) + ( (gteIR0*(signed short)FlimA2S(gteGFC-(gteG0<<4)) ) >>12);
	gteMAC3 = (gteB0<<4) + ( (gteIR0*(signed short)FlimA3S(gteBFC-(gteB0<<4)) ) >>12);
	gteFLAG = 0;
	MAC2IR();
    gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		G_GD(20);
		G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

/*
#define GTE_NCS(vn) { \
	RR0 = ((double)gteVX##vn * gteL11 + (double)gteVY##vn * (double)gteL12 + (double)gteVZ##vn * gteL13) / 4096.0; \
	GG0 = ((double)gteVX##vn * gteL21 + (double)gteVY##vn * (double)gteL22 + (double)gteVZ##vn * gteL23) / 4096.0; \
	BB0 = ((double)gteVX##vn * gteL31 + (double)gteVY##vn * (double)gteL32 + (double)gteVZ##vn * gteL33) / 4096.0; \
	t1 = LimitAU(RR0, 24); \
	t2 = LimitAU(GG0, 23); \
	t3 = LimitAU(BB0, 22); \
 \
	RR0 = (double)gteRBK + ((double)gteLR1 * t1 + (double)gteLR2 * t2 + (double)gteLR3 * t3) / 4096.0; \
	GG0 = (double)gteGBK + ((double)gteLG1 * t1 + (double)gteLG2 * t2 + (double)gteLG3 * t3) / 4096.0; \
	BB0 = (double)gteBBK + ((double)gteLB1 * t1 + (double)gteLB2 * t2 + (double)gteLB3 * t3) / 4096.0; \
	t1 = LimitAU(RR0, 24); \
	t2 = LimitAU(GG0, 23); \
	t3 = LimitAU(BB0, 22); \
 \
	gteRGB0 = gteRGB1; gteRGB1 = gteRGB2; \
	gteR2 = (unsigned char)LimitB(RR0/16.0, 21); \
	gteG2 = (unsigned char)LimitB(GG0/16.0, 20); \
	gteB2 = (unsigned char)LimitB(BB0/16.0, 19); \
	gteCODE2=gteCODE0; \
}*/

#define LOW(a) (((a) < 0) ? 0 : (a))
/*
#define	GTE_NCS(vn)  \
RR0 = LOW((gteL11*gteVX##vn + gteL12*gteVY##vn + gteL13*gteVZ##vn)/4096.0f); \
GG0 = LOW((gteL21*gteVX##vn + gteL22*gteVY##vn + gteL23*gteVZ##vn)/4096.0f); \
BB0 = LOW((gteL31*gteVX##vn + gteL32*gteVY##vn + gteL33*gteVZ##vn)/4096.0f); \
gteMAC1 = gteRBK + (gteLR1*RR0 + gteLR2*GG0 + gteLR3*BB0)/4096.0f; \
gteMAC2 = gteGBK + (gteLG1*RR0 + gteLG2*GG0 + gteLG3*BB0)/4096.0f; \
gteMAC3 = gteBBK + (gteLB1*RR0 + gteLB2*GG0 + gteLB3*BB0)/4096.0f; \
gteRGB0 = gteRGB1; \
gteRGB1 = gteRGB2; \
gteR2 = FlimB1(gteMAC1 >> 4); \
gteG2 = FlimB2(gteMAC2 >> 4); \
gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;*/
/*gteR2 = limB1(gteMAC1 / 16.0f); \
gteG2 = limB2(gteMAC2 / 16.0f); \
gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/

#define	GTE_NCS(vn)  \
	gte_LL1 = F12limA1U((gteL11*gteVX##vn + gteL12*gteVY##vn + gteL13*gteVZ##vn) >> 12); \
	gte_LL2 = F12limA2U((gteL21*gteVX##vn + gteL22*gteVY##vn + gteL23*gteVZ##vn) >> 12); \
	gte_LL3 = F12limA3U((gteL31*gteVX##vn + gteL32*gteVY##vn + gteL33*gteVZ##vn) >> 12); \
	gteMAC1 = F12limA1U(gteRBK + ((gteLR1*gte_LL1 + gteLR2*gte_LL2 + gteLR3*gte_LL3) >> 12)); \
	gteMAC2 = F12limA2U(gteGBK + ((gteLG1*gte_LL1 + gteLG2*gte_LL2 + gteLG3*gte_LL3) >> 12)); \
	gteMAC3 = F12limA3U(gteBBK + ((gteLB1*gte_LL1 + gteLB2*gte_LL2 + gteLB3*gte_LL3) >> 12));

void gteNCS() {
//	double RR0,GG0,BB0;
	s32 gte_LL1,gte_LL2,gte_LL3;
//	s32 RR0,GG0,BB0;
//	double t1, t2, t3;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif

#ifdef GTE_LOG
	GTE_LOG("GTE_NCS\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("NCS", 14);
		G_SD(0);
		G_SD(1);
		G_SD(6);

		G_SC(8);
		G_SC(9);
		G_SC(10);
		G_SC(11);
		G_SC(12);
		G_SC(13);
		G_SC(14);
		G_SC(15);
		G_SC(16);
		G_SC(17);
		G_SC(18);
		G_SC(19);
		G_SC(20);
	}
#endif
/*	gteFLAG = 0;

	GTE_NCS(0);

	gteMAC1=(long)RR0;
	gteMAC2=(long)GG0;
	gteMAC3=(long)BB0;

	gteIR1=(long)t1;
	gteIR2=(long)t2;
	gteIR3=(long)t3;

	if (gteFLAG & 0x7f87e000) gteFLAG|=0x80000000;*/
	gteFLAG = 0;

	GTE_NCS(0);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	MAC2IR1();

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteNCT() {
//	double RR0,GG0,BB0;
	s32 gte_LL1,gte_LL2,gte_LL3;
//	s32 RR0,GG0,BB0;
//	double t1, t2, t3;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif

#ifdef GTE_LOG
	GTE_LOG("GTE_NCT\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("NCT", 30);
		G_SD(0);
		G_SD(1);
		G_SD(2);
		G_SD(3);
		G_SD(4);
		G_SD(5);
		G_SD(6);

		G_SC(8);
		G_SC(9);
		G_SC(10);
		G_SC(11);
		G_SC(12);
		G_SC(13);
		G_SC(14);
		G_SC(15);
		G_SC(16);
		G_SC(17);
		G_SC(18);
		G_SC(19);
		G_SC(20);
	}
#endif
/*
	gteFLAG = 0;

//V0
	GTE_NCS(0);
//V1
	GTE_NCS(1);
//V2
	GTE_NCS(2);

	gteMAC1=(long)RR0;
	gteMAC2=(long)GG0;
	gteMAC3=(long)BB0;

	gteIR1=(long)t1;
	gteIR2=(long)t2;
	gteIR3=(long)t3;

	if (gteFLAG & 0x7f87e000) gteFLAG|=0x80000000;*/
	gteFLAG = 0;
	
	GTE_NCS(0);

	gteR0 = FlimB1(gteMAC1 >> 4);
	gteG0 = FlimB2(gteMAC2 >> 4);
	gteB0 = FlimB3(gteMAC3 >> 4); gteCODE0 = gteCODE;

	GTE_NCS(1);
	gteR1 = FlimB1(gteMAC1 >> 4);
	gteG1 = FlimB2(gteMAC2 >> 4);
	gteB1 = FlimB3(gteMAC3 >> 4); gteCODE1 = gteCODE;

	GTE_NCS(2);
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	MAC2IR1();

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		G_GD(20);
		G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteCC() {
//	double RR0,GG0,BB0;
	s32 RR0,GG0,BB0;
//	double t1,t2,t3;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif
#ifdef GTE_LOG
	GTE_LOG("GTE_CC\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("CC", 11);
		G_SD(6);
		G_SD(9);
		G_SD(10);
		G_SD(11);

		G_SC(13);
		G_SC(14);
		G_SC(15);
		G_SC(16);
		G_SC(17);
		G_SC(18);
		G_SC(19);
	}
#endif
/*	gteFLAG = 0;

	RR0 = (double)gteRBK + ((double)gteLR1 * gteIR1 + (double)gteLR2 * gteIR2 + (double)gteLR3 * gteIR3) / 4096.0;
	GG0 = (double)gteGBK + ((double)gteLG1 * gteIR1 + (double)gteLG2 * gteIR2 + (double)gteLG3 * gteIR3) / 4096.0;
	BB0 = (double)gteBBK + ((double)gteLB1 * gteIR1 + (double)gteLB2 * gteIR2 + (double)gteLB3 * gteIR3) / 4096.0;
	t1 = LimitAU(RR0, 24);
	t2 = LimitAU(GG0, 23);
	t3 = LimitAU(BB0, 22);

	RR0=((double)gteR * t1)/256.0;
	GG0=((double)gteG * t2)/256.0;
	BB0=((double)gteB * t3)/256.0;
	gteIR1 = (long)LimitAU(RR0,24);
	gteIR2 = (long)LimitAU(GG0,23);
	gteIR3 = (long)LimitAU(BB0,22);

	gteCODE0=gteCODE1; gteCODE1=gteCODE2; 
	gteC2 = gteCODE0;
	gteR2 = (unsigned char)LimitB(RR0/16.0, 21);
	gteG2 = (unsigned char)LimitB(GG0/16.0, 20);
	gteB2 = (unsigned char)LimitB(BB0/16.0, 19);

	if (gteFLAG & 0x7f87e000) gteFLAG|=0x80000000;*/
	gteFLAG = 0;
	
/*	RR0 = NC_OVERFLOW1(gteRBK + (gteLR1*gteIR1 + gteLR2*gteIR2 + gteLR3*gteIR3) / 4096.0f);
	GG0 = NC_OVERFLOW2(gteGBK + (gteLG1*gteIR1 + gteLG2*gteIR2 + gteLG3*gteIR3) / 4096.0f);
	BB0 = NC_OVERFLOW3(gteBBK + (gteLB1*gteIR1 + gteLB2*gteIR2 + gteLB3*gteIR3) / 4096.0f);

	gteMAC1 = gteR * RR0 / 256.0f;
	gteMAC2 = gteG * GG0 / 256.0f;
	gteMAC3 = gteB * BB0 / 256.0f;*/
	RR0 = FNC_OVERFLOW1(gteRBK + ((gteLR1*gteIR1 + gteLR2*gteIR2 + gteLR3*gteIR3) >> 12));
	GG0 = FNC_OVERFLOW2(gteGBK + ((gteLG1*gteIR1 + gteLG2*gteIR2 + gteLG3*gteIR3) >> 12));
	BB0 = FNC_OVERFLOW3(gteBBK + ((gteLB1*gteIR1 + gteLB2*gteIR2 + gteLB3*gteIR3) >> 12));

	gteMAC1 = (gteR * RR0) >> 8;
	gteMAC2 = (gteG * GG0) >> 8;
	gteMAC3 = (gteB * BB0) >> 8;
	
	MAC2IR1();
	
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteINTPL() { //test opcode
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif
#ifdef GTE_LOG
	GTE_LOG("GTE_INTP\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("INTPL", 8);
		G_SD(6);
		G_SD(8);
		G_SD(9);
		G_SD(10);
		G_SD(11);

		G_SC(21);
		G_SC(22);
		G_SC(23);
	}
#endif
	/* NC: old
	gteFLAG=0;
    gteMAC1 = gteIR1 + gteIR0*limA1S(gteRFC-gteIR1);
	gteMAC2 = gteIR2 + gteIR0*limA2S(gteGFC-gteIR2);
	gteMAC3 = gteIR3 + gteIR0*limA3S(gteBFC-gteIR3);
	//gteFLAG = 0;
	MAC2IR();
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;
	*/
	
/*	gteFLAG=0;
    gteMAC1 = gteIR1 + gteIR0*(gteRFC-gteIR1)/4096.0;
	gteMAC2 = gteIR2 + gteIR0*(gteGFC-gteIR2)/4096.0;
	gteMAC3 = gteIR3 + gteIR0*(gteBFC-gteIR3)/4096.0;

	//gteMAC3 = (int)((((psxRegs).CP2D).n).ir3+(((psxRegs).CP2D).n).ir0 * ((((psxRegs).CP2C).n).bfc-(((psxRegs).CP2D).n).ir3)/4096.0);

	if(gteMAC3 > gteIR1 && gteMAC3 > gteBFC)
	{
		gteMAC3 = gteMAC3;
	}
	//gteFLAG = 0;*/
	//NEW CODE
/*	gteMAC1 = gteIR1 + ((gteIR0*(signed short)limA1S(gteRFC-gteIR1))>>12);
	gteMAC2 = gteIR2 + ((gteIR0*(signed short)limA2S(gteGFC-gteIR2))>>12);
	gteMAC3 = gteIR3 + ((gteIR0*(signed short)limA3S(gteBFC-gteIR3))>>12);*/
	gteMAC1 = gteIR1 + ((gteIR0*(signed short)FlimA1S(gteRFC-gteIR1))>>12);
	gteMAC2 = gteIR2 + ((gteIR0*(signed short)FlimA2S(gteGFC-gteIR2))>>12);
	gteMAC3 = gteIR3 + ((gteIR0*(signed short)FlimA3S(gteBFC-gteIR3))>>12);
	gteFLAG = 0;

	MAC2IR();
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}

void gteCDP() { //test opcode
	double RR0,GG0,BB0;
//	s32 RR0,GG0,BB0;
#ifdef GTE_DUMP
	static int sample = 0; sample++;
#endif

#ifdef GTE_LOG
	GTE_LOG("GTE_CDP\n");
#endif

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_OP("CDP", 13);
		G_SD(6);
		G_SD(8);
		G_SD(9);
		G_SD(10);
		G_SD(11);

		G_SC(13);
		G_SC(14);
		G_SC(15);
		G_SC(16);
		G_SC(17);
		G_SC(18);
		G_SC(19);
		G_SC(20);
		G_SC(21);
		G_SC(22);
		G_SC(23);
	}
#endif

	gteFLAG = 0;

	RR0 = NC_OVERFLOW1(gteRBK + (gteLR1*gteIR1 +gteLR2*gteIR2 + gteLR3*gteIR3));
	GG0 = NC_OVERFLOW2(gteGBK + (gteLG1*gteIR1 +gteLG2*gteIR2 + gteLG3*gteIR3));
	BB0 = NC_OVERFLOW3(gteBBK + (gteLB1*gteIR1 +gteLB2*gteIR2 + gteLB3*gteIR3));
	gteMAC1 = gteR*RR0 + gteIR0*limA1S(gteRFC-gteR*RR0);
	gteMAC2 = gteG*GG0 + gteIR0*limA2S(gteGFC-gteG*GG0);
	gteMAC3 = gteB*BB0 + gteIR0*limA3S(gteBFC-gteB*BB0);

/*	RR0 = FNC_OVERFLOW1(gteRBK + (gteLR1*gteIR1 +gteLR2*gteIR2 + gteLR3*gteIR3));
	GG0 = FNC_OVERFLOW2(gteGBK + (gteLG1*gteIR1 +gteLG2*gteIR2 + gteLG3*gteIR3));
	BB0 = FNC_OVERFLOW3(gteBBK + (gteLB1*gteIR1 +gteLB2*gteIR2 + gteLB3*gteIR3));
	gteMAC1 = gteR*RR0 + gteIR0*FlimA1S(gteRFC-gteR*RR0);
	gteMAC2 = gteG*GG0 + gteIR0*FlimA2S(gteGFC-gteG*GG0);
	gteMAC3 = gteB*BB0 + gteIR0*FlimA3S(gteBFC-gteB*BB0);*/

	MAC2IR1();
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	
/*	gteR2 = limB1(gteMAC1 / 16.0f);
	gteG2 = limB2(gteMAC2 / 16.0f);
	gteB2 = limB3(gteMAC3 / 16.0f); gteCODE2 = gteCODE;*/
	gteR2 = FlimB1(gteMAC1 >> 4);
	gteG2 = FlimB2(gteMAC2 >> 4);
	gteB2 = FlimB3(gteMAC3 >> 4); gteCODE2 = gteCODE;

	SUM_FLAG

#ifdef GTE_DUMP
	if(sample < 100)
	{
		G_GD(9);
		G_GD(10);
		G_GD(11);

		//G_GD(20);
		//G_GD(21);
		G_GD(22);

		G_GD(25);
		G_GD(26);
		G_GD(27);

		G_GC(31);
	}
#endif
}
