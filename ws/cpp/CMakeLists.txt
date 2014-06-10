cmake_minimum_required (VERSION 2.6) 
project (LAB1231_ECG_PRJ) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")# Activate C++11
set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}) # used to determine the root of the install destination; you may change it to, e.g.: /usr/local/lab1231-ecg-prj

add_subdirectory (src)

## Print out all accessible variables in a script, 
## see http://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
    #message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

