#include "ecg_file_util.h"

 
using namespace lab1231_ecg_prj;

char ECGFileUtil::read_RR_file(char *filename, int *arr_out, char verbose) {
	using namespace std;
	char resx = 0;
	int i,j;
	 
	FILE *fp;
	char *root = "../../../../dataset/incartdb/";
	//char *root = "~//lab1231-ecg-prj//dataset//incartdb_csv//";
	char *ext = "-rr.txt";
	
	char *file_in = static_cast<char*>(malloc(strlen(root) + strlen(filename) + strlen(ext) + 1));
	if (file_in != NULL)
	{
		strcpy(file_in, root);
		strcat(file_in, filename);
		strcat(file_in, ext);
	}
	
	fp = fopen(file_in,"r");
	
	if(fp){
			printf("berhasil di read RR\n");
			
			int iread, id_line;
			id_line = 0;
			
			//fscanf(fp, "%*[^\n]\n", NULL);	//skipp first line
			//printf("hahaha %d",max_line<<3);
			
			while(!feof(fp))
			{
					//for (j=0;j<276;j++){
					
						//for(i=0;i<2;i++){
							iread = fscanf(fp,"%d",&arr_out[id_line+1]);
							id_line++;
						//}
						
				//	}
					
					
				}
			fclose(fp);
		//	printf("id_line %d\n", id_line-1);
			arr_out[0]=id_line-1;
		
		
		}
		
		else
		{
			printf("tidak terbaca \n");
			resx = 1;
		}
	
	//for(i=0;i<10;i++){
		//printf("arr_out %d \n",arr_out[i]);
		//}
		
	//printf("terkahir %d\n", arr_out[552]);
		
	if(verbose)
		printf("read RR file from %s = %d\n", file_in, resx);

	//cout << "\nRead File: END\n";
	return resx;
  
	
}


char ECGFileUtil::read_csv_file(char *filename, double *arr_out, int max_line, double scale_data, char verbose) {
	using namespace std;
	char resx = 0;
	 
	FILE *fp;
	char *root = "../../../../dataset/incartdb/";
	//char *root = "~//lab1231-ecg-prj//dataset//incartdb_csv//";
	char *ext = ".csv";
	
	char *file_in = static_cast<char*>(malloc(strlen(root) + strlen(filename) + strlen(ext)+ 1));
	if (file_in != NULL)
	{
		strcpy(file_in, root);
		strcat(file_in, filename);
		strcat(file_in, ext);
	}
	
	fp = fopen(file_in,"r");
	
	if(fp){
			//printf("berhasil di read\n");
			
			int iread, id_line;
			id_line = 0;
			
			fscanf(fp, "%*[^\n]\n", NULL);	//skipp first line
			//printf("hahaha %d",max_line<<3);
			
			while(!feof(fp))
				{
					if(id_line<(max_line<<3))
						{
							iread = fscanf(fp,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&arr_out[id_line],&arr_out[id_line+1],&arr_out[id_line+2],&arr_out[id_line+3],&arr_out[id_line+4],&arr_out[id_line+5],&arr_out[id_line+6],&arr_out[id_line+7]);
						//printf("%d idline",id_line);
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

	/*
	int i;
	for(i=0;i<8;i++){
		printf("arr_out %f \n",arr_out[i]);
		}
	*/
	if(verbose)
		printf("read input file from %s = %d\n", file_in, resx);

	//cout << "\nRead File: END\n";
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

	char *root = "../../../../dataset/cpp-exp/";
	
	//build the filename
	
	//strcpy(fnx, root);
	//strcpy(fnx, filename);
	//strcat(fnx, ext);
	
	strcpy(fnx, root);
	strcat(fnx, filename);
	strcat(fnx, ext);
	
	
	printf("\nsave to %s\n", fnx);


	fp = fopen(fnx,"w+");
	if(fp)
		{
			//write it
			int i,j;
		//	fprintf(fp, "CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8\n" );
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
		printf("save data to %s = %d\n", fnx, resx);

	return resx;
}

  
/*
 * save multichannel arrange data to files
 * */
char ECGFileUtil::write_multichannel_arranged_data_to_file(char *filename, char *ext, int sample_len, double *data_in, char verbose)
{
	char resx = 0;
	FILE *fp;
	char fnx[100];	//max 100 char

	//build the filename
	char *root = "../../../../dataset/cpp-exp/";
		
	strcpy(fnx, root);
	strcat(fnx, filename);
	strcat(fnx, ext);

	printf("\nsave to %s\n", fnx);

	fp = fopen(fnx,"w+");
	if(fp)
		{
			//write it
			int i,j,k;
			//fprintf(fp, "CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8\n" );
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
		printf("save data to %s = %d\n", fnx, resx);

	return resx;
}
  
 
/*
 * save multichannel arrange data to files
 * */
char ECGFileUtil::write_multichannel_arranged_data_to_file_vector(char *filename, char *ext, int sample_len, int coloumn, std::vector<double> & data_in, char verbose)
{
	char resx = 0;
	FILE *fp;
	char fnx[100];	//max 100 char

	//build the filename
	char *root = "../../../../dataset/cpp-exp/";
		
	strcpy(fnx, root);
	strcat(fnx, filename);
	strcat(fnx, ext);

	printf("\nsave to %s\n", fnx);

	fp = fopen(fnx,"w+");
	if(fp)
		{
			//write it
			int i,j,k;
			//fprintf(fp, "CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8\n" );
			for(i=0;i<sample_len;i++)	//loop per sample len
			{
				//~ for(j=(i*sample_len);j<((i+1)*sample_len);j++)	//loop
				for(j=0;j<coloumn;j++)
					fprintf(fp, "%.15g%s", data_in[(j*sample_len)+i], (j==coloumn-1) ? "\n":"," );
			}

			fclose(fp);
		}
	else
		{
			resx = 1;
		}

	if(verbose)
		printf("save data to %s = %d\n", fnx, resx);

	return resx;
}
  

  
