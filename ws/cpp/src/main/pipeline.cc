/*! \mainpage ECG Analyzer Project of LAB1231 CS UI
 *
 * \section intro_sec Introduction
 * This is the introduction.
 *
 * \section install_sec Installation
 * \subsection step1 Step 1: Opening the box
 *
 * \section misc_sec Miscellany
 * Authors: Grafika Jati, Vektor Dewanto
 */
 
#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <util/csv_io.h>
#include <util/debugger.h>
#include <util/eigenlib_support.h>
#include <util/csv_io.h>
#include <spiht/ecg_spiht.h>
#include <dwt/wavelet.h>
#include <array-const/ecg_array_const.h>
 
int main(int argc, char **argv) {
  using namespace std;
  using namespace boost;
  using namespace lab1231_ecg_prj;
  using namespace Eigen;

  const uint64_t n_frame = 18; // TODO remove me
  
  // For below paths, assume: run from <...>/ws/cpp/build/
  const string octave_out_path_dir = "../../octave/main/out/";
  const string cpp_out_dir_path = "../out/";
  
  // Array Construction -------------------------------------------------------------------------------------------
  const MatrixXd norm_ecg= CSVIO::load(string(octave_out_path_dir+"normalized/"+"ecg8_norm_matlab.csv"));
  const MatrixXd rr_ann = CSVIO::load(string(octave_out_path_dir+"preprocessed/"+"rr_ann_matlab.csv"));
  const uint64_t n_sample_per_bit = 128;// TODO why 128?

  vector< MatrixXd > ecg_mat_all_frame;
  ecg_mat_all_frame = ECGArrayConstructor::construct(norm_ecg, rr_ann, n_sample_per_bit, string(cpp_out_dir_path+"array-const/"));
  
  // OBTAIN wavelet-img (the DWT) ---------------------------------------------------------------------------------
  cout << "Computing wavelet_img_all_frame: BEGIN\n";
  vector<Eigen::MatrixXd> wavelet_img_all_frame(n_frame);
  for (uint8_t i=0; i<n_frame; ++i) {
    vector< vector<double> > mat_1_in;
    mat_1_in = EigenLibSupport::mat2stdvec(ecg_mat_all_frame.at(i));
    
    int rr1,cc1;
    dwt_output_dim(mat_1_in, rr1, cc1 );
    vector< vector<double> > dwt_output(rr1, vector<double>(cc1));
    
    const int level = 7;
    const string wave_name = "bior4.4";
    vector<double> flag;
    dwt_2d(mat_1_in,level,wave_name,dwt_output,flag);
    
    MatrixXd wavelet_img;
    wavelet_img = EigenLibSupport::stdvec2mat(dwt_output);

    wavelet_img_all_frame.at(i) = wavelet_img;
  }
  cout << "Computing wavelet_img_all_frame: END\n";
  
  // SPHIT coding -------------------------------------------------------------------------------------------------
  const uint8_t CR = 4;
  const uint64_t res = 12;
  
  Debugger::set_octave_param_dir_path(std::string("../../octave/main/out/param/"));// Assume: run from <...>/ws/cpp/build/
  if (argc==2) {
    Debugger::set_cpp_param_dir_path(argv[1]);
  }
  if (argc==3) {
    Debugger::set_cpp_base_param_dir_path(argv[2]);
  }
  
  ECGSPIHT::run_spiht(wavelet_img_all_frame, CR, res, string(cpp_out_dir_path+"bit-str/"));

  //
  cout << "PIPELINE: end :)\n";
  return 0;
}
