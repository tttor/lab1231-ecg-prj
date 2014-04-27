#include <stdio.h>
#include <math.h>	//for math ops
#include <string.h>	//for memcpy

//gsl lib
#define GSL_DISABLE_DEPRECATED 1	//make sure it wont named twice

#include "ecg_denoising.h"
#include "wavelet_util.h"
#include "ecg_extraction_util.h"
#include "gsl_fft_resampler.h"
#include "file_util.h"
#include "ecg_sorting.h"
#include "wavelet_statistic.h"

//main program
int
main (int argc, char **argv)
{
	int i,j, 
		sample_number_tested = 1024,		//sample number to be tested
		sample_rate_data_input = 256,		//sample rate data evaluated
		sample_num_per_frame = 128,			//sampel per frame
		max_num_feature_extracted = 128,	//limit of feature extracted
		level_filter_denoising = 9;			//level filtering ecg wavelet
		
	double r_peak_threshold = 0.4;	//threshold deteksi r peak
		
	char *fname, *outdir = "./";		//input file name
	double *data;
	//~ char *fname_proc = fname;
	
	char *line = "======================================================";
	char *guide = 
		"-f	= filename\n" \
		"-o = output directory(must be ended with /)\n" \
		"-n	= sampel number to be evaluated(must be 2^n)\n" \
		"-s	= sample rate of the data per seconds\n" \
		"-S	= sample per frame data\n" \
		"-R	= maximum number of R peak to be extracted\n" \
		"-T	= threshold for R peak detection\n" \
		"-L	= wavelet level thresholding if no bwr profile found\n";
	
	//parsing command line
	if(argc<2)
	{
		printf("not enough argument. read below for options\n%s", guide);
		return -1;
	}
	else
	{	
		for(i=1;i<(argc-1);i++)
		{
			//filename
			if(strcmp("-f",argv[i])==0)
				fname = argv[++i];
				
			//outdir
			if(strcmp("-o",argv[i])==0)
				outdir = argv[++i];
				
			//sampel number
			if(strcmp("-n",argv[i])==0)
				sample_number_tested = atoi(argv[++i]);
				
			//sample rate
			if(strcmp("-s",argv[i])==0)
				sample_rate_data_input = atoi(argv[++i]);
				
			//sampel per frame
			if(strcmp("-S",argv[i])==0)
				sample_num_per_frame = atoi(argv[++i]);
				
			//max r peak
			if(strcmp("-R",argv[i])==0)
				max_num_feature_extracted = atoi(argv[++i]);
				
			//threshold
			if(strcmp("-T",argv[i])==0)
				r_peak_threshold = atof(argv[++i]);
				
			//level wavelet filtering
			if(strcmp("-L",argv[i])==0)
				level_filter_denoising = atof(argv[++i]);
		}
	}
	
	//empty file name
	if(fname=="")
	{
		printf("no file specified");
		return -1;
	}
	
	//prepare memory
	if(sample_number_tested>0)
		data = malloc (sample_number_tested * sizeof (double));
		
	//print info
	printf("%s\nData Analysis Info\n" \
	"File\t\t\t\t\t: %s\n" \
	"Out Folder\t: %s\n" \
	"Sample Number\t\t\t: %d\n" \
	"Sample Rate\t\t\t\t: %d sps\n" \
	"Sampel Per Frame\t\t: %d\n" \
	"Max Frame\t\t\t\t: %d\n" \
	"R Threshold\t\t\t\t: %g\n" \
	"Wavelet Filter Level\t: %d\n%s\n",
	line, fname, outdir, sample_number_tested, sample_rate_data_input, sample_num_per_frame, max_num_feature_extracted, r_peak_threshold, level_filter_denoising,
	line
	);
	
	printf("\nread input file from %s = %d", fname, read_csv_file(fname, data, sample_number_tested, 1));
	
	//write file
	printf("\nsave input signal to %s.data = %d", fname, write_double_to_file(fname,".data", "%g\n", sample_number_tested,  data));

	//get clean ecg signal
	double *bwr_profile =(double *) malloc(sample_number_tested * sizeof(double));
	
	//~ double *ecg_clean = get_clean_signal(sample_number_tested, data, level_filter_denoising, bwr_profile); //sargolzaei
	
	double *ecg_clean = denoising_signal_mra(sample_number_tested, data, sample_rate_data_input); //mra
	
	//~ printf("\nbwr = ");
	for(i=0;i<sample_number_tested;i++)
	{
		//~ printf("\n%g", bwr_profile[i]);
		bwr_profile[i] = data[i] - ecg_clean[i];
	}
	
	//write to file
	printf("\nsave clean signal to %s.clean = %d", fname, write_double_to_file(fname,".clean", "%g\n", sample_number_tested, ecg_clean));
	
	//write to file
	printf("\nsave bwr signal to %s.bwr = %d", fname, write_double_to_file(fname,".bwr", "%g\n", sample_number_tested, bwr_profile));
	
	//~ return 0;
		
	//extract feature beat 
	int numR = 0; 
	ecg_feature *fitur_ecg = ecg_extract_feature(sample_number_tested, ecg_clean, sample_rate_data_input, r_peak_threshold, &numR); 
	
	//limit feature extracted
	if(numR>max_num_feature_extracted)
		numR = max_num_feature_extracted;
		
	//wave to file
	printf("\nsave feature list to %s.feature = %d", fname, write_ecg_feature_to_file(fname,".feature", numR, fitur_ecg));	
	
	//write R peak file
	int *RPeak = (int *) malloc(sample_number_tested * sizeof(int));
	memset(RPeak, 0, sample_number_tested * sizeof(int));
	for(i=0;i<numR;i++)
	{
		RPeak[fitur_ecg[i].R] = 1;
	}
	printf("\nsave R peak to %s.rpeak = %d", fname, write_int_to_file(fname,".rpeak", "%d\n",sample_number_tested, RPeak));	
		
	//normalize signal
	double *norm_signal = normalize_input(ecg_clean, sample_number_tested, fitur_ecg, numR, sample_num_per_frame);
	
	//write file	
	printf("\nsave normalize signal to %s.normalize = %d", fname, write_double_to_file(fname,".normalize","%g\n", numR * sample_num_per_frame,  norm_signal));
	
	printf("\nsave normalize signal as 2d to %s.normalize2d = %d", fname, write_double_as_matrix_to_file(fname, ".normalize2d", numR * sample_num_per_frame , numR, norm_signal));	 
	
	//sort by its similarity
	int sorted_order[numR];
	double *sorted_signal = sort_by_similarity(norm_signal,(numR * sample_num_per_frame), numR, sample_num_per_frame, &sorted_order[0] );
	
	//write file
	printf("\nsave sorted signal to %s.sorted = %d", fname, write_double_to_file(fname,".sorted", "%g\n", numR * sample_num_per_frame,  sorted_signal));
	
	printf("\nsave sorted signal as 2d to %s.sorted2d = %d", fname, write_double_as_matrix_to_file(fname, ".sorted2d", numR * sample_num_per_frame , numR, sorted_signal));
	
	//write file
	printf("\nsave order sorting to %s.order = %d", fname, write_int_to_file(fname,".order", "%d\n", numR, sorted_order));
	
	printf("\nframe order =\n");
	for (i = 0; i < numR; i++)
	{
	  printf ("%d%s", sorted_order[i], (i<(numR-1)) ? ",":"\n");
	}
	
	//do wavelet statistic analysis
	//on data in, clean ecg, normalize, and sorted
	wavelet_statistic_out wavelet_stat_data[4];
	double *wavelet_data;
	
	//write wavelet input data file
	wavelet_data = wavelet_calc_statistic(sample_number_tested, data , gsl_wavelet_daubechies, 6, 1, "input",&wavelet_stat_data[0]);
	printf("\nsave wavelet input signal to %s.data.wavelet = %d", fname, write_double_to_file(fname,".data.wavelet", "%g\n", sample_number_tested,  wavelet_data));
	
	//write wavelet clean ecg file
	wavelet_data = wavelet_calc_statistic(sample_number_tested, ecg_clean , gsl_wavelet_daubechies, 6, 1, "clean",&wavelet_stat_data[1]);
	printf("\nsave wavelet clean signal to %s.clean.wavelet = %d", fname, write_double_to_file(fname,".clean.wavelet", "%g\n", sample_number_tested,  wavelet_data));
	
	//write wavelet normalize ecg file
	wavelet_data = wavelet_calc_statistic(numR * sample_num_per_frame, norm_signal , gsl_wavelet_daubechies, 6, 1, "normalize",&wavelet_stat_data[2]);
	printf("\nsave wavelet normalize signal to %s.normalize.wavelet = %d", fname, write_double_to_file(fname,".normalize.wavelet", "%g\n", numR * sample_num_per_frame,  wavelet_data));
	
	//write wavelet sorted ecg file
	wavelet_data = wavelet_calc_statistic(numR * sample_num_per_frame, sorted_signal , gsl_wavelet_daubechies, 6, 1, "sorted",&wavelet_stat_data[3]);
	printf("\nsave wavelet sorted signal to %s.sorted.wavelet = %d", fname, write_double_to_file(fname,".sorted.wavelet", "%g\n", numR * sample_num_per_frame,  wavelet_data));
	
	//write stat to file	
	printf("\nsave wavelet input signal to %s.stat = %d", fname, write_wavelet_statistic_to_file(fname, ".stat", 4 , wavelet_stat_data));
	
	//~ printf("\nstatistic of wavelet data input :\n" \
	//~ "name\t= %s\n" \
	//~ "mean\t= %g\n" \
	//~ "median\t= %g\n" \
	//~ "max\t= %g\n" \
	//~ "min\t= %g\n" \
	//~ "range\t= %g\n" \
	//~ "stdev\t= %g",
	//~ wavelet_stat_data[0].name,
	//~ wavelet_stat_data[0].mean, wavelet_stat_data[0].median, wavelet_stat_data[0].max, wavelet_stat_data[0].min, wavelet_stat_data[0].range, wavelet_stat_data[0].stdev);

	return 0;
}
