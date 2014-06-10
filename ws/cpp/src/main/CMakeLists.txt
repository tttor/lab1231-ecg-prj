add_executable (pipeline pipeline.cc) 
target_link_libraries (pipeline lab1231ecgprj_sphit lab1231ecgprj_util) 

# add the install targets, invoked by: $ make install
install (TARGETS pipeline DESTINATION bin)
