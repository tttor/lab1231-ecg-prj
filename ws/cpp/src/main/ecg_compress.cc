#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>

#include <ecg_signal_preprocessor.h>
#include <ecg_file_util.h>
#include <ecg_array_const.h>
#include <ecg_spiht.h>

int main(int argc, char **argv) {
  using namespace lab1231_ecg_prj;
  
  //Inisialisasi variable
	int i,j, 
		sample_number_tested = 46260,
		sample_rate_data_input = 250,	
		sample_num_per_frame = 128,
    lead_num = 8, 
    CR = 4,		
    resolution = 12,
    max_bits = sample_number_tested*lead_num*resolution/CR,	
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
	}
	
	
	//print info
	printf("\n%s\nData Analysis Info\n" \
	"%-25s : %s\n" 
	"%-25s : %d\n"  
	"%-25s : %d sps\n"  
	"%-25s : %d\n" 
  "%-25s : %d\n" 
  "%-25s : %d\n" 
  "%-25s : %d\n"
  "%-25s : %d\n" 
  "%s\n",
	line, 
	"File", fname, 
	"Sample Number", sample_number_tested, 
	"Sample Rate", sample_rate_data_input, 
	"Sample Per Frame", sample_num_per_frame,
  "Lead Num", lead_num,
  "CR", CR,
  "Resolution", resolution,
  "Max Bits", max_bits,
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

	std::vector< double > data_norm_multi;
  std::vector< int > rpeak;
  std::vector< int > rr;
  
	resx = ECGSignalPreprocessor::preprocess(fname,sample_number_tested, data_voltage, sample_rate_data_input, data_norm_multi, rpeak, rr, verbose_mode);
	
	//printf("data_norm multi %d \n",data_norm_multi.size());
	//ECGFileUtil::write_multichannel_arranged_data_to_file_vector(fname, ".normalize.csv", data_norm_multi.size()/8, 8, data_norm_multi, verbose_mode);
	
  //OVERWRITE HASIL NORMALISASI biar ga lama
  double *temp_norm_overwrite = new double[35456<<3];
  resx = ECGFileUtil::read_csv_file(fname, temp_norm_overwrite, 35456, 1, verbose_mode);
  ECGSignalPreprocessor::adc_rearrange_voltage(35456, temp_norm_overwrite);
  
  for(i=0;i<35456*8;i++){
    data_norm_multi.push_back(temp_norm_overwrite[i]);  
  }
  
  
  //selesai OVERWRITE
  
  std::vector< std::vector<double> > frame_all;
  
	ECGArrayContructor::set_array_to_frame(sample_num_per_frame, rpeak.size(), data_norm_multi, frame_all, verbose_mode);
	
  //printing
  std::vector<double> print_frame_all;
  for(i=0;i<frame_all.size();i++){
    for(j=0;j<pow(sample_num_per_frame,2);j++){
      print_frame_all.push_back(frame_all[i][j]);
    }
  }
  ECGFileUtil::write_multichannel_arranged_data_to_file_vector(fname, ".frame.csv", pow(sample_num_per_frame,2), 18,  print_frame_all, verbose_mode);
  
  //panggil masing masing matrix
  std::vector< std::vector<double> > mat_1;
  std::vector< double > temp_frame; 
  
  //cetak 1 matrix
  //dipanggil satu persatu lalu di spiht-kan
  
  cout << "SPIHT coding : BEGIN\n";
  int frame_num = frame_all.size();
  max_bits = round(max_bits/frame_num);
  int percent = 0;
  int ten_percent = floor(frame_num/10);
  vector< int > bit_str_1;
  std::vector< std::vector<int> > bit_str_all;
  
  //printf("frame num %d\n",frame_num);
  //printf("max bit %d\n",max_bits);
  //printf("ten_percent %d\n",ten_percent);
  
  for(i=0;i<1;i++){
    if((i%ten_percent)==0 && percent < 100){
      percent = 10*i/ten_percent;
      printf("Percent : %3.0f\n",percent);
    }
    for(j=0;j<pow(sample_num_per_frame,2);j++){
      temp_frame.push_back(frame_all[i][j]); 
    }
    ECGArrayContructor::set_frame_to_matrix(sample_num_per_frame, temp_frame, mat_1);
    
    ECGSpiht::get_spiht_enc(max_bits,mat_1,bit_str_1); 
    bit_str_all.push_back(bit_str_1);
    bit_str_1.clear();
  }
  
  //for(i=0;i<pow(sample_num_per_frame,2);i++){
    //temp_frame.push_back(frame_all[0][i]);
  //}
  
  //ECGArrayContructor::set_frame_to_matrix(sample_num_per_frame, temp_frame, mat_1);  
  
  //std::vector< double > print_mat_1; 
  //for(i=0;i<mat_1.size();i++){
    //for(j=0;j<mat_1.size();j++){
      //print_mat_1.push_back(mat_1[i][j]);
    //}
  //}
  //printf("mat_1 size %d", mat_1.size());
  //ECGFileUtil::write_multichannel_arranged_data_to_file_vector(fname, ".mat_1.csv", sample_num_per_frame, 128,  print_mat_1, verbose_mode);
  
  
	cout << "Compression: END\n";
  
  return 0;
}
