#include <iostream>
#include <cstdlib>

#include <stdio.h>
#include <string.h>	//for memcpy
#include <stdlib.h>

#include <ecg_signal_preprocessor.h>

#include <ecg_file_util.h>

//#include <../util/file_util_old.h>

int main(int argc, char **argv) {
  using namespace std;
  using namespace lab1231_ecg_prj;
  
  //Inisialisasi variable
	int i,j, 
		sample_number_tested = 1024,
		sample_rate_data_input = 250,	
		sample_num_per_frame = 128,			
		mode_program = -1,				//program work mode : -1=from file , 0=adc noise tes mode, 1=adc signal tes mode, 2=adc real measurement mode
		verbose_mode = 1;					//0=no message, else = display message
		
	double *data_voltage;
	double *data_clean;
		
		
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
		data_clean = new double[sample_number_tested<<3]; 
		
	//	data_voltage = std::malloc ((sample_number_tested<<3) * sizeof (double));
		//data_clean = std::malloc ((sample_number_tested<<3) * sizeof (double));
		
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
	
	/*
	 * READ FILE MULTILEAD IN mode_program = -1
	 * */
	if(mode_program==-1)	//FILE Mode
	{
		//read file , make the order like Pak Sani's code
		char resx = ECGFileUtil::read_csv_file(fname, data_voltage, sample_number_tested, 1, verbose_mode);
		//char resx = read_csv_file(fname, data_voltage, sample_number_tested, 1, verbose_mode);
		
		if(resx!=0)
		{
			printf("\ncannot read input file.\nprogram terminated\n");
			return 0;
		}
	}
	
	cout << "File READY\n";
	
	for(i=1000;i<1024;i++){
		printf("data_voltage %f \n",data_voltage[i]);
		}
	
	/*
	 * START OF NORMAL OPERATION
	 * */	
	/*
	 * WRITE DATA TO FILE
	 * data IN FORM OF VOLTAGE = CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH1,CH2,CH3,CH4,CH5,CH6,CH7.......
	 * */
	ECGFileUtil::write_double_multichannel_to_file(fname,".data", sample_number_tested,  data_voltage, verbose_mode);
	
		
		
	cout << "Compression: BEGIN\n";
	ECGSignalPreprocessor::preprocess();
  
	cout << "Compression: END\n";
  
  return 0;
}
