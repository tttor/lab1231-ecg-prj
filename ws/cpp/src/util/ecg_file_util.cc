#include "ecg_file_util.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
  
using namespace lab1231_ecg_prj;

char ECGFileUtil::read_csv_file(char *filename, double *arr_out, int max_line, double scale_data, char verbose) {
	using namespace std;
	char resx = 0;
	 
	cout << "\nRead File :Begin\n";
	FILE *fp;
	fp = fopen(filename,"r");
	
	if(fp){
			printf("berhasil di read\n");
			
			int iread, id_line;
			id_line = 0;
			
			fscanf(fp, "%*[^\n]\n", NULL);	//skipp first line
			//printf("hahaha %d",max_line<<3);
			
			while(!feof(fp))
				{
					if(id_line<(max_line<<3))
						{
							//iread = fscanf(fp,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&arr_out[id_line],&arr_out[id_line+1],&arr_out[id_line+2],&arr_out[id_line+3],&arr_out[id_line+4],&arr_out[id_line+5],&arr_out[id_line+6],&arr_out[id_line+7]);
							//iread = fscanf(fp,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&tes[id_line],&tes[id_line+1],&tes[id_line+2],&tes[id_line+3],&tes[id_line+4],&tes[id_line+5],&tes[id_line+6],&tes[id_line+7]);
							iread = fscanf(fp,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&arr_out[id_line],&arr_out[id_line+1],&arr_out[id_line+2],&arr_out[id_line+3],&arr_out[id_line+4],&arr_out[id_line+5],&arr_out[id_line+6],&arr_out[id_line+7]);
						printf("%d idline",id_line);
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

	int i;
	for(i=0;i<8;i++){
		printf("arr_out %f \n",arr_out[i]);
		}
	
	if(verbose)
		printf("\nread input file from %s = %d", filename, resx);

	cout << "\nRead File: END\n";
	return resx;
  
	
}

/*
 * write double multichannel to file
 * */
char ECGFileUtil::write_double_multichannel_to_file(char *filename, char *ext, int data_len, double *data_in, char verbose)
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
			//for(i=0;i<(data_len*8);i++)
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

  
