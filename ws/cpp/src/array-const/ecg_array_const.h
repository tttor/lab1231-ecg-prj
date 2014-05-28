#ifndef ECG_ARRAY_CONST_H
#define ECG_ARRAY_CONST_H

#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>


namespace lab1231_ecg_prj {
  
class ECGArrayContructor {
 public:
 static char set_array_to_frame(int sample_num_per_frame, int beat_found, std::vector<double>& data_norm_in, std::vector< std::vector<double> > &frame_all_out, char verbose);
 
 static char set_frame_to_matrix(int sample_num_per_frame, std::vector<double>& frame_in, std::vector< std::vector<double> >& matrix_out);
 
 private:
 
};

}// namespace lab1231_ecg_prj

#endif
