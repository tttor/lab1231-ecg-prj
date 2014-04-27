#ifndef _file_util_
#define _file_util_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

//#include "ecg_extraction_util.h"
#include "wavelet_statistic.h"

//proto
char read_csv_file(char *filename, double *arr_out, int max_line, double scale_data);
char read_csv_file2(char *filename, double *arr_out, int max_line, double scale_data, char verbose);
char read_csv_file_data_adc(char *filename, int *arr_out, int max_line);
int *read_sorted_order_from_file(char *filename,char *ext, int max_line);
//ecg_feature *read_ecg_feature_from_file(char *filename, char *ext, int max_line);

//write
//char write_ecg_feature_to_file_binary(char *filename, char *ext, int max_line, ecg_feature *aecg_feature, char verbose);
char write_double_to_file(char *filename,char *ext, char *fmt,int data_len , double *data_in);
char write_int_to_file(char *filename,char *ext, char *fmt,int data_len , int *data_in);
//char write_ecg_feature_to_file(char *filename, char *ext, int data_len , ecg_feature *fitur_ecg, char verbose);
char write_wavelet_statistic_to_file(char *filename, char *ext, int data_len , wavelet_statistic_out *wv_stat);
char write_double_as_matrix_to_file(char *filename, char *ext, int data_len , int row_len, double *data_in, char verbose);

//multilead
char write_double_multichannel_to_file(char *filename, char *ext, int data_len, double *data_in, char verbose);
char write_multichannel_arranged_data_to_file(char *filename, char *ext, int sample_len, double *data_in, char verbose);
char write_raw_adc_multichannel_to_file(char *filename, char *ext, int sample_len, int *data_in, char verbose);

//piksel file
char write_char_to_file(char *filename,char *ext, char *fmt,int data_len , unsigned char *data_in, char is_verbose);

/*
 * read csv 1 column style file
 * into array
 * */
char read_csv_file(char *filename, double *arr_out, int max_line, double scale_data)
{
	char resx = 0;
	FILE *fp;

	fp = fopen(filename,"r");
	if(fp)
		{
			int iread, id_line;

			id_line = 0;
			while(!feof(fp))
				{
					if(id_line<max_line)
						{
							iread = fscanf(fp,"%lg",&arr_out[id_line]);
						}
					else
						{
							break;
						}
					id_line++;
				}
			fclose(fp);

			//scale
			id_line = 0;
			while(id_line<max_line)
				{
					arr_out[id_line] /= scale_data;
					id_line++;
				}
		}
	else
		{
			resx = 1;
		}

	return resx;
}

/*
 * read csv 8 column style file
 * into array
 * */
char read_csv_file2(char *filename, double *arr_out, int max_line, double scale_data, char verbose)
{
	char resx = 0;
	FILE *fp;

	fp = fopen(filename,"r");
	printf("masuuuk read \n");


	if(fp)
		{

			printf("berhasil di read\n");

			int iread, id_line;

			id_line = 0;
			fscanf(fp, "%*[^\n]\n", NULL);	//skipp first line
			while(!feof(fp))
				{
					if(id_line<(max_line<<3))
						{
							iread = fscanf(fp,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&arr_out[id_line],&arr_out[id_line+1],&arr_out[id_line+2],&arr_out[id_line+3],&arr_out[id_line+4],&arr_out[id_line+5],&arr_out[id_line+6],&arr_out[id_line+7]);
						}
					else
						{
							break;
						}
					id_line+=8;
				}
			fclose(fp);

			//scale
			id_line = 0;
			while(id_line<(max_line<<3))
				{
					arr_out[id_line] /= scale_data;
					id_line++;
				}
		}
	else
		{
			printf("tidak terbaca \n");

			resx = 1;
		}

	if(verbose)
		printf("\nread input file from %s = %d", filename, resx);

	return resx;
}

/*
 * read csv 8 column style file
 * into array
 * */
