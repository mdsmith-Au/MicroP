/**
 * Factorization testbench
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>


int gcd_by_prime_fact(int prime_factors_a[], int prime_factors_b[]);
int lcm(int a, int b, int prime_factors[]);
void get_prime_factors(int n, int prime_factors[], int* idx);
void quick_sort(int prime_factors[], int start, int stop);

int fact_testbench() {
	int n_list[] = {-5, 0, 1, 2, 3, 9, 15, 17, 25, 53, 583, 617, 7957, 503809, 887869, 278872, 3273897};

	
		for (int i = 0; i < sizeof(n_list)/sizeof(int); i++) {
	
			int prime_factors[32];
			int index = 0;
			/* Initializes array to 0 */
			memset(prime_factors, 0, sizeof(*prime_factors)/sizeof(int));
			
			get_prime_factors(n_list[i], prime_factors, &index);
			
			printf("Sorting....\n");
			quick_sort(prime_factors, 0, index - 1);
			
			printf("N = %d\n", n_list[i]);
			printf("Prime factors:\n");
			for (int j = 0; j < index; j++){
				printf("%d\n", prime_factors[j]);
			
			}
		}
	
	//call gcd, lcm on factors
	
	//answer
	
	return 0;
}





