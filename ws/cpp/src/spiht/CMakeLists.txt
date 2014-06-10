add_library (lab1231ecgprj_sphit ecg_spiht.cc)
target_link_libraries (lab1231ecgprj_sphit lab1231ecgprj_util)

# add the install targets, invoked by: $ make install
install (FILES "ecg_spiht.h" DESTINATION include/spiht)
