#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <string>
#include <cstdint>
#include <iostream>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "csv_io.h"
#include "eigenlib_support.h"

namespace lab1231_ecg_prj {

class Debugger{
 public:
  static Eigen::MatrixXd get_param(const std::string& param_path);
  static bool debug_param(const std::string& param_name, const Eigen::MatrixXd& param, const std::string& place, bool write=true);
  static void reset(const uint64_t& base_outer_while_ctr=0);
  static void load_outerwhile_param( const uint64_t& base_outer_while_ctr,
                                      Eigen::MatrixXd* LSP, Eigen::MatrixXd* LIP, Eigen::MatrixXd* LIS,
                                      uint64_t* bits_LSP, uint64_t* bits_LIP, uint64_t* bits_LIS,
                                      Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx,                  int64_t* n, uint64_t* bitctr, uint64_t* if_ctr, uint64_t* else_ctr,uint64_t* outer_while_ctr);
  static void write_param(const std::string& param_name, const Eigen::MatrixXd& param, const std::string& place);
  static void set_octave_param_dir_path(const std::string& path);
  static void set_cpp_param_dir_path(const std::string& path);
  static void set_cpp_base_param_dir_path(const std::string& path);
  
  static std::string octave_param_dir_path;
  static std::string cpp_param_dir_path;
  static std::string cpp_base_param_dir_path;
  static std::string msg;
};

}// namespace lab1231_ecg_prj
#endif
