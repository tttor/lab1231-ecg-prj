#ifndef ECG_SPIHT_H
#define ECG_SPIHT_H

#include <cmath>
#include <bitset>
#include <string>
#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <boost/lexical_cast.hpp>
#include <util/csv_io.h>
#include <util/eigenlib_support.h>
#include <util/debugger.h>

namespace lab1231_ecg_prj {

// This class (tries to) ports the SPHIT implementation in Matlab by Sani M Isa, Comp. Sci. Dept. University of Indonesia  
class ECGSPIHT {
 public:
  static std::vector<Eigen::RowVectorXd> run_spiht(const std::vector<Eigen::MatrixXd>& wavelet_img_all_frame, const uint64_t& CR, const uint64_t& res, const std::string& out_dir_path);
  static Eigen::RowVectorXd spiht_enc(const Eigen::MatrixXd& wavelet_img, const uint64_t& max_bits, const uint64_t& level);
  static void spiht_enc_helper(const Eigen::MatrixXd& wavelet_img, const uint8_t& x, const uint8_t& y, const int64_t& n, Eigen::MatrixXd* LSP, Eigen::MatrixXd* LIP, Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx, uint64_t* bitctr, uint64_t* bits_LIS);
  static bool spiht_enc_helper_2(const uint64_t& bitctr, const uint64_t& max_bits, Eigen::RowVectorXd* bit_str);
  static void spiht_enc_helper_3(const uint8_t& x, const uint8_t& y, Eigen::MatrixXd* LIS);
  static void spiht_enc_helper_4(const uint8_t bit_str_val, Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx, uint64_t* bitctr, uint64_t* bits_LIS_or_LIP=0);
  static double get_descendant(const Eigen::RowVectorXd& LIS_row, const Eigen::MatrixXd& mat);
  static double get_descendant(const uint8_t& type, const uint64_t& ith);
  static void get_pq(const uint8_t& case_num, const uint64_t& x, const uint64_t& y, uint64_t* p, uint64_t* q);
  static Eigen::RowVectorXd modify_idxes(const Eigen::RowVectorXd& idxes, const uint16_t& n_row);
 
 private:
};

}// namespace lab1231_ecg_prj

#endif
