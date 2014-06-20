add_library (lab1231ecgprj_dwt wavelet.cpp)

# add the install targets, invoked by: $ make install
install (FILES wavelet.h
         DESTINATION include/dwt)
