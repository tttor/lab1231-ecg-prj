#include "csv_io.h"

using namespace lab1231_ecg_prj;

Eigen::MatrixXd CSVIO::load(std::string csv_path) {
  // Adopted from: https://gist.github.com/stephenjbarr/2266900
  using namespace std;
  using namespace boost;
  using Eigen::MatrixXd;
  
  ifstream csv(csv_path);
  BOOST_ASSERT_MSG(csv.is_open(), string("ERROR: Cannot find: " + csv_path).c_str());
  
  typedef tokenizer< escaped_list_separator<char> > Tokenizer;
  
  vector< string > vec;
  string line;
  vector< vector< string > > matrows;
  
  while (getline(csv,line)) {
    Tokenizer tok(line);
    vec.assign(tok.begin(),tok.end());
    matrows.push_back(vec);
  }
  csv.close();
  
  // FIGURE OUT HOW MANY OF THE ROWS HAVE THE RIGHT NUMBER
  // OF COLUMNS
  int Nrows = matrows.size();
  int Ncols = matrows[0].size();
  int Ngoodrows = 0;
  for(int i = 0; i < Nrows; i++) {
    if(matrows[i].size() == Ncols) {
      Ngoodrows++;
    }
  }
  
  // TRANSFORM THE VECTOR OF ROWS INTO AN EIGEN DOUBLE MATRIX
  MatrixXd xmat = MatrixXd(Ngoodrows, Ncols);
  //cout << "INPUT MATRIX: " << Nrows << "x" << Ncols << endl;
  int rc = 0;
  for(int i = 0; i < Nrows; i++) {
    int rowsize = matrows[i].size();
    if(rowsize != Ncols) {
      cout << "WARNING: Row " << i << " has bad column count" << endl;
      continue;
    } 
    
    for(int j = 0; j < Ncols; j++) {
      xmat(rc,j) = int( round(strtod(matrows[i][j].c_str(), NULL)) );
      xmat(rc,j) = lexical_cast<double>(matrows[i][j]);
    }
    rc++;
  }
  
  return xmat;
}

void CSVIO::write(Eigen::MatrixXd m, std::string csv_path) {
  using namespace std;
  using namespace boost;
  
  ofstream csv;
  csv.open(csv_path);
  BOOST_ASSERT_MSG(csv.is_open(), string("ERROR: Cannot open: " + csv_path).c_str());
  
  for(uint64_t  i=0; i<m.rows(); i++){
    for(uint64_t j=0; j<m.cols(); j++){
       string str = lexical_cast<std::string>(m(i,j));
       if(j+1 == m.cols()){
           csv << str;
       }else{
           csv << str << ",";
       }
    }
    csv << "\n";
  }
  
  csv.close();
}
