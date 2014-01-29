/**
 * GCD Algorithm
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

extern uint64_t fermat_fact(int n);

int gcd(int a, int b) {
		
	while (b != 0) {
		int tmp = b;
		b = a % b;
		a = tmp;
	}
	
	return a;
}
