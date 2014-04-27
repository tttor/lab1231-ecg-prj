#include <iostream>

#include <ecg_signal_preprocessor.h>

int main() {
  using namespace std;
  using namespace lab1231_ecg_prj;
  
  cout << "Compression: BEGIN\n";
  ECGSignalPreprocessor::preprocess();
  
  cout << "Compression: END\n";
  return 0;
}
