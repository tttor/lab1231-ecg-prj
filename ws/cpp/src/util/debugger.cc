#include "debugger.h"

using namespace lab1231_ecg_prj;

std::string Debugger::msg = std::string("");

// These paths assume that the main exe is run from <...>/ws/cpp/build/
std::string Debugger::octave_param_dir_path = std::string("../../octave/main/out/param/");
std::string Debugger::cpp_param_dir_path = std::string("../out/param/");
std::string Debugger::cpp_base_param_dir_path = cpp_param_dir_path;

Eigen::MatrixXd Debugger::get_param(const std::string& param_path) {
  Eigen::MatrixXd param;
  param = CSVIO::load(param_path);
  
  return param;
}

bool Debugger::debug_param(const std::string& param_name, const Eigen::MatrixXd& param, const std::string& place, bool write) {
  using namespace std;
  using namespace boost;
  
  if (write) {
    write_param(param_name, param, place);
  }
  
  msg.clear();
  msg += "ASSERTION FAILURE: AT:" + place;

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
  param_star_path = string(octave_param_dir_path + place + param_name);
  //cout << "param_star_path= " << param_star_path << endl;
  
  Eigen::MatrixXd param_star;// _star means from Matlab's runs
  param_star = get_param(param_star_path);

  // Adapt the param_star matrix that contains indexes since Matlab's index begins at 1, while cpp's begin at 0, hence _minus_
  if (param_name=="LIP" or param_name=="tmp_LIP" or param_name=="LSP" or param_name=="tmp_LSP" or param_name=="bit_str_idx") {
    param_star = param_star - Eigen::MatrixXd::Ones(param.rows(),param.cols());
  }
  else if (param_name=="LIS" or param_name=="tmp_LIS") {
    // Note: the third column does _not_ contain indexes
    param_star.col(0) = param_star.col(0) - Eigen::MatrixXd::Ones(param.rows(),1); 
    param_star.col(1) = param_star.col(1) - Eigen::MatrixXd::Ones(param.rows(),1); 
  }
  
  // Test
  msg += ": " + param_name;
  if (param.size() != param_star.size()) {
    msg += ": UNMATCHED SIZE";
    return false;
  }
  if (param != param_star) {
    msg += ": UNMATCHED ELEMENT VALUES";
    if (param.size()==1) {
      msg += ": WANT: " + lexical_cast<string>(param_star(0)) + " GOT: " + lexical_cast<string>(param(0));
    }
    return false;
  }
  return true;
}

void Debugger::reset(const uint64_t& base_outer_while_ctr) {
  namespace fs = boost::filesystem;
  using namespace std;
  using namespace boost;
 
  if (base_outer_while_ctr == 0) {
    boost::filesystem::remove_all( boost::filesystem::path(cpp_param_dir_path) );
    boost::filesystem::create_directories(cpp_param_dir_path);
    return;
  }
  fs::path dir_path(cpp_param_dir_path);
  fs::directory_iterator end_iter;
  
  for (fs::directory_iterator dir_iter(dir_path); dir_iter != end_iter; ++dir_iter) {
    //cout << *dir_iter << endl;
    
    // Split
    vector<string> path_comp;
    split( path_comp, dir_iter->path().string(), boost::algorithm::is_any_of("/"), token_compress_on );
    
    string outerwhile_str;
    outerwhile_str = path_comp.back();
    
    vector<string> comp;
    split( comp, outerwhile_str, boost::algorithm::is_any_of("-"), token_compress_on );
    
    uint64_t out_while_ctr;
    out_while_ctr = lexical_cast<uint64_t>(comp.back());
    
    //
    if (out_while_ctr > base_outer_while_ctr) {
      fs::remove_all(*dir_iter);
    }
  }
}

