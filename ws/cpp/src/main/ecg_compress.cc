
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>


#include <stdio.h>
#include <string.h>	
#include <stdlib.h>

#include <ecg_signal_preprocessor.h>
#include <ecg_file_util.h>
#include <ecg_array_const.h>

int main(int argc, char **argv) {
  using namespace std;
  using namespace lab1231_ecg_prj;
  
  //Inisialisasi variable
	int i,j, 
		//sample_number_tested = 16384,
		sample_number_tested = 46260,
		//sample_number_tested = 1024,
		//sample_number_tested = 65536,
		
		sample_rate_data_input = 250,	
		sample_num_per_frame = 128,			
		mode_program = -1,				//program work mode : -1=from file , 0=adc noise tes mode, 1=adc signal tes mode, 2=adc real measurement mode
		verbose_mode = 1;					//0=no message, else = display message
		
	double *data_voltage;
	char resx;
		
		
	char *fname;
	static char *line = "======================================================";
	static char *guide = 
		"-f	= filename\n" \
		"-M = work mode (default -1)\n" \
		"     -1 = ADC data from file. Continued with 2D-SPIHT encoding with bit rate specified by -b\n";
	
/*
	 * PARSING COMMAND LINE
	 * */
	if(argc<2){
		printf("not enough argument. read below for options\n%s", guide);
		return -1;
	}
	else {
		for(i=1;i<(argc-1);i++)
		{
			//filename
			if(strcmp("-f",argv[i])==0)
				fname = argv[++i];
			
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
	
	if(sample_number_tested>0)
	{
		data_voltage = new double[sample_number_tested<<3];
		//data_clean = new double[sample_number_tested<<3]; 
		
	}
	
	
	//print info
	printf("\n%s\nData Analysis Info\n" \
	"%-25s : %s\n" 
	"%-25s : %d\n"  
	"%-25s : %d sps\n"  
	"%-25s : %d\n"  
	"%s\n",
	line, 
	"File", fname, 
	"Sample Number", sample_number_tested, 
	"Sample Rate", sample_rate_data_input, 
	"Sample Per Frame", sample_num_per_frame,
	line
	);
	
	cout << "Compression: BEGIN\n";
	cout << "Read File :BEGIN\n";
	
	/*
	 * READ FILE MULTILEAD IN mode_program = -1
	 * */
	if(mode_program==-1)	//FILE Mode
	{
		//read file , make the order like Pak Sani's code
		 resx = ECGFileUtil::read_csv_file(fname, data_voltage, sample_number_tested, 1, verbose_mode);
		//char resx = read_csv_file(fname, data_voltage, sample_number_tested, 1, verbose_mode);
		
		if(resx!=0)
		{
			printf("cannot read input file.\nprogram terminated\n");
			return 0;
		}
	}
	
	
	/*
	 * START OF NORMAL OPERATION
	 * */	
	/*
	 * WRITE DATA TO FILE
	 * data IN FORM OF VOLTAGE = CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH1,CH2,CH3,CH4,CH5,CH6,CH7.......
	 * */
	ECGFileUtil::write_double_multichannel_to_file(fname,".data.csv", sample_number_tested,  data_voltage, verbose_mode);

	vector< double > data_norm_multi;
	resx = ECGSignalPreprocessor::preprocess(fname,sample_number_tested, data_voltage, sample_rate_data_input, data_norm_multi, verbose_mode);
	
	printf("data_norm multi %d \n",data_norm_multi.size());
	ECGFileUtil::write_multichannel_arranged_data_to_file_vector(fname,".normalize.csv",data_norm_multi.size()/8,  data_norm_multi, verbose_mode);
	
	//ECGARRAYCONST::set_array_to_matrix(data_norm_multi,sample_number_tested,verbose_mode);
	
	cout << "Compression: END\n";
  
  return 0;
}
