/**
 * Bismillah
 *@file 	ecg_12.c
 * 
 *@brief 	Program utama untuk ecg multilead 12 kanal
 * note : normalization pad with 2 sample before and after
 *
 *@author 	EX4 (ekapujiw2002@gmail.com)
 *@modif	GrafikaJ (grafika.jati@gmail.com)	
 *@date		2014
 *@version 	2.0
 */
 
/*
 * multi lead ecg analysis tools
 * compile it with :
 * WIN : 
 * gcc -o ../bin/ecg_out.exe ecg10072013.c ads1298.c gpio_mmap.c lib/tilib/src/*.c spi_soft.c -lgsl -lm
 * NIX :
 * gcc -o out_ecg_12.exe ecg_12.c ads1298.c gpio_mmap.c lib/tilib/src/*.c spi_soft.c -lgsl -lgslcblas -lm
 * 
 * sementara sebelum ada SPIHT: gcc -o out_ecg_12.exe ecg_12.c ads1298.c gpio_mmap.c spi_soft.c -lgsl -lgslcblas -lm 
 * 
 * Libraries gsl installed in:
   /usr/local/lib
 * */
 



#include <stdio.h>
#include <math.h>	//for math ops
#include <string.h>	//for memcpy

//gsl lib
#define GSL_DISABLE_DEPRECATED 1	//make sure it wont named twice

#include "ecg_denoising.h"
#include "wavelet_util.h"

#include "gsl_fft_resampler.h"
#include "file_util.h"
#include "ecg_sorting.h"
#include "wavelet_statistic.h"
#include "ads1298.h"

//buat normalisasi
#include "normalization.h"   

//buat rr
#include "qrs_det.h"


/*
 * rearrange adc data as stream of double from a channel
 * input : V1,V2,V3,V4,v5,V6,V7,V8,V1,V2,v3,....,V8
 * out : V1,V1,V1,V2,V2,V2,.....,V8,V8,V8
 * */
void adc_rearrange_voltage(int adc_sample_len, double *adc_volt_data)
{
	int i,j,k;
	double *adc_volt_aligned = (double *) malloc((adc_sample_len<<3) * sizeof(double));	
	
	memset(adc_volt_aligned,0,(adc_sample_len<<3) * sizeof(double));	//set default = 0
	
	for(i=0;i<adc_sample_len;i++)
	{
		for(j=(i<<3), k=0 ;j<((i+1)<<3), k<8;j++, k++)	//8channel
		{
			adc_volt_aligned[(k*adc_sample_len) + i] = adc_volt_data[j];	//assign to new data
		}
	}
	
	//by default override the input data
	memcpy(adc_volt_data, adc_volt_aligned, (adc_sample_len<<3) * sizeof(double));
	
	//free mem
	free(adc_volt_aligned);
}


/*
 * cleanup arranged adc voltage data
 * using mra
 * */
void adc_multichannel_denoising(int sample_len, double *adc_volt_aligned, int adc_sampling_rate) 
{
	int i;
	double *tmpd0 = (double *) malloc(sample_len * sizeof(double));
	
	for(i=0;i<8;i++)	//clean up all channel
	{
		//copy data
		//~ memcpy(tmpd0,&adc_volt_aligned[i*sample_len], sample_len * sizeof(double));
		
		tmpd0 = denoising_signal_mra(sample_len, &adc_volt_aligned[i*sample_len], adc_sampling_rate); //mra
		
		//copy back
		memcpy(&adc_volt_aligned[i*sample_len], tmpd0, sample_len * sizeof(double));
	}
	
	//free memory
	free(tmpd0);
}

/*
 * calculate bwr from signal input and clean out
 * clean data must be arrange first
 * */
double *calculate_bwr_multilead(int sample_len,double *data_input, double *data_clean_arranged)
{
	double *bwr_profile_multilead =(double *) malloc((sample_len<<3) * sizeof(double));
	int i;
	
	if(bwr_profile_multilead!=NULL)
	{
		for(i=0;i<(sample_len<<3);i++)	
		{
			bwr_profile_multilead[i] = data_input[i] - data_clean_arranged[i];
		}
	}
	
	return bwr_profile_multilead;
}


