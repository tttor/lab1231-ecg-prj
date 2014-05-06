#ifndef _file_util_
#define _file_util_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>



char read_csv_file(char *filename, double *arr_out, int max_line, double scale_data, char verbose);

/*
 * read csv 8 column style file
 * into array
 * */
char read_csv_file(char *filename, double *arr_out, int max_line, double scale_data, char verbose)
{
	char resx = 0;
	FILE *fp;

	printf("sebelum fp\n");
	fp = fopen(filename,"r");
	

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


#ifdef __cplusplus
}
#endif

#endif
