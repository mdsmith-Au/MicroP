/**
 * Prime factorization algorithm
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern uint64_t fermat_fact(int n);

void get_prime_factors(int n, int prime_factors[]);
void get_factors(int n, int* f1, int* f2);

/* The array of prime factors to fine. */
int prime_factors[32];

/* Global index keeps track of prime_factors array position. */
int idx = 0;


int find_prime_factors(int n) {
	// !!!!! FOR TEST PURPOSES ONLY - TO REMOVE !!!!!
	n = 1054638; 	// ans: 2, 3, 3, 13, 4507
	
	/* Initializes array to 0 */
	memset(prime_factors, 0, sizeof(prime_factors)/sizeof(int));
		
	/* Computes prime factors of n and stores them in prime_factors array */
	get_prime_factors(n, prime_factors);
	
	/* Display prime factors */
	int j;
	for (j = 0; j < sizeof(prime_factors)/sizeof(int); j++)
		printf("%d: %d\n", j, prime_factors[j]);
	
	/* TODO: Should prime_factors be sorted? */
	/* TODO: What to return? Pointer to the array? */
	return 0;
}

/** 
 * Computes the prime factors of a number n. This function is 
 * called recursively, factoring each factor until a factor of 
 * 1 is obtained, meaning the other factor is prime. 
 */
void get_prime_factors(int n, int prime_factors[]) {
	int f1, f2;
	get_factors(n, &f1, &f2);
	
	/* Stop condition: one of the factors is 1 */
	if (f2 == 1) {
		prime_factors[idx] = f1;
		idx = idx + 1;
	} 
	else {
		get_prime_factors(f1, prime_factors);
		get_prime_factors(f2, prime_factors);
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
	*f1 = result & 0xFFFFFFFF;
	
	/* The second factor is stored in the upper half */
	*f2 = (result >> 32) & (0xFFFFFFFF);
}

