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
	
	/* List of numbers to test */
	int n_list[] = {2875, 8724, 8763, 9813, 54382, 27653};
		
	/* Test numbers, in pairs of 2 */
	for (short i = 0; i < sizeof(n_list)/sizeof(int); i = i+2) {
		
		int num1 = n_list[i];
		int num2 = n_list[i+1];
		
		/* Arrays of prime factors */
		int array1[32];
		int array2[32];
	
		/* Clear each array */
		memset(array1, 0, sizeof(*array1)/sizeof(int));
		memset(array2, 0, sizeof(*array1)/sizeof(int));
		
		/* Index for each array */
		short idx1 = 0;
		short idx2 = 0;
		
		/* Get all prime factors, put into appropriate array */
		get_prime_factors(num1, array1, &idx1);
		get_prime_factors(num2, array2, &idx2);
		
		/* Sort each array in increasing order using normal quicksort */
		quick_sort(array1, 0, idx1 - 1);
		quick_sort(array2, 0, idx2 - 1);
		
		/* Calculate GCD, LCM using the sorted array of prime factors */
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
	}
	
	return 0;
}





