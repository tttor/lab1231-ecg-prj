/*
 * wavelet utility
 * author : EX4
 * year : 2013
 * */

#ifndef _wavelet_util_
#define _wavelet_util_

#ifdef __cplusplus
	extern "C" {
#endif

//lib
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//gsl lib
#include <gsl/gsl_wavelet.h>
//#include </usr/local/include/gsl/gsl_wavelet.h>

//macros for real and imaginary part extract
#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

//void and func
double *wavelet_do_transform(int len_in, double *data_in, const gsl_wavelet_type *wavelet_type, int wavelet_level, int wavelet_transform_type);
double *wavelet_scale_detail_coeff(int len_in, double *data_in, double scale, int level_detail);
int wavelet_find_detail_level_from_freq(int freq_sampling, double freq_in);

//detil
/*
 * decompose input into its wavelet component
 * wavelet type can be :
 * gsl_wavelet_daubechies
 * gsl_wavelet_daubechies_centered
 * gsl_wavelet_haar
 * gsl_wavelet_haar_centered
 * gsl_wavelet_bspline
 * gsl_wavelet_bspline_centered
 * wavelet_transform_type =
 *  1  = forward
 *  -1 = inverse
 * wavelet_level = 2,4,6,8,....
 * */
double *wavelet_do_transform(int len_in, double *data_in, const gsl_wavelet_type *wavelet_type, int wavelet_level, int wavelet_transform_type)
{
	//var
	double *data_out;
	
	//wavelet var
	gsl_wavelet *w;
	gsl_wavelet_workspace *work;
	
	//allocate memory for output
	data_out = (double *) malloc(len_in * sizeof(double));
	
	//copy data in
	memcpy(data_out, data_in, len_in * sizeof(double));
	
	//allocate wavelet var used
	w = gsl_wavelet_alloc(wavelet_type, wavelet_level);
	work = gsl_wavelet_workspace_alloc(len_in);
	
	//allocation memory ok???
	if((data_out != NULL) && (w != NULL) && (work != NULL))
	{
		//do wavelet transform on all level
		if(wavelet_transform_type == 1)	//transform
			gsl_wavelet_transform_forward (w, data_out, 1, len_in, work);
		else //inverse
			gsl_wavelet_transform_inverse (w, data_out, 1, len_in, work);		
		
		//free resource using in wavelet
		gsl_wavelet_free (w);
		gsl_wavelet_workspace_free (work);		
	}
	
	return data_out;
} 
 
/*
 * scale some detail coeff level of data_input wavelet
 * 
 * */
double *wavelet_scale_detail_coeff(int len_in, double *data_in, double scale, int level_detail)
{
	int i,
		max_level = log(len_in) / log(2);
	int level_ok = (level_detail<1) ? 1: (level_detail>max_level) ? (max_level):level_detail;
	
	//allocate memory
	double *data_out = (double *) malloc(len_in * sizeof(double));
	
	//debug
	//printf("\nscale = %g\tlevel = %d\n", scale, level_ok);
	
	if(data_out != NULL)
	{
		//copy data in
		memcpy(data_out, data_in, len_in * sizeof(double));
		
		//debug
		//~ printf("\nd1 removed input\n");
		//~ for (i = 0; i < len_in; i++)
		//~ {
		  //~ printf ("%g\n", data_out[i]);
		//~ }
		
		//multiply with scale
		//~ printf("\nscaling element %d to %d\n", (len_in>>level_ok),  (len_in>>(level_ok-1))-1);
		for(i=(len_in>>level_ok);i<(len_in>>(level_ok-1));i++)
			data_out[i] *= scale;
			
		if(level_ok==max_level)
		{
			data_out[0] *= scale; 
			data_out[1] *= scale;		
		}
			
		//debug
		//~ printf("\nd1 after removed input\n");
		//~ for (i = 0; i < len_in; i++)
		//~ {
		  //~ printf ("%g\n", data_out[i]);
		//~ }
	}
	
	return data_out;
}

/*
 * find level detail of a frequency
 * level start from 1
 * */
int wavelet_find_detail_level_from_freq(int freq_sampling, double freq_in) 
{
	int i, i_max = log(freq_sampling) / log(2);
	double f1 = freq_sampling;	
	
	for(i=1;i<=i_max;i++)
	{
		f1 /= 2;
		if(f1<=freq_in)
		{
			break;
		}
	}
	
	return i;
}

#ifdef __cplusplus
}
#endif

#endif
