#ifndef ECG_FILE_UTIL_H
#define ECG_FILE_UTIL_H

#include <iostream>

namespace lab1231_ecg_prj {
  
class ECGFileUtil {
 public:
  static char read_csv_file(char *filename, double *arr_out, int max_line, double scale_data, char verbose);
  static char write_double_multichannel_to_file(char *filename, char *ext, int data_len, double *data_in, char verbose);
 private:
 
};

}// namespace lab1231_ecg_prj

#endif
