/**
 * Fermat factorization Algorithm
 */ 

#include <math.h>
#include <stdio.h>

void c_fermat_fact(int n, int* f1, int* f2);
int find_square(int y2, int* y);

int main()
{	
	int n = 53817;
	int f1;
	int f2;
	
	c_fermat_fact(n, &f1, &f2);
	
	printf("f1 = %d, f2 = %d", f1, f2);
	
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
	
	/* n > 0 is guaranteed to be true.
	   Begin Fermat factorization algorithm */
	
	int x = ceil(sqrt(n));
	int y = 0;
	int y2 = x * x - n;
	while (find_square(y2, &y) == 0) {
		x = x + 1;
		y2 = x * x - n;
	}
	
	*f1 = x + y;
	*f2 = x - y;	
}

/* Returns 1 if the given parameter y2 is a square number; 0 otherwise 
   Parameter passes the value of y (= sqrt(y2)) */
int find_square(int a, int* b) {
	*b = sqrt(a);
	
	return (*b) * (*b) == a;
}	




