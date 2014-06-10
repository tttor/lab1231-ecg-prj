add_library (lab1231ecgprj_util csv_io.cc eigenlib_support.cc debugger.cc)

target_link_libraries (lab1231ecgprj_util boost_filesystem boost_system)

# add the install targets, invoked by: $ make install
install (FILES eigenlib_support.h csv_io.h debugger.h
         DESTINATION include/util)
