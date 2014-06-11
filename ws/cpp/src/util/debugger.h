#ifndef DEBUGGER_H
#define DEBUGGER_H

//#include <boost/lexical_cast.hpp>
//#include <boost/tokenizer.hpp>
//#include <Eigen/Dense>
#include <string>
//#include <fstream>
#include <cstdint>
#include <iostream>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "csv_io.h"

namespace lab1231_ecg_prj {

class Debugger{
 public:
  static Eigen::MatrixXd get_param(const std::string& param_path);
  static bool debug_param(const std::string& param_name, const Eigen::MatrixXd& param, const std::string& place, bool write=true);
  static void reset();
  
  static std::string param_root_path;
  static std::string out_param_root_path;
  static std::string msg;
};

}// namespace lab1231_ecg_prj
#endif
