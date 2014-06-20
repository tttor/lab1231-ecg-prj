add_library (lab1231ecgprj_array_const ecg_array_const.cc)

target_link_libraries (lab1231ecgprj_array_const lab1231ecgprj_util)

# add the install targets, invoked by: $ make install
install (FILES ecg_array_const.h
         DESTINATION include/array-const)
