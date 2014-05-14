#include "ecg_norm.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
  
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>  
  
using namespace lab1231_ecg_prj;
using namespace std;

double static phi = M_PI;

void bit_reversal(double* tmp, int min_index, int max_index);
void fft(double * signal_real, double *signal_imag, int length, int p, int q, double *terima_invers_bit);
void inversefft(double* real, double* imag,int n, double *terima_invers_bit);


//void tes(vector<double> & v){
	//int i;
	//for(i=0;i<v.size();i++){
		//printf("haha %d \n",v[i]);
		
		//}
	
	//}



/*
fungsi untuk normalisasi per beat
nanti di main baru digabung 8 beat yang ada
*/


char ECGNORM::norm_lead(double *signal_real, int *letak_r,int *jarak_per_bit, int beat_num, double *signal_norm){
    using namespace std;
    char resx = 0;
    int i,j,k,l;
	
    printf("\nmasuk bantu normalisasi\n");
	int length = (beat_num-1)*128;
	
  
	vector< double > vsignal_per_lead;
	//double *vsignal_per_bit = new double [(beat_num-1)*128];
	
    printf("beat_num %d\n",beat_num);
	
	//for(i=0;i<length;i++){
	////	vsignal_per_bit[i]=0.0;
		//printf("hoho %f",vsignal_per_bit[i]);
		//}

    int start;
    int finish;

	vector< double > v1;
	
	
   
	double signal_real_bit[600];	
    double signal_imag_bit[600];
     
    //double tempvec [500];
    vector< double > tempvec;
    
	double tempvec_out [200];
	
	for(i=0;i<512;i++){
	//	printf("i %d  haha %f\n",i,signal_real_bit[i]);
		signal_real_bit[i]=0.0;
		signal_imag_bit[i]=0.0;
		
	}
    double terima_invers_bit[200];//harusnya 128
    
    for(i=0;i<128;i++){
            terima_invers_bit[i]=0.0;
        }

    for(i=0; i<beat_num-1; i++) {

       // printf("masuk pengerkaaan");

        start = letak_r[i];
        finish = letak_r[i+1]-1;

        
       // printf("i nya : jarak %d %d", i,jarak_per_bit[i]);
        
        //m=0;
        for (j=start; j<=finish; j++) {
          
            tempvec.push_back(signal_real[j]);
          //  m++;
        }
  
            for(k=0;k<512;k++){

                signal_imag_bit[k] = 0.0;

                if(k<jarak_per_bit[i]){
                    signal_real_bit[k] = tempvec[k];
                }
                else {
                    signal_real_bit[k] = 0.0;
                }

            }
           // fft(signal_real_bit, signal_imag_bit, 512, 128, jarak_per_bit[i]-1, terima_invers_bit);
			//coba
			    
        for(l=0;l<128;l++){
            //tempvec_out.push_back(terima_invers_bit[l]);
            //tempvec_out[l]=terima_invers_bit[l];
			vsignal_per_lead.push_back(terima_invers_bit[l]);
            
        }

       tempvec.clear();

    }

	for(i=0;i<length;i++){
	     signal_norm[i]=vsignal_per_lead[i];

	}
   
   vsignal_per_lead.clear();


 return resx;

}





void inversefft(double* real, double* imag,int n, double *terima_invers_bit) {
    int i, j, k;
    FILE *fp;
    fp = fopen("inverseFFT.csv", "w");

   // printf("nnnnnnn %d", n);

    //double hasil[n];

    double rexbar[1+(n/2)],imxbar[1+(n/2)];
    for (j=0; j<= n/2;j++) {
        rexbar[j]=real[j]/(n/2);
        if (j==0||j== n/2) {
            rexbar[j]/= 2;
        }

        imxbar[j]=imag[j]/(n/2)*-1;
    }

    for (i=0;i<n;i++) {
        double re = 0, im = 0;

        for (k=0; k<= n/2; k++) {
            re += rexbar[k]*cos(2*i*phi*k/n);
            im += imxbar[k]*sin(2*i*phi*k/n);
        }
       // hasil[i] =re+im;
        terima_invers_bit[i] = re+im;
    }


    fprintf(fp,"hasil inverse");
    fprintf(fp,"\n");
    for(i = 0; i< 128; i++) {
            fprintf(fp, "%f, ", terima_invers_bit[i]);
    }
   // printf("masuk inverse");


}



