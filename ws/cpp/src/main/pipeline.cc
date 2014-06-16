#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <util/csv_io.h>
#include <util/debugger.h>
#include <util/eigenlib_support.h>
#include <spiht/ecg_spiht.h>

int main(int argc, char **argv) {
  using namespace std;
  using namespace boost;
  using namespace lab1231_ecg_prj;
  
  const uint64_t n_frame = 18;
  
  // Read wavelet-img files
  const string wavelet_img_dir_path = "../../octave/main/out/wavelet-img/";// Assume: run from <...>/ws/cpp/build/
  vector<Eigen::MatrixXd> wavelet_img_all_frame(n_frame);
  
  for (uint8_t i=0; i<n_frame; ++i) {
    string wavelet_img_csv = wavelet_img_dir_path + "wavelet_img-" + lexical_cast<string>(i+1) +".csv";
    
    cout << "Loading " << wavelet_img_csv << endl;
    Eigen::MatrixXd wavelet_img;
    wavelet_img = CSVIO::load(wavelet_img_csv);
    
    wavelet_img_all_frame.at(i) = wavelet_img;
  }
  cout << "wavelet_img_all_frame.size()= " << wavelet_img_all_frame.size() << endl;
  
  // SPHIT coding
  const uint8_t CR = 4;
  const uint64_t res = 12;
  
  Debugger::set_octave_param_dir_path(std::string("../../octave/main/out/param/"));// Assume: run from <...>/ws/cpp/build/
  if (argc==2) {
    Debugger::set_cpp_param_dir_path(argv[1]);
  }
  if (argc==3) {
    Debugger::set_cpp_base_param_dir_path(argv[2]);
  }
  
  const string out_dir_path = "../out/bit-str/";
  ECGSPIHT::run_spiht(wavelet_img_all_frame, CR, res, out_dir_path);

  //
  cout << "PIPELINE: end :)\n";
  return 0;
}
