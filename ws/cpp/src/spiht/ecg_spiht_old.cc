#include "ecg_spiht.h"
#include <ecg_file_util.h>

using namespace lab1231_ecg_prj;

char ECGSpiht::get_spiht_enc(int max_bit_in, std::vector< std::vector<double> >& mat_1_in, std::vector<int>& bit_str_1_out ){
  using namespace std;
  cout << "wavelet decomposition \n";
    
  int i,j,k;
  int level = 1;
  string wave_name = "bior4.4";
  vector<double> flag;
  vector<int> length;
  
  int rr1,cc1;
  dwt_output_dim(mat_1_in, rr1, cc1 );
  vector<vector<double> >  dwt_output(rr1, vector<double>(cc1));
  printf("rr1 %d ==== cc1 %d\n", rr1,cc1);
  
  //jalankan wavelet
  dwt_2d(mat_1_in,level,wave_name,dwt_output,flag);
  
  printf("ukuran output %d\n", dwt_output.size());
   
  //hasilnnya panggil spiht
  spiht_enc(max_bit_in,level,rr1*cc1,dwt_output,bit_str_1_out);
  

  
}

char ECGSpiht::spiht_enc(int max_bit_in, int level, int rowcol, std::vector< std::vector<double> >& wave_mat_in, std::vector<int>& bit_str_out ){
  using namespace std;
  
  printf("spiht encoding\n");
  //loop var
  int i,j,k,l;
  
  //-------------------------initialization-------------------
  int bitctr = 0;
  vector <int> out;
  for(i=0;i<max_bit_in;i++){
    out.push_back(2);  
  }
  
   
  double temp_max=0;
  for(i=0;i<wave_mat_in.size();i++){
    for(j=0;j<wave_mat_in.size();j++){
      if(temp_max<wave_mat_in[i][j]){
        temp_max=wave_mat_in[i][j];
        }  
    }
    
  }
  
  printf("max_bit %d\n", max_bit_in);
  printf("temp_max %f\n", temp_max);
  int n_max = floor(log2(abs(temp_max)));
  printf("n_max %d\n", n_max);
     
  int bits_header = 0;
  int bits_LSP = 0;
  int bits_LIP = 0;
  int bits_LIS = 0;

  //-------------output bit stream header-------------------------------
  //image size, number of bit plane, wavelet dec level shoud be written as bit stream header
  printf("outbegin%d\n",out.begin());
  out.erase(out.begin());
  out.erase(out.begin());
  out.erase(out.begin());
  out.insert(out.begin(),level);
  out.insert(out.begin(),n_max);
  out.insert(out.begin(),wave_mat_in.size());
  
  printf("out.size %d\n",out.size());
  for(i=0;i<out.size();i++){
    //printf("%d,",out[i]);  
  }
  printf("\n");
  
  bitctr = bitctr+24;
  int index = 4;
  bits_header = bits_header+24;
  
  //-----------initialize LIP, LSP, LIS---------------------------------
  vector < vector<int> > temp;
  int bandsize = pow(2,log2(wave_mat_in.size())-level+1);
  printf("bandsize %d\n",bandsize);
  vector <int> temp1;
  for(i=0;i<bandsize;i++){
    temp1.push_back(i+1);
    //printf("%d ",temp1[i]);
  }
  printf("\n");
  for(i=0;i<bandsize;i++){
    for(j=0;j<bandsize;j++){
      temp.push_back(temp1);  
     // printf("%d ",temp[i][j]);
    }  
    //printf("\n");
  }
  
  //MatrixXd LIP(bandsize*bandsize,2);
  //for(i=0;i<bandsize*bandsize;i++){
    //for(j=0;j<1;j++){
      ////LIP(i,j)=temp()
      //}  
  //}
  
  vector <int> LIP;
  k=1;
  for(i=1;i<=bandsize*bandsize;i++){
    LIP.push_back(k);
    if(i%128==0){
      k++;
      }
  }
  k=1;
  for(i=1;i<=bandsize*bandsize;i++){
    LIP.push_back(k);
    if(i%128==0){
      k=1;
    }
    else{k++;}  
  }
  //for(i=0;i<bandsize*bandsize*2;i++){
    //printf("%d\n",LIP[i]);
  //}
  
  vector <int> LIS;
  for(i=0;i<bandsize*bandsize*3;i++){
    if(i<bandsize*bandsize*2){
      LIS.push_back(LIP[i]);
    }else{
      LIS.push_back(0);
      }
    //printf("%d\n",LIS[i]);
  }
  int pstart = 0;
  int pend = bandsize/2;
  int pdel;
  
  //hapus elemen kolom pertama
  for(i=0;i<bandsize/2;i++){
      LIS.erase(LIS.begin()+pstart,LIS.begin()+pend);
      pdel = pend - pstart;
      pstart = pstart + bandsize - pdel;
      pend = pend + bandsize - pdel;
  }
  pstart =0;
  pend = bandsize/2;
  pdel=0;
  
  //hapus elemen kolom kedua  
  int start2 = bandsize*bandsize-((bandsize/2)*(bandsize/2));
  printf("start2 %d\n", start2);
  for(i=start2;i<start2+(bandsize/2);i++){
      LIS.erase(LIS.begin()+start2+pstart,LIS.begin()+start2+pend);
      pdel = pend - pstart;
      pstart = pstart + bandsize - pdel;
      pend = pend + bandsize - pdel;
  }
  
  pstart =0;
  pend = bandsize/2;
  pdel=0;
  
  //hapus elemen kolom ketiga
  int start3 = bandsize*bandsize*2-((bandsize/2)*(bandsize/2));
  printf("start2 %d\n", start3);
  for(i=start3;i<start3+(bandsize/2);i++){
      LIS.erase(LIS.begin()+start3+pstart,LIS.begin()+start3+pend);
      pdel = pend - pstart;
      pstart = pstart + bandsize - pdel;
      pend = pend + bandsize - pdel;
  }
  
  printf("LIS size%d\n",LIS.size());
  
  for(i=0;i<start2*3;i++){
   // printf("%d,",LIS[i]);
  
  }
  
  vector <int> LSP;
  int n = n_max;
  vector <int> LIPtemp;
  vector <int> LIStemp;
  
  
  int temp_var=0;
  //-------------------- coding ----------------------------------------
  while(bitctr < max_bit_in){
    //sorting pass
    for(i=0;i<LIP.size();i++){
      LIPtemp.push_back(LIP[i]);
    } 
    for(i=0;i<LIP.size()/2;i++){
      temp_var = temp_var+1;
        if((bitctr+1) >= max_bit_in){
          if(bitctr<max_bit_in){
            out.erase(out.end());
            }  
          break; 
        }
        //baris,kolom, dimana kolom+LIPtemp/2
        printf("LIPtempi %d\n", LIPtemp[i]);
        printf("LIPtempi2 %d\n",LIPtemp[(LIPtemp.size()/2)+i]);
        printf("wave-mat-in %f\n",abs(wave_mat_in[LIPtemp[i]][LIPtemp[(LIPtemp.size()/2)+i]]));
        
        printf("hasilnya %d",abs(wave_mat_in[LIPtemp[i]][LIPtemp[(LIPtemp.size()/2)+i]])>=pow(2,n));
       
        if(abs(wave_mat_in[LIPtemp[i]][LIPtemp[(LIPtemp.size()/2)+i]])>=pow(2,n)){
          out.erase(out.begin()+index-1);
          out.insert(out.begin()+index-1,1);
          bitctr++;
          index++;
          bits_LIP++;
          int sgn = wave_mat_in[LIPtemp[i]][LIPtemp[(LIPtemp.size()/2)+i]] >=0;
          out.erase(out.begin()+index-1);
          out.insert(out.begin()+index-1,sgn);
          bitctr++;
          index++;
          bits_LIP++;
          //LSP = [LSP; LIPtemp(i,:)];
          LSP.push_back(LIPtemp[i]);
          LSP.push_back(LIPtemp[LIPtemp.size()/2+i]);
          
          LIP.erase(LIP.begin()+LIP.size()/2+temp_var-1);
          LIP.erase(LIP.begin()+temp_var-1);
          temp_var--;    
          
        }
        else{
          out.erase(out.begin()+index-1);
          out.insert(out.begin()+index-1,0);
          bitctr++;
          index++;
          bits_LIP++;
        }
          
    } 
    
    //for(i=0;i<LIStemp.size();i++){
      //LIStemp.push_back(LIS[i]);
    //}
    //temp_var=0;
    //i=0; 
    
  
  
  }
  
}