void fft(double *signal_real, double *signal_imag, int length, int p, int q, double *terima_invers_bit) 
{
   int divisor = 1;
   int signal_count = length/divisor;

   // printf("length mbul %d \n", length);
    //printf("sin count %d\n", signal_count);

    int k;
    int i;
    int j;
    int current_index;
    current_index = 0;
    //double ddddM;
    double UR;
    double UI;
    double SR;
    double SI;
    double TR;
    double TI;
    int LE;
    int LE2;
    int stage;
    int pad_size;

    //FILE *fp;
    //fp = fopen("outputFFT.csv","w");


    //double *tempInverse;

   // printf("masuk FFT \n");


    /**
      * Proses dekomposisi untuk mendapatkan bit reversal keseluruhan
      */
    while (divisor <= length) {
        for(i = 0; i<divisor; i++) {
            int min_ind = i*signal_count;
            int max_ind = min_ind + signal_count-1;
            bit_reversal(signal_real, min_ind, max_ind);
            bit_reversal(signal_imag, min_ind, max_ind);
        }
        divisor *= 2;
        signal_count = length/divisor;
    }

    /**
      * Lakukan implementasi proses Butterfly di sini
      */
    //printf("\n masuk butterfly %f \n",signal_count);
    if((log(length)/log(2)) == 0){
        stage = log(length)/log(2);
    } else {
        stage = ceil(log(length)/log(2));
    }
      pad_size = pow(2,stage);

   double pad_real [length];
    for(i=0; i<length; i++){
        if (i < length) pad_real[i] = signal_real[i];
            else pad_real[i] = 0;
    }

    double pad_imag [length];
      for(i=0; i<length; i++){
        if (i < length) pad_imag[i] = signal_imag[i];
            else pad_imag[i] = 0;
    }

    int JM1,IP;
    for(i=0;i<=stage;i++){
        LE = pow(2,i);
        LE2 = LE/2;
        UR=1.0;
        UI=0.0;
        SR = cos(phi/LE2);
        SI = -1*sin(phi/LE2);


        for(j=1;j<=LE2;j++){
             JM1 = j-1;
            for(k=JM1;k<=(pad_size-1);k=k+LE){
                IP=k+LE2;
                TR = pad_real[IP]*UR - pad_imag[IP]*UI;
                TI = pad_real[IP]*UI + pad_imag[IP]*UR;
                pad_real[IP]= pad_real[k]-TR;
                pad_imag[IP]= pad_imag[k]-TI;
                pad_real[k]= pad_real[k]+TR;
                pad_imag[k]= pad_imag[k]+TI;

            }
            TR=UR;
            UR = (TR*SR)-(UI*SI);
            UI = (TR*SI)+(UI*SR);

        }

    }

    //fprintf(fp,"tes real");
    //fprintf(fp,"\n");
    //for(i = 0; i< pad_size; i++) {
        //fprintf(fp, "%f, ", pad_real[i]);
    //}
    //fprintf(fp,"\n");
    //fprintf(fp,"tes imag");
    //fprintf(fp,"\n");
    //for(i = 0; i< pad_size; i++) {
            //fprintf(fp, "%f, ", pad_imag[i]);
    //}


	//==========================================================
	//Proses normalisasi
	//printf("\n =================================== \n");
	//printf(" Masuk Normalisasi p = %d, q = %d\n ", p, q );
	int d;
	int hilang1;
	int hilang2;
	int delta;

	int len1 = pad_size;
	//double len2 = round(len1*p/q);
	double len2 = len1*p/q;

	if(len2>=0){len2+0.5;}
	else{len2-0.5;}


	int lby2 = 1 + (len1/2);
	/*
	printf("len1 %d\n", len1);
	printf("len2 %f\n", len2);
	printf("lby2 %d\n", lby2);
	*/

	if (len2 < len1){
		//printf("Dikecilkan");
		//hilangkan high frekuensi sampling
		d = len1 - len2;
		//hilangkan komponen dari
		hilang1 = floor(lby2-(d-1)/2);
		hilang2 = floor(lby2+(d-1)/2);
		delta = hilang2-hilang1;
		int del = pad_size-(delta)-1;
		double res_nor[del];

		/*
		printf("hilang1 %d\n", hilang1);
		printf("hilang2 %d\n", hilang2);
		printf("pad_size-(delta) %d\n", del);
		*/
		 //printf(" \nhasil hilangkan\n");
		j=0;
		for(i=0;i<pad_size;i++){
			if(i >= (hilang1-1) && i <= (hilang2-1) ){
				// do nothing
			} else {
			   // printf(" ada : %d \n", i);
					res_nor[j]=pad_real[i];
					j++;

			}

		}
	   // res[0]=1;

		/*
		for(j=0;j<(del);j++){
			printf("%f ", res_nor[j]);
		}
		*/
		double res_nor_imag[del];

		for(i=0;i<del;i++){
			res_nor_imag[i] = pad_imag[i];

		}
		 inversefft(res_nor,res_nor_imag,del,terima_invers_bit);

	} else if(len2 > len1) {
		//printf("\nDibesarkan\n");

		d = len2-len1;
		lby2 = floor(lby2);
		int ambil = 0;
		double res_nor[pad_size+d];

		//printf("d %d\n", d);
		//printf("lby2 %d\n", lby2);

		j=0;
		for (i=0; i < pad_size+d ; i++){

			if(i>=lby2 && ambil < d){
				res_nor[i] = 0.0;
				ambil++;
			}else {
				res_nor[i] = pad_real[j];
				j++;
			}
		}

		double res_nor_imag[pad_size+d];
		j=0;
		ambil=0;
		for (i=0; i < pad_size+d ; i++){

			if(i>=lby2 && ambil < d){
				res_nor_imag[i] = 0.0;
				ambil++;
			}else {
				res_nor_imag[i] = pad_imag[j];
				j++;
			}
		}

		inversefft(res_nor,res_nor_imag,pad_size+d,terima_invers_bit);
		//printf("pad size + d %d", pad_size+d);

	} else {
	 //  printf("\nTetap\n");
	   inversefft(pad_real,pad_imag,pad_size,terima_invers_bit);

	}
	//===========================================================   // inversefft(pad_real,pad_imag,pad_size);
	  //  printf("\n =================================== \n");
		//return tempInverse;
	   // printf("Process is done");
}

/**
* Fungsi ini berfungsi untuk melakukan bit reversal satu level
*/
void bit_reversal(double* tmp, int min_index, int max_index) {
    double div1[max_index-min_index+1];
    double div2[max_index-min_index+1];
    int index1=0, index2=0, index=min_index;
    int i;
    for(i = min_index; i <= max_index; i++) {
        if((i%2)==0) div1[index1++] = tmp[i];
        else div2[index2++] = tmp[i];
    }
    for(i = 0; i<index1; i++) {
        tmp[index++] = div1[i];
    }
    for(i = 0; i<index2; i++) {
        tmp[index++] = div2[i];
    }
}

