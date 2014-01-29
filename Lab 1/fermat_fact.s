; *******************************************************
; Fermat Factorization Algorithm
;
; Takes an integer as input in register R0 (N)
; and finds the factors if possible.
;
; Input registers:
;   R0     holds an integer N
; Output registers:
;   R0     holds the first factor F1 
;   R1     holds the second factor F2
;
; USES REGISTERS *********************************
;
; By: Michael Smith | Kevin Dam
;         260481943 | 260457383
; *******************************************************
	
	area fermat_factorization, code, readonly
	export fermat_fact

fermat_fact
	
	push			{R2 - R4, LR}
	; Check if N <= 0. If yes, exit.
	cmp             R0, #0
	ble             stop
	
	; N > 0 is guaranteed to be true
	; Checks the LSB to determine parity of N
	tst             R0, #1
	
	; Number is even, don't need Fermat
	beq             even_case
	
	; Number is odd -> execute Fermat factorization algorithm
	
	; Set FPSCR flag RMode (rounding mode) to round up (ceil function)
	; Follows example from ARM; See Ex. 5.19.1 of
	; http://infocenter.arm.com/help/topic/com.arm.doc.dui0473c/DUI0473C_using_the_arm_assembler.pdf,
	vmrs            R1, FPSCR              ; loads FPSCR flags to temp register
	bic             R1, R1, #0x00c00000    ; clears RMode flag bits
	orr             R1, R1, #0x00400000    ; sets RMode bits to round up mode
	vmsr            FPSCR, R1              ; stores modified flags to FPSCR
	
	; Convert integer in R0 to IEEE 754 floating point in S0
	vmov.F32        S0, R0
	vcvt.F32.U32    S0, S0
	
	; Calculate square root
	vsqrt.F32       S0, S0
	 
	; Convert result back to integer, store in R1
	; Uses FPSCR RMode flag (set to round up)
	vcvtr.U32.F32   S0, S0
	vmov.F32        R1, S0                 ; R1 = x = ceil(sqrt(N))
	
	; Set FPSCR flag RMode back to default
	vmrs            R2, FPSCR              ; copies FPSCR flags to temp register
	bic             R2, R2, #0x00c00000    ; clears RMode flag bits
	vmsr            FPSCR, R2              ; stores modified flags to FPSCR
	
	mul             R2, R1, R1             ; R2 = x * x 
	sub             R2, R2, R0             ; R2 = y^2 = x*x - N)
	
loop
	b               find_square
	
is_not_square
	add             R1, R1, #1             ; x = x + 1
	mul             R2, R1, R1             ; Replace old y^2 with new (partial) y^2 = x * x
	sub             R2, R2, R0             ; y^2 = x * x - N
	b               loop
	

factorsoln
	add             R0, R1, R4            ; F1 = R0 = x + y
	sub             R1, R1, R4            ; F2 = R1 = x - y
	
	b				exit
	
find_square 
	; Finding if a num is square
	; Takes R2 as input, writes to **************************************
	
	; Idea: take an integer, find the square root,
	; convert it to an integer (stripping everything after
	; the decimal) and then square it.  If the input
	; was a square, the integer conversion does nothing
	; and you get the same number as was input.  Otherwise,
	; the result is different because information was lost
	; during the conversion - hence, it is not a square.
	
	; Grab R2, convert to IEEE 754
	vmov.F32		S0, R2
	vcvt.F32.U32	S0, S0
	
	vsqrt.F32		S0, S0	              ; S10 = sqrt(R10)
	
	; Convert sqrt to integer: int(y)
	vcvt.U32.F32	S0, S0
	vmov.F32		R3, S0
	
	; Copy int(y) to a seperate register for (possible) usage later
	mov				R4, R3	

	mul				R3, R3, R3         		 ; R3 = int(y) * int(y)
	cmp				R2, R3              	 ; compares int(y) * int(y) and y^2
	beq				factorsoln		     	 ; if R3 == R2, we are done, goto factorsoln
	b				is_not_square         	 ; if R3 != R2, run the loop
	
even_case
	; Set R0 (F1) to 2
	; Set R1 (F2) to N/2 by shifting, return
	lsr             R1, R0, #1
	mov             R0, #2
	b				exit
	
stop
	; Set R0, R1 to 0 (F1, F2), return
	mov             R0, #0
	mov             R1, #0
	b				exit

exit
	pop				{R2 - R4, PC}
	end
