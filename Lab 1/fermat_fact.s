; *******************************************************
; Fermat Factorization Algorithm
;
; Takes an integer as input in register R1 (N)
; and finds the factors if possible.
;
; Input registers:
;   R1     holds an integer N
; Output registers:
;   R5     holds the first factor F1 
;   R6     holds the second factor F2
;
; Uses registers R1 to R3, R5, R6, R7, R10 - R12
; Uses FPU registers S0, S10
;
; By: Michael Smith | Kevin Dam
;         260481943 | 260457383
; *******************************************************
	
	area fermat_factorization, code, readonly
	export fermat_fact

fermat_fact
	; Number to be factored
	; TODO: Parameter passing
	mov             R1, #53817
	
	; Check if N <= 0. If yes, exit.
	cmp             R1, #0
	ble             stop
	
	; N > 0 is guaranteed to be true
	; Checks the LSB to determine parity of N
	tst             R1, #1
	
	; Number is even, don't need Fermat
	beq             even_case
	
	; Number is odd -> execute Fermat factorization algorithm
	
	; Set FPSCR flag RMode (rounding mode) to round up (ceil function)
	; Follows example from ARM; See Ex. 5.19.1 of
	; http://infocenter.arm.com/help/topic/com.arm.doc.dui0473c/DUI0473C_using_the_arm_assembler.pdf,
	; We reserve R7 in this subroutine for FPSCR configuration
	vmrs            R7, FPSCR              ; loads FPSCR flags to temp register
	bic             R7, R7, #0x00c00000    ; clears RMode flag bits
	orr             R7, R7, #0x00400000    ; sets RMode bits to round up mode
	vmsr            FPSCR, R7              ; stores modified flags to FPSCR
	
	; Convert integer in R1 to IEEE 754 floating point in S0
	vmov.F32        S0, R1
	vcvt.F32.U32    S0, S0
	
	; Calculate square root
	vsqrt.F32       S0, S0
	 
	; Convert result back to integer, store in R2
	; Uses FPSCR RMode flag (set to round up)
	vcvtr.U32.F32   S0, S0
	vmov.F32        R2, S0                 ; x = ceil(sqrt(N))
	
	; Set FPSCR flag RMode back to default
	vmrs            R7, FPSCR              ; copies FPSCR flags to temp register
	bic             R7, R7, #0x00c00000    ; clears RMode flag bits
	vmsr            FPSCR, R7              ; stores modified flags to FPSCR
	
	mul             R3, R2, R2             ; R3 = x * x 
	sub             R3, R3, R1             ; R3 = y^2 = x*x - N)
	
loop
	mov             R10, R3
	b               find_square
	
is_not_square
	add             R2, R2, #1             ; x = x + 1
	mul             R3, R2, R2             ; Replace old y^2 with new y^2 = x * x
	sub             R3, R3, R1             ; y^2 = x * x - N
	b               loop
	

factorsoln
	add             R5, R2, R12            ; F1 = R5 = x + y
	sub             R6, R2, R12            ; F2 = R6 = x - y
	
	bx              lr	
	
find_square 
	; Finding if a num is square
	; Takes R10 as input, writes to R11, R12 and S10
	; Guarantees R12 to hold value of int(y)
	
	; Idea: take an integer, find the square root,
	; convert it to an integer (stripping everything after
	; the decimal) and then square it.  If the input
	; was a square, the integer conversion does nothing
	; and you get the same number as was input.  Otherwise,
	; the result is different because information was lost
	; during the conversion - hence, it is not a square.
	
	; Grab R10, convert to IEEE 754
	vmov.F32		S10, R10
	vcvt.F32.U32	S10, S10
	
	vsqrt.F32		S10, S10               ; S10 = sqrt(R10)
	
	; Convert sqrt to integer: int(y)
	vcvt.U32.F32	S10, S10
	vmov.F32		R11, S10
	
	; Copy int(y) to a seperate register for (possible) usage later
	mov				R12, R11	

	mul				R11, R11, R11          ; R11 = int(y) * int(y)
	cmp				R11, R10               ; compares int(y) * int(y) and y^2
	beq				factorsoln		       ; if R11 == R10, we are done, goto factorsoln
	b				is_not_square          ; if R11 != R10, run the loop
	
even_case
	; Set R5 (F1) to 2
	; Set R6 (F2) to N/2 by shifting, return
	mov             R5, #2
	lsr             R6, R1, #1
	bx              lr
	
stop
	; Set R5, R6 to 0 (F1, F2), return
	mov             R5, #0
	mov             R6, #0
	bx	            lr
	end
