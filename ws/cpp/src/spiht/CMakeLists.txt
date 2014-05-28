add_library (ecg_spiht ecg_spiht.cc wavelet.cpp)

include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/util) 
include_directories (${fftw3_LIBRARIES}/fftw3) 
include_directories (-I/usr/local/include/eigen3) 

