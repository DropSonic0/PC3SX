// ppc_mnemonics.h

#define INSTR		(*(ppcPtr)++)

/* Link register related */
#define MFLR(REG) \
	{int _reg = (REG); \
        INSTR = (0x7C0802A6 | (_reg << 21));}

#define MTLR(REG) \
	{int _reg = (REG); \
        INSTR = (0x7C0803A6 | (_reg << 21));}

#define MTCTR(REG) \
	{int _reg = (REG); \
        INSTR = (0x7C0903A6 | (_reg << 21));}

#define BLR() \
	{INSTR = (0x4E800020);}

#define BGTLR() \
	{INSTR = (0x4D810020);}


/* Load ops */
#define LI(REG, IMM) \
	{int _reg = (REG); \
        INSTR = (0x38000000 | (_reg << 21) | ((IMM) & 0xffff));}

#define LIS(REG_DST, IMM) \
	{int _dst = (REG_DST); \
        INSTR = (0x3C000000 | (_dst << 21) | ((IMM) & 0xffff));}

#define LWZ(REG_DST, OFFSET, REG) \
	{int _reg = (REG); int _dst=(REG_DST); \
        INSTR = (0x80000000 | (_dst << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define LD(REG_DST, OFFSET, REG) \
	{int _reg = (REG); int _dst=(REG_DST); \
        INSTR = (0xE8000000 | (_dst << 21) | (_reg << 16) | ((OFFSET) & 0xfffc));}

#define LWZX(REG_DST, REG, REG_OFF) \
	{int _reg = (REG), _off = (REG_OFF); int _dst=(REG_DST); \
        INSTR = (0x7C00002E | (_dst << 21) | (_reg << 16) | (_off << 11));}

#define LWBRX(REG_DST, REG, REG_OFF) \
	{int _reg = (REG), _off = (REG_OFF); int _dst=(REG_DST); \
        INSTR = (0x7C00042C | (_dst << 21) | (_reg << 16) | (_off << 11));}

#define LHZ(REG_DST, OFFSET, REG) \
	{int _reg = (REG); int _dst=(REG_DST); \
        INSTR = (0xA0000000 | (_dst << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define LHA(REG_DST, OFFSET, REG) \
	{int _reg = (REG); int _dst=(REG_DST); \
        INSTR = (0xA8000000 | (_dst << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define LHBRX(REG_DST, REG, REG_OFF) \
	{int _reg = (REG), _off = (REG_OFF); int _dst=(REG_DST); \
        INSTR = (0x7C00062C | (_dst << 21) | (_reg << 16) | (_off << 11));}

#define LBZ(REG_DST, OFFSET, REG) \
	{int _reg = (REG); int _dst=(REG_DST); \
        INSTR = (0x88000000 | (_dst << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define LMW(REG_DST, OFFSET, REG) \
	{int _reg = (REG); int _dst=(REG_DST); \
        INSTR = (0xB8000000 | (_dst << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}



/* Store ops */
#define STMW(REG_SRC, OFFSET, REG) \
	{int _reg = (REG), _src=(REG_SRC); \
        INSTR = (0xBC000000 | (_src << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define STW(REG_SRC, OFFSET, REG) \
	{int _reg = (REG), _src=(REG_SRC); \
        INSTR = (0x90000000 | (_src << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define STD(REG_SRC, OFFSET, REG) \
	{int _reg = (REG), _src=(REG_SRC); \
        INSTR = (0xF8000000 | (_src << 21) | (_reg << 16) | ((OFFSET) & 0xfffc));}

#define STWBRX(REG_SRC, REG, REG_OFF) \
	{int _reg = (REG), _src=(REG_SRC), _off = (REG_OFF); \
        INSTR = (0x7C00052C | (_src << 21) | (_reg << 16) | (_off << 11));}

#define STH(REG_SRC, OFFSET, REG) \
	{int _reg = (REG), _src=(REG_SRC); \
        INSTR = (0xB0000000 | (_src << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define STHBRX(REG_SRC, REG, REG_OFF) \
	{int _reg = (REG), _src=(REG_SRC), _off = (REG_OFF); \
        INSTR = (0x7C00072C | (_src << 21) | (_reg << 16) | (_off << 11));}

#define STB(REG_SRC, OFFSET, REG) \
	{int _reg = (REG), _src=(REG_SRC); \
        INSTR = (0x98000000 | (_src << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}

#define STWU(REG_SRC, OFFSET, REG) \
	{int _reg = (REG), _src=(REG_SRC); \
        INSTR = (0x94000000 | (_src << 21) | (_reg << 16) | ((OFFSET) & 0xffff));}


/* Arithmic ops */
#define ADDI(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x38000000 | (_dst << 21) | (_src << 16) | ((IMM) & 0xffff));}

#define ADDIS(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x3C000000 | (_dst << 21) | (_src << 16) | ((IMM) & 0xffff));}

#define MR(REG_DST, REG_SRC) \
	{int __src = (REG_SRC); int __dst=(REG_DST); \
        if (__src != __dst) {ADDI(__dst, __src, 0)}}

#define ADD(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000214 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define ADDO(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000614 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define ADDEO(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000514 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define ADDE(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000114 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define ADDCO(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000414 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define ADDIC(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x30000000 | (_dst << 21) | (_src << 16) | ((IMM) & 0xffff));}

#define ADDIC_(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x34000000 | (_dst << 21) | (_src << 16) | ((IMM) & 0xffff));}

#define ADDZE(REG_DST, REG_SRC) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x7C000194 | (_dst << 21) | (_src << 16));}

#define SUBF(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000050 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define SUBFO(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000450 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define SUBFC(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000010 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define SUBFE(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000110 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define SUBFCO(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000410 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define SUBFCO_(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000411 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define SUB(REG_DST, REG1, REG2) \
	{SUBF(REG_DST, REG2, REG1)}

#define SUBO(REG_DST, REG1, REG2) \
	{SUBFO(REG_DST, REG2, REG1)}

#define SUBCO(REG_DST, REG1, REG2) \
	{SUBFCO(REG_DST, REG2, REG1)}

#define SUBCO_(REG_DST, REG1, REG2) \
	{SUBFCO_(REG_DST, REG2, REG1)}

#define MULHW(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000096 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define MULLW(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C0001D6 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define MULHWU(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000016 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define MULLI(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x1C000000 | (_dst << 21) | (_src << 16) | ((IMM) & 0xffff));}

#define DIVW(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C0003D6 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}

#define DIVWU(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000396 | (_dst << 21) | (_reg1 << 16) |  (_reg2 << 11));}


/* Branch ops */
#define B_FROM(VAR) VAR = ppcPtr
#define B_DST(VAR) *VAR = *VAR | (((s16)((u32)ppcPtr - (u32)VAR)) & 0xfffc)

#define B(DST) \
	{INSTR = (0x48000000 | (((s32)(((DST)+1)<<2)) & 0x3fffffc));}

#define B_L(VAR) \
	{B_FROM(VAR); INSTR = (0x48000000);}

#define BA(DST) \
	{INSTR = (0x48000002 | ((s32)((DST) & 0x3fffffc)));}

#define BLA(DST) \
	{INSTR = (0x48000003 | ((s32)((DST) & 0x3fffffc)));}

#define BNS(DST) \
	{INSTR = (0x40830000 | (((s16)(((DST)+1)<<2)) & 0xfffc));}

#define BNE(DST) \
	{INSTR = (0x40820000 | (((s16)(((DST)+1)<<2)) & 0xfffc));}

#define BNE_L(VAR) \
	{B_FROM(VAR); INSTR = (0x40820000);}

#define BEQ(DST) \
	{INSTR = (0x41820000 | (((s16)(((DST)+1)<<2)) & 0xfffc));}

#define BEQ_L(VAR) \
	{B_FROM(VAR); INSTR = (0x41820000);}

#define BLT(DST) \
	{INSTR = (0x41800000 | (((s16)(((DST)+1)<<2)) & 0xfffc));}

#define BLT_L(VAR) \
	{B_FROM(VAR); INSTR = (0x41800000);}

#define BGT(DST) \
	{INSTR = (0x41810000 | (((s16)(((DST)+1)<<2)) & 0xfffc));}

#define BGT_L(VAR) \
	{B_FROM(VAR); INSTR = (0x41810000);}

#define BGE(DST) \
	{INSTR = (0x40800000 | (((s16)(((DST)+1)<<2)) & 0xfffc));}

#define BGE_L(VAR) \
	{B_FROM(VAR); INSTR = (0x40800000);}

#define BLE(DST) \
	{INSTR = (0x40810000 | (((s16)(((DST)+1)<<2)) & 0xfffc));}

#define BLE_L(VAR) \
	{B_FROM(VAR); INSTR = (0x40810000);}

#define BCTRL() \
	{INSTR = (0x4E800421);}

#define BCTR() \
	{INSTR = (0x4E800420);}


/* compare ops */
#define CMPLWI(REG, IMM) \
	{int _reg = (REG); \
        INSTR = (0x28000000 | (_reg << 16) | ((IMM) & 0xffff));}

#define CMPLWI2(REG, IMM) \
	{int _reg = (REG); \
        INSTR = (0x29000000 | (_reg << 16) | ((IMM) & 0xffff));}

#define CMPLWI7(REG, IMM) \
	{int _reg = (REG); \
        INSTR = (0x2B800000 | (_reg << 16) | ((IMM) & 0xffff));}

#define CMPLDI(REG, IMM) \
	{int _reg = (REG); \
        INSTR = (0x28200000 | (_reg << 16) | ((IMM) & 0xffff));}

#define CMPLW(REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); \
        INSTR = (0x7C000040 | (_reg1 << 16) | (_reg2 << 11));}

#define CMPLD(REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); \
        INSTR = (0x7C200040 | (_reg1 << 16) | (_reg2 << 11));}

#define CMPLW1(REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); \
        INSTR = (0x7C800040 | (_reg1 << 16) | (_reg2 << 11));}

#define CMPLW2(REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); \
        INSTR = (0x7D000040 | (_reg1 << 16) | (_reg2 << 11));}

#define CMPW(REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); \
        INSTR = (0x7C000000 | (_reg1 << 16) | (_reg2 << 11));}

#define CMPW1(REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); \
        INSTR = (0x7C800000 | (_reg1 << 16) | (_reg2 << 11));}

#define CMPW2(REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); \
        INSTR = (0x7D000000 | (_reg1 << 16) | (_reg2 << 11));}

#define CMPWI(REG, IMM) \
	{int _reg = (REG); \
        INSTR = (0x2C000000 | (_reg << 16) | ((IMM) & 0xffff));}

#define CMPWI2(REG, IMM) \
	{int _reg = (REG); \
        INSTR = (0x2D000000 | (_reg << 16) | ((IMM) & 0xffff));}

#define MTCRF(MASK, REG) \
	{int _reg = (REG); \
        INSTR = (0x7C000120 | (_reg << 21) | (((MASK)&0xff)<<12));}

#define MFCR(REG) \
	{int _reg = (REG); \
        INSTR = (0x7C000026 | (_reg << 21));}

#define CROR(CR_DST, CR1, CR2) \
	{INSTR = (0x4C000382 | ((CR_DST) << 21) | ((CR1) << 16) |  ((CR2) << 11));}

#define CRXOR(CR_DST, CR1, CR2) \
	{INSTR = (0x4C000182 | ((CR_DST) << 21) | ((CR1) << 16) |  ((CR2) << 11));}

#define CRNAND(CR_DST, CR1, CR2) \
	{INSTR = (0x4C0001C2 | ((CR_DST) << 21) | ((CR1) << 16) |  ((CR2) << 11));}

#define CRANDC(CR_DST, CR1, CR2) \
	{INSTR = (0x4C000102 | ((CR_DST) << 21) | ((CR1) << 16) |  ((CR2) << 11));}


/* shift ops */
#define RLWINM(REG_DST, REG_SRC, SHIFT, START, END) \
	{int _src = (REG_SRC); int _dst = (REG_DST); \
        INSTR = (0x54000000 | (_src << 21) | (_dst << 16) | (SHIFT << 11) | (START << 6) | (END << 1));}

#define RLWINM_(REG_DST, REG_SRC, SHIFT, START, END) \
	{int _src = (REG_SRC); int _dst = (REG_DST); \
        INSTR = (0x54000001 | (_src << 21) | (_dst << 16) | (SHIFT << 11) | (START << 6) | (END << 1));}

#define CLRRWI(REG_DST, REG_SRC, LEN) \
	RLWINM(REG_DST, REG_SRC, 0, 0, 31-LEN)

#define SLWI(REG_DST, REG_SRC, SHIFT) \
	{int _shift = (SHIFT); \
        if (_shift==0) {MR(REG_DST, REG_SRC)} else \
        {RLWINM(REG_DST, REG_SRC, _shift, 0, 31-_shift)}}

#define SRWI(REG_DST, REG_SRC, SHIFT) \
	{int _shift = (SHIFT); \
        if (_shift==0) {MR(REG_DST, REG_SRC)} else \
        RLWINM(REG_DST, REG_SRC, 32-_shift, _shift, 31)}

#define SLW(REG_DST, REG_SRC, REG_SHIFT) \
	{int _src = (REG_SRC), _shift = (REG_SHIFT); int _dst = (REG_DST); \
        INSTR = (0x7C000030 | (_src << 21) | (_dst << 16) | (_shift << 11));}

#define SRW(REG_DST, REG_SRC, REG_SHIFT) \
	{int _src = (REG_SRC), _shift = (REG_SHIFT); int _dst = (REG_DST); \
        INSTR = (0x7C000430 | (_src << 21) | (_dst << 16) | (_shift << 11));}

#define SRAW(REG_DST, REG_SRC, REG_SHIFT) \
	{int _src = (REG_SRC), _shift = (REG_SHIFT); int _dst = (REG_DST); \
        INSTR = (0x7C000630 | (_src << 21) | (_dst << 16) | (_shift << 11));}

#define SRAWI(REG_DST, REG_SRC, SHIFT) \
	{int _src = (REG_SRC); int _dst = (REG_DST); int _shift = (SHIFT); \
        if (_shift==0) {MR(REG_DST, REG_SRC)} else \
        INSTR = (0x7C000670 | (_src << 21) | (_dst << 16) | (_shift << 11));}

#define RLWNM(REG_DST, REG_SRC, REG_SHIFT, START, END) \
	{int _src = (REG_SRC), _shift = (REG_SHIFT); int _dst = (REG_DST); \
        INSTR = (0x5C000000 | (_src << 21) | (_dst << 16) | (_shift << 11) | (START << 6) | (END << 1));}

/* other ops */
#define ORI(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC), _imm = (IMM); int _dst = (REG_DST); \
        if (!((_imm == 0) && ((_src^_dst) == 0))) \
        INSTR = (0x60000000 | (_src << 21) | (_dst << 16) | (_imm & 0xffff));}

#define ORIS(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC), _imm = (IMM); int _dst = (REG_DST); \
        if (!((_imm == 0) && ((_src^_dst) == 0))) \
        INSTR = (0x64000000 | (_src << 21) | (_dst << 16) | (_imm & 0xffff));}

#define OR(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000378 | (_reg1 << 21) | (_dst << 16) | (_reg2 << 11));}

#define OR_(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000379 | (_reg1 << 21) | (_dst << 16) | (_reg2 << 11));}

#define XORI(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x68000000 | (_src << 21) | (_dst << 16) | ((IMM) & 0xffff));}

#define XOR(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000278 | (_reg1 << 21) | (_dst << 16) | (_reg2 << 11));}

#define XOR_(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000279 | (_reg1 << 21) | (_dst << 16) | (_reg2 << 11));}

#define ANDI_(REG_DST, REG_SRC, IMM) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x70000000 | (_src << 21) | (_dst << 16) | ((IMM) & 0xffff));}

#define AND(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C000038 | (_reg1 << 21) | (_dst << 16) | (_reg2 << 11));}

#define NOR(REG_DST, REG1, REG2) \
	{int _reg1 = (REG1), _reg2 = (REG2); int _dst=(REG_DST); \
        INSTR = (0x7C0000f8 | (_reg1 << 21) | (_dst << 16) | (_reg2 << 11));}

#define NEG(REG_DST, REG_SRC) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x7C0000D0 | (_dst << 21) | (_src << 16));}

#define NOP() \
	{INSTR = 0x60000000;}

#define MCRXR(CR_DST) \
	{INSTR = (0x7C000400 | (CR_DST << 23));}

#define EXTSB(REG_DST, REG_SRC) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x7C000774 | (_src << 21) | (_dst << 16));}

#define EXTSH(REG_DST, REG_SRC) \
	{int _src = (REG_SRC); int _dst=(REG_DST); \
        INSTR = (0x7C000734 | (_src << 21) | (_dst << 16));}


/* 64-bit ops */
#define RLDICR(REG_DST, REG_SRC, SHIFT, MASK_END) \
	{int _src = (REG_SRC), _dst = (REG_DST); \
        INSTR = (0x78000004 | (_src << 21) | (_dst << 16) | (((SHIFT) & 0x1f) << 11) | (((MASK_END) & 0x1f) << 6) | (((SHIFT) & 0x20) >> 4) | ((MASK_END) & 0x20));}

#define SLDI(REG_DST, REG_SRC, SHIFT) \
	RLDICR(REG_DST, REG_SRC, SHIFT, 63 - (SHIFT))

#define LID(REG, IMM) \
{ \
    uint64_t _lid_imm = (uint64_t)(IMM); \
    LIS(REG, ((_lid_imm >> 48) & 0xffff)); \
    ORI(REG, REG, ((_lid_imm >> 32) & 0xffff)); \
    SLDI(REG, REG, 32); \
    ORIS(REG, REG, ((_lid_imm >> 16) & 0xffff)); \
    ORI(REG, REG, (_lid_imm & 0xffff)); \
}

/* floating point ops */
#define FDIVS(FPR_DST, FPR1, FPR2) \
	{INSTR = (0xEC000024 | (FPR_DST << 21) | (FPR1 << 16) | (FPR2 << 11));}

#define FDIV(FPR_DST, FPR1, FPR2) \
	{INSTR = (0xFC000024 | (FPR_DST << 21) | (FPR1 << 16) | (FPR2 << 11));}

#define FMULS(FPR_DST, FPR1, FPR2) \
	{INSTR = (0xEC000032 | (FPR_DST << 21) | (FPR1 << 16) | (FPR2 << 11));}

#define FMUL(FPR_DST, FPR1, FPR2) \
	{INSTR = (0xFC000032 | (FPR_DST << 21) | (FPR1 << 16) | (FPR2 << 11));}

#define FADDS(FPR_DST, FPR1, FPR2) \
	{INSTR = (0xEC00002A | (FPR_DST << 21) | (FPR1 << 16) | (FPR2 << 11));}

#define FADD(FPR_DST, FPR1, FPR2) \
	{INSTR = (0xFC00002A | (FPR_DST << 21) | (FPR1 << 16) | (FPR2 << 11));}

#define FRSP(FPR_DST, FPR_SRC) \
	{INSTR = (0xFC000018 | (FPR_DST << 21) | (FPR_SRC << 11));}

#define FCTIW(FPR_DST, FPR_SRC) \
	{INSTR = (0xFC00001C | (FPR_DST << 21) | (FPR_SRC << 11));}


#define LFS(FPR_DST, OFFSET, REG) \
	{INSTR = (0xC0000000 | (FPR_DST << 21) | (REG << 16) | ((OFFSET) & 0xffff));}

#define STFS(FPR_DST, OFFSET, REG) \
	{INSTR = (0xD0000000 | (FPR_DST << 21) | (REG << 16) | ((OFFSET) & 0xffff));}

#define LFD(FPR_DST, OFFSET, REG) \
	{INSTR = (0xC8000000 | (FPR_DST << 21) | (REG << 16) | ((OFFSET) & 0xffff));}

#define STFD(FPR_DST, OFFSET, REG) \
	{INSTR = (0xD8000000 | (FPR_DST << 21) | (REG << 16) | ((OFFSET) & 0xffff));}



/* extra combined opcodes */
#if 1
#define LIW(REG, IMM) /* Load Immidiate Word */ \
{ \
	int _liw_reg = (REG); u32 _liw_imm = (u32)(IMM); \
	if ((s32)_liw_imm == (s32)((s16)_liw_imm)) \
	{ \
		LI(_liw_reg, (s32)((s16)_liw_imm)); \
	} else if (_liw_reg == 0) { \
		LIS(_liw_reg, (((u32)_liw_imm)>>16)); \
		if ((((u32)_liw_imm) & 0xffff) != 0) \
		{ \
			ORI(_liw_reg, _liw_reg, _liw_imm); \
		} \
	} else { \
		if ((((u32)_liw_imm) & 0xffff) == 0) { \
			LIS(_liw_reg, (((u32)_liw_imm)>>16)); \
		} else { \
			LI(_liw_reg, _liw_imm); \
			if ((_liw_imm & 0x8000) == 0) { \
				ADDIS(_liw_reg, _liw_reg, ((u32)_liw_imm)>>16); \
			} else { \
				ADDIS(_liw_reg, _liw_reg, ((((u32)_liw_imm)>>16) & 0xffff) + 1); \
			} \
		} \
		/*if ((((u32)_liw_imm) & 0xffff) != 0) \
		{ \
			ORI(_liw_reg, _liw_reg, _liw_imm); \
		}*/ \
	} \
}
#else
#define LIW(REG, IMM) /* Load Immidiate Word */ \
{ \
        int __reg = (REG); u32 __imm = (u32)(IMM); \
	if ((s32)__imm == (s32)((s16)__imm)) \
	{ \
		LI(__reg, (s32)((s16)__imm)); \
	} \
	else \
	{ \
		LIS(__reg, (((u32)__imm)>>16)); \
		if ((((u32)__imm) & 0xffff) != 0) \
		{ \
			ORI(__reg, __reg, __imm); \
		} \
	} \
}
#endif
