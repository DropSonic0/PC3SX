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
	mflr	r0            # move from LR to r0
	stmw	r14, -72(r1)  # store non-volatiles (-72 == -(32-14)*4)
	stw		r0, 4(r1)     # store old LR
	stwu	r1, -80(r1)   # increment and store sp (-80 == -((32-14)*4+8))
	
	# execute code
	mtctr	r3            # move func ptr to ctr
	mr	r31, r4         # save hw1 to r31
	mr	r30, r5         # save hw2 to r30
	bctrl               # branch to ctr (*func)
	
	
.text
.align  4
.globl  returnPC
.returnPC:
	# end code
	lwz		r0, 84(r1)    # re-load LR (84 == (32-14)*4+8+4)
	addi	r1, r1, 80    # increment SP (80 == (32-14)*4+8)
	mtlr	r0            # set LR
	lmw		r14, -72(r1)  # reload non-volatiles (-72 == -((32-14)*4))
	blr                 # return
