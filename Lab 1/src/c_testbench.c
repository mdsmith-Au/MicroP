/**
 * Fermat factorization algorithm test bench
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

void c_fermat_fact(int n, int* f1, int* f2);

extern uint64_t fermat_fact(int n);

/* Testbench to test performance (and correctness) of C vs ASM
End result: ASM is 2.5x to 7x faster, depending on the numbers used */

int c_testbench() {
	
	/* Lists of numbers with their factors in seperate arrays */
	int n_list[] = {-5, 0, 1, 2, 3, 9, 15, 17, 25, 53, 583, 617, 7957, 503809, 887869, 278872, 3273897, 8388608, 787993 };
	int f1_list[] = {0, 0, 1, 2, 3, 3,  5, 17,  5, 53,  53, 617,  109,   3967,  38603,      2,    3421,    2,    787993};
	int f2_list[] = {0, 0, 1, 1, 1, 3,  3,  1,  5,  1,  11,   1,   73,    127,     23, 139436,     957, 4194304,    1}; 
	
	int f1, f2;

	short i;
	for (i = 0; i < sizeof(n_list)/sizeof(int); i++) {
		
		/* Pick between C and ASM here */
		
		/* C */
		//c_fermat_fact(n_list[i], &f1, &f2);
		
		/* ASM */
		uint64_t result = fermat_fact(n_list[i]);
		f1 = result & 0xFFFFFFFF;
		f2 = (result >> 32) & (0xFFFFFFFF);
		/* END ASM  */
		
		printf("N = %d\n", n_list[i]);
		printf("  F1: %d : %d == %d\n", f1_list[i] == f1, f1_list[i], f1);
		printf("  F2: %d : %d == %d\n", f2_list[i] == f2, f2_list[i], f2);
		
		if (f1_list[i] != f1 || f2_list[i] != f2) {
			printf("Number %i failed.\n", n_list[i]);
		}
	}
	
	return 0;
}





