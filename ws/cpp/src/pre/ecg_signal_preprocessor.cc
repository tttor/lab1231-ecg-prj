
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>



#include "ecg_signal_preprocessor.h"
#include <ecg_file_util.h>
#include <ecg_norm.h>
  
#include "ecg_denoising.h"  
#include "qrs_det.h"

  
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



char multichannel_normalization(double *input_signal,int sample_len,int *input_rpeak, int *input_rr, int beat_found,vector<double> & vmulti) 
{
	using namespace std;
	char resx = 0;
	printf("Multichanel normalization\n");
	int i,j,k;

	
	//double *norm_multi = new double[(((beat_found-1)*128)<<3)];
	//vector< double > vmulti;
	
	//double signal_olah[sample_len];
	
	vector< double > signal_olah;
	vector< double > norm_single;

	//sementara 
	int start=0;
	int end=0;
	
	for (j = 0; j < 8; j++)
	{
		//olah semua lead
		signal_olah.clear();
		norm_single.clear();
		
		start = end;
		end = start+sample_len; 
		for (i = start; i < end; ++i)
		{
			signal_olah.push_back(input_signal[i]);
		}
		
		printf("signal olah %d\n", signal_olah.size());
		
		char resx =  ECGNormalizer::norm_lead(beat_found,signal_olah,input_rpeak,input_rr, norm_single);
  
        for(k=0;k<norm_single.size();k++)
		{
			vmulti.push_back(norm_single[k]);
		}

       
		
        printf("Norm lead size %d\n",norm_single.size());
		printf("Normalisasi Lead %d\n",j+1);
		
		
	}
	
		
	//for(i=0;i<vmulti.size();i++){
		//norm_multi[i]=vmulti[i];
	//}
	
	printf("size multi%d", vmulti.size());
	//vmulti.clear();	

	//return norm_multi;
	return resx;
}



char ECGSignalPreprocessor::preprocess(char *fname,int sample_len, double *data_voltage, int adc_sampling_rate, vector<double> & data_out, vector<int>& rpeak_out, vector<int>& rr_out, char verbose_mode) {
  using namespace std;
  char resy = 0;
  cout << "Preprocessing: BEGIN\n";
  
  double *data_clean = new double[sample_len<<3]; 
  int wave_len = 65536;
  double *temp_data_clean = new double[wave_len<<3]; 
  
  int i,j,k;
  
  ///*
	 //* ARRANGE VOLTAGE DATA WITH ORDER CH1-CH8 
	 //* sample_number_tested EACH
	 //* */
	//adc_rearrange_voltage(sample_len, data_voltage);
		
	///*
	 //* COPY IT TO data_clean
	 //* */
	////memcpy(data_clean, data_voltage, (sample_len<<3) * sizeof(double));
		
	///*
	 //* WRITE data_voltage ARRANGED TO FILE
	 //* */
	//ECGFileUtil::write_multichannel_arranged_data_to_file(fname,".data_arrange.csv",sample_len,  data_voltage, verbose_mode);
	
	//for(k=0;k<sample_len*8;k++)	//loop per sample len
		//{	
			//temp_data_clean[k]=data_voltage[k];				
		//}	
			
	///*
	 //* WRITE data_voltage ARRANGED TO FILE
	 //* */
	////ECGFileUtil::write_multichannel_arranged_data_to_file(fname,".data_temp",wave_len,  temp_data_clean, verbose_mode);
	///*
	 //* CLEANUP ALL CHANNEL
	 //* OUTPUT OVERRIDE INPUT
	 //* */
	 
	//adc_multichannel_denoising(wave_len, temp_data_clean, adc_sampling_rate) ;
	
	//for(k=0;k<sample_len*8;k++)	//loop per sample len
		//{	
			//data_clean[k]=temp_data_clean[k];
					
		//}	
			
	///*
	 //* WRITE CLEAN DATA TO FILE
	 //* */
	//ECGFileUtil::write_multichannel_arranged_data_to_file(fname,".clean.csv",sample_len,  data_clean, verbose_mode);
	
    /*
	 * RR DETECTION
	 * */
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
	beat_found=r_out[0]/2;
	
  //Set rr interval to support last beat
  int rpeak_new[beat_found+2];  
  int rr_new[beat_found+1];
  
  for(i=0;i<beat_found;i++){
    rpeak_new[i]=rpeak[i];
    rr_new[i]=rr[i];
  }
     
  rpeak_new[beat_found]=rpeak_new[beat_found-1]+rr_new[beat_found-1];
  rpeak_new[beat_found+1]=sample_len;
  rr_new[beat_found]=sample_len-rpeak_new[beat_found];
  
  for(i=0;i<beat_found+2-1;i++){
    rpeak_out.push_back(rpeak_new[i]);
    rr_out.push_back(rr_new[i]);
  }
  //for(i=0;i<beat_found+2-1;i++){
    //rpeak_out.push_back(rpeak_new[i]);
  //}   
  
  
   // vector< double > data_norm_multi;
   //OVERWRITE CLEAN DARI MATLAB
  resx = ECGFileUtil::read_csv_file(fname, data_clean, sample_len, 1, verbose_mode);
  adc_rearrange_voltage(sample_len, data_clean);
  multichannel_normalization(data_clean,sample_len,rpeak_new,rr_new,beat_found,data_out);
   
	
	cout << "Preprocessing: END\n";
	return resy;
	
	
}

  