/*
 * log data to out
 * */
void Print_Data(unsigned int num_sampel, int *adc_status_array, int *adc_data_array) 
{
	int i,j;
	
	for(j=0; j<num_sampel; j++) {
		printf("%d,", *adc_status_array++);
		for(i=0; i<8; i++) {
			printf("%d", *adc_data_array++);
			printf((i<7) ? ",":"\r\n");
		}
	}
}

/*
 * print data ecg
 * V1-V6,Lead I,Lead II,Lead III, aVR,aVL,aVF
 * 
 * Lead I = LA - RA
 * Lead II = LL - RA
 * Lead III = Lead2 - Lead1
 * a = artificial
 * aVR = - (Lead I + Lead II ) / 2
 * aVL = Lead I - (Lead II /2)
 * aVF = Lead II - (Lead I/2)
 * 
 *
 * */
void Print_Data_ECG(unsigned int num_sampel, int *adc_data_array) 
{
	int i,j;
	printf("V1,V2,V3,V4,V5,V6,LEAD1,LEAD2,LEAD3,aVR,aVL,aVF\r\n");
	for(j=0; j<num_sampel; j++) {
		for(i=0; i<12; i++) {
			printf("%d", *adc_data_array++);
			printf((i<11) ? ",":"\r\n");
		}
	}
}


//==========================================================================================================================================================//
//grafika was here
/*
 * normalize all channel
 * data input must be clean and arranged first from CH1-CH8
 * */
/*
 * cleanup arranged adc voltage data
 * using mra
 * */

void tes_ambil_data(int sample_len, double *input){

	FILE *fp;
	fp = fopen("ambil_data_normalisasi.csv", "w");
	int i;

	for (i = 0; i < sample_len; ++i)
	{
		fprintf(fp, "%f,", input[i]);
	}

}


double *multichannel_normalization(double *input_signal,int sample_len,int *input_rpeak, int *input_rr, int beat_found) 
{
	FILE *printN;
	printN = fopen("ambil_data_normalisasi.csv", "w");
	printf("Multichanel normalization\n");
	int i,j,k,m;

	double *norm_single =(double *) malloc((((beat_found-1)*128)<<3) * sizeof(double));
	
	double *norm_multi =(double *) malloc((((beat_found-1)*128*8)<<3) * sizeof(double));
	
	double signal_olah[sample_len];

	//sementara 
	
	for (i = 0; i < beat_found; ++i)
	{
		printf("%d,%d\n", input_rpeak[i],input_rr[i]);
	}

	/*
	for (i = 0; i < sample_len; ++i)
	{
		fprintf(fp, "%f,",input_signal[i]);
	}*/
	int start=0;
	int end=0;
	int start_res=0;
	int end_res=0;

	
	for (j = 0; j < 8; j++)
	{
		//olah semua lead
		start = end;
		end = start+sample_len; 
		m=0;
		for (i = start; i < end; ++i)
		{
			signal_olah[m]=input_signal[i];
			m++;
		}

		norm_single = bantuNormalization(signal_olah,input_rpeak,input_rr,beat_found);

		
		start_res = end_res;
        end_res = start_res + (beat_found-1)*128;
        m=0;
        for(k=start_res;k<end_res;k++){

            norm_multi[k]=norm_single[m];
            m++;
        }
        printf("Normalisasi Lead %d\n",j+1);

	}
	
	//multi
	
	for (i = 0; i < (beat_found-1)*128*8; ++i)
	{
		fprintf(printN, "%f,",norm_multi[i]);
	}
	//single	
	/*
	for (i = 0; i < (beat_found-1)*128; ++i)
	{
		fprintf(printN, "%f,",norm_single[i]);
	}*/


//	return norm_single;
	return norm_multi;

}


