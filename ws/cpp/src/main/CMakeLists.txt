include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/pre) 

link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/pre) 

add_executable (ecg_compress ecg_compress.cc) 

target_link_libraries (ecg_compress ecg_signal_preprocessing) 
