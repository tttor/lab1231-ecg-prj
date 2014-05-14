#include "ecg_signal_preprocessor.h"
#include <ecg_file_util.h>
  
#include "ecg_denoising.h"  
#include "qrs_det.h"
#include "normalization.h"
  
using namespace lab1231_ecg_prj;

/*
 * cleanup arranged adc voltage data
 * using mra
 * */
void ECGSignalPreprocessor::adc_multichannel_denoising(int sample_len, double *adc_volt_aligned, int adc_sampling_rate) 
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
 * rearrange adc data as stream of double from a channel
 * input : V1,V2,V3,V4,v5,V6,V7,V8,V1,V2,v3,....,V8
 * out : V1,V1,V1,V2,V2,V2,.....,V8,V8,V8
 * */
void ECGSignalPreprocessor::adc_rearrange_voltage(int adc_sample_len, double *adc_volt_data)
{
	int i,j,k;
	double *adc_volt_aligned = (double *) malloc((adc_sample_len<<3) * sizeof(double));	
	//double *adc_volt_aligned = new double[sample_number_tested<<3]; 	
	
	
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



double *multichannel_normalization(double *input_signal,int sample_len,int *input_rpeak, int *input_rr, int beat_found) 
{
	FILE *printN;
	printN = fopen("ambil_data_normalisasi.csv", "w");
	printf("Multichanel normalization\n");
	int i,j,k,m;

	double *norm_single =(double *) malloc((((beat_found-1)*128)<<3) * sizeof(double));
	
	//double *norm_multi =(double *) malloc((((beat_found-1)*128*8)<<3) * sizeof(double));
	double *norm_multi = new double[(((beat_found-1)*128*8)<<3)];
	double signal_olah[sample_len];

	//sementara 
	
	for (i = 0; i < beat_found; ++i)
	{
		printf("%d,%d\n", input_rpeak[i],input_rr[i]);
	}

	//jarak per bit baru
	//int input_rr_new[beat_found-1];
	//for (i = 0; i < beat_found-1; ++i)
	//{
		//input_rr_new[i]=input_rr[i];
	//}

	
	//for (i = 0; i < sample_len; ++i)
	//{
		//printf("%f,",input_signal[i]);
	//}
	int start=0;
	int end=0;
	int start_res=0;
	int end_res=0;

	
	for (j = 0; j < 2; j++)
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

	//	norm_single = bantuNormalization(signal_olah,input_rpeak,input_rr_new,beat_found);
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
	
	//for (i = 0; i < (beat_found-1)*128*8; ++i)
	//{
		////fprintf(printN, "%f,",norm_multi[i]);
		//norm_multi[i]=2.1;
	//}
	////single	
	
	//for (i = 0; i < (beat_found-1)*128; ++i)
	//{
		//fprintf(printN, "%f,",norm_single[i]);
	//}


////	return norm_single;
	return norm_multi;

}



void ECGSignalPreprocessor::preprocess(char *fname,int sample_len, double *data_voltage, int adc_sampling_rate, char verbose_mode) {
  using namespace std;
  
  cout << "Preprocessing: BEGIN\n";
  
  double *data_clean = new double[sample_len<<3]; 
  int wave_len = 65536;
  double *temp_data_clean = new double[wave_len<<3]; 
  
  int i,j,k;
  
  /*
	 * ARRANGE VOLTAGE DATA WITH ORDER CH1-CH8 
	 * sample_number_tested EACH
	 * */
	adc_rearrange_voltage(sample_len, data_voltage);
		
	/*
	 * COPY IT TO data_clean
	 * */
	memcpy(data_clean, data_voltage, (sample_len<<3) * sizeof(double));
		
	/*
	 * WRITE data_voltage ARRANGED TO FILE
	 * */
	ECGFileUtil::write_multichannel_arranged_data_to_file(fname,".data_arrange",sample_len,  data_voltage, verbose_mode);
	
	 
	 for(k=0;k<sample_len*8;k++)	//loop per sample len
		{	
			temp_data_clean[k]=data_voltage[k];
					
		}	
			
	/*
	 * WRITE data_voltage ARRANGED TO FILE
	 * */
	//ECGFileUtil::write_multichannel_arranged_data_to_file(fname,".data_temp",wave_len,  temp_data_clean, verbose_mode);
	
	/*
	 * CLEANUP ALL CHANNEL
	 * OUTPUT OVERRIDE INPUT
	 * */
	 
	adc_multichannel_denoising(wave_len, temp_data_clean, adc_sampling_rate) ;
	
	
	 for(k=0;k<sample_len*8;k++)	//loop per sample len
		{	
			data_clean[k]=temp_data_clean[k];
					
		}	
			
	/*
	 * WRITE CLEAN DATA TO FILE
	 * */
	ECGFileUtil::write_multichannel_arranged_data_to_file(fname,".clean",sample_len,  data_clean, verbose_mode);
	
    /*
	 * RR DETECTION
	 * */
	//int *rpeak,*rr;	
	//float *hP = highPass(data_clean, sample_len);
	//float *lP = lowPass(hP, sample_len);
	//int *qrs = QRS(lP, sample_len);
	//rpeak = getR(qrs, sample_len);
	//rr = getRR(rpeak, sample_len);
	
	//int i,beat_found;
	//printf("%d\n",sample_len);
	//beat_found=0;
	//while(1)
	//{
		//printf("%d,%d\n",rpeak[i],rr[i]);			
		//i++;
		//if(rpeak[i]!=0){
			//beat_found++;
		
		//}else {
			//break;
		//}
	//}
	
	//beat_found = beat_found + 2;
	//printf("\nbeat_found %d\n",beat_found);
	
	int *rpeak,*rr;	
	int beat_found;
	int *r_out = new int[sample_len];
	char resx = ECGFileUtil::read_RR_file(fname,r_out, verbose_mode);
	if(resx!=0)
	{
		printf("cannot read RR input file.\nprogram terminated\n");
	}
		
	rpeak = new int[r_out[0]/2];
	rr=new int[r_out[0]/2];
	int m,n;
	m=0;
	n=0;
	
	//printf("panjang %d\n",r_out[0]/2);
	for(i=1;i<r_out[0]+1;i++){
		
		if(i%2==0){ //rpeak
			rr[m]=r_out[i];
			m++;
			}
		else{
			rpeak[n]=r_out[i];
			n++;
			}
	
	}
	//for(i=0;i<r_out[0]/2;i++){
		//printf("%d %d \n",rpeak[i],rr[i]);
		
		//}
	
	beat_found=r_out[0]/2;
	 
	 //for(k=0;k<wave_len*8;k++)	//loop per sample len
		//{	
			//temp_data_clean[k]=0.0;
					
		//}	
	  //for(k=0;k<sample_len*8;k++)	//loop per sample len
		//{	
			//temp_data_clean[k]=data_clean[k];
					
		//}		
	
	//double *data_norm_multi = multichannel_normalization(data_clean,sample_len,rpeak,rr,beat_found);
   
	//ECGFileUtil::write_multichannel_arranged_data_to_file(fname,".normalize",(beat_found-1)*128,  data_norm_multi, verbose_mode);
	
  
	cout << "Preprocessing: END\n";
	
	
	
}

  