double *multichannel_denormalization(double *input_signal,int sample_len,int *input_rpeak, int *input_rr, int beat_found) 
{
	FILE *printN;
	printN = fopen("ambil_data_denormalisasi.csv", "w");
	printf("Multichanel denormalization\n");
	int i,j,k,m;

	double *denorm_single =(double *) malloc((((beat_found-1)*128)<<3) * sizeof(double));
	
	double *norm_multi =(double *) malloc((((beat_found-1)*128*8)<<3) * sizeof(double));
	
	double signal_olah[sample_len];

	//sementara 
	
	for (i = 0; i < beat_found; ++i)
	{
		printf("%d,%d\n", input_rpeak[i],input_rr[i]);
	}

	/*
	for (i = 0; i < sample_len; ++i)
	{
		fprintf(fp, "%f,",input_signal[i]);
	}*/
	int start=0;
	int end=0;
	int start_res=0;
	int end_res=0;

	
	for (j = 0; j < 8; j++)
	{
		//olah semua lead
		start = end;
		end = start+sample_len; 
		m=0;
		for (i = start; i < end; ++i)
		{
			signal_olah[m]=input_signal[i];
			m++;
		}

		norm_single = bantuNormalization(signal_olah,input_rpeak,input_rr,beat_found);

		
		start_res = end_res;
        end_res = start_res + (beat_found-1)*128;
        m=0;
        for(k=start_res;k<end_res;k++){

            norm_multi[k]=norm_single[m];
            m++;
        }
        printf("Normalisasi Lead %d\n",j+1);

	}
	
	//multi
	
	for (i = 0; i < (beat_found-1)*128*8; ++i)
	{
		fprintf(printN, "%f,",norm_multi[i]);
	}
	//single	
	/*
	for (i = 0; i < (beat_found-1)*128; ++i)
	{
		fprintf(printN, "%f,",norm_single[i]);
	}*/


//	return norm_single;
	return norm_multi;

}




//qrs proc

int *qrs_proc(double *signal_input, int sample_number_tested){
	FILE *fp;
	fp = fopen("ambil_data_qrs_lead2.csv", "w");
	 
	printf("qrs_proc\n");

	int i;
	double signIn [sample_number_tested];
	int *rpeak_out,rr_out;

	int m=sample_number_tested;
	for (i = 0; i < sample_number_tested; ++i)
	{
		signIn [i] = signal_input[m];
		m++;
	}

	for (i = 0; i < sample_number_tested; i++)
	{
		//printf("\n sinyal %f\n" , signIn[i]);
		fprintf(fp, "%f,", signIn[i]);
	}

	float *hP = highPass(signIn, sample_number_tested);
	int lenHigh = sizeof(hP)/sizeof(hP[0]);

	float *lP = lowPass(hP, sample_number_tested);
	int lenLow = sizeof(lP)/sizeof(lP[0]);

	int *qrs = QRS(lP, sample_number_tested);
	//int lenqrs = sizeof(qrs)/sizeof(qrs[0]);

	rpeak_out = getR(qrs, sample_number_tested);
	//int lenrr = sizeof(rpeak_out)/sizeof(rpeak_out[0]);

	rr_out = getRpeak(rpeak_out, sample_number_tested);
	//int lenrpeak = sizeof(rr_out)/sizeof(rr_out[0]);

	printf("hasil deteksi dalam\n");
	//printf("%d === %d\n",lenrr, lenrpeak );
	
	/*
	for (i = 0; i < 127; i++)
	{
		//printf("%d,%d,%d\n", i,rpeak_out[i], rr_out[i]);
		//fprintf(fp,"%d,%d,%d\n", i,rpeak_out[i], rr_out[i]);
		printf("%d,%d\n", i,rpeak_out[i]);
			
	}*/


	//memcpy(&rr_out[105], rr_out, 105 * sizeof(int));
	return rpeak_out;


}



