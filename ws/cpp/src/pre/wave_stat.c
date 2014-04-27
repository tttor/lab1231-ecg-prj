/*
 * calculate statistic of wavelet from a signal
 * - mean
 * - stdev
 * - min
 * - max
 * - range = max - min
 * - median
 * */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//gsl lib
#define GSL_DISABLE_DEPRECATED 1	//make sure it wont named twice

#include "file_util.h"

int main(int argc, char **argv)
{
	int i,j;
	
	//get fname
	char *fname = argv[1];
	int sample_number_tested = atoi(argv[2]);
	int feature_length = atoi(argv[3]);
	
	if(fname=="")
	{
		printf("no filename!!!");
		return -1;
	}
	
	//read it
	double *data = (double *) malloc(feature_length * sizeof(double));
	printf("\nread input file from %s = %d", fname, read_csv_file(fname, data, feature_length, 1));
	
	//got data
	double *out = (double *) malloc(sample_number_tested * sizeof(double));
	memset(out,0,sample_number_tested * sizeof(double));
	
	for(i=0;i<feature_length;i++)
	{
		out[(int)data[i]] = 1;
	}
	
	//printf out
	printf("\ndata out = ");
	for(i=0;i<sample_number_tested;i++)
	{
		printf("\n%d", (int) out[i]);
	}
	
	return 0;
}
