/*
 * fft resampler with gsl
 * author : EX4
 * year : 2013
 * */

#ifndef _gsl_fft_resampler_
#define _gsl_fft_resampler_

#ifdef __cplusplus
	extern "C" {
#endif

//lib
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//gsl lib
//#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

//macros for real and imaginary part extract
#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

//void and func
void print_complex_var(char *var_name, double *complex_var, int len_data, int is_csv);
void normalize_fft_result(double *data,int len_out, int scale);
double *gsl_fft_resampling_signal(double *data_in, int data_in_len, int data_out_len);

/*
 * printout complex variable complex_var
 * complex_var = double[][2] => [0] = real, [1] = imaginary
 * */
void print_complex_var(char *var_name, double *complex_var, int len_data, int is_csv)
{
	int i;
	
	printf("%s = \n", var_name);
	for(i=0;i<len_data;i++)
	{
		if(is_csv==0)
			printf("%d = %g + %g i\n", i, REAL(complex_var,i), IMAG(complex_var,i));
		else
			printf("%g\n", REAL(complex_var,i));
	}
}

/*
 * normalize the ifft output
 * len_in is the original signal length
 * len_out is the new signal length
 * for optimum result make sure if you do downsampling
 * the scale is above 0.5 (niquist theorem)
 * */
void normalize_fft_result(double *data,int len_out, int len_in)
{	
	int i;
	double fx = (double) len_out / len_in;
	
	for(i=0;i<len_out;i++)
	{	
		REAL(data,i) *= fx;
		IMAG(data,i) *= fx;
	}
}

/*
 * resampling signal based on fft
 * signal length is arbitrary
 * output is pointer to the real part or NULL if fail
 * can do upsampling and downsampling
 * downsampling ratio (data_out_len/data_in_len) suggested above 0.5
 * */
double *gsl_fft_resampling_signal(double *data_in, int data_in_len, int data_out_len)
{
	//var
	int i,j;
	int newLength = data_out_len;
	double *data_out;
	double *xin, *xout;	
	
	//fft wavetable and worskapce
	gsl_fft_complex_wavetable * wavetable;
	gsl_fft_complex_workspace * workspace;
	
	//allocate memory according to 
	//which one is higher between data_in_len and data_out_len
	xin = (double *) malloc(2 * ((data_in_len>data_out_len) ? data_in_len:data_out_len) * sizeof(double));
	xout = (double *) malloc(2 * ((data_in_len>data_out_len) ? data_in_len:data_out_len) * sizeof(double));
	data_out = (double *) malloc(data_out_len * sizeof(double));
	
	//fill in the data from data_in to xin
	//only real part
	for(i=0;i<data_in_len;i++)
	{
		REAL(xin,i) = data_in[i];
		IMAG(xin,i) = 0.0;
	}
	
	//debug info
	//print_complex_var("xin", xin, data_in_len, 0);
	
	//prepare memory according to data_in_len
	wavetable = gsl_fft_complex_wavetable_alloc (data_in_len);
	workspace = gsl_fft_complex_workspace_alloc (data_in_len);
	
	//allocation success
	if((wavetable != NULL) && (workspace != NULL))
	{	
		//forward fft
		//result in xin
		gsl_fft_complex_forward(xin, 1, data_in_len, wavetable, workspace);
		
		//show the result
		//print_complex_var("xout fft", xin, data_in_len, 0);
		
		//==============================================================				
		//DOWNSAMPLING UPSAMPLING IN FREQ DOMAIN
		int xsize = data_in_len;
		int xmsize = data_out_len;
		
		//pi level position
		int leven = 2*((double)(xsize>>1) - (double)xsize/2) + 1;
		int meven = 2*((double)(xmsize>>1) - (double)xmsize/2) + 1;		
		
		int fp= (xsize>>1) + 1;
		int fpc = xsize - fp + leven;
 
        int fm = (xmsize>>1)+ 1;
		int fmc = fm- 1 -meven;
		
		//debug info
		//printf(" xsize=%d\n xmsize = %d\n leven=%d\n meven=%d\n fp=%d\n fpc=%d\n fm=%d\n fmc=%d\n", xsize, xmsize, leven, meven, fp, fpc, fm, fmc);
		
		//decimation or interpolation
		if(fm<fp)	//decimation (downsamplin)
		{
			//build data		
			j = (xsize - fmc);
			
			//debug info
			//printf("j = %d\n", j);
			
			for(i=0;i<xmsize;i++)
			{
				//low part				
				if(i<fm)
				{
					REAL(xout,i) = REAL(xin,i);
					IMAG(xout,i) = IMAG(xin,i);
				}
				else //high part
				{
					//printf("j = %d => %g\n", j, xout[j][0]);
					REAL(xout,i) = REAL(xin,j);
					IMAG(xout,i) = IMAG(xin,j);
					j++;
				}
			}
		}
		else //interpolation (upsampling)
		{
			//check sama pjg hasilnya?
			if(xmsize==data_in_len)	//no process, just copy it
			{
				xout = xin;
			}
			else
			{
				//interpolate
				j = xsize - fpc;
				for(i=0;i<xmsize;i++)
				{
					//copy data dari 0 < fp
					if(i<fp)
					{
						REAL(xout,i) = REAL(xin,i);
						IMAG(xout,i) = IMAG(xin,i);
					}
					
					//insert zeros after it
					else if((i>=fp) && (i<(fp + xmsize - fpc - fp)))
					{
						REAL(xout,i) = IMAG(xout,i) = 0;
					}
					
					//fill the rest
					else
					{
						REAL(xout,i) = REAL(xin,j);
						IMAG(xout,i) = IMAG(xin,j);
						j++;
					}
				}
			}
		}
				
		//debug info
		//print_complex_var("xin filtered", xout, xmsize, 0);	
		
		//realloc the memory according to xmsize
		wavetable = gsl_fft_complex_wavetable_alloc (xmsize);
		workspace = gsl_fft_complex_workspace_alloc (xmsize);
		
		//do the ifft
		//IFFT according to new data len
		//inverse fft
		gsl_fft_complex_inverse(xout, 1, xmsize, wavetable, workspace);
		
		//debug info
		//print_complex_var("xout ifft before normalize", xout, data_out_len, 0);
		
		//normalize result
		//according to original data len
		
		//debug info
		//printf("normalize count %d\n", newLength);
		
		normalize_fft_result(xout, data_out_len, data_in_len);
		
		//debug info
		//print_complex_var("xout ifft after normalize", xout, newLength, 1);
		
		//copy data to data_out
		for(i=0;i<newLength;i++)
			data_out[i] = REAL(xout, i);
		
		//destroy the table and space
		gsl_fft_complex_wavetable_free (wavetable);
		gsl_fft_complex_workspace_free (workspace);
	}
	else
	{
		data_out = NULL;
	}
	
	//free resource
	free(xin);
	free(xout);
	
	//result
	return data_out;
}

#ifdef __cplusplus
}
#endif

#endif