//==========================================================================================================================================================//
//main program
int
main (int argc, char **argv)
{
	//Inisialisasi variable
	int i,j, 
		//sample_number_tested = 16384,		//sample number to be tested
		//sample_number_tested = 10895,		//sample number to be tested
		sample_number_tested = 1024,

		sample_rate_data_input = 250,		//sample rate data evaluated
		sample_num_per_frame = 128,			//sampel per frame
		max_num_feature_extracted = 16,		//limit of feature extracted
		mode_program = -1,					//program work mode : -1=from file , 0=adc noise tes mode, 1=adc signal tes mode, 2=adc real measurement mode
		channel_for_r_detect = 0,			//channel for r detection
		num_pad = 4,						//pad data len for normalize and denormalize
		bypass_confirmation = 0,			//bypass confirmation
		verbose_mode = 1;					//0=no message, else = display message
		
	double r_peak_threshold = 0.4,	//threshold deteksi r peak
		bit_rate_encoding = 5.5 ,	//bit rate encoding
		compression_rate_encoding = 8/5.5;	//compression rate, if set will override bit_rate_encoding
		
	char *fname, *outdir = "./";		//input file name
	double *data_voltage, //raw data from pasien simulator and file
		*data_clean; 
	double *res_normalization_beat,*res_denormalization_beat;	

	//buat debugging, grafika was here
	FILE *printNorm,*printDnorm, *printdata_clean,*printRR;
    
    printNorm = fopen("gabung_normalization_main.csv", "w");
    printDnorm = fopen("gabung_denormalization_main","w");
	printRR = fopen("RR_detection.csv", "w");
	

	//	

	printf("Bismillah GrafikaJ\n");

	//k
	int *rr, *rpeak;	
	
	static char *line = "======================================================";
	static char *guide = 
		"-f	= filename\n" \
		"-o = output directory(must be ended with /)\n" \
		"-n	= sampel number to be evaluated(must be 2^n) (default 1024)\n" \
		"-s	= sample rate of the data per seconds (default 250 sps)\n" \
		"-S	= sample per frame data (default 128)\n" \
		"-R	= maximum number of R peak to be extracted (default 16)\n" \
		"-c = channel for R detection (0-7, default 2)\n" \
		"-T	= threshold for R peak detection (default 0.4)\n" \
		"-b = bit rate for encoding (default = 5.5)\n" \
		"-C = compression rate for encoding (default = 8/5.5, will override bit rate if set)\n" \
		"-y = bypass confirmation on decoding(default = 0)\n" \
		"-M = work mode (default -1)\n" \
		"     -4 = Wavelet transform - syntesize 2D\n"
		"     -3 = Decoding file data with 2D-SPIHT\n" \
		"     -2 = Encoding file data with 2D-SPIHT\n" \
		"     -1 = ADC data from file. Continued with 2D-SPIHT encoding with bit rate specified by -b\n" \
		"     0  = ADC noise level mode\n" \
		"     1  = ADC signal test mode\n" \
		"     2  = ADC real mode\n" ;

/*
	 * PARSING COMMAND LINE
	 * */
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
				
			//channel for r detect
			if(strcmp("-c",argv[i])==0)
				channel_for_r_detect = atoi(argv[++i]);
				
			//threshold
			if(strcmp("-T",argv[i])==0)
				r_peak_threshold = atof(argv[++i]);
				
			//bit rate encoding
			if(strcmp("-b",argv[i])==0)
				bit_rate_encoding = atof(argv[++i]);
				
			//compression rate encoding
			if(strcmp("-C",argv[i])==0)
			{
				compression_rate_encoding = atof(argv[++i]);
				bit_rate_encoding = 8/compression_rate_encoding;
			}
			
			//bypass confirmation
			if(strcmp("-y",argv[i])==0)
			{
				bypass_confirmation = atoi(argv[++i]);
			}
				
			//normalize pad len
			if(strcmp("-p",argv[i])==0)
				num_pad = atoi(argv[++i]);
				
			//work mode
			if(strcmp("-M",argv[i])==0)
				mode_program = atoi(argv[++i]);
		}
	}

//empty file name on file input mode
	if(mode_program==-1)
	{
		if(fname[0]=='\0')
		{
			printf("no file specified");
			return -1;
		}
	}

