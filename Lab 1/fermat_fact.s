	area fermat_factorization, code, readonly
	export fermat_fact

fermat_fact
	; Number to be factored
	movs  R1, #21
	
	; Negative/nonzero case
	;cmp  R1, #0
	blt   stop
	beq   stop
	
	; Even case
	tst   R1, #1
	beq   even_case
	
	; Odd case (Fermat factorization algorithm)
	
	; Set FPSCR flag RMode
	vmrs R10, FPSCR
	bic R10, R10, #0x00c00000
	orr R10, R10, #0x00400000
	vmsr FPSCR, R10
	
	; Converts integer to IEEE 754 floating point
	vmov.F32        S1, R1
	vcvt.F32.U32    S1, S1
	
	vsqrt.F32       S2, S1
	 
	; Floating point to integer
	vcvtr.U32.F32 S2, S2
	vmov.F32 R2, S2
	
	mul R3, R2, R2
	sub R3, R3, R1
	
	
	
	 
	
	
even_case
	mov  R5, #2
	lsr  R6, R1, #1
	bx   lr
	
stop

	end
