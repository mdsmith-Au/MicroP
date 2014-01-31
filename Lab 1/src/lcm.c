/**
 * LCM Algorithm
 */ 

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>


int lcm_by_prime_fact(int prime_factors_a[], short idx_a, int prime_factors_b[], short idx_b) {
	
	short current_pos_a = 0, current_pos_b = 0;
	int lcm = 1;
	short stay_in_loop = 1;
	
	/* Check elements in first list against
	elements in second */
	while (stay_in_loop) {
		
		/* Get # in a; compute only once for speed */
		int num_a = prime_factors_a[current_pos_a];
		int num_b = prime_factors_b[current_pos_b];
		
		/* num_a > b; a bigger; numtiply b and move pointer to bigger value */
		if (num_a > num_b) {
			printf("Multiplying LCM %d by %d\n", lcm, num_b);
			lcm *= num_b;
			current_pos_b++;
		}
		
		/* Match, multiply only once and move a & b */
		else if (num_a == num_b) {
			printf("Multiplying LCM %d by %d\n", lcm, num_a);
			lcm *= num_a;
			current_pos_a++;
			current_pos_b++;
		}
		/* num_a < num_b, multiply aand increase pointer */
		else {
			printf("Multiplying LCM %d by %d\n", lcm, num_a);
			lcm *= num_a;
			current_pos_a++;
		}
		
		/* Out of bounds check */
		char a_out_of_bounds = (current_pos_a >= idx_a);
		char b_out_of_bounds = (current_pos_b >= idx_b);
		
		/* a out of bounds; get remaining b */
		if (a_out_of_bounds && !b_out_of_bounds) {
			while (current_pos_b < idx_b) {
				num_b = prime_factors_b[current_pos_b];
				printf("Multiplying LCM %d by %d\n", lcm, num_b);
				lcm *= num_b;
				current_pos_b++;
			}
			stay_in_loop = 0;
		}
		
		/* b out of bounds; get remaining a */
		else if (!a_out_of_bounds && b_out_of_bounds) {
			while (current_pos_a < idx_a) {
				num_a = prime_factors_a[current_pos_a];
				printf("Multiplying LCM %d by %d\n", lcm, num_a);
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
