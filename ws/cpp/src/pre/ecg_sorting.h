#ifndef _ecg_sorting_
#define _ecg_sorting_

#ifdef __cplusplus
	extern "C" {
#endif

//lib
#include <gsl/gsl_statistics_double.h>	//for mean and other

//proto void
double calc_signal_power(double *input, int input_len);
void swap_frame_signal(double *input, int from_id, int to_id, int len_per_frame);
int find_closest_similar_signal_frame_id(double *input,int frame_template_id, int start_find_id, int frame_num, int sample_per_frame);
double *sort_by_similarity(double *input,int input_len,int frame_num, int sample_per_frame, int *frame_order );

/*
 * find power of signal
 * */
double calc_signal_power(double *input, int input_len)
{
	return sqrt( gsl_stats_tss_m(input, 1, input_len, 0) );
}

/*
 * swap signal frame
 * will override signal input
 * */
void swap_frame_signal(double *input, int from_id, int to_id, int len_per_frame)
{
	double *tmp1 = (double *) malloc( len_per_frame * sizeof(double) );
	
	//printf("\ntmp1 alloc ok\n");
	
	//save from_id signal
	memcpy(tmp1, &input[from_id * len_per_frame], len_per_frame * sizeof(double));
	
	//swap to_id to from_id
	memcpy(&input[from_id * len_per_frame], &input[to_id * len_per_frame], len_per_frame * sizeof(double));
	
	//copy tmp1 to to_id
	memcpy(&input[to_id * len_per_frame], tmp1, len_per_frame * sizeof(double));
	
	//printf("\ntmp1 to to_id ok\n");
	
	free(tmp1);
	
	//printf("\ntmp1 dealloc ok\n");
}

/*
 * find similar signal id with the template
 * */
int find_closest_similar_signal_frame_id(double *input,int frame_template_id, int start_find_id, int frame_num, int sample_per_frame)
{
	int i, closest_frame_id=0;
	//double *tmpi = (double *) malloc( sample_per_frame * sizeof(double) );
	double cr = 0, tcr;	//start with uncorellated signal
	
	//debug
	//~ printf("\nsearch from %d", start_find_id);
		
	for(i=start_find_id;i<frame_num;i++)
	{
		//calculate norm cross corellation value
		tcr = gsl_stats_correlation(&input[frame_template_id * sample_per_frame],1,&input[i * sample_per_frame],1,sample_per_frame);
		
		//debug
		//printf("\n%d\tcr = %g\ttcr = %g", i, cr, tcr);
		
		//close to 1
		if(tcr>cr)		
		{
			cr = tcr;
			closest_frame_id = i;
		}
	}
	
	//debug
	//~ printf("\nclosest frame = %d (%g)", closest_frame_id, cr);
	
	return closest_frame_id;
}

/*
 * sort signal input with the biggest energy first
 * by its similarity using normalize cross correlation pearson
 * */
double *sort_by_similarity(double *input,int input_len,int frame_num, int sample_per_frame, int *frame_order )
{
	//var
	//int frame_num = input_len/sample_per_frame;
	int i,j,k;	
	double *out_ordered = (double *) malloc(input_len * sizeof(double));
	//int *tmp_frame_id = (int *) malloc(frame_num * sizeof(int));
	
	//frame_order = (int *) malloc(frame_num * sizeof(int));
	
	//fill frame temp info
	for(i=0;i<frame_num;i++)
	{
		frame_order[i] = i;
	}
	
	//~ //debug 
	//~ printf("\nframe order =\n");
	//~ for(i=0;i<frame_num;i++)
	//~ {
		//~ printf("%d\n", tmp_frame_id[i]);
	//~ }
	
	//find biggest power frame
	double max_pwr = 0, tmp_pwr;
	int max_pwr_frame_id = 0;
	for(i=0;i<frame_num;i++)
	{
		tmp_pwr = calc_signal_power(&input[i * sample_per_frame], sample_per_frame);
		
		//debug
		//~ printf("\npwr[%d] = %g (max = %g)", i, tmp_pwr, max_pwr);
		
		if(tmp_pwr>max_pwr)
		{
			max_pwr = tmp_pwr;
			max_pwr_frame_id = i;
		}
	}
	
	//debug
	//~ printf("\nbiggest frame power of %d = %g at frame %d",frame_num, max_pwr, max_pwr_frame_id);
	
	//reorder the frame sequence
	//TODO HERE : find frame number sequence!!!!
	j = frame_order[0];
	frame_order[0] = frame_order[max_pwr_frame_id];
	frame_order[max_pwr_frame_id] = j;
	//frame_order[0] = max_pwr_frame_id;
	
	//debug 
	//~ printf("\nframe order =\n");
	//~ for(i=0;i<frame_num;i++)
	//~ {
		//~ printf("%d\n", frame_order[i]);
	//~ }
	
	//copy signal input
	mempcpy(out_ordered, input, input_len * sizeof(double));
	
	//debug
	//~ printf("\ncopy signal ok\n");
	
	//copy ordered input to output
	int id1;
	if(max_pwr_frame_id!=0)
		swap_frame_signal(out_ordered, 0, max_pwr_frame_id, sample_per_frame);	//swap frame id 0 with the maxest
		
	//debug
	//~ printf("\nswap frame ok\n");
	
	for(i=1;i<frame_num;i++)
	{
		//debug 
		//~ printf("\nframe order before calc at %d =\n", i);
		//~ for(k=0;k<frame_num;k++)
		//~ {
			//~ printf("%d-", tmp_frame_id[k]);
		//~ }
		
		//calculate most similar signal with the previous one
		//DONE!!!!!!
		id1 = find_closest_similar_signal_frame_id(out_ordered, i-1, i, frame_num, sample_per_frame);
		
		//save it
		j = frame_order[i];
		frame_order[i] = frame_order[id1];
		frame_order[id1] = j;
		
		//debug 
		//~ printf("\nframe order at %d =\n", i);
		//~ for(k=0;k<frame_num;k++)
		//~ {
			//~ printf("%d-", tmp_frame_id[k]);
		//~ }
		
		//debug
		//~ printf("\nclosest order %d = %d\n", i, id1);
		
		//swap it
		swap_frame_signal(out_ordered, i, id1, sample_per_frame);	//swap frame id i with the maxest
	}	
	
	//debug 
	//~ printf("\nframe order =\n");
	//~ for(i=0;i<frame_num;i++)
	//~ {
		//~ printf("%d\n", frame_order[i]);
	//~ }
	
	//free mem
	//free(tmp_in);
	
	//return
	return out_ordered;
}

#ifdef __cplusplus
}
#endif

#endif
