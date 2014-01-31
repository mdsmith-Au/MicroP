/**
 * Fermat factorization Algorithm
 */ 
#include <math.h>
#include <stdio.h>

void c_fermat_fact(int n, int* f1, int* f2);
int find_square(int a, int* b);


void c_fermat_fact(int n, int* f1, int* f2) {
	
	/* Exit with factors of 0 is negative or 0 number to be factored */
	if (n <= 0) {
		*f1 = 0;
		*f2 = 0;
		return;
	}
	
	/* Number is even; simple /2 calculation */
	if (n % 2 == 0) {
		*f1 = 2;
		*f2 = n/2;
		return;
	}
	
	/* n is now guaranteed to be positive and odd
	   Begin Fermat factorization algorithm */
	
	int x = ceil(sqrt(n));
	int y = 0;
	int y2 = x * x - n;
	while (find_square(y2, &y) == 0) {
		
		x = x + 1;
		y2 = x * x - n;
		
		/* If overflow, break */
		if (y2 < 0) {
			break;
		}
	}
	
	*f1 = x + y;
	*f2 = x - y;
	
	/* Overflow = likely prime #.  Thus, factors = 1 and itself.
	   Not necessarily true, will still fail if one of the factors are too
	   large to find (ex: 8388607 = 178481 * 47) but better
     than a guaranteed wrong answer. Sacrifices speed for correctness;
	   at least 1 and itself are always factors :)	*/
	if (y2 < 0) {
		*f1 = n;
		*f2 = 1;
	}
}

/**
 * Checks if the given argument a is a square number.
 * 
 * Procedure: take an integer, find the square root,
 * convert it to an integer (stripping everything after
 * the decimal) and then square it.  If the input
 * was a square, the integer conversion does nothing
 * and you get the same number as was input.  Otherwise,
 * the result is different because information was lost
 * during the conversion - hence, it is not a square.
 * Returns 1 if square, 0 if not
 */
int find_square(int a, int* b) {
	*b = sqrt(a);
	
	return (*b) * (*b) == a;
}
