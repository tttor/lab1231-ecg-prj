#ifndef EIGENLIB_SUPPORTXXX_H
#define EIGENLIB_SUPPORTXXX_H

#include <eigen3/Eigen/Dense>

namespace lab1231_ecg_prj {

class EigenLibSupport {
 public:
  static Eigen::MatrixXd remove_rows(const Eigen::MatrixXd& M, const uint64_t& pstart, const uint64_t& pend);
  static Eigen::MatrixXd remove_row(const Eigen::MatrixXd& M, const uint64_t& ith);
  static Eigen::MatrixXd scalar2mat(const double& scalar);
  static double mat2scalar(const Eigen::MatrixXd& mat);
};

}// namespace lab1231_ecg_prj

#endif