//prepare memory
	if(sample_number_tested>0)
	{
		data_voltage = malloc ((sample_number_tested<<3) * sizeof (double));
		data_clean = malloc ((sample_number_tested<<3) * sizeof (double));
		
		
		//kholid
		rr = (int *) malloc(105 * sizeof(int));
		rpeak = (int *) malloc(105 * sizeof(int));
	}

//print info
	printf("\n%s\nData Analysis Info\n" \
	"%-25s : %s\n" 
	"%-25s : %s\n"  
	"%-25s : %d\n"  
	"%-25s : %d sps\n"  
	"%-25s : %d\n" 
	"%-25s : %d\n"  
	"%-25s : %f\n"  
	"%-25s : %d\n"  
	"%-25s : %d\n"  
	"%-25s : %f\n"  
	"%-25s : %f\n"  
	"%-25s : %d\n"  
	"%s\n",
	line, 
	"File", fname, 
	"Out Folder", outdir, 
	"Sample Number", sample_number_tested, 
	"Sample Rate", sample_rate_data_input, 
	"Sample Per Frame", sample_num_per_frame, 
	"Max Frame", max_num_feature_extracted, 
	"R Threshold", r_peak_threshold, 
	"R Channel", channel_for_r_detect, 
	"Work Mode", mode_program,
	"Bit Rate", bit_rate_encoding,
	"Compression Rate", compression_rate_encoding,
	"Pad Length", num_pad,
	line
	);

/*
	 * READ FILE MULTILEAD IN mode_program = -1
	 * */
	if(mode_program==-1)	//FILE Mode
	{
		//read file , make the order like Pak Sani's code
		char resx = read_csv_file2(fname, data_voltage, sample_number_tested, 1, verbose_mode);
		
		
		if(resx!=0)
		{
			printf("\ncannot read input file.\nprogram terminated");
			return 0;
		}
	}

