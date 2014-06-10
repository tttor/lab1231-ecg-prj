#ifndef CSV_IO_H
#define CSV_IO_H

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <eigen3/Eigen/Dense>

namespace lab1231_ecg_prj {

class CSVIO{
 public:
  static Eigen::MatrixXd load(std::string csv_path);
  static void write(Eigen::MatrixXd, std::string csv_path);
  
};

}// namespace lab1231_ecg_prj
#endif
