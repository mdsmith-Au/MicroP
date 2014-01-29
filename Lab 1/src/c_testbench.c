/**
 * Fermat factorization algorithm test bench
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>

void c_fermat_fact(int n, int* f1, int* f2);

int c_testbench() {
	int n_list[] = {-5, 0, 1, 2, 3, 9, 15, 17, 25, 53, 583, 617, 7957, 503809, 887869, 278872, 3273897};
	int f1_list[] = {0, 0, 1, 2, 3, 3,  5, 17,  5, 53,  53, 617,  109,   3967,  38603,      2,    3421};
	int f2_list[] = {0, 0, 1, 1, 1, 3,  3,  1,  5,  1,  11,   1,   73,    127,     23, 139436,     957}; 
	
	int f1, f2;

	int i;
	for (i = 0; i < sizeof(n_list)/sizeof(int); i++) {
		c_fermat_fact(n_list[i], &f1, &f2);
		
		printf("N = %d\n", n_list[i]);
		printf("  F1: %d : %d == %d\n", f1_list[i] == f1, f1_list[i], f1);
		printf("  F2: %d : %d == %d\n", f2_list[i] == f2, f2_list[i], f2);
		
		assert(f1_list[i] == f1);
		assert(f2_list[i] == f2);
	}
	
	return 0;
}