void Debugger::load_outerwhile_param( const uint64_t& base_outer_while_ctr,
                                      Eigen::MatrixXd* LSP, Eigen::MatrixXd* LIP, Eigen::MatrixXd* LIS,
                                      uint64_t* bits_LSP, uint64_t* bits_LIP, uint64_t* bits_LIS,
                                      Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx,
                                      int64_t* n, uint64_t* bitctr, uint64_t* if_ctr, uint64_t* else_ctr, uint64_t* outer_while_ctr
                            ) {
  using Eigen::MatrixXd;
  using namespace std;
  using namespace boost;
  
  cout << "Debugger::load_outerwhile_param(): BEGIN\n";
  
  if (base_outer_while_ctr == 0)
    return;
  
  string param_dir_path;
  param_dir_path = cpp_base_param_dir_path + "outerwhile-" + lexical_cast<string>(base_outer_while_ctr) + "/" + "event-3/";
  
  //   
  *outer_while_ctr = base_outer_while_ctr;
  
  //LSP
  MatrixXd saved_LSP;
  saved_LSP = CSVIO::load(string(param_dir_path+"LSP"));
  *LSP = saved_LSP;
  
  //LIP
  MatrixXd saved_LIP;
  saved_LIP = CSVIO::load(string(param_dir_path+"LIP"));
  cout << "saved_LIP=\n" << saved_LIP << endl;
  *LIP = saved_LIP;
  cout << "LIP=\n" << *LIP << endl;
  
  //LIS
  MatrixXd saved_LIS;
  saved_LIS = CSVIO::load(string(param_dir_path+"LIS"));
  *LIS = saved_LIS;
  
  //bits_LSP
  MatrixXd saved_bits_LSP;
  saved_bits_LSP = CSVIO::load(string(param_dir_path+"bits_LSP"));
  *bits_LSP = EigenLibSupport::mat2scalar(saved_bits_LSP);
  
  //bits_LIP
  MatrixXd saved_bits_LIP;
  saved_bits_LIP = CSVIO::load(string(param_dir_path+"bits_LIP"));
  *bits_LIP = EigenLibSupport::mat2scalar(saved_bits_LIP);
  
  //bits_LIS
  MatrixXd saved_bits_LIS;
  saved_bits_LIS = CSVIO::load(string(param_dir_path+"bits_LIS"));
  *bits_LIS = EigenLibSupport::mat2scalar(saved_bits_LIS);
  
  //bit_str
  MatrixXd saved_bit_str;
  saved_bit_str = CSVIO::load(string(param_dir_path+"bit_str"));
  *bit_str = saved_bit_str.row(0);
  
  //bit_str_idx
  MatrixXd saved_bit_str_idx;
  saved_bit_str_idx = CSVIO::load(string(param_dir_path+"bit_str_idx"));
  *bit_str_idx = EigenLibSupport::mat2scalar(saved_bit_str_idx);
  
  //n
  MatrixXd saved_n;
  saved_n = CSVIO::load(string(param_dir_path+"n"));
  *n = EigenLibSupport::mat2scalar(saved_n);
  cout << "*n= " << *n << endl;
  
  //bitctr
  MatrixXd saved_bitctr;
  saved_bitctr = CSVIO::load(string(param_dir_path+"bitctr"));
  *bitctr = EigenLibSupport::mat2scalar(saved_bitctr);
  cout << "*bitctr= " << *bitctr << endl;

  //if_ctr
  MatrixXd saved_if_ctr;
  saved_if_ctr = CSVIO::load(string(param_dir_path+"if_ctr"));
  *if_ctr = EigenLibSupport::mat2scalar(saved_if_ctr);
  
  //else_ctr
  MatrixXd saved_else_ctr;
  saved_else_ctr = CSVIO::load(string(param_dir_path+"else_ctr"));
  *else_ctr = EigenLibSupport::mat2scalar(saved_else_ctr);
  
  cout << "Debugger::load_outerwhile_param(): END\n";
}

void Debugger::write_param(const std::string& param_name, const Eigen::MatrixXd& param, const std::string& place) {
  using namespace std;
  
  string param_dir_path;
  param_dir_path = string(cpp_param_dir_path + place);
  boost::filesystem::create_directories(param_dir_path);
  
  string param_path;
  param_path = string(param_dir_path + param_name);
  //cout << "param_path= " << param_path << endl;
  CSVIO::write(param, param_path);
}

void Debugger::set_cpp_param_dir_path(const std::string& path) {
  cpp_param_dir_path = path;
  std::cout << "SET cpp_param_dir_path to " << cpp_param_dir_path << std::endl;
}

void Debugger::set_cpp_base_param_dir_path(const std::string& path) {
  cpp_base_param_dir_path = path;
  std::cout << "SET cpp_base_param_dir_path to " << cpp_base_param_dir_path << std::endl;
}

void Debugger::set_octave_param_dir_path(const std::string& path) {
  octave_param_dir_path = path;
  std::cout << "SET octave_base_param_dir_path to " << octave_param_dir_path << std::endl;
}
