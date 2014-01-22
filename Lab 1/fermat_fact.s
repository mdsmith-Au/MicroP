	area fermat_factorization, code, readonly
	export fermat_fact

fermat_fact
	; Number to be factored
	movs  R1, #21
	
	; movs by default compares against 0
	blt   stop
	beq   stop
	
	; Even case
	tst   R1, #1
	beq   even_case
	
	; Odd case (Fermat factorization algorithm)
	
	; Set FPSCR flag RMode to ceil
	vmrs R7, FPSCR
	bic R7, R7, #0x00c00000
	orr R7, R7, #0x00400000
	vmsr FPSCR, R7
	
	; Converts integer to IEEE 754 floating point
	vmov.F32        S1, R1
	vcvt.F32.U32    S1, S1
	
	vsqrt.F32       S2, S1
	 
	; Floating point to integer
	vcvtr.U32.F32 S2, S2
	vmov.F32 R2, S2 ; x = ceil(sqrt(N))
	
	; Set FPSCR flag RMode back to default
	vmrs R7, FPSCR
	bic R7, R7, #0x00c00000
	vmsr FPSCR, R7
	
	mul R3, R2, R2 ; R3 = R2 * R2 (x * x)
	sub R4, R3, R1 ; R4 = R3 - R1 (z = y^2 = x*x - N)
	
loop
	mov R10, R4
	b find_square
	
is_not_square
	add R2, R2, #1
	mul R3, R2, R2
	sub R4, R3, R1
	b loop
	

factorsoln
	add R5, R2, R11
	sub R6, R2, R11
	
	bx lr	
	
find_square 
	; Finding if a num is square
	; Uses S10 for floating point calculations
	; Set R10 to the number to check
	; Guarantees R11 to hold value of int(y)
	
	; Grab R10, convert to IEE 754
	vmov.F32		S10, R10
	vcvt.F32.U32	S10, S10
	; SQRT of # -> S10
	vsqrt.F32		S10, S10
	
	vcvt.U32.F32	S10, S10
	vmov.F32		R11, S10	; R11 is now the integer of the square root (int(y))
	
	mul				R11, R11, R11  ; R11 = int(y) * int(y)
	cmp				R11, R10       ; compares int(y) * int(y) and y^2
	beq				factorsoln		; if R11 == R10, we are done, goto factorsoln
	b				is_not_square   ; if R11 != R10, run the damn loop
	
	
	
	
even_case
	mov  R5, #2
	lsr  R6, R1, #1
	bx   lr
	
stop

	end
