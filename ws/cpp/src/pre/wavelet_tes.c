#include <stdio.h>
#include <math.h>
#include <string.h>

//~ #include "daubechies4.h"

#include <gsl/gsl_wavelet.h>

/*
 * scale some detail coeff level of data_input wavelet
 * 
 * */
double *wavelet_scale_detail_coeff(int len_in, double *data_in, double scale, int level_detail)
{
	int i,
		max_level = log(len_in) / log(2);
	int level_ok = (level_detail<1) ? 1: (level_detail>max_level) ? (max_level):level_detail;
	
	//allocate memory
	double *data_out = (double *) malloc(len_in * sizeof(double));
	
	//debug
	//printf("\nscale = %g\tlevel = %d\n", scale, level_ok);
	
	if(data_out != NULL)
	{
		//copy data in
		memcpy(data_out, data_in, len_in * sizeof(double));
		
		//debug
		//~ printf("\nd1 removed input\n");
		//~ for (i = 0; i < len_in; i++)
		//~ {
		  //~ printf ("%g\n", data_out[i]);
		//~ }
		
		//multiply with scale
		//~ printf("\nscaling element level %d from %d to %d\n", level_ok, (len_in>>level_ok),  (len_in>>(level_ok-1))-1);
		for(i=(len_in>>level_ok);i<(len_in>>(level_ok-1));i++)
			data_out[i] *= scale;
			
		if(level_ok==max_level)
		{
			data_out[0] *= scale; 
			data_out[1] *= scale;		
		}
		//debug
		//~ printf("\nd1 after removed input\n");
		//~ for (i = 0; i < len_in; i++)
		//~ {
		  //~ printf ("%g\n", data_out[i]);
		//~ }
	}
	
	return data_out;
}

/*
 * write double to file
 * */
char write_double_to_file(char *filename, char *ext, char *fmt,int data_len , double *data_in)
{
	char resx = 0;
	FILE *fp;
	char fnx[100];	//max 100 char
	
	//build the filename
	strcpy(fnx, filename);
	strcat(fnx, ext);

	fp = fopen(fnx,"w+");
	if(fp)
		{
			//write it
			int i;
			for(i=0;i<data_len;i++)
				fprintf(fp, fmt, data_in[i]);
			
			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	return resx;
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
double *denoising_data_mra(int data_len, double *data_in, int freq_sampling)
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

int
main (int argc, char **argv)
{
  int i, n = atoi(argv[2]), nc = 20;
  double *data = malloc (n * sizeof (double));
  double *clean = malloc (n * sizeof (double));

  FILE * f;
  //~ gsl_wavelet *w;
  //~ gsl_wavelet_workspace *work;

  //~ w = gsl_wavelet_alloc (gsl_wavelet_haar, 2);	//error 7mV -- more precise
  //~ w = gsl_wavelet_alloc (gsl_wavelet_daubechies_centered, 4);	//rough
  //~ work = gsl_wavelet_workspace_alloc (n);
  
  //~ w->nc = 0;
  //~ w->offset = atoi(argv[5]);

  f = fopen (argv[1], "r");
  for (i = 0; i < n; i++)
    {
      fscanf (f, "%lg", &data[i]);
    }
  fclose (f);
  
  //~ printf("\nwavelet name = %s\nnc = %d\noffset = %d",w->type->name, w->nc, w->offset);
  
  printf("\nsave data to %s.data = %d", argv[1], write_double_to_file(argv[1],".data", "%.15lg\n", n,  data));
  
  //copy data
  memcpy(clean, data, n * sizeof (double));
  
  //clean up per 1024 sample with mra
  clean = denoising_data_mra(n, data, 250);
  
  //do transform
  //~ gsl_wavelet_transform_forward (w, data, 1, n, work);
  
  //remove level detail coeff
  //~ printf("\nremoving level %d to %d\n", atoi(argv[3]), atoi(argv[4]));
  //~ for(i=atoi(argv[3]);i<=atoi(argv[4]);i++)
  //~ {
	  //~ data = wavelet_scale_detail_coeff(n, data, 0, i);
  //~ }
  
  //write file
	//~ printf("\nsave wavelet input signal to %s.data.wavelet = %d", argv[1], write_double_to_file(argv[1],".data.wavelet", "%g\n", n,  data));
	//~ printf("\nsave data to %s.data.wavelet1 = %d", argv[1], write_double_to_file(argv[1],".data.wavelet1", "%.15lg\n", n,  data));
    
  //~ gsl_wavelet_transform_inverse (w, data, 1, n, work);
  
  //~ printf("\nsave data to %s.data.wavelet2 = %d", argv[1], write_double_to_file(argv[1],".data.wavelet2", "%.15lg\n", n,  data));
  
  //~ for (i = 0; i < n; i++)
    //~ {
      //~ printf ("%.15g\n", data[i]);
    //~ }
    
   
	//calc bwr
  for (i = 0; i < n; i++)
    {
      data[i] -= clean[i];
    }
    
  printf("\nsave bwr to %s.bwr = %d", argv[1], write_double_to_file(argv[1],".bwr", "%.15lg\n", n,  data));
  
  //~ for (i = 0; i < n; i++)
    //~ {
      //~ clean[i] -= data[i];
    //~ }
    
   printf("\nsave clean signal to %s.clean = %d", argv[1], write_double_to_file(argv[1],".clean", "%.15lg\n", n,  clean));
  
  //~ gsl_wavelet_free (w);
  //~ gsl_wavelet_workspace_free (work);

  free (data);
  //~ free (abscoeff);
  //~ free (p);
  return 0;
}
