/**
 * Factorization testbench
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>


int gcd_by_prime_fact(int prime_factors_a[], short idx_a, int prime_factors_b[], short idx_b);
int lcm_by_prime_fact(int prime_factors_a[], short idx_a, int prime_factors_b[], short idx_b);
void get_prime_factors(int n, int prime_factors[], short* idx);
void quick_sort(int prime_factors[], short start, short stop);

int fact_testbench() {
	int n_list[] = {-5, 0, 1, 2, 3, 9, 15, 17, 25, 53, 583, 617, 7957, 503809, 887869, 278872, 3273897};

		/*
		for (short i = 0; i < sizeof(n_list)/sizeof(int); i++) {
	
			int prime_factors[32];
			short index = 0; */
			/* Initializes array to 0 */
			/*memset(prime_factors, 0, sizeof(*prime_factors)/sizeof(int));
			
			get_prime_factors(n_list[i], prime_factors, &index);
			
			printf("Sorting....\n");
			quick_sort(prime_factors, 0, index - 1);
			
			printf("N = %d\n", n_list[i]);
			printf("Prime factors:\n");
			for (short j = 0; j < index; j++){
				printf("%d\n", prime_factors[j]);
			
			}
		}*/
	
	int num1 = 2875;
	int num2 = 8724;
	
	int array1[32];
	int array2[32];
	memset(array1, 0, sizeof(*array1)/sizeof(int));
	memset(array2, 0, sizeof(*array1)/sizeof(int));
	
	short idx1 = 0;
	short idx2 = 0;
	
	get_prime_factors(num1, array1, &idx1);
	get_prime_factors(num2, array2, &idx2);
	
	quick_sort(array1, 0, idx1 - 1);
	quick_sort(array2, 0, idx2 - 1);
	
	int gcd = gcd_by_prime_fact(array1, idx1, array2, idx2);
	int lcm = lcm_by_prime_fact(array1, idx1, array2, idx2);
	
	printf("Prime factors of %d:\n", num1);
	for (short j = 0; j < idx1; j++){
			printf("%d\n", array1[j]);
	}
		
	printf("Prime factors of %d:\n", num2);
	for (short j = 0; j < idx1; j++){
			printf("%d\n", array2[j]);
	}
	
	printf("GCD: %d\n", gcd);
	printf("LCM : %d\n", lcm);
	
	return 0;
}





