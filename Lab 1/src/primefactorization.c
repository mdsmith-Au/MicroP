/**
 * Prime factorization algorithm
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern uint64_t fermat_fact(int n);

void get_factors(int n, int* f1, int* f2);
int partition(int array[], short start, short stop);


/** 
 * Computes the prime factors of a number n. This function is 
 * called recursively, factoring each factor until a factor of 
 * 1 is obtained, meaning the other factor is prime. 
 */
void get_prime_factors(int n, int prime_factors[], short *idx) {
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
	/* Continue, find factors of each factor */
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

/* Standard quicksort */
void quick_sort(int prime_factors[], short start, short stop){
	
	/* Exit if zero or one element, otherwise loop until done */
	if (start < stop) {
			/* Split up, quick sort each piece */
			short pivot = partition(prime_factors, start, stop);
			quick_sort(prime_factors, start, pivot - 1);
			quick_sort(prime_factors, pivot + 1, stop);
	}
	
}

int partition(int array[], short start, short stop) {
	short pivot = array[stop];
	short left = start;
	short right = stop - 1;
	while (left <= right) {
		while ((left <= right) && array[left] < pivot) {
				left = left + 1;
		}
		while ((left <= right) && array[right] >= pivot) {
			right = right - 1;
		}
		
		if (left < right) {
			/* Exchange right, left */
			int temp = array[right];
			array[right] = array[left];
			array[left] = temp;
		}
	}
	
	/* Exchange stop, left */
	int temp = array[stop];
	array[stop] = array[left];
	array[left] = temp;
	
	return left;
}

void insertion_sort(int a[], short n) {
	int k;
	for (k = 0; k < n; k++) {
		int temp = a[k];
		int i = k;
		while (i > 0 && temp < a[i-1]) {
			a[i] = a[i-1];
			i = i - 1;
		}
		a[i] = temp;
	}
}

