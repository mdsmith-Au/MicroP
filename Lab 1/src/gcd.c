/**
 * GCD Algorithm
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

/* Calculates the Greatest common division between two numbers, a and b.
   Unlike standard GCD algorithms, however, this algorithm uses the fact
	 that the prime factors have already been computed (a lengthy calculation)
	 and as such requires arrays containing the prime factors
	 of both numbers along with an integer pointing to the next
	 empty position in the list.  Lists must be sorted in increasing order.*/
int gcd_by_prime_fact(int prime_factors_a[], short idx_a, int prime_factors_b[], short idx_b) {
	
	/* Current position in arrays */
	short current_pos_a = 0, current_pos_b = 0;
	int gcd = 1;
	
	/* Check elements in first list against
	elements in second.  We only consider elements
  in both lists for GCD	*/
	while ((current_pos_a < idx_a) && (current_pos_b < idx_b)) {
		
		/* Get # in a,b; compute only once for speed */
		int num_a = prime_factors_a[current_pos_a];
		int num_b = prime_factors_b[current_pos_b];
		
		/* num_a > b; a bigger; ignore check next b*/
		if (num_a > num_b) {
				current_pos_b++;
		}
		
		/* Match, add to gcd computation and move to next a & b */
		else if (num_a == num_b) {
			gcd *= num_a;
			current_pos_a++;
			current_pos_b++;
		}
		/* num_a < num_b; no match, ignore and check next a */
		else {
			current_pos_a++;
		}
	}
	return gcd;
}
