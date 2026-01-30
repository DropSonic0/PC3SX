###FUNCTION ENTRY POINT DECLARATION###
.section .opd, "aw"

recRun:  
   .long .recRun, .TOC.@tocbase32

returnPC:  
   .long .returnPC, .TOC.@tocbase32


###FUNCTION CODE HERE###
# void recRun(u32 *func,u32 hw1,u32 hw2)
.text
.align  4
.globl  .recRun
.recRun:
	# prologue code 
	/* prologue code */
	mflr	r0
	stmw	r14, -(32-14)*4(r1)
	stw		r0, 4(r1)
	stwu	r1, -((32-14)*4+8)(r1)
	
	/* execute code */
	mtctr	r3
	bctrl


	
.text
.align  4
.globl  returnPC
.returnPC:
	# end code
	lwz		r0, (32-14)*4+8+4(r1)
	addi	r1, r1, (32-14)*4+8
	mtlr	r0
	lmw		r14, -(32-14)*4(r1)
	blr

