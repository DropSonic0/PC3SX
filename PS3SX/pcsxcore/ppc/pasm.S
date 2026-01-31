#if defined (__ppc__) || defined (__ppc64__) || defined (__powerpc__) || (__powerpc64__) || defined __POWERPC__

#if defined (ELF) || defined (__linux__) || defined(MDFNPS3) //Token decoration
#define C(label) label
#else
#define C(label) _##label
#endif

#ifdef MDFNPS3
.section .opd, "aw"
.align 3
.globl recRun
recRun:
    .long .recRun, .TOC.@tocbase32
.globl returnPC
returnPC:
    .long .returnPC, .TOC.@tocbase32
#endif

#define OLD_REGISTER_OFFSET	(19*4)
#define SP_SIZE			(OLD_REGISTER_OFFSET+4+8)

/*asm void recRun(register void (*func)(), register u32 hw1, register u32 hw2)*/
        .text
        .align  4
#ifndef MDFNPS3 //Token decoration
        .globl  C(recRun)
C(recRun):
#else
        .globl  C(.recRun)
C(.recRun):
#endif
	/* prologue code */
	mflr	0
#ifdef MDFNPS3
	std		2, 40(1)	/* Save original TOC in caller frame (64-bit) */
	std		31, -8(1)	/* Save non-volatiles individually for 64-bit */
	std		30, -16(1)
	std		29, -24(1)
	std		28, -32(1)
	std		27, -40(1)
	std		26, -48(1)
	std		25, -56(1)
	std		24, -64(1)
	std		23, -72(1)
	std		22, -80(1)
	std		21, -88(1)
	std		20, -96(1)
	std		19, -104(1)
	std		18, -112(1)
	std		17, -120(1)
	std		16, -128(1)
	std		15, -136(1)
	std		14, -144(1)
	std		13, -152(1)
	std		0, 16(1)	/* Save LR in caller frame */
	stdu	1, -176(1)	/* Create frame (152 + padding = 176) */
#else
	stmw	13, -76(1)
	stw		0, 4(1)
	stwu	1, -96(1)
#endif
	
	/* execute code */
	mtctr	3
	mr	31, 4
	mr	30, 5
	bctr
/*
}
asm void returnPC()
{*/
        .text
        .align  4
#ifndef MDFNPS3
        .globl  C(returnPC)
C(returnPC):
#else
        .globl  C(.returnPC)
C(.returnPC):
        .globl  returnPC_recomp
returnPC_recomp:
#endif
	// end code
#ifdef MDFNPS3
	ld		1, 0(1)		/* Restore SP */
	ld		0, 16(1)	/* Restore LR */
	mtlr	0
	ld		13, -152(1)	/* Restore non-volatiles */
	ld		14, -144(1)
	ld		15, -136(1)
	ld		16, -128(1)
	ld		17, -120(1)
	ld		18, -112(1)
	ld		19, -104(1)
	ld		20, -96(1)
	ld		21, -88(1)
	ld		22, -80(1)
	ld		23, -72(1)
	ld		24, -64(1)
	ld		25, -56(1)
	ld		26, -48(1)
	ld		27, -40(1)
	ld		28, -32(1)
	ld		29, -24(1)
	ld		30, -16(1)
	ld		31, -8(1)
	ld		2, 40(1)	/* Restore TOC */
#else
	lwz		0, 96+4(1)
	addi	1, 1, 96
	mtlr	0
	lmw		13, -76(1)
#endif
	blr
//}*/

// Memory functions that only works with a linear memory

/*
u32 MEM_read32(u32 addr)
{
*/
        .text
        .align  4
        .globl  C(MEM_read32)
C(MEM_read32):
	lwbrx	3, 0, 3
	blr
//}

/*
u16 MEM_read16(u32 addr)
{
*/
        .text
        .align  4
        .globl  C(MEM_read16)
C(MEM_read16):
	lhbrx	3, 0, 3
	blr
//}

/*
void MEM_write32(u32 addr, u32 val)
{
*/
        .text
        .align  4
        .globl  C(MEM_write32)
C(MEM_write32):
	stwbrx	4, 0, 3
	blr
//}

/*
void MEM_write16(u32 addr, u16 val)
{
*/
        .text
        .align  4
        .globl  C(MEM_write16)
C(MEM_write16):
	sthbrx	4, 0, 3
	blr
//}

#endif
