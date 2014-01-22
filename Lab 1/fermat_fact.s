	area fermat_factorization, code, readonly
	export fermat_fact

fermat_fact
	mov R1, #6	; Number to be factored
	cmp R1, #0  ; Compare with 0
	
	end