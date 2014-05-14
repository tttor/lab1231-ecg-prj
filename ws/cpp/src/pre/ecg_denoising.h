/*
 * ecg utility function
*/
#ifndef _ecg_denoising_
#define _ecg_denoising_

#ifdef __cplusplus
	extern "C" {
#endif

//uses lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug_util.h"
#include "wavelet_util.h"

//gsl lib
#include <gsl/gsl_wavelet.h>
#include <gsl/gsl_statistics_double.h>	//for mean and other
//#include </usr/local/include/gsl/gsl_wavelet.h>
//#include </usr/local/include/gsl/gsl_statistics_double.h>		


/*
 * proto function
 * */
double *get_clean_signal(int NInput, double *input, char minimum_filtered_level, double *bwr_profile);
double *calc_bwr_profile(int N, double *vec, char minimum_filtered_level);
void calc_clean_ecg(int N, double *vec_in, double *vec_bwr, double *vec_out);
double *denoising_signal(int N, double *vec, int freq_sampling);
double *denoising_signal_mra(int data_len, double *data_in, int freq_sampling);

/*
 * get cleaned signal
 * */
double *get_clean_signal(int NInput, double *input, char minimum_filtered_level, double *bwr_profile)
{
	double *clean_signal = (double *) malloc(NInput * sizeof(double));
	
	if(clean_signal != NULL)
	{
		double *bwr_profilex = calc_bwr_profile(NInput, input,minimum_filtered_level);
		if(bwr_profile == NULL)
			bwr_profile = (double *) malloc(NInput * sizeof(double));
		memcpy(bwr_profile, bwr_profilex, NInput * sizeof(double));
		
		calc_clean_ecg(NInput, input, bwr_profilex, clean_signal);
	}
	
	return clean_signal;
}

/*
 * calc bwr profile from input signal *vec with length N 
 * with minimum forces filtered signal at level minimum_filtered_level
 * if no bwr profile found
 * */
double *calc_bwr_profile(int N, double *vec, char minimum_filtered_level)
{
	int i,j,k,l, i_max = log(N)/log(2);
	double pwr[3]= {0,0,0};
	double *bwr_profile;
	char bwr_found = 0;
	
	//wavelet var
	gsl_wavelet *w;
	gsl_wavelet_workspace *work;
	
	//allocate memory
	bwr_profile = (double *) malloc(sizeof(double)*N);
	
	//copy input signal to bwr_profile
	if(bwr_profile != NULL)
		memcpy(bwr_profile,vec,sizeof(double)*N);
		
	//debug info
	//~ pr_debug("\ninput\t----",N,"%g,", &bwr_profile[0]);

	//allocate wavelet var used
	//wavelet is db4
	w = gsl_wavelet_alloc(gsl_wavelet_daubechies, 6);
	work = gsl_wavelet_workspace_alloc(N);
	
	//allocation memory ok???
	if((bwr_profile != NULL) && (w != NULL) && (work != NULL))
	{
		//do wavelet transform on all level
		gsl_wavelet_transform_forward (w, bwr_profile, 1, N, work);
		
		//debug info
		//~ pr_debug("\ninput\t----",N,"%g,", &bwr_profile[0]);
		
		//calc bwr
		//iterate on all detail coeff starting from biggest level(backward)
		for(i=(i_max-1);i>0;i--)
		//~ for(i=1;i<(i_max-1);i++)
		{
			//calc power
			pwr[0] = pwr[1];
			pwr[1] = pwr[2];
			
			k = N>>(i_max-i);	//limit bawah
			l = (k<<1);	//limit atas
			pwr[2] = 0;
			
			//debug
			//~ printf("i = %d\tk = %d\tl = %d\n",i, k,l);
			
			for(j=k;j<l;j++)
			{
				pwr[2] += (bwr_profile[j] * bwr_profile[j]);
			}
			pwr[2] = sqrt(pwr[2]);	//current power level
			
			//debug
			//~ printf("pwr at %d = %g\n", i, pwr[2]);
			
			//compare power
			if(i<(i_max-2))
			//~ if(i>=3)
			{
				if((pwr[2]>pwr[1]) && (pwr[1]<pwr[0]))
				{
					//debug
					//~ printf("\nbwr found at %d\n", i);
				
					//make all detail coeff above level i = 0
					for(j=((i+1)<<2);j<N;j++)
					{
						bwr_profile[j] = 0;					
					}
					
					//~ for(j=(i-1);j<=i_max;j++)
						//~ bwr_profile = wavelet_scale_detail_coeff(N, bwr_profile, 0, j);
					
					bwr_found = 1;
					break;
				}
			}	
		}	
		
		//if no bwr found, force it starting at level minimum_filtered_level
		if(!bwr_found)
		{
			j = (minimum_filtered_level>i_max) ? i_max:minimum_filtered_level;				
			for(i=(j<<2);i<N;i++)
			{
				bwr_profile[i] = 0;					
			}
		}
		
		//debug
		//~ bwr_profile[0] = bwr_profile[1] = 0;
		//~ pr_debug("\ndwt after filtering\t----",N,"\n%g", &bwr_profile[0]);
		
		//do idwt
		//result is bwr profile
		//gsl_wavelet_transform_inverse (w, bwr_profile, 1, N>>(i_max-i), work);
		
		gsl_wavelet_transform_inverse (w, bwr_profile, 1, N, work);
		
		//free resource using in wavelet
		gsl_wavelet_free (w);
		gsl_wavelet_workspace_free (work);		
	}
	else //no memory allocated
	{
		bwr_profile = NULL;
	}
	
	//result
	return bwr_profile;
}

/*
 * calculate clean ecg signal
 * */
void calc_clean_ecg(int N, double *vec_in, double *vec_bwr, double *vec_out)
{
	int n;

	for(n=0; n<N; n++)
		*vec_out++ = *vec_in++ - *vec_bwr++;
}

/*
 * calc clean signal from input signal *vec with length N 
 * with minimum forces filtered signal at level minimum_filtered_level
 * */
double *denoising_signal(int N, double *vec, int freq_sampling)
{
	int i,j,k,l, i_max = log(N)/log(2);
	//double pwr[3]= {0,0,0};
	double *bwr_profile;
	//char bwr_found = 0;
	
	//wavelet var
	gsl_wavelet *w;
	gsl_wavelet_workspace *work;
	
	//allocate memory
	bwr_profile = (double *) malloc(sizeof(double)*N);
	
	//copy input signal to bwr_profile
	if(bwr_profile != NULL)
		memcpy(bwr_profile,vec,sizeof(double)*N);
		
	//debug info
	//~ pr_debug("\ninput\t----",N,"%g,", &bwr_profile[0]);

	//allocate wavelet var used
	//wavelet is db4
	w = gsl_wavelet_alloc(gsl_wavelet_daubechies, 10);
	work = gsl_wavelet_workspace_alloc(N);
	
	//allocation memory ok???
	if((bwr_profile != NULL) && (w != NULL) && (work != NULL))
	{
		//do wavelet transform on all level
		gsl_wavelet_transform_forward (w, bwr_profile, 1, N, work);
		
		//debug info
		//~ pr_debug("\ninput\t----",N,"%g,", &bwr_profile[0]);
		
		//remove bwr band at the lowest wavelet
		bwr_profile[0] = 0;	//remove approximation highest level, its noise
		bwr_profile[1] = 0; //remove the detail
		//~ bwr_profile[2] = bwr_profile[3] = 0; //remove the detail
		
		//remove level 8 above, it it noise
		//rmove all freq band below 2Hz
		j = wavelet_find_detail_level_from_freq(freq_sampling, 3);
		//~ j =8;
		printf("\nremoving level wavelet above %d with fs = %d\n", j, freq_sampling);
		for(i=j;i<=i_max;i++)
			bwr_profile = wavelet_scale_detail_coeff(N, bwr_profile, 0, i);
			
		//remove high freq above 100Hz
		//at level 1 and 2
		j = wavelet_find_detail_level_from_freq(freq_sampling, 100);
		//~ j=2;
		printf("\nremoving level wavelet form 1 to %d with fs = %d\n", j, freq_sampling);
		for(i=1;i<=j;i++)
			bwr_profile = wavelet_scale_detail_coeff(N, bwr_profile, 0, i);	
		
		//debug
		pr_debug("\ndwt after filtering\t----",N,"%g\n", &bwr_profile[0]);
		
		//do idwt
		//result is bwr profile
		gsl_wavelet_transform_inverse (w, bwr_profile, 1, N, work);
		
		//subtract with dc offset
		//~ double mn = gsl_stats_mean(bwr_profile, 1, N);
		//~ for(i=0;i<N;i++)
			//~ bwr_profile[i] -= mn;
		
		//free resource using in wavelet
		gsl_wavelet_free (w);
		gsl_wavelet_workspace_free (work);		
	}
	else //no memory allocated
	{
		bwr_profile = NULL;
	}
	
	//result
	return bwr_profile;
}

/*
 * denoising chunk of data with mra
 * data chunk recommended is 1024 or 2048
 * input is 2^n length
 * n must be more or same as 10
 * frequency removed :
 * -bwr at freq lower than 2Hz
 * -high freq noise above 100Hz
 * freq sampling :
 * - 250sps	= keep level 2-7
 * - 500sps = keep level 3-8
 * - 1000sps = keep level 4-9
 * */
double *denoising_signal_mra(int data_len, double *data_in, int freq_sampling)
{
	int i;
	double *data_out;
	
	//wavelet memory
	gsl_wavelet *w;
	gsl_wavelet_workspace *work;
	
	//allocate memory
	data_out = (double *) malloc(data_len * sizeof(double));
	
	//copy data input
	memcpy(data_out, data_in, data_len * sizeof(double));
	
	//aloocate wavelet memory
	w = gsl_wavelet_alloc(gsl_wavelet_haar, 2);	//db4
	work = gsl_wavelet_workspace_alloc(data_len);	//allocate workspace
	
	//max iteration
	int level_start_clean = 1, level_end_clean = 7;
	
	//level start end clean
	if(freq_sampling>=500)
	{
		level_start_clean++; 
		level_end_clean++;
	}
			
	if(freq_sampling>=1000)
	{
		level_start_clean++; 
		level_end_clean++;
	}
	
	//debug ino
	//~ printf("\nremoving %d to %d", level_start_clean, level_end_clean);
	
	//do transform
	gsl_wavelet_transform_forward (w, data_out, 1, data_len, work);
	
	//removing data freq band to get the noise 
	for(i=level_start_clean;i<=level_end_clean;i++)
	{
	  data_out = wavelet_scale_detail_coeff(data_len, data_out, 0, i);
	}
		
	//inverse dwt
	gsl_wavelet_transform_inverse (w, data_out, 1, data_len, work);

	//free wavelet memory
	gsl_wavelet_free(w);
	gsl_wavelet_workspace_free(work);	
	
	//out as clean signal
	for(i=0;i<data_len;i++)
	{
	  data_out[i] = data_in[i] - data_out[i];
	}	
	
	//return result
	return data_out;
}

#ifdef __cplusplus
}
#endif

#endif

