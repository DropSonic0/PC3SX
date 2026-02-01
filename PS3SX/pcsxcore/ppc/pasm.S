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
    .quad .recRun, .TOC.@tocbase, 0
.globl returnPC
returnPC:
    .quad .returnPC, .TOC.@tocbase, 0
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
	std		0, 16(1)	/* Save LR in caller frame */
	std		2, 40(1)	/* Save original TOC in caller frame */
	stdu	1, -256(1)	/* Create frame */
	std		31, 248(1)	/* Save non-volatiles in our frame */
	std		30, 240(1)
	std		29, 232(1)
	std		28, 224(1)
	std		27, 216(1)
	std		26, 208(1)
	std		25, 200(1)
	std		24, 192(1)
	std		23, 184(1)
	std		22, 176(1)
	std		21, 168(1)
	std		20, 160(1)
	std		19, 152(1)
	std		18, 144(1)
	std		17, 136(1)
	std		16, 128(1)
	std		15, 120(1)
	std		14, 112(1)
#else
	stmw	13, -(32-13)*4(1)
	stw		0, 4(1)
	stwu	1, -((32-13)*4+8)(1)
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
#endif
	// end code
#ifdef MDFNPS3
	ld		31, 248(1)	/* Restore non-volatiles */
	ld		30, 240(1)
	ld		29, 232(1)
	ld		28, 224(1)
	ld		27, 216(1)
	ld		26, 208(1)
	ld		25, 200(1)
	ld		24, 192(1)
	ld		23, 184(1)
	ld		22, 176(1)
	ld		21, 168(1)
	ld		20, 160(1)
	ld		19, 152(1)
	ld		18, 144(1)
	ld		17, 136(1)
	ld		16, 128(1)
	ld		15, 120(1)
	ld		14, 112(1)
	ld		1, 0(1)		/* Restore SP */
	ld		0, 16(1)	/* Restore LR */
	mtlr	0
	ld		2, 40(1)	/* Restore TOC */
#else
	lwz		0, (32-13)*4+8+4(1)
	addi	1, 1, (32-13)*4+8
	mtlr	0
	lmw		13, -(32-13)*4(1)
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
