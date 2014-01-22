;****************************
; Fermat Factorization Algorithm
; Takes an integer as input in register R1
; and finds the factors if possible.
;
; Uses registers **** to ***
;
; By Michael Smith / Kevin Dam
; 260481943 / ********
;
	
	area fermat_factorization, code, readonly
	export fermat_fact

fermat_fact
	; Number to be factored
	; TODO: Parameter passing
	movs  R1, #21
	
	; movs by default compares against 0,
	; so we can use less than, equal to, etc.
	; Below: Exit if number <= 0
	ble   stop
	
	; OK; number is > 0
	; Now, check if number is even/odd using
	; the last bit
	tst   R1, #1
	
	; Number is even, don't need Fermat
	beq   even_case
	
	; Number is odd -> Fermat factorization algorithm
	
	; Set FPSCR flag RMode to ceil
	; Follows example from ARM
	; See http://infocenter.arm.com/help/topic/com.arm.doc.dui0473c/DUI0473C_using_the_arm_assembler.pdf, Ex. 5.19.1
	vmrs R7, FPSCR
	bic R7, R7, #0x00c00000
	orr R7, R7, #0x00400000
	vmsr FPSCR, R7
	
	; Converts integer in R1 to IEEE 754 floating point in S0
	vmov.F32        S0, R1
	vcvt.F32.U32    S0, S0
	
	; Calculate square root
	vsqrt.F32       S0, S0
	 
	; Convert answer back to integer, store in R2
	vcvtr.U32.F32 S0, S0
	vmov.F32 R2, S0 ; x = ceil(sqrt(N))
	
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
	; F1, F2 in R5, R6 respectively
	add R5, R2, R12
	sub R6, R2, R12
	
	bx lr	
	
find_square 
	; Finding if a num is square
	; Takes R10 as input, writes to R11, R12 and S10
	; Guarantees R12 to hold value of int(y)
	
	; Grab R10, convert to IEE 754
	vmov.F32		S10, R10
	vcvt.F32.U32	S10, S10
	; SQRT of # -> S10
	vsqrt.F32		S10, S10
	
	vcvt.U32.F32	S10, S10
	vmov.F32		R11, S10
	
	; Copy int(y) to a seperate register for (possible) usage later
	mov				R12, R11	

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
