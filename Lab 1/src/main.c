/**
 * Fermat factorization Algorithm
 */ 

#include <math.h>
#include <stdio.h>

void c_fermat_fact(int n, int* f1, int* f2);
int find_square(int a, int* b);

int main()
{	
	int n = 787993;
	int f1;
	int f2;
	
	c_fermat_fact(n, &f1, &f2);
	
	printf("f1 = %u, f2 = %u\n", f1, f2);
	
	return 0;
}

void c_fermat_fact(int n, int* f1, int* f2) {
	
	if (n <= 0) {
		*f1 = 0;
		*f2 = 0;
		return;
	}
	
	if (n % 2 == 0) {
		*f1 = 2;
		*f2 = n/2;
		return;
	}
	
	/* n is guaranteed to be positive and odd
	   Begin Fermat factorization algorithm */
	
	int x = ceil(sqrt(n));
	int y = 0;
	int y2 = x * x - n;
	while (find_square(y2, &y) == 0) {
		x = x + 1;
		y2 = x * x - n;
		
		if (y2 < 0) break; /* overflow */
	}
	
	*f1 = x + y;
	*f2 = x - y;
	
	/* FIXME: Not quite true. Will still fail if one of the factors are too
	   large to find. Ex: 8388607 = 178481 * 47 */
	if (y2 < 0) {
		*f1 = n;
		*f2 = 1;
	}
}

/**
 * Checks if the given argument a is a square number.
 * Returns  1 if a is a square number
 *          0 if a is not a square number
 */
int find_square(int a, int* b) {
	*b = sqrt(a);
	
	return (*b) * (*b) == a;
}
