/**
 * LCM Algorithm
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

int gcd(int a, int b);

int lcm(int a, int b) {
		
		return (a * b)/gcd(a,b);
}
