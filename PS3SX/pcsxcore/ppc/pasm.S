###FUNCTION ENTRY POINT DECLARATION###
.section .opd, "aw"

.globl recRun
recRun:  
   .long .recRun, .TOC.@tocbase32

.globl returnPC
returnPC:  
   .long .returnPC, .TOC.@tocbase32


###FUNCTION CODE HERE###
# void recRun(u32 *func,u32 hw1,u32 hw2)
.text
.align  4
.globl  .recRun
.recRun:
	# prologue code
	mflr	0            # move from LR to r0
	stmw	14, -72(1)   # store non-volatiles (-72 == -(32-14)*4)
	stw		0, 16(1)     # store old LR in caller's frame (offset 16 for PS3 ABI)
	mr		12, 1        # copy old SP to scratch register
	stwu	12, -112(1)  # update SP and store back-chain using scratch to avoid RS=RA
	
	# execute code
	mtctr	3            # move func ptr to ctr
	mr	31, 4            # save hw1 to r31
	mr	30, 5            # save hw2 to r30
	bctrl                # branch to ctr (*func)
	
	
.text
.align  4
.globl  .returnPC
.returnPC:
	# end code
	lwz		0, 128(1)    # re-load LR (112 + 16)
	mtlr	0            # set LR
	lmw		14, 40(1)    # reload non-volatiles (newSP + 40 == oldSP - 72)
	addi	1, 1, 112    # increment SP
	blr                  # return
