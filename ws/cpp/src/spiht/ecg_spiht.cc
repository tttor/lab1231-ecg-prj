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
  using namespace arma;
  
  printf("spiht encoding\n");
  //loop var
  int i,j,k,l;
  
  //-------------------------initialization------------------------------
  mat m;
  m.set_size(wave_mat_in.size(),wave_mat_in.size());
  int block_size = m.n_rows*m.n_cols;
  int bitctr = 0;
  
  printf("%d\n", block_size);
  for(i=0;i<wave_mat_in.size();i++){
    for(j=0;j<wave_mat_in.size();j++){
      m(i,j)=wave_mat_in[i][j];  
    //  printf("%f,",m(i,j));
    }
    //printf("\n");
  }
  
  Mat<int> out;
  out.set_size(1,max_bit_in);
  out.fill(2);
  //cout << out << endl;
  mat temp_m;
  temp_m = trans(m);
  int n_max = floor(log2(abs(max(max(temp_m)))));
  //cout << n_max << endl;
  int bits_header = 0;
  int bits_LSP = 0;
  int bits_LIP = 0;
  int bits_LIS = 0;
  
  //----------------output bit stream header-------------------
  //image size, number of bit plane, wavelet dec level shoud be written as bit stream header
  out(0,0)=m.n_cols;
  out(0,1)=n_max;
  out(0,2)=level;
  //cout << out.n_cols << endl;
  //cout << out(0,0) << endl;
  //cout << out(0,1) << endl;
  //cout << out(0,2) << endl;
  bitctr = bitctr + 24;
  int index = 4;
  bits_header = bits_header + 24;
  
  //-----------initialize LIP, LSP, LIS-----------------------
  Mat<int> temp;
  int bandsize = pow(2,log2(m.n_rows)-level+1);
  //cout << bandsize << endl;
  Mat<int> temp1;
  temp1.set_size(1,bandsize);
  for(i=0;i<bandsize;i++){
    temp1(0,i)=i+1;
  }
  //cout << temp1 << endl;
  temp.set_size(bandsize,bandsize);
  for(i=0;i<bandsize;i++){
    temp.row(i)=temp1;
  }
  //cout << temp << endl;
  Mat<int> LIP;
  LIP.set_size(bandsize*bandsize,2);
  k=1;
  for(i=1;i<=bandsize*bandsize;i++){
    LIP(i-1,0)=k;
    if(i%128==0){
      k++;
    }
  }
  k=1;
  for(i=1;i<=bandsize*bandsize;i++){
    LIP(i-1,1)=k;
    if(i%128==0){
      k=1;
    }
    else{k++;}  
  }
  //cout << LIP << endl;
  Mat<int> LIS;
  LIS.set_size(bandsize*bandsize,3);
  LIS.col(0)=LIP.col(0);
  LIS.col(1)=LIP.col(1);
  Mat<int> A;
  A.zeros(bandsize*bandsize,1);
  LIS.col(2)=A.col(0);
  A.clear();
  //cout << LIS << endl;
  int pstart = 0;
  int pend = (bandsize/2)-1;
  int pdel;
  for(i=0;i<bandsize/2;i++){
    LIS.shed_rows(pstart,pend);
    pdel = pend - pstart + 1;
    pstart = pstart + bandsize - pdel;
    pend = pend + bandsize - pdel;
  }
  //cout << LIS.n_rows << endl;
  Mat<int> LSP;
  int n = n_max;
  
  //---------------------------coding-----------------------------------
  Mat<int> LIPtemp;
  Mat<int> LIStemp;
    
  LSP.set_size(1,2);
  int tempvar;
  int sgn;
  int max_d;
  int x,y; 
  int value;
  int s;
  
  
  while(bitctr<max_bit_in){
    //sorting pass
    LIPtemp=LIP;
    tempvar=1;
    //cout << LIPtemp << endl;
    printf("LIPtemp row %d",LIPtemp.n_rows);
    for(i=0;i<LIPtemp.n_rows;i++){
      tempvar++;
      if(bitctr+1>=max_bit_in){
        if(bitctr < max_bit_in){
          out.shed_col(out.n_cols-1);  
        }
        break;  
      }
      //printf("mmamamamam %d\n",i);
      //cout << LIPtemp(i,1-1) <<endl;
      //cout << LIPtemp(i,2-1) <<endl;
      //cout << m(LIPtemp(i,1-1),LIPtemp(i,2-1)) <<endl;
      if(abs(m(LIPtemp(i,1-1)-1,LIPtemp(i,2-1)-1)) >= pow(2,n)){
        out(0,index-1)=1;
        bitctr++;
        index++;
        bits_LIP++;
        sgn=m(LIPtemp(i,1-1),LIPtemp(i,2-1))>=0;
        out(0,index-1)=sgn;
        bitctr++;
        index++;
        bits_LIP++;
        if(i>0){//resize
          LSP.resize(LSP.n_rows+1,LSP.n_cols);  
        }
        LSP(i,0)=LIPtemp(i,0);
        LSP(i,1)=LIPtemp(i,1);
        LIP.shed_row(tempvar-1);
        tempvar--;
      }else{
        out(0,index-1)=0;
        bitctr++;
        index++;
        bits_LIP++;  
      }
    }
    
    LIStemp=LIS;
    tempvar=1;
    i=0;
    while(i<=LIStemp.n_rows){
      tempvar++;
      if(LIStemp(i,3-1)==0){
        if(bitctr>=max_bit_in){
          break;  
        }
       // max_d =getDescendant(LIStemp(i,1-1),LIStemp(i,2-1),LIStemp(i,3-1,m));
        if(max_d>=pow(2,n)){
          out(index-1)=1;
          bitctr++;
          index++;
          bits_LIS++;
          x = LIStemp(i,1-1);
          y = LIStemp(i,2-1);
          if(bitctr+1 >= max_bit_in){
            if(bitctr<max_bit_in){
              out.shed_col(out.n_cols-1);    
            }
            break;  
          }
          //(2*x-1-1,2*y-1-1)
          if(abs(m(2*x-1-1,2*y-1-1)) >= pow(2,n)){
            //rentang salah
            LSP.resize(LSP.n_rows+1,LSP.n_cols);  
            LSP(LSP.n_rows-1,0)=2*x-1;
            LSP(LSP.n_rows-1,1)=2*y-1;
            out(index-1)=1;
            bitctr++;
            index++;
            bits_LIS++;
            sgn=m(2*x-1-1,2*y-1-1)>=0;
            out(index-1)=sgn;
            index++;
            bits_LIS++;
          }
          else{
            out(index-1)=0;
            bitctr++;
            index++;
            bits_LIS++;
            LIP.resize(LIP.n_rows+1,LIP.n_cols); 
            LIP(LIP.n_rows-1,0)=2*x-1;
            LIP(LIP.n_rows-1,1)=2*y-1;
          }
          if(bitctr+1 >=max_bit_in){
            if(bitctr<max_bit_in){
              out.shed_col(out.n_cols-1);      
            }
            break;
          }
          
          //(2*x-1-1,2*y-1)
           if(abs(m(2*x-1-1,2*y-1)) >= pow(2,n)){
            //rentang salah
            LSP.resize(LSP.n_rows+1,LSP.n_cols);  
            LSP(LSP.n_rows-1,0)=2*x-1;
            LSP(LSP.n_rows-1,1)=2*y;
            out(index-1)=1;
            bitctr++;
            index++;
            bits_LIS++;
            sgn=m(2*x-1-1,2*y-1)>=0;
            out(index-1)=sgn;
            index++;
            bits_LIS++;
          }
          else{
            out(index-1)=0;
            bitctr++;
            index++;
            bits_LIS++;
            LIP.resize(LIP.n_rows+1,LIP.n_cols); 
            LIP(LIP.n_rows-1,0)=2*x-1;
            LIP(LIP.n_rows-1,1)=2*y;
          }
          if(bitctr+1 >=max_bit_in){
            if(bitctr<max_bit_in){
              out.shed_col(out.n_cols-1);      
            }
            break;
          }
          
          //(2*x-1,2*y-1-1)
          if(abs(m(2*x-1,2*y-1-1)) >= pow(2,n)){
            //rentang salah LSPNYA indexnya
            LSP.resize(LSP.n_rows+1,LSP.n_cols);  
            LSP(LSP.n_rows-1,0)=2*x;
            LSP(LSP.n_rows-1,1)=2*y-1;
            out(index-1)=1;
            bitctr++;
            index++;
            bits_LIS++;
            sgn=m(2*x-1,2*y-1-1)>=0;
            out(index-1)=sgn;
            index++;
            bits_LIS++;
          }
          else{
            out(index-1)=0;
            bitctr++;
            index++;
            bits_LIS++;
            LIP.resize(LIP.n_rows+1,LIP.n_cols); 
            LIP(LIP.n_rows-1,0)=2*x;
            LIP(LIP.n_rows-1,1)=2*y-1;
          }
          if(bitctr+1 >=max_bit_in){
            if(bitctr<max_bit_in){
              out.shed_col(out.n_cols-1);      
            }
            break;
          }
          
          //(2*x-1,2*y-1)
          if(abs(m(2*x-1,2*y-1)) >= pow(2,n)){
            //rentang salah
            LSP.resize(LSP.n_rows+1,LSP.n_cols);  
            LSP(LSP.n_rows-1,0)=2*x;
            LSP(LSP.n_rows-1,1)=2*y;
            out(index-1)=1;
            bitctr++;
            index++;
            bits_LIS++;
            sgn=m(2*x-1,2*y-1)>=0;
            out(index-1)=sgn;
            index++;
            bits_LIS++;
          }
          else{
            out(index-1)=0;
            bitctr++;
            index++;
            bits_LIS++;
            LIP.resize(LIP.n_rows+1,LIP.n_cols); 
            LIP(LIP.n_rows-1,0)=2*x;
            LIP(LIP.n_rows-1,1)=2*y;
          }
          
          if((2*(2*x)-1)<m.n_rows && (2*(2*y)-1)<m.n_rows){
            LIS.resize(LIS.n_rows+1,LIS.n_cols); 
            LIS(LIS.n_rows-1,0)=LIStemp(i,1-1);
            LIS(LIS.n_rows-1,1)=LIStemp(i,2-1);
            LIS(LIS.n_rows-1,2)=1;
            LIStemp.resize(LIStemp.n_rows+1,LIStemp.n_cols); 
            LIStemp(LIStemp.n_rows-1,0)=LIStemp(i,1-1);
            LIStemp(LIStemp.n_rows-1,1)=LIStemp(i,2-1);
            LIStemp(LIStemp.n_rows-1,2)=1;
              
          }
          LIS.shed_row(tempvar-1);
          tempvar--;
        }
        else{
          out(index-1)=0;
          bitctr++;
          index++;
          bits_LIS++;  
          
        }
        
      }
      else{ //if(LIStemp(i,3)==0)
        if(bitctr>=max_bit_in){
          break;
        }
       // max_d = getDescendant(LIStemp(i,1-1),LIStemp(i,2-1),LIStemp(i,3-1),m);
        if(max_d >=pow(2,n)){
          out(index-1)=1;
          bitctr++;
          x = LIStemp(i,1-1);
          y = LIStemp(i,2-1);
          LIS.resize(LIS.n_rows+1,LIS.n_cols); 
          LIS(LIS.n_rows-1,0)=2*x-1;
          LIS(LIS.n_rows-1,1)=2*y-1;
          LIS(LIS.n_rows-1,2)=0;
          LIS.resize(LIS.n_rows+1,LIS.n_cols); 
          LIS(LIS.n_rows-1,0)=2*x-1;
          LIS(LIS.n_rows-1,1)=2*y;
          LIS(LIS.n_rows-1,2)=0;
          LIS.resize(LIS.n_rows+1,LIS.n_cols); 
          LIS(LIS.n_rows-1,0)=2*x;
          LIS(LIS.n_rows-1,1)=2*y-1;
          LIS(LIS.n_rows-1,2)=0;
          LIS.resize(LIS.n_rows+1,LIS.n_cols); 
          LIS(LIS.n_rows-1,0)=2*x;
          LIS(LIS.n_rows-1,1)=2*y;
          LIS(LIS.n_rows-1,2)=0;
          LIS.shed_row(tempvar-1);
          tempvar--;
        }
        else{
          out(index-1)=0;
          bitctr++;
          index++;
          bits_LIS++;
        }
      
      
      }
    
      i++;
    }
    
    //Refinement Pass
    tempvar=1;
    value=floor(abs(pow(2,n_max-n+1)*m(LSP(tempvar,1-1),LSP(tempvar,2-1))));
    while(value>=pow(2,n_max+2) && tempvar<=LSP.n_rows ){
      if(bitctr>=max_bit_in){
        break;  
      }
      //s = bitget(value,n_max+2);
      bitset<10> bit_temp(value);
      s = bit_temp[n_max+2];
      out(index-1)=s;
      bitctr++;
      index++;
      bits_LSP++;
      tempvar++;
      if(tempvar<=LSP.n_rows){
        value=floor(abs(pow(2,n_max-n+1)*m(LSP(tempvar,1-1),LSP(tempvar,2-1))));
      }
    }
    n--;
  }

}

