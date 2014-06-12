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

double EigenLibSupport::mat2scalar(const Eigen::MatrixXd& mat){
  assert(mat.size()==1);
  return mat(0);
}

double EigenLibSupport::truncate(const double& val, const uint8_t& precision, const std::string type) {
  const uint64_t factor = pow(10, precision);
  
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
    assert(false && "Unknwon truncation type");
  }
  return result;
}