/*
	 * READ DATA FROM ADS1298 MODULE
	 * */
	 
	else if (mode_program>=0)	//ADC Mode
	{
		/*
		 * SETUP ADS1298
		 * */
		 char adc_sps = (sample_rate_data_input==250) ? ADS1298_250SPS: \
			(sample_rate_data_input==500) ? ADS1298_500SPS: \
			(sample_rate_data_input==1000) ? ADS1298_1000SPS: ADS1298_500SPS;
		 char adc_status = ADS1298_Setup_ECG(mode_program, adc_sps);
		 
		 /*
		  * ADC SETUP CHECK
		  * */
		 if(adc_status!=0)
		 {
			 printf("\nADC setup fail : %d\nProgram terminated", adc_status);
			 return 0;
		 }
		 else //adc setup ok		 
		 {
			 /*
			  * READ ALL SETUP REGISTER
			  * */
			 printf("\nADC register information :\n");
			 ADS1298_Read_All_Register();
			 
			 /*
			  * START MEASURING
			  * */
			 if((adc_status = ADS1298_Start_Measurement()) != 0)
			 {
				printf("\nCannot start ADC measurement : %d\nProgram terminated", adc_status);
				return 0;
			 }
			 else //start measurement ok ==> retrieve data
			 {
				 /*
				  * GET SAMPLES DATA
				  * */
			
				int *adc_sts, *adc_data;  
				
				//allocate memory
				adc_sts = (int *) malloc(sample_number_tested * sizeof(int));
				adc_data = (int *) malloc(sample_number_tested * 8 * sizeof(int));	//per data is 8 channel
				
				//memory ok?
				if(adc_sts == NULL || adc_data == NULL)
				{
					printf("\nmalloc() failed\nProgram terminated");
					return 0;
				}
				else //memori ok => grab data
				{
					//grab data
					ADS1298_Get_Samples(sample_number_tested, adc_sts, &adc_data[0]);
					
					//save raw data
					printf("\nraw adc data save to %s = %d", fname, write_raw_adc_multichannel_to_file(fname, ".raw", sample_number_tested, adc_data, verbose_mode));
					
						
					//remove offset
					ADS1298_Include_Offset(sample_number_tested, &adc_data[0]);
					
					//save raw data with offset removed
					printf("\nraw adc data offset removed save to %s = %d", fname, write_raw_adc_multichannel_to_file(fname, ".offsetted", sample_number_tested, adc_data, verbose_mode));
				
					//convert ke tegangan mV
					//for all channel at sample_number_tested
					ADS1298_Convert_To_Voltage(sample_number_tested<<3, adc_data, data_voltage, ADS1298_mV);
					
					//free memory
					free(adc_sts);
					free(adc_data);
				}				
			 }
		 }
		 
		//release all gpio handle
		GPIO_Release();
	}
	

	/*
	 * START OF NORMAL OPERATION
	 * */	
	/*
	 * WRITE DATA TO FILE
	 * data IN FORM OF VOLTAGE = CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH1,CH2,CH3,CH4,CH5,CH6,CH7.......
	 * */
	write_double_multichannel_to_file(fname,".data", sample_number_tested,  data_voltage, verbose_mode);

	/*
	 * ARRANGE VOLTAGE DATA WITH ORDER CH1-CH8 
	 * sample_number_tested EACH
	 * */
	 //gw masih ga ngerti kenapa harus diarrange dulu?????
	adc_rearrange_voltage(sample_number_tested, data_voltage);

	/*
	 * COPY IT TO data_clean
	 * */
	memcpy(data_clean, data_voltage, (sample_number_tested<<3) * sizeof(double));
	
	/*
	 * WRITE data_voltage ARRANGED TO FILE
	 * */
	write_multichannel_arranged_data_to_file(fname,".data_arrange",sample_number_tested,  data_voltage, verbose_mode);
	//write_multichannel_arranged_data_to_file(fname,".data_file",sample_number_tested,  data_voltage, verbose_mode);

	/*
	 * CLEANUP ALL CHANNEL
	 * OUTPUT OVERRIDE INPUT
	 * */
	adc_multichannel_denoising(sample_number_tested, data_clean, sample_rate_data_input) ;
	
	/*
	 * CALCULATE BWR AND SAVE IT TO FILE
	 * */
	double *bwr_profile_multilead = calculate_bwr_multilead(sample_number_tested, data_voltage, data_clean);
	
	/*
	 * WRITE CLEAN DATA TO FILE
	 * */
	write_multichannel_arranged_data_to_file(fname,".clean",sample_number_tested,  data_clean, verbose_mode);
	
	/*
	 * WRITE BWR TO FILE
	 * */	
	write_multichannel_arranged_data_to_file(fname,".bwr",sample_number_tested,  bwr_profile_multilead, verbose_mode);
	printf("\nPreprocess is done\n");
	printf("\nStart R-R detection\n");
	
	
	//dapat rpeak
	rpeak = qrs_proc(data_clean, sample_number_tested);

	printf("out\n");
	//angkanya khusus buat data I01, sample_number_tested
	//dapat rr
	rr = getRpeak(rpeak, sample_number_tested);
	int beat_found;
	beat_found=1;


	for (i = 0; i < 127; i++)
	{
		fprintf(printRR,"%d,%d,%d\n", i,rpeak[i],rr[i]);			
		if(!rpeak[i]==0){
			beat_found++;
		}
	}
	
	printf("beat_found %d\n", beat_found);
	printf("\nStart normalization\n");
	

	//int jarak_per_bit[]={115,163,165,163,97,273};
	//int letak_r[]={0,115,278,443,606,703,940};
	
	double *data_norm_multi = multichannel_normalization(data_clean,sample_number_tested,rpeak,rr,beat_found);

	/*
	for (i = 0; i < (beat_found-1)*128*8; ++i)
	{
		fprintf(printNorm, "%f,",data_norm_multi[i]);
	}*/

	write_multichannel_arranged_data_to_file(fname,".normalize",(beat_found-1)*128,  data_norm_multi, verbose_mode);

	printf("\nStart denormalization\n");
	
	double *data_denorm_multi = multichannel_denormalization(data_norm_multi,(beat_found-1)*128,rpeak,rr,beat_found);


return 0;
	
}


