#include "ecg_norm.h"

using namespace lab1231_ecg_prj;


const double ECGNormalizer::phi = 3.14;

char ECGNormalizer::norm_lead(int beat_num, std::vector<double>& signal_real, int* letak_r,int* jarak_per_bit, std::vector<double> & vsignal_per_lead){
  using namespace std;
  
  printf("\nmasuk bantu normalisasi\n");
  printf("beat_num %d\n",beat_num);
	
  char resx = 0;
  int i,j,k,l;
  int start;
  int finish;
  double signal_real_bit[600];
  double signal_imag_bit[600];
  vector< double > tempvec;
  vector< double > terima_invers_bit;
  
  for(i=0;i<512;i++){
    signal_real_bit[i]=0.0;
    signal_imag_bit[i]=0.0;
  }

  for(i=0; i<beat_num+2-1; i++) {
    start = letak_r[i];
    finish = letak_r[i+1]-1;
    
    for (j=start; j<=finish; j++) {
      tempvec.push_back(signal_real[j]);
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
    fft(signal_real_bit, signal_imag_bit, 512, 128, jarak_per_bit[i]-1, terima_invers_bit);
    
    for(l=0;l<128;l++){
      vsignal_per_lead.push_back(terima_invers_bit[l]);
    }

    terima_invers_bit.clear();
    tempvec.clear();

  }

  return resx;

}

void ECGNormalizer::inversefft(double* real, double* imag,int n, std::vector<double> & terima_invers_bit) {
  int i, j, k;
  double temp_hasil[n];
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
  
    temp_hasil[i] = re+im;
    terima_invers_bit.push_back(temp_hasil[i]);
  }
}


void ECGNormalizer::fft(double *signal_real, double *signal_imag, int length, int p, int q, std::vector<double>& terima_invers_bit) 
{
  int divisor = 1;
  int signal_count = length/divisor;
  int k;
  int i;
  int j;
  int current_index;
  current_index = 0;
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

  //normalisasi
    //variabel normalisasi  
  int d;
  int hilang1;
  int hilang2;
  int delta;
  int len1 = pad_size;
  double len2 = len1*p/q;

  if(len2>=0){len2+0.5;}
  else{len2-0.5;}

  int lby2 = 1 + (len1/2);
  
  
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
 
    double res_nor_imag[del];
    for(i=0;i<del;i++){
      res_nor_imag[i] = pad_imag[i];
    }
    inversefft(res_nor,res_nor_imag,del,terima_invers_bit);

  } 
  else if(len2 > len1) {
    //printf("\nDibesarkan\n");

    d = len2-len1;
    lby2 = floor(lby2);
    int ambil = 0;
    double res_nor[pad_size+d];
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
  }
  else {
    inversefft(pad_real,pad_imag,pad_size,terima_invers_bit);
  }

}

/**
* Fungsi ini berfungsi untuk melakukan bit reversal satu level
*/
void ECGNormalizer::bit_reversal(double* tmp, int min_index, int max_index) {
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