char ECGSpiht::getDescendant(int i, int j, int type, arma::mat m_in, double value_out){
  using namespace arma;
  using namespace std;
  
  printf("getDescendant\n");
  int s_kecil = m_in.n_rows;
  mat S;
  
  int index=0;
  int a=0;
  int b=0;
  mat mind;
  mat nind;
  vector <int> chk;
  int len;
  mat value;
  
  //variable pembantu c++
  int nilai_awal;
  int nilai_akhir;
  int jarak;
  int k,l,h;
  int start_mind;
  int start_nind;
  mat S_temp;
  
  while((2*i-1)<s_kecil && (2*j-1)<s_kecil){
    a = i-1;
    b = j-1;

    //isi mind  
    nilai_awal = 2*(a+1)-1;
    nilai_akhir = 2*(a+pow(2,index));
    jarak = nilai_akhir-nilai_awal+1;
    mind.set_size(1,jarak);  
    for(k=0;k<jarak;k++){
      mind(0,k)=nilai_awal++;
    }
    
    
    //isi nind  
    nilai_awal = 2*(b+1)-1;
    nilai_akhir = 2*(b+pow(2,index));
    jarak = nilai_akhir-nilai_awal+1;
    nind.set_size(1,jarak);  
    for(k=0;k<jarak;k++){
      nind(0,k)=nilai_awal++;
    }
    
    //cek mind <= s
    for(k=0;k<mind.n_cols;k++){
      chk.push_back(mind(0,k)<=s_kecil);  
      len=len+chk[k];
    }
    
    if(len<mind.n_cols){
      mind.shed_cols(len+1,mind.n_cols);
    }
    
    //cek nind <= s
    chk.clear();
    len = 0;
    for(k=0;k<nind.n_cols;k++){
      chk.push_back(nind(0,k)<=s_kecil);  
      len=len+chk[k];
    }
    
    if(len<nind.n_cols){
      nind.shed_cols(len+1,nind.n_cols);
    }
    
    //S = [S reshape(m(mind,nind),1,[])];
    S_temp.clear();
    S_temp=m_in.submat(mind(0),mind(mind.n_cols-1),nind(0),nind(nind.n_cols-1));
    S.set_size(1,S.n_cols+(mind.n_cols*nind.n_cols));
    //S bakal ketambah terus
    k=S.n_cols-(mind.n_cols*nind.n_cols);
    for(l=0;l<S_temp.n_cols;l++){
      for(h=0;h<S_temp.n_rows;h++){
        S(0,k)=S_temp(h,l);  
        k++;
      }
    }
    
    index++;
    i=2*a+1;
    j=2*b+1;
  }
  
  if(type==1){
     //S(:,1:4) = [];; 
     S.cols(4,S.n_cols-1);
  }
  
  value = max(abs(S));
  value_out=value(0,0);
}

