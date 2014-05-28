include_directories (${gsl_LIBRARIES}/gsl) 
include_directories (${fftw3_LIBRARIES}/fftw3) 


include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/pre) 
include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/util) 
include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/array-const) 
include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/norm) 
include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/spiht) 

link_directories (${gsl_LIBRARIES}/gsl) 
link_directories (${fftw3_LIBRARIES}/fftw3) 



link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/pre) 
link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/util) 
link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/array-const) 
link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/norm) 
link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/spiht) 


add_executable (ecg_compress ecg_compress.cc) 

target_link_libraries (ecg_compress ecg_signal_preprocessing)
target_link_libraries (ecg_compress ecg_file_util)  
target_link_libraries (ecg_compress ecg_array_const) 
target_link_libraries (ecg_compress ecg_norm) 
target_link_libraries (ecg_compress ecg_spiht)


target_link_libraries( ecg_compress ${gsl_LIBRARIES} )
target_link_libraries( ecg_compress ${fftw3_LIBRARIES} )

