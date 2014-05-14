#ifndef ECG_NORM_H
#define ECG_NORM_H

#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
  
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>  

namespace lab1231_ecg_prj {
  using namespace std;
class ECGNORM {
 public:
  static char norm_lead(int sample_len,vector<double> & signal_real, int *letak_r,int *jarak_per_bit, int beat_num, vector<double> & vsignal_per_lead);
  
 private:
 
};

}// namespace lab1231_ecg_prj

#endif
