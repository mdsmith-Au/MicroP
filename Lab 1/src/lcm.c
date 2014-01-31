/**
 * LCM Algorithm
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

/* Calculates the Least Common Multiplier between two numbers, a and b.
   Unlike standard LCM algorithms, however, this algorithm uses the fact
	 that the prime factors have already been computed (a lengthy calculation)
	 and as such requires arrays containing the prime factors
	 of both numbers along with an integer pointing to the next
	 empty position in the list.  Lists must be sorted in increasing order.
	 
	 A faster implementation could use a precomputed GCD to calculate the LCM, but
	 we do not assumed GCD is available here, only prime factors.*/
int lcm_by_prime_fact(int prime_factors_a[], short idx_a, int prime_factors_b[], short idx_b) {
	
	/* Current position in arrays */
	short current_pos_a = 0, current_pos_b = 0;
	int lcm = 1;
	
	short stay_in_loop = 1;
	
	/* Check elements in first list against
	elements in second */
	while (stay_in_loop) {
		
		/* Get # in a,b; compute only once for speed */
		int num_a = prime_factors_a[current_pos_a];
		int num_b = prime_factors_b[current_pos_b];
		
		/* num_a > b; multiply b and move to next b */
		if (num_a > num_b) {
			lcm *= num_b;
			current_pos_b++;
		}
		
		/* Match, multiply only one number and move both a & b */
		else if (num_a == num_b) {
			lcm *= num_a;
			current_pos_a++;
			current_pos_b++;
		}
		/* num_a < num_b, multiply a and move to next a*/
		else {
			lcm *= num_a;
			current_pos_a++;
		}
		
		/* While the above code works, it can fail to check numbers
		 in an array if it reaches the end of one but not the other
		(whether it's a > b, a < b or a = b).
		 Thus, we check if pointers are out of bounds and
		 take appropriate action */

		/* Essentially booleans; calculate once for efficiency */
		char a_out_of_bounds = (current_pos_a >= idx_a);
		char b_out_of_bounds = (current_pos_b >= idx_b);
		
		/* a out of bounds only; get remaining numbers in b */
		if (a_out_of_bounds && !b_out_of_bounds) {
			while (current_pos_b < idx_b) {
				num_b = prime_factors_b[current_pos_b];
				lcm *= num_b;
				current_pos_b++;
			}
			stay_in_loop = 0;
		}
		
		/* b out of bounds only; get remaining numbers in a */
		else if (!a_out_of_bounds && b_out_of_bounds) {
			while (current_pos_a < idx_a) {
				num_a = prime_factors_a[current_pos_a];
				lcm *= num_a;
				current_pos_a++;
			}
			stay_in_loop = 0;
		}
		
		/* Both out of bounds; exit */
		else if (a_out_of_bounds && b_out_of_bounds){
			stay_in_loop = 0;
		}
			
	}
	return lcm;
}
