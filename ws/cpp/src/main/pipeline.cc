#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <util/csv_io.h>
#include <util/debugger.h>
#include <spiht/ecg_spiht.h>

int main(int argc, char **argv) {
  using namespace std;
  using namespace lab1231_ecg_prj;
  
  // Read wavelet-img files
  string wavelet_img_csv = "../../octave/main/out/wavelet-img/wavelet_img-1.csv";// Assume: run from <...>/
  Eigen::MatrixXd wavelet_img;
  wavelet_img = CSVIO::load(wavelet_img_csv);
  
  // SPHIT coding
  const uint8_t CR = 4;
  const uint64_t res = 12;
  const uint64_t n_frame = 18;
  
  Debugger::set_octave_param_dir_path(std::string("../../octave/main/out/param/"));
  
  if (argc==2) {
    Debugger::set_cpp_param_dir_path(argv[1]);
  }
  
  if (argc==3) {
    Debugger::set_cpp_base_param_dir_path(argv[2]);
  }
  
  vector<Eigen::RowVectorXd> bit_str;
  bit_str = ECGSPIHT::run_spiht(wavelet_img, CR, res, n_frame);

  cout << "end...\n";
  return 0;
}
