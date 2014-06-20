#ifndef ECG_ARRAY_CONST_H
#define ECG_ARRAY_CONST_H

#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <Eigen/Dense>
#include <util/csv_io.h>
#include <util/eigenlib_support.h>

namespace lab1231_ecg_prj {
  
class ECGArrayConstructor {
 public:
  //! Perform some structurization on the ECG data
  /*!
   * More ...
   */
  static std::vector< Eigen::MatrixXd > construct(const Eigen::MatrixXd& norm_ecg, const Eigen::MatrixXd& rr_ann, const uint64_t& n_sample_sample_per_beat, const std::string& out_dir_path);
  
  //! Convert ECG readings from mat to vec
  /*!
   * In the Matlab's implementation, this function is named: multi2single()
   */
  static Eigen::MatrixXd mat2vec(const Eigen::MatrixXd& mat, const Eigen::MatrixXd& rr_ann, const uint64_t& n_sample_per_beat, const uint8_t& grouping_mode);
  
  //! Convert ECG readings from vec to the so-called frame
  /*!
   * In the Matlab's implementation, this function is named: segment_beat()
   */
  static Eigen::MatrixXd vec2frame(const Eigen::RowVectorXd& ecg_vec, const uint64_t& n_beat);  
  
  //! Structure the frame with some rules as follows.
  /*!
   * More on the rules ...
   */
  static std::vector<Eigen::MatrixXd> structure(const Eigen::MatrixXd& ecg_frame, const uint64_t& n_beat, const uint64_t& n_sample_sample_per_beat);
 
 private:
};

}// namespace lab1231_ecg_prj

#endif
