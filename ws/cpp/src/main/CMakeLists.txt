include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/pre) 
include_directories (${LAB1231_ECG_PRJ_SOURCE_DIR}/util) 

link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/pre) 
link_directories (${LAB1231_ECG_PRJ_BINARY_DIR}/util) 

add_executable (ecg_compress ecg_compress.cc) 

target_link_libraries (ecg_compress ecg_signal_preprocessing)
target_link_libraries (ecg_compress ecg_file_util)  
