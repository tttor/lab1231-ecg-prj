/*
 * wavelet stat utility
 * author : EX4
 * year : 2013
 * */

#ifndef _wavelet_statistic_
#define _wavelet_statistic_

#ifdef __cplusplus
	extern "C" {
#endif

//lib
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//gsl lib
#include "wavelet_util.h"
#include <gsl/gsl_statistics_double.h>	//for mean and other
#include <gsl/gsl_sort.h>				//sorting the data

//data out struct
typedef struct {
	char *name;
	double mean, median, max, min, range, stdev;	
} 
wavelet_statistic_out;

//void and func
double *wavelet_calc_statistic(int data_len_in, double *data_in, const gsl_wavelet_type *wavelet_type, int wavelet_level, int wavelet_transform_type, char *stat_name, wavelet_statistic_out *statistic_data);

//detil
double *wavelet_calc_statistic(int data_len_in, double *data_in, const gsl_wavelet_type *wavelet_type, int wavelet_level, int wavelet_transform_type,char *stat_name, wavelet_statistic_out *statistic_data)
{	
	double *wavelet_coeff = wavelet_do_transform(data_len_in, data_in, wavelet_type, wavelet_level, wavelet_transform_type);
	
	if(wavelet_coeff != NULL)
	{
		double *tmp_sorted = (double *) malloc(data_len_in * sizeof(double));
		memcpy(tmp_sorted, wavelet_coeff, data_len_in * sizeof(double));
		
		statistic_data->name = stat_name;
		
		statistic_data->stdev = gsl_stats_sd(wavelet_coeff, 1, data_len_in);
		statistic_data->mean = gsl_stats_mean(wavelet_coeff, 1, data_len_in);
		statistic_data->max = gsl_stats_max(wavelet_coeff, 1, data_len_in);
		statistic_data->min = gsl_stats_min(wavelet_coeff, 1, data_len_in);
		statistic_data->range = statistic_data->max - statistic_data->min;		
		
		//sort the data
		gsl_sort(tmp_sorted, 1, data_len_in);
		statistic_data->median = gsl_stats_median_from_sorted_data(tmp_sorted,1,data_len_in);
	}
	
	//return the wavelet coeff
	return wavelet_coeff;
}

#ifdef __cplusplus
}
#endif

#endif
