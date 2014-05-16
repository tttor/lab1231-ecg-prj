#ifndef ECG_NORM_H
#define ECG_NORM_H

#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace lab1231_ecg_prj {

class ECGNormalizer {
 public:
  const static double phi;
  static char norm_lead(int beat_num, std::vector<double>& signal_real, int* letak_r_in,int* jarak_per_bit_in, std::vector<double>& vsignal_per_lead);

 private:
  static void bit_reversal(double* tmp, int min_index, int max_index);
  static void fft(double * signal_real, double *signal_imag, int length, int p, int q, std::vector<double>& terima_invers_bit);
  static void inversefft(double* real, double* imag,int n, std::vector<double> & terima_invers_bit);

};

}// namespace lab1231_ecg_prj

#endif
