#ifndef EIGENLIB_SUPPORTXXX_H
#define EIGENLIB_SUPPORTXXX_H

#include <eigen3/Eigen/Dense>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace lab1231_ecg_prj {

class EigenLibSupport {
 public:
  static Eigen::MatrixXd remove_rows(const Eigen::MatrixXd& M, const uint64_t& pstart, const uint64_t& pend);
  static Eigen::MatrixXd remove_row(const Eigen::MatrixXd& M, const uint64_t& ith);
  static Eigen::MatrixXd scalar2mat(const double& scalar);
  static double mat2scalar(const Eigen::MatrixXd& mat);
  static double round(const double& val, const uint8_t& precision, const std::string type="nearest");
  static Eigen::MatrixXd truncate(const Eigen::MatrixXd& mat_in, const uint8_t& n_float_digit);
  static Eigen::MatrixXd shift_mat(Eigen::MatrixXd mat);
};

}// namespace lab1231_ecg_prj

#endif



