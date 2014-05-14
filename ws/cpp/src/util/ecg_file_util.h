#ifndef ECG_FILE_UTIL_H
#define ECG_FILE_UTIL_H

#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>

namespace lab1231_ecg_prj {
   using namespace std;
class ECGFileUtil {
 public:
  static char read_RR_file(char *filename, int *arr_out,char verbose);
  static char read_csv_file(char *filename, double *arr_out, int max_line, double scale_data, char verbose);
  static char write_double_multichannel_to_file(char *filename, char *ext, int data_len, double *data_in, char verbose);
  static char write_multichannel_arranged_data_to_file(char *filename, char *ext, int sample_len, double *data_in, char verbose);
  static char write_multichannel_arranged_data_to_file_vector(char *filename, char *ext, int sample_len, vector<double> & data_in, char verbose);
 private:
 
};

}// namespace lab1231_ecg_prj

#endif
