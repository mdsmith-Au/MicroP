/**
 * @file main.c
 * ECSE 426 Lab 2 main entry point.
 * The main entry point is responsible for configuring and setting up 
 * any and all necessary hardware components for the temperature circuit 
 * to operate correctly.
 */

#include <stdio.h>
#include "adc_init.h"
#include "display_driver.h"
#include "gpio_example.h"
#include "temp_processing.h"

/** 
 * The interval at which the SysTick timer will generate interrupts.
 * Computed as 1/freq * Core_Clock, where freq = 25Hz, Core_Clock = 168MHz.
 */
#define TICK_DELAY 6720000 
//#define TICK_DELAY 2240000 

/** 
 * Software flag set by the SysTick timer when an interrupt is generated 
 * at a fixed interval.
 * @c uint_fast16_t is used so the compiler uses the max speed implementation 
 * dependent on processor architecture 
 */
static volatile uint_fast16_t ticks;

/**
 * Main entry point. Calls various external functions to set up the hardware
 * and controls the main time loop for the SysTick timer.
 * @returns 0 on successful exit, 1 otherwise.
 */
int main() {	
	GPIO_example_config();               // Set up GPIO
	GPIO_SetBits(GPIOD, GPIO_Pin_12);    // Light up green LED
	ADCConfig();                         // Configure ADC (temp sensor @ ADC1_IN16)
	calibrateTemp();                     // Calibrate temp sensor using factory data
	initializeBuffer();                  // Ensure memory is clean in filter
	
	
	ticks = 0;
	SysTick_Config(TICK_DELAY);
	
    // This should be moved somewhere else, to its own file probably
	/* Filter Test 
	uint16_t data[] = {
        1672, 1020, 2672, 1312,  425, 2194,  676, 3618, 2730, 3472, 3124, 3305, 2592, 2910, 2821, 1315,
        2178, 3576,  224, 2050, 1773, 3704, 2581, 4026, 2397, 3443, 1920, 2233,  734, 2599, 3944, 2187,
        1965, 3251,  380, 3607,   16, 2095, 2779, 2317, 1960, 1313, 2464, 3740, 2795, 3877,  406, 2093,
         451, 2233, 2821,  604, 3185, 1635, 3679, 1258,  251,  899,  340, 3892,   68,  470,   51,  886,
          47, 2631, 2118, 1006,  794,  373, 1509,   32, 2469, 1961, 1262, 3049, 3438, 1075, 2106, 1830,
        1398, 3437, 4024, 2566,  743,  504, 2375, 1346, 1099, 2254,  740, 2779,  229,  140, 1174,  317,
        3688, 3467, 1621,  693, 1763, 1705, 2985, 1665, 3898, 3735, 3897, 1417, 1189, 3632,  861,  536,
        2132, 3708, 1649,  884,  323, 3821, 2469, 1546, 2723, 3245, 1366, 2837,  835, 3926, 2915,  684,
        1814, 2593, 3809, 2168, 2566, 2788, 3781,  626, 1662, 1280, 2842, 3648, 2010, 3300, 1337, 2252,
        1593, 3673, 2769, 3393,  451, 1144, 3144,  885,  140, 1788, 3837, 1074, 2334, 1473,  110, 2050,
        3387, 1061,  188, 1010, 2706, 1349, 2701,   54, 2941, 1602,  138, 1663, 2934, 3773, 4030, 4028,
        3671, 3546, 3280, 2273, 1716,  521, 2681, 3538, 1125, 3441,  290, 1552, 1099,  627, 2584, 1296,
        3928, 2043, 3025,   53, 2479, 2361, 3307, 2683, 3597, 3696,  624,  789, 3240,  249, 1597, 1229,
        3007,  427, 3246, 3206, 2181, 1038,  291, 2563,  102,  255,  531, 1846, 2754, 3506, 2042,  200,
        1286, 2628, 3221, 1185, 2039, 3352, 2438, 2197, 1355, 1686, 3252, 1406, 1895, 1507, 2783, 2326,
        2670, 2012, 1632, 1956,  273, 1684, 3969, 3198, 2986, 3136, 3099, 3454, 3154, 4008,  457, 1622,
        2015, 1057,  152, 3991, 2975,  606,  606, 2887, 1561,  313, 1683,  586, 3272, 3810,   20, 2662,
        2779, 1039, 3453, 1204,  110,  383, 3268, 2914, 3209, 2555, 3381,  144, 1661, 1023, 1970, 3608,
        1150, 2454,  108,  636, 3415,  799, 3398, 1385, 2749,  215, 3007, 2046, 3863, 1187, 1543,  466,
        3952, 1772,  347, 2935, 2076, 1344, 3086, 3424, 1039, 2189, 1783,  646, 2459, 3839,  442, 3686,
        2255, 1751,  624, 1014, 1832, 2182, 1453, 3166, 3611, 3007, 1665, 2475, 2626,  522, 2032, 1272,
        2370, 3865, 1749,  136, 3807, 3788, 1468, 1065, 3223, 2095, 2304, 2805,  379, 3574, 3862,  396,
        3464, 3724,   47, 2145, 2664, 1578, 2659, 3124, 2358, 2588, 1140, 3440, 1748, 2587, 3414, 1107,
        1642, 2270, 1818,  371, 3049,  134, 1760,  153, 3996, 2139, 3725, 1570, 3622, 1045, 3723, 3664,
        1632, 2560, 2325, 3664,  878,   16, 3606,  963, 1003, 2625, 1248, 3381, 3619, 3872, 1601, 3282,
         644, 2561, 2863,  352, 2176, 3639, 1080,  962, 3439, 2030,  624,  945, 2695, 2306, 1196, 2549,
        2932, 1150, 1689, 1484, 3200,  555, 3694, 1187, 2046, 3209, 2773,  614, 2853,  529, 3874, 3630,
        2109, 2783, 4000,  514, 3081, 3387, 3200,  782, 1756,   60, 1333,  552, 1845, 2344, 3244, 1719,
        2181, 3791, 3682, 2232, 3691,  213, 3312, 1372,  937, 3368, 1427,  678,  116, 3913, 2786, 3525,
        3846, 2786, 3757, 1052, 3627, 3768, 1229,  301, 3143,  348, 2985, 1835, 2667,  695, 2177, 2596,
          58, 1927, 3630,  467, 1813, 2701, 1208, 3892, 2844,  847, 2272, 3601, 2285, 3081, 3665, 3448,
         536,  775,  630,  119,   38, 2443, 2495, 3763, 3004, 1234, 2030, 1058, 3002,  479, 3056, 3317
    };
    
	for (int i = 0; i < sizeof(data)/sizeof(uint16_t); i++) {
		printf("%u\n", processTemp(data[i]));
	}
    */
	
	while(1) {
		/*
		for (int j = 0; j < 3; j++) {
			int i = 0;
			while(i < 10000) {
				i++;
			}
			draw();
		}	*/
		
		// Wait for interrupt for tick; don't do anything
		while(!ticks);
		
		
		//Great, interrupt! Reset interrupt ticks and do stuff
		ticks = 0;
		
		
		float temp = getAndAverageTemp();
		printf("Temp: %f\n", temp);
		displayNum(temp);
		
		draw();
	}
}

/** 
 * Interrupt handler for the SysTick timer.
 * When SysTick generates an interrupt, set @c ticks to 1
 */
void SysTick_Handler() {
	ticks = 1;
}


