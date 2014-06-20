#include "eigenlib_support.h"

using namespace lab1231_ecg_prj;

Eigen::MatrixXd EigenLibSupport::remove_row(const Eigen::MatrixXd& M_in, const uint64_t& ith) {
  using namespace std;
  
  Eigen::MatrixXd M;
  M = M_in;
  
  unsigned int numRows = M.rows()-1;
  unsigned int numCols = M.cols();

  if (ith < numRows) {
    M.block(ith,0,numRows-ith,numCols) = M.block(ith+1,0,numRows-ith,numCols);
  }
  
  M.conservativeResize(numRows,numCols);
  
  //cout << "remove_row(): M=\n" << M << endl;
  return M;
}

Eigen::MatrixXd EigenLibSupport::remove_rows(const Eigen::MatrixXd& M_in, const uint64_t& pstart, const uint64_t& pend) {
  using namespace std;
  
  Eigen::MatrixXd M;
  M = M_in;
  
  uint64_t n_del = pend-pstart+1;
  for (uint8_t i = 0; i < n_del; i++){
    M = remove_row(M, pstart);
  }
  
  //cout << "remove_rows(): M=\n" << M << endl;
  return M;
}

Eigen::MatrixXd EigenLibSupport::scalar2mat(const double& scalar) {
  Eigen::MatrixXd mat(1,1);// for converting a single scalar to a matrix
  mat(0,0) = scalar;
  
  return mat;
}

Eigen::MatrixXd EigenLibSupport::rowvec2mat(const Eigen::RowVectorXd& vec) {
  Eigen::MatrixXd mat(1,vec.size());// for converting a single scalar to a matrix
  mat.row(0) = vec;
  
  return mat;
}

double EigenLibSupport::mat2scalar(const Eigen::MatrixXd& mat){
  assert(mat.size()==1);
  return mat(0);
}

double EigenLibSupport::round(const double& val, const uint8_t& n_float_digit, const std::string type) {
  const uint64_t factor = pow(10, n_float_digit);
  
  double result;
  if (type == "rounded_down"){
    result = floor(val * factor) / factor;
  } 
  else if (type == "nearest"){
    result = floor(val * factor + 0.5) / factor;  
  }
  else if(type=="rounded_up"){
    result = ceil(val * factor) / factor;
  }
  else {
    assert(false && "Unknown truncation type");
  }
  return result;
}

