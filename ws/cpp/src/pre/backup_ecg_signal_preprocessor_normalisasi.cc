using namespace std;
	FILE *printN;
	printN = fopen("ambil_data_normalisasi.csv", "w");
	printf("Multichanel normalization\n");
	int i,j,k,m;
	
	//sementara
	double *norm_multi = new double[(((beat_found-1)*128*8)<<3)];
	
	
	double *norm_single =new double[(((beat_found-1)*128)<<3)];
	vector< vector<double> > vec_multi_norm;
	vector< double > vec_single_norm;
	
	double signal_olah[sample_len];
	
	printf("beat %d",beat_found);
	
	
	int start=0;
	int end=0;
	int start_res=0;
	int end_res=0;
	
	
	for(j=0;j<8;j++){
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
	
		for(k=0;k<(beat_found-1)*128;k++){
		
			vec_single_norm.push_back(norm_single[k]);
		
		}
		
		vec_multi_norm.push_back(vec_single_norm);
	 printf("Normalisasi Lead %d\n",j+1);
	}
	
	
	
	
	return norm_multi;