#ifndef ECG_SIGNAL_PREPROCESSOR_H
#define ECG_SIGNAL_PREPROCESSOR_H

#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>

namespace lab1231_ecg_prj {
   using namespace std;
class ECGSignalPreprocessor {
 public:
  static char preprocess(char *fname,int sample_len, double *data_voltage, int adc_sampling_rate, vector<double> & data_out, vector<int>& rpeak_out, vector<int>& rr_out, char verbose_mode);   
  static void adc_multichannel_denoising(int sample_len, double *adc_volt_aligned, int adc_sampling_rate); 
  static void adc_rearrange_voltage(int adc_sample_len, double *adc_volt_data);
 private:
 
};

}// namespace lab1231_ecg_prj

#endif