Eigen::MatrixXd EigenLibSupport::truncate(const Eigen::MatrixXd& mat_in, const uint8_t& n_float_digit) {
  using namespace std;
  using namespace boost;
  
  Eigen::MatrixXd mat(mat_in.rows(), mat_in.cols());
  
  for (uint64_t i=0; i<mat.size(); ++i) {
    double e = mat_in(i);
    //cout << "e= " << e << endl;
    
    // Convert element to str
    string e_str = lexical_cast<string>(e);
    //cout << "e_str= " << e_str << endl;
    
    if ( e_str.substr(0,1) == "-") { // if negative value
      // Split elements into int_comp and float_comp
      vector<string> comp;
      split( comp, e_str, boost::algorithm::is_any_of("."), token_compress_on );
      
      string int_comp_str = comp.at(0);
      //cout << "int_comp_str= " << int_comp_str << endl;
      
      string float_comp_str = comp.at(1);
      //cout << "float_comp_str= " << float_comp_str << endl;
      
      // Find "e sign" for accomodating scientific formats
      int e_sign_pos;// buggy if use int64_t
      e_sign_pos = float_comp_str.find("e");
      
      if (e_sign_pos != -1){ // if sci. format
        string one_char_after_e_sign = float_comp_str.substr(e_sign_pos+1, 1);
        //cout << "one_char_after_e_sign= " << one_char_after_e_sign << endl;
        
        if ( one_char_after_e_sign == string("-")) {// value is less than 1.0
          int_comp_str = "0";
          
          string all_char_after_minus_sign = float_comp_str.substr(e_sign_pos+2, float_comp_str.size());
          uint64_t exp_val = lexical_cast<uint64_t>(all_char_after_minus_sign);
          
          uint64_t n_zero_after_dot;
          n_zero_after_dot = exp_val - 1;
          
          string last_float_chars = float_comp_str;
          float_comp_str.clear();
          for (uint64_t j=0; j<n_zero_after_dot; ++j) {
            float_comp_str += "0";
          }
          float_comp_str += last_float_chars;
        }
        else {
          BOOST_ASSERT_MSG(false, "Sorry, we do not accomodate positive exponential in sci. format for now :)");
          //int_comp_str += float_comp_str.substr(0, e_sign_pos);// e_sign_pos becomes the number of digits before "e"
          
          //string all_char_after_e = float_comp_str.substr(e_sign_pos+1, float_comp_str.size());
          //for (uint64_t j=0; j<lexical_cast<uint64_t>(all_char_after_e)-e_sign_pos; ++j) {// e_sign_pos becomes the number of digits before "e"
            //int_comp_str += "0";
          //}
        }
      }  
    
      // Truncate float comp
      float_comp_str = float_comp_str.substr(0, n_float_digit);
      //cout << "(trunc) float_comp_str= " << float_comp_str << endl;
      
      // Join
      e_str = int_comp_str + "." + float_comp_str;
      
      // Assign
      mat(i) = lexical_cast<double>(e_str);
    }// if ( e_str.substr(0,1) == "-") { // if negative value
    else {
      //mat(i) = round(e,5,"rounded_up");
      mat(i) = round(e,5,"nearest");
    }
  }//  for (uint64_t i=0; i<mat.size(); ++i)
  
  return mat;
}

Eigen::MatrixXd EigenLibSupport::shift_mat(Eigen::MatrixXd mat) {
  using namespace std;
  //cout << "EigenLibSupport::shift_mat(): BEGIN\n";
  
  // This shift a matrix mat _one_ step to the right and to the bottom
  using namespace Eigen;
  using namespace std;
  
  MatrixXd mat_2;
  mat_2 = MatrixXd::Zero(mat.rows()+1, mat.cols()+1);// +1 as shifted _one_ step only
  
  //mat_2.block<mat.rows(),mat.cols()>(1,1) = mat;// Leads to compile-time error
  mat_2.block<128,128>(1,1) = mat;// TODO should 128 (=mat.rows()) be hardcoded?

  //cout << "EigenLibSupport::shift_mat(): END\n";
  return mat_2;
}

std::vector< std::vector<double> > EigenLibSupport::mat2stdvec(const Eigen::MatrixXd& mat) {
  std::vector< std::vector<double> > vec(mat.rows());
  
  for (uint64_t i=0; i<vec.size(); ++i) {
    std::vector<double> row_vec(mat.cols());
    for (uint64_t j=0; j<row_vec.size(); ++j) {
      row_vec.at(j) = mat(i,j);
    }
    vec.at(i) = row_vec;
  }
  
  return vec;
}

Eigen::MatrixXd EigenLibSupport::stdvec2mat(const std::vector< std::vector<double> >& vec) {
  Eigen::MatrixXd mat(vec.size(), vec.at(0).size());
  
  for (uint64_t i=0; i<mat.rows(); ++i) {
    for (uint64_t j=0; j<mat.cols(); ++j) {
      mat(i,j) = vec.at(i).at(j);
    }
  }
    
  return mat;
}

Eigen::MatrixXd EigenLibSupport::round(const Eigen::MatrixXd& mat, const uint8_t& n_float_digit, const std::string type) {
  Eigen::MatrixXd r_mat(mat.rows(), mat.cols());
  
  for (uint64_t i=0; i<mat.rows(); ++i) {
    for (uint64_t j=0; j<mat.cols(); ++j) {
      r_mat(i,j) = round(mat(i,j), n_float_digit, type);
    }
  }
  
  return r_mat;
}
