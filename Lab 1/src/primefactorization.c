/**
 * Prime factorization algorithm
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern uint64_t fermat_fact(int n);

void get_prime_factors(int n, int prime_factors[], int *idx);
void get_factors(int n, int* f1, int* f2);


/** 
 * Computes the prime factors of a number n. This function is 
 * called recursively, factoring each factor until a factor of 
 * 1 is obtained, meaning the other factor is prime. 
 */
void get_prime_factors(int n, int prime_factors[], int *idx) {
	int f1, f2;
	get_factors(n, &f1, &f2);
	
	/* Exit if error (i.e. bad n)*/
	if (f2 == 0) {
		return;
	}
	
	/* Stop condition: one of the factors is 1 */
	if (f2 == 1) {
		prime_factors[*idx] = f1;
		*idx = *idx + 1;
	} 
	else {
		get_prime_factors(f1, prime_factors, idx);
		get_prime_factors(f2, prime_factors, idx);
	}
}

/**
 * Gets the factors f1, f2 of an integer n.
 * f1 and f2 will not necessarily be prime factors.
 */
void get_factors(int n, int* f1, int* f2) {
	/* fermat_fact stores both factors in a single 64-bit int */
	uint64_t result = fermat_fact(n);
	
	/* The first factor is stored in the lower half */
	*f1 = (result & 0xFFFFFFFF);
	
	/* The second factor is stored in the upper half */
	*f2 = ((result >> 32) & (0xFFFFFFFF));
}

