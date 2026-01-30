
#if defined (__ppc__) || defined (__ppc64__) || defined (__powerpc__) || (__powerpc64__) || defined __POWERPC__

#if defined (ELF) || defined (__linux__) || defined(MDFNPS3) //Token decoration
#define C(label) label
#else
#define C(label) _##label
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
	stmw	13, -(32-13)*4(1)
	stw		0, 4(1)
	stwu	1, -((32-13)*4+8)(1)
	
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
        .globl  C(returnPC)
C(returnPC):
	// end code
	lwz		0, (32-13)*4+8+4(1)
	addi	1, 1, (32-13)*4+8
	mtlr	0
	lmw		13, -(32-13)*4(1)
	blr
//}*/

// Memory functions that only works with a linear memory

        .text
        .align  4
        .globl  C(dynMemRead8)
C(dynMemRead8):
// assumes that memory pointer is in r30
	addis    2,3,-0x1f80
	srwi.     4,2,16
	bne+     .norm8
	cmplwi   2,0x1000
	blt-     .norm8
	b        C(psxHwRead8)
.norm8:
	clrlwi   5,3,3
	lbzx     3,5,30
	blr

        .text
        .align  4
        .globl  C(dynMemRead16)
C(dynMemRead16):
// assumes that memory pointer is in r30
	addis    2,3,-0x1f80
	srwi.     4,2,16
	bne+     .norm16
	cmplwi   2,0x1000
	blt-     .norm16
	b        C(psxHwRead16)
.norm16:
	clrlwi   5,3,3
	lhbrx    3,5,30
	blr

        .text
        .align  4
        .globl  C(dynMemRead32)
C(dynMemRead32):
// assumes that memory pointer is in r30
	addis    2,3,-0x1f80
	srwi.     4,2,16
	bne+     .norm32
	cmplwi   2,0x1000
	blt-     .norm32
	b        C(psxHwRead32)
.norm32:
	clrlwi   5,3,3
	lwbrx    3,5,30
	blr

/*
	N P Z
	0 0 0 X
-	0 0 1 X
	1 0 0 X
	1 0 1 X

P | (!N & Z)
P | !(N | !Z)
*/

        .text
        .align  4
        .globl  C(dynMemWrite32)
C(dynMemWrite32):
// assumes that memory pointer is in r30
	addis    2,3,-0x1f80
	srwi.    5,2,16
	bne+     .normw32
	cmplwi   2,0x1000
	blt      .normw32
	b        C(psxHwWrite32)
.normw32:
	mtcrf    0xFF, 3
	clrlwi   5,3,3
	crandc   0, 2, 0
	cror     2, 1, 0
	bne+     .okw32
	// write test
	li			2,0x0130
	addis    2,2,0xfffe
	cmplw    3,2
	bnelr
.okw32:
	stwbrx   4,5,30
	blr

#endif
