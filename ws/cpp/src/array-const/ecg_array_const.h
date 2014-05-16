#ifndef ECG_ARRAY_CONST_H
#define ECG_ARRAY_CONST_H

#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>


namespace lab1231_ecg_prj {
  
class ECGArrayContructor {
 public:
 static char set_array_to_matrix(int sample_num_per_frame, int beat_found, std::vector<double>& data_norm_in, std::vector<double>& print_frame_all_out, char verbose);
 
 private:
 
};

}// namespace lab1231_ecg_prj

#endif
