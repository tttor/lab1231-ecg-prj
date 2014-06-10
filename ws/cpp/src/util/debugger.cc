#include "debugger.h"

using namespace lab1231_ecg_prj;

// These paths assume that the main exe is run from <...>/ws/cpp/build/
std::string Debugger::param_root_path = std::string("../../octave/main/out/param/");
std::string Debugger::out_param_root_path = std::string("../out/param/");

Eigen::MatrixXd Debugger::get_param(const std::string& param_path) {
  Eigen::MatrixXd param;
  param = CSVIO::load(param_path);
  
  return param;
}

bool Debugger::debug_param(const std::string& param_name, const Eigen::MatrixXd& raw_param, const std::string& place) {
  using namespace std;
  using namespace boost;
  
  // Split
  vector<string> place_comp;
  split( place_comp, place, boost::algorithm::is_any_of("/"), token_compress_on );
  place_comp.erase(place_comp.end()-1);// remove the empty element as there is an ending "/"
  
  if (place_comp.size()==3) {// at innerwhile
    place_comp.erase(place_comp.begin());
  }
  
  vector<string> whiletype_comp;
  split( whiletype_comp, place_comp.at(0), boost::algorithm::is_any_of("-"), token_compress_on );
  string while_type = whiletype_comp.at(0);
  uint64_t while_ctr = lexical_cast<uint64_t>(whiletype_comp.at(1));
  
  vector<string> eventstr_comp;
  split( eventstr_comp, place_comp.at(1), boost::algorithm::is_any_of("-"), token_compress_on );
  uint16_t event_num = lexical_cast<uint16_t>(eventstr_comp.at(1));
  
  // Obtain the true param value 
  string param_star_path;
  param_star_path = string(param_root_path + place + param_name);
  //cout << "param_star_path= " << param_star_path << endl;
  
  Eigen::MatrixXd param_star;// _star means from Matlab's runs
  param_star = get_param(param_star_path);
  //cout << "param_star=\n" << param_star << endl;
  
  // Adapt the matrix that contains indexes since Matlab's index begins at 1
  Eigen::MatrixXd param = raw_param;
  
  if (param_name=="LIP" or param_name=="tmp_LIP" or param_name=="LSP" or param_name=="tmp_LSP" or param_name=="bit_str_idx") {
    param = param + Eigen::MatrixXd::Ones(param.rows(),param.cols());
  }
  else if (param_name=="LIS" or param_name=="tmp_LIS") {
    // Note: the third column does _not_ contain indexes
    param.col(0) = param.col(0) + Eigen::MatrixXd::Ones(param.rows(),1); 
    param.col(1) = param.col(1) + Eigen::MatrixXd::Ones(param.rows(),1); 
  }
  //cout << "param= \n" << param << endl;
  
  // Save the computed param 
  string param_dir_path;
  param_dir_path = string(out_param_root_path + place);
  boost::filesystem::create_directories(param_dir_path);
  
  string param_path;
  param_path = string(param_dir_path + param_name);
  //cout << "param_path= " << param_path << endl;
  CSVIO::write(param, param_path);
  
  // Test
  if (param.size() != param_star.size()) {
    return false;
  }
  if (param != param_star) {
    return false;
  }
  return true;
}

void Debugger::reset() {
  boost::filesystem::remove_all( boost::filesystem::path(out_param_root_path) );
  boost::filesystem::create_directories(out_param_root_path);
}
