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
	stw		2, 40(1)	/* Save original TOC in caller frame */
#endif
	stmw	13, -76(1)	/* Save r13-r31 (below sp) */
	stw		0, 4(1)
	stwu	1, -96(1)	/* Create frame (84 bytes aligned to 16 = 96) */
	
	/* execute code */
	mtctr	3
	mr	31, 4
	mr	30, 5
	bctrl
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
	lwz		0, 96+4(1)	/* Restore LR from frame info */
	addi	1, 1, 96	/* Back to old frame */
	mtlr	0
	lmw		13, -76(1)	/* Restore r13-r31 */
#ifdef MDFNPS3
	lwz		2, 40(1)	/* Restore original TOC */
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