char read_csv_file_data_adc(char *filename, int *arr_out, int max_line)
{
	char resx = 0;
	FILE *fp;

	fp = fopen(filename,"r");
	if(fp)
		{
			int iread, id_line;

			id_line = 0;
			while(!feof(fp))
				{
					if(id_line<(max_line<<3))
						{
							iread = fscanf(fp,"%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld",&arr_out[id_line],&arr_out[id_line+1],&arr_out[id_line+2],&arr_out[id_line+3],&arr_out[id_line+4],&arr_out[id_line+5],&arr_out[id_line+6],&arr_out[id_line+7]);
						}
					else
						{
							break;
						}
					id_line+=8;
				}
			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	return resx;
}

/*
 * read sort order
 * into array
 * */
int *read_sorted_order_from_file(char *filename, char *ext, int max_line)
{
	int ix;
	int *result = (int *) malloc(max_line * sizeof(int));
	char fn[200];
	FILE *fp;

	snprintf(fn,199,"%s%s", filename, ext);

	fp = fopen(fn,"r");
	if(fp)
		{
			int idx = 0;
			while(!feof(fp))
				{
					fscanf(fp,"%d", &ix);
					result[idx] = ix;
					idx++;
				}
			fclose(fp);
		}
	else
		{
			return NULL;
		}

	return result;
}

/*
 * read ecg feature
 * into array
 * */
/*
ecg_feature *read_ecg_feature_from_file(char *filename, char *ext, int max_line)
{
	int ix;
	ecg_feature *result = (ecg_feature *) malloc(max_line * sizeof(ecg_feature));
	char fn[200];
	FILE *fp;

	snprintf(fn,199,"%s%s", filename, ext);

	fp = fopen(fn,"rb");
	if(fp)
		{
			int idx = 0;
			while(!feof(fp) && idx<max_line)
				{
					fread(&result[idx],sizeof(ecg_feature), 1, fp);
					idx++;
				}
			fclose(fp);
		}
	else
		{
			return NULL;
		}

	return result;
}
*/
/*
 * write ecg feature
 * into file
 * */
/*
char write_ecg_feature_to_file_binary(char *filename, char *ext, int max_line, ecg_feature *aecg_feature, char verbose)
{
	int ix;
	char fn[200];
	FILE *fp;

	snprintf(fn,199,"%s%s", filename, ext);

	fp = fopen(fn,"wb+");
	if(fp)
		{
			for(ix=0;ix<max_line;ix++)
				fwrite(&aecg_feature[ix], sizeof(ecg_feature),1,fp);
			fclose(fp);
		}
	else
		{
			return 1;
		}

	if(verbose)
		printf("\nsave data to %s = 0", fn);

	return 0;
}
*/

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
 * write int to file
 * */
char write_int_to_file(char *filename,char *ext, char *fmt,int data_len , int *data_in)
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
 * write ecg feature to file
 * */
 /*
char write_ecg_feature_to_file(char *filename, char *ext, int data_len , ecg_feature *fitur_ecg, char verbose)
{
	char resx = 0;
	FILE *fp, *fr;
	char fnx[100], fnr[100];	//max 100 char

	//build the filename
	strcpy(fnx, filename);
	strcat(fnx, ext);

	fp = fopen(fnx,"w+");	//all the feature

	strcpy(fnr, fnx);
	strcat(fnr, ".rpeak");	//rpeak filename
	fr = fopen(fnr, "w+");
	if(fp)
		{
			//write it
			int i;
			fprintf(fp, "No\tP\tOq\tQ\tR\tS\tOs\tT\n");
			for(i=0;i<data_len;i++)
			{
				fprintf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",i+1, fitur_ecg[i].P, fitur_ecg[i].Oq, fitur_ecg[i].Q, fitur_ecg[i].R, fitur_ecg[i].S,fitur_ecg[i].Os, fitur_ecg[i].T);

				fprintf(fr,"%d\n", fitur_ecg[i].R);
			}

			fclose(fp);
			fclose(fr);
		}
	else
		{
			resx = 1;
		}

	if(verbose)
		printf("\nsave data to %s = %d\nsave data to %s = %d", fnx, resx, fnr, resx);

	return resx;
}
*/

/*
 * write wavelet stat to file
 * */
char write_wavelet_statistic_to_file(char *filename, char *ext, int data_len , wavelet_statistic_out *wv_stat)
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
			fprintf(fp, "No\tName\tMean\tMedian\tMax\tMin\tRange\tStDev\n");
			for(i=0;i<data_len;i++)
				fprintf(fp, "%d\t%s\t%g\t%g\t%g\t%g\t%g\t%g\n",i+1, wv_stat[i].name, wv_stat[i].mean, wv_stat[i].median, wv_stat[i].max, wv_stat[i].min,wv_stat[i].range, wv_stat[i].stdev);

			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	return resx;
}

