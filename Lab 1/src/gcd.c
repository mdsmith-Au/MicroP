/**
 * GCD Algorithm
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

int gcd(int a, int b) {
		
	while (b != 0) {
		int tmp = b;
		b = a % b;
		a = tmp;
	}
	
	return a;
}
