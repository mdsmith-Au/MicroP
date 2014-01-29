	AREA assembly_testbench, CODE, READONLY
	EXPORT asm_testbench
	
	IMPORT fermat_fact
	; N - number to be factored - is placed in R0 before execution
	; F1, F2 are R0, R1 after execution
	
asm_testbench
	; Load 2 into R0, execute Fermat
	MOV		R0, #2
	LDR     R1, =fermat_fact
	BLX		R1
	
	; Verify Fermat got good factors
	MUL		R0, R0, R1
	CMP		R0, #2
	BNE		error
	
	; Rinse and repeat for other numbers, 11 in this csae
	MOV 	R0, #11
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #11
	BNE		error
	
	; 8
	MOV 	R0, #8
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #8
	BNE		error
	
	; 24
	MOV 	R0, #24
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #24
	BNE		error
	
	; 87 
	MOV 	R0, #87
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #87
	BNE		error
	
	; 88
	MOV 	R0, #88
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #88
	BNE		error
	
	;2876
	MOV 	R0, #2876
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV		R1, #2876
	CMP		R0, R1
	BNE		error
	
	; 0
	MOV 	R0, #0
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #0
	BNE		error
	
	; 1
	MOV 	R0, #1
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #1
	BNE		error
	
	; -985
	MOV 	R0, #-985
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #0
	BNE		error
	
	;25985
	MOV 	R0, #25985
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV		R1, #25985
	CMP		R0, R1
	BNE		error
	
	;2^23
	MOV 	R0, #8388608
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	CMP		R0, #8388608
	BNE		error
	
	;787993
	MOV32 	R0, #787993
	LDR		R1, =fermat_fact
	BLX		R1
	
	MUL		R0, R0, R1
	MOV32	R1, #787993
	CMP		R0, R1
	BNE		error
	
	
	BX		lr
	
error	NOP
		BX lr
		end