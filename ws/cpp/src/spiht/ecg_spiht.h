#ifndef ECG_SPIHT_H
#define ECG_SPIHT_H

#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <armadillo>
#include <bitset>


#include "wavelet.h"

namespace lab1231_ecg_prj {
  
class ECGSpiht {
 public:
 static char get_spiht_enc(int max_bit_in, std::vector< std::vector<double> >& mat_1_in, std::vector<int>& bit_str_1_out );
 static char spiht_enc(int max_bit_in, int level, int rowcol, std::vector< std::vector<double> >& wave_mat_in, std::vector<int>& bit_str_out );
 static char getDescendant(int row_coor, int col_coor, int type, arma::mat m_in, double value_out);
 
 private:
 
};

}// namespace lab1231_ecg_prj

#endif
