/*
 * daubechies4 with lifting scheme
*/

#ifndef _daubechies4_
#define _daubechies4_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include <math.h>	//for sqrt
#include <string.h>	//for memcpy

//constant for lift scheme db4
#define U1_c1 	sqrt(3)
#define P1_c1	U1_c1/4
#define P1_c2	(U1_c1-2)/4
#define N_c1	(U1_c1-1)/sqrt(2)
#define N_c2	(U1_c1+1)/sqrt(2)

//proto
void forwardStep( int N, double *S );
void forwardTrans( int N, double *vec );
void forwardTrans1Level( int N, int Level, double *vec );
void forwardTransNLevel( int N, int Lvl,  double *vec );
void inverseStep(int N, double *S);
void inverseTrans( int N, double *vec );
void inverseTrans1Level( int N, int Level, double *vec );
void inverseTransNLevel( int N, int Lvl,  double *vec );
void merge(int num_sample, double *arr_in);
void split(int num_sample, double *arr_in);

/*
 * split array into odd and even elements
 * even at 1st half
 * odd at 2nd half
*/
void split(int num_sample, double *arr_in)
{
	int start = 1;
	int end = num_sample - 1, i;
	double tmpd;

	while (start < end)
		{
			for ( i = start; i < end; i+=2)
				{
					tmpd = arr_in[i];
					arr_in[i] = arr_in[i+1];
					arr_in[i+1] = tmpd;
				}
			start++;
			end--;
		}
}

/*
 * merge element back with order
 * even0,odd0,even1,odd1,even2,odd2,....
 * */
void merge(int num_sample, double *arr_in)
{
	int half =num_sample >> 1;
	int start = half-1;
	int end = half, i;
	double tmp;

	while (start > 0)
		{
			for (i = start; i < end; i += 2)
				{
					tmp = arr_in[i];
					arr_in[i] = arr_in[i+1];
					arr_in[i+1] = tmp;
				}
			start--;
			end++;
		}
}

/*
 * forward db4 transform 1 level
 * */
void forwardStep( int N, double *S )
{
	int half = N>>1, n;

	// update1
	for (n = 0; n < half; n++)
		S[n] += U1_c1 * S[half+n];

	// predict1
	S[half] = S[half] - P1_c1*S[0] - (P1_c2*S[half-1]);
	for (n = 1; n < half; n++)
		S[half+n] += - P1_c1*S[n] - (P1_c2*S[n-1]);

	// update2
	for (n = 0; n < half-1; n++)
		S[n] -= S[half+n+1];
	S[half-1] -= S[half];

	// normalize
	for (n = 0; n < half; n++)
		{
			S[n] = N_c1 * S[n];
			S[n+half] = N_c2 * S[n+half];
		}
}

/*
 * inverse step 1 level
 * */
void inverseStep(int N, double *S)
{
	int half = N>>1, n;

	//normalize'
	for (n = 0; n < half; n++)
		{
			S[n] = N_c2 * S[n];
			S[n+half] = N_c1 * S[n+half];
		}

	//update2'
	for (n = 0; n < half-1; n++)
		S[n] += S[half+n+1];
	S[half-1] += S[half];

	//predict1'
	S[half] += P1_c1*S[0] + (P1_c2*S[half-1]);
	for (n = 1; n < half; n++)
		S[half+n] += P1_c1*S[n] + (P1_c2*S[n-1]);

	//update1'
	for (n = 0; n < half; n++)
		S[n] -= U1_c1 * S[half+n];
}

/*
 * forward transform multi level
 * */
void forwardTrans( int N, double *vec )
{
	int n;

	for (n = N; n > 1; n >>= 1)
		{
//		split(n, vec );
//		forwardStep( n, vec );
			forwardTrans1Level(n,0,vec);
		}
}

/*
 * inverse transform db4
 * */
void inverseTrans( int N, double *vec )
{
	int n;

	for (n = 2; n <= N; n <<= 1)
		{
//		inverseStep( n, vec );
//		merge( n,vec );
			inverseTrans1Level(n,0,vec);
		}
}

/*
 * forward transform 1 level
 * Level : 1...
 * 0 = count from N
 * */
void forwardTrans1Level( int N, int Level, double *vec )
{
	if(Level==0)
		Level = 1;
	split(N>>(Level-1), vec );
	forwardStep( N>>(Level-1), vec );
}

/*
 * inverse transform db4
 * Level : 1...
 * 0 : count from N
 * */
void inverseTrans1Level( int N, int Level, double *vec )
{
	if(Level==0)
		Level = 1;
	inverseStep( N>>(Level-1), vec );
	merge( N>>(Level-1),vec );
}

/*
 * forward transform N level
 * */
void forwardTransNLevel( int N, int Lvl, double *vec )
{
	int n;

	for (n = N; n > (N>>(Lvl-1)); n >>= 1)
		{
			forwardTrans1Level(n,0, vec);
		}
}

/*
 * inverse transform db4 N level
 * */
void inverseTransNLevel( int N, int Lvl, double *vec )
{
	int n;

	if(Lvl==0) Lvl=1;

	for (n = (N>>(Lvl-1)); n <= N; n <<= 1)
		{
			inverseTrans1Level(n,0, vec);
		}
}

#ifdef __cplusplus
}
#endif

#endif
