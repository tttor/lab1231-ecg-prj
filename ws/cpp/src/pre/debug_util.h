/*
 * utility function
 *
 */
#ifndef _debug_util_
#define _debug_util_

#include <stdio.h>
#include <math.h>

/*
 * proto function
*/
void pr_debug(char *str_pre, int N, char *fmt, double *ptr);
double calc_sqrt(int N, int Level, double *vec);

/*
* print debug information
*
* */
void pr_debug(char *str_pre, int N, char *fmt, double *ptr) {
	printf("%s", str_pre);
	while(N--)
		printf(fmt, *ptr++);
}

/*
 * find square root energi
 * Level : 1...
 * */
double calc_sqrt(int N, int Level, double *vec) {
	double tmp = 0;
	int n = N>>((Level==0) ? 1:Level),
	    start_idx;

	start_idx = n;
	while(n--)
		tmp += vec[start_idx] * vec[start_idx++];
	return sqrt(tmp);
}

#endif
