	AREA assembly_testbench, CODE, READONLY
	EXPORT asm_testbench
	
	IMPORT fermat_fact
	; N - number to be factored - is placed in R0 before execution
	; F1, F2 are R0, R1 after execution
	
asm_testbench
	; Load -5 into R0, execute Fermat
	MOV		R0, #-5
	LDR     R1, =fermat_fact
	BLX		R1
	
	; We multiply the factors to get the final result, and compare it to an expected value
	; If it doesn't match, we branch to error; otherwise, we continue
	; In this case, -5 is invalid so the answer should be 0
	MUL		R0, R0, R1
	CMP		R0, #0
	BNE.W	error
	
	; Rinse and repeat for other numbers
	MOV 	R0, #0
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #0
	BNE.W	error
	
	; # 1
	MOV 	R0, #1
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #1
	BNE.W	error
	
	; # 2
	MOV 	R0, #2
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #2
	BNE.W	error
	
	; # 3
	MOV 	R0, #3
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #3
	BNE.W	error
	
	; # 9
	MOV 	R0, #9
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #9
	BNE.W	error
	
	; # 15
	MOV 	R0, #15
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #15
	BNE.W	error
	
	; # 17
	MOV 	R0, #17
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #17
	BNE.W	error
	
	; # 25
	MOV 	R0, #25
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #25
	BNE.W	error
	
	; # 53
	MOV 	R0, #53
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #53
	BNE.W	error
	
	; # 583
	MOV 	R0, #583
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	; # Many numbers cannot be represented in the immediate field of CMP; this is a workaround
	MOV		R1, #583
	CMP		R0, R1
	BNE.W	error
	
	; # 617
	MOV 	R0, #617
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV		R1, #617
	CMP		R0, R1
	BNE.W	error
	
	; # 7957
	MOV 	R0, #7957
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV		R1, #7957
	CMP		R0, R1
	BNE.W	error
	
	; # 503809
	; Note: some numbers (like the one below) can't be placed in normal
	; MOV operations as immediates - need special Macro
	; MOV32 specified as a solution in ARM InfoCenter documentation
	MOV32 	R0, #503809
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV32	R1, #503809
	CMP		R0, R1
	BNE.W	error
	
	; # 887869
	MOV32 	R0, #887869
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV32	R1, #887869
	CMP		R0, R1
	BNE.W	error
	
	; # 287872
	MOV32 	R0, #287872
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV32	R1, #287872
	CMP		R0, R1
	BNE.W	error
	
	; # 3273897
	MOV32 	R0, #3273897
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV32	R1, #3273897
	CMP		R0, R1
	BNE.W	error
	
	; 2^23 -> should be OK
	MOV 	R0, #8388608
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #8388608
	BNE.W	error
	
	; 787993
	; Will produce an error because of lack of precision when calculating the square root
	MOV32 	R0, #787993
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV32	R1, #787993
	CMP		R0, R1
	BNE.W	error
	
	
error	
	;NOP is to give us some time when debugging
	NOP
	; Align needed to avoid compiler padding warning
	ALIGN
	END