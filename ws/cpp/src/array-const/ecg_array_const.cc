#include "ecg_array_const.h"

using namespace lab1231_ecg_prj;

std::vector< Eigen::MatrixXd > ECGArrayConstructor::construct(const Eigen::MatrixXd& norm_ecg, const Eigen::MatrixXd& rr_ann, const uint64_t& n_sample_per_beat, const std::string& out_dir_path) {
  using namespace Eigen;
  using namespace std;
  cout << "ECGArrayConstructor::construct(): BEGIN\n";
  
  // Convert ECG data from Mat to (very long) Vec (flattening) -----------------------------------------------
  uint8_t grouping_mode = 0;// 0: no grouping
  RowVectorXd ecg_vec = mat2vec(norm_ecg, rr_ann, n_sample_per_beat, grouping_mode);
  
  // Convert ECG data vector into ECG frames------------------------------------------------------------------
  const uint64_t n_beat = n_sample_per_beat;
  MatrixXd ecg_frame;
  ecg_frame = vec2frame(ecg_vec, n_beat);
  
  // Break down the ECG frame --------------------------------------------------------------------------------
  // from (n_frame)x(n_beat*n_sample_per_beat) to (n_frame)x(n_beat)x(n_sample_per_bit)
  vector<MatrixXd> ecg_mat_all_frame(ecg_frame.rows());
  ecg_mat_all_frame = structure(ecg_frame, n_beat, n_sample_per_beat);
  
  cout << "ECGArrayConstructor::construct(): END\n";
  return ecg_mat_all_frame;
}

std::vector<Eigen::MatrixXd> ECGArrayConstructor::structure(const Eigen::MatrixXd& ecg_frame, const uint64_t& n_beat, const uint64_t& n_sample_per_beat) {
  using namespace Eigen;
  using namespace std;
  cout << "ECGArrayConstructor::structure(): BEGIN\n";
  
  const uint64_t n_frame = ecg_frame.rows();
  
  vector<MatrixXd> c_mat_all_frame(n_frame);
  for (uint64_t i=0; i<c_mat_all_frame.size(); ++i) {
    
    MatrixXd mat(n_beat, n_sample_per_beat);
    uint64_t last_idx = 0;//post the end
    for (uint64_t j=0; j<mat.rows(); ++j) {
      const RowVectorXd long_vec = ecg_frame.row(i);
      const uint64_t start_idx = last_idx;
    
      mat.row(j) = long_vec.segment(start_idx, n_sample_per_beat);
      
      last_idx = start_idx + n_sample_per_beat; 
    }
    
    c_mat_all_frame.at(i) = mat; 
  }
  
  cout << "ECGArrayConstructor::structure(): END\n";
  return c_mat_all_frame;
}

Eigen::MatrixXd ECGArrayConstructor::mat2vec(const Eigen::MatrixXd& mat, const Eigen::MatrixXd& rr_ann, const uint64_t& n_sample_per_beat, const uint8_t& grouping_mode) {
  using namespace std;
  using namespace Eigen;
  cout << "ECGArrayConstructor::mat2vec(): BEGIN\n";
  
  const uint8_t n_lead = 8;// TODO hardcode 8 here?
  const uint64_t n_beat = rr_ann.rows() + 1; // +1 to account for (maybe incomplete) last beat
  //cout << "n_beat= " << n_beat << endl;
  
  RowVectorXd vec;
  switch (grouping_mode) {
    case 0: {// no grouping
      for (uint64_t i=0; i<n_beat; ++i) {
        const uint64_t start_idx = (i*n_sample_per_beat);
        //const uint64_t end_idx = (i+1)*n_sample_per_beat-1;
        for (uint64_t j=0; j<n_lead; ++j) {
          RowVectorXd one_beat;
          one_beat = mat.row(j).segment(start_idx, n_sample_per_beat);
          
          const uint64_t init_target_idx = vec.size();
          vec.conservativeResize(vec.size()+one_beat.size());
          for (uint64_t k=0, target_idx= init_target_idx; k<one_beat.size(); ++k, ++target_idx) {
            vec(target_idx) = one_beat(k);
          }
        }
      }
      break;
    }
    case 1: {
      assert(false && "group_mode has not been implemented yet :(");
      //vector<RowVectorXd> groups;
      //groups.resize(3);
      
      //// The assignment belows follow the rule invented by Sani M. Isa
      //group.at(0) << 1, 7, 8;
      //group.at(1) << 2, 3;// TODO mirror lead 3
      //group.at(2) << 4, 5, 6;
      break;
    }
    case 2: {
      assert(false && "group_mode has not been implemented yet :(");
      break;
    }
    default: {
      assert(false && "UNKNOWN grouping mode");
    }
  }
  
  cout << "ECGArrayConstructor::mat2vec(): END\n";
  return vec;
}

Eigen::MatrixXd ECGArrayConstructor::vec2frame(const Eigen::RowVectorXd& ecg_vec, const uint64_t& n_beat) {
  using namespace Eigen;
  using namespace std;
  cout << "ECGArrayConstructor::segment_beat(): BEGIN\n";
  //cout << "ecg_vec.size()= " << ecg_vec.size() << endl;
  //cout << "n_beat= " << n_beat << endl;
  
  const uint64_t n_sample_per_beat = n_beat; 
  const uint64_t n_complete_frame = ecg_vec.size() / (n_sample_per_beat*n_beat);// yes, truncate
  //cout << "n_complete_frame= " << n_complete_frame << endl;
  
  MatrixXd ecg_frame(n_complete_frame, n_beat*n_sample_per_beat);
  for (uint64_t i=0; i<ecg_frame.rows(); ++i) {
    const uint64_t start_idx = i*(n_beat*n_sample_per_beat);
    ecg_frame.row(i) = ecg_vec.segment(start_idx, n_beat*n_sample_per_beat);
  } 
  //cout << "(ori) ecg_frame.size()= " << ecg_frame.size() << endl;
  
  if (ecg_frame.size() < ecg_vec.size()) {
    cout << "Take care the incomplete last frame; which becomes the last row in ecg_frame\n";
    RowVectorXd last_frame;
    last_frame = ecg_vec.segment(ecg_frame.size(), ecg_vec.size()-ecg_frame.size());
    
    const uint64_t pad_len = (n_beat*n_sample_per_beat) - last_frame.size();
    RowVectorXd pad = MatrixXd::Zero(1, pad_len).row(0);
    
    ecg_frame.conservativeResize(ecg_frame.rows()+1, ecg_frame.cols());
    ecg_frame.row(ecg_frame.rows()-1) << last_frame, pad;
  }
  //cout << "(padded) ecg_frame.size()= " << ecg_frame.size() << endl;
  
  cout << "ECGArrayConstructor::segment_beat(): END\n";
  return ecg_frame;
}