/*
 * write double to file
 * with multiple row
 * */
char write_double_as_matrix_to_file(char *filename, char *ext, int data_len , int row_len, double *data_in, char verbose)
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
			int i,j,
				k = (int) data_len/row_len;

			for(i=0;i<row_len;i++)
			{
				for(j=0;j<k;j++)
				{
					fprintf(fp, "%g%s", *data_in++,(j==(k-1)) ? "\n":" ");
				}
			}
						//~
			//~ for(i=0;i<data_len;i++)
				//~ fprintf(fp, fmt, data_in[i]);

			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	if(verbose)
		printf("\nsave data to %s = %d", fnx, resx);

	return resx;
}

/*
 * write double multichannel to file
 * */
char write_double_multichannel_to_file(char *filename, char *ext, int data_len, double *data_in, char verbose)
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
			int i,j;
			fprintf(fp, "CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8\n" );
			for(i=0;i<(data_len<<3);i++)
			{
				fprintf(fp, "%.15g%s", data_in[i], (i%8==7) ? "\n":"," );
			}

			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	if(verbose)
		printf("\nsave data to %s = %d", filename, resx);

	return resx;
}

/*
 * write raw adc data to file
 * raw data is ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8,ch1,ch2,ch3,......,ch8
 * */
char write_raw_adc_multichannel_to_file(char *filename, char *ext, int sample_len, int *data_in, char verbose)
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
			int i,j;
			fprintf(fp, "CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8\n" );
			for(i=0;i<(sample_len<<3);i++)
			{
				fprintf(fp, "%d%s", data_in[i], (i%8==7) ? "\n":"," );
			}

			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	if(verbose)
		printf("\nsave data to %s = %d", filename, resx);

	return resx;
}

/*
 * save multichannel arrange data to files
 * */
char write_multichannel_arranged_data_to_file(char *filename, char *ext, int sample_len, double *data_in, char verbose)
{
	char resx = 0;
	FILE *fp;
	char fnx[100];	//max 100 char

	//build the filename
	strcpy(fnx, filename);
	strcat(fnx, ext);

	//~ printf("\nsave to %s", fnx);

	fp = fopen(fnx,"w+");
	if(fp)
		{
			//write it
			int i,j,k;
			fprintf(fp, "CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8\n" );
			for(i=0;i<sample_len;i++)	//loop per sample len
			{
				//~ for(j=(i*sample_len);j<((i+1)*sample_len);j++)	//loop
				for(j=0;j<8;j++)
					fprintf(fp, "%.15g%s", data_in[(j*sample_len)+i], (j==7) ? "\n":"," );
			}

			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	if(verbose)
		printf("\nsave data to %s = %d", fnx, resx);

	return resx;
}

/*
 * write char to file
 * */
char write_char_to_file(char *filename,char *ext, char *fmt,int data_len , unsigned char *data_in, char is_verbose)
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

	if(is_verbose)
		printf("\nsave data to %s = %d", fnx, resx);

	return resx;
}

#ifdef __cplusplus
}
#endif

#endif
