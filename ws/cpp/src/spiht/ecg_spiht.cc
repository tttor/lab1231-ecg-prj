#include "ecg_spiht.h"

using namespace lab1231_ecg_prj;

std::vector<Eigen::RowVectorXd> ECGSPIHT::run_spiht(const std::vector<Eigen::MatrixXd>& wavelet_img_all_frame, const uint64_t& CR, const uint64_t& res, const std::string& out_dir_path) {
  using namespace std;
  using namespace boost;
  using Eigen::MatrixXd;
  using Eigen::RowVectorXd;
  
  cout << "run_spiht(): BEGIN" << endl;
  
  const uint8_t n_lead = 8;
  const uint64_t n_sample = 46260;// TODO why this number?
  const uint8_t n_frame = wavelet_img_all_frame.size();
  const uint64_t max_bits = (n_sample*n_lead*res/CR) / n_frame;// Indeed: truncation!
  
  vector<RowVectorXd> bit_str_all_frames(n_frame);
  
  // FAILED frame ith= 10, 12, 15, 16, 17
  const uint8_t ith_frame_begin = 10;// \in 0,1,2,...
  const uint8_t ith_frame_end = 11;//n_frame;// post the end
  for (uint8_t i=ith_frame_begin; i<ith_frame_end; ++i) {
    cout << "RUNNING SPIHT CODING FOR FRAME ith= " << lexical_cast<string>(i+1) << ": BEGIN zzzzzzzzzzzzzzzzzzzzzzzzzzzz"<< endl;
    RowVectorXd bit_str;
    const uint8_t level = 7;// TODO why 7?

    bit_str = spiht_enc(wavelet_img_all_frame.at(i), max_bits, level);
    //cout << "bit_str=\n" << bit_str << endl;
    //cout << "bit_str.size()= " << bit_str.size() << endl;
    
    //
    const string bit_str_octave_dir_path = string("../../octave/main/out/bit-str/");
    const string bit_str_star_path = string(bit_str_octave_dir_path + "bit_str-" + lexical_cast<string>(i+1) + ".csv");
    cout << "bit_str_star_path= " << bit_str_star_path << endl;
    
    MatrixXd bit_str_star_mat;
    bit_str_star_mat = CSVIO::load(bit_str_star_path);
    RowVectorXd bit_str_star = bit_str_star_mat.row(0);
    
    BOOST_ASSERT_MSG( bit_str==bit_str_star, string("bit_str!=bit_str_star: AT FRAME ith= " + lexical_cast<string>(i+1)).c_str() );
    cout << "bit_str==bit_str_star; JOSS :)\n";
    
    //
    bit_str_all_frames.at(i) = bit_str;
    
    // Save
    MatrixXd bit_str_mat(1, bit_str.size());
    bit_str_mat.row(0) = bit_str;
      
    boost::filesystem::create_directories(out_dir_path);
    string bit_str_csv = string(out_dir_path + "bit_str-" + lexical_cast<string>(i+1) + ".cpp.csv");
    
    cout << "Saving " << bit_str_csv << endl;
    CSVIO::write(bit_str_mat, bit_str_csv);
    
    cout << "RUNNING SPIHT CODING FOR FRAME ith= " << lexical_cast<string>(i+1) << ": END zzzzzzzzzzzzzzzzzzzzzzzzzzz"<< endl;   
  }
  cout << "bit_str_all_frames.size()= " << bit_str_all_frames.size() << endl;
  
  cout << "run_spiht(): END" << endl;
  return bit_str_all_frames;
}

Eigen::RowVectorXd ECGSPIHT::spiht_enc(const Eigen::MatrixXd& wavelet_img, const uint64_t& max_bits, const uint64_t& level) {
  using Eigen::MatrixXd;
  using Eigen::VectorXd;
  using Eigen::RowVectorXd;
  using namespace std;
  using namespace boost;
  
  cout << "spiht_enc(): BEGIN" << endl;
  //-------------------------initialization------------------------------
  cout << "initialization" << endl;
  
  MatrixXd wavelet_img_tr;
  wavelet_img_tr = wavelet_img.transpose();
  
  double max_val;
  max_val = wavelet_img.rowwise().maxCoeff().maxCoeff();
  
  const uint16_t n_max = floor( log2(abs(max_val)) );// threshold to determine whether a wavelet coeff is significant
  //cout << "n_max= " << n_max << endl;
  
  RowVectorXd bit_str;
  
  bit_str = MatrixXd::Ones(1, max_bits) * 2;// multiply by 2 for error detection, normal values are either 0 or 1
  bit_str(0) = wavelet_img.cols();
  bit_str(1) = n_max;
  bit_str(2) = level;
 
  uint16_t bit_str_idx = 3;// since the first 3 elements have been filled, see above
  
  //-----------initialize LIP, LSP, LIS-----------------------
  cout << "initializise LIP" << endl;
  
  const uint16_t bandsize = pow( 2,log2(wavelet_img.rows())-level+1 );
  
  RowVectorXd raw_LIP_row(bandsize);
  for (uint16_t i=0; i<bandsize; ++i) {
    raw_LIP_row(i) = i;
  }
  
  MatrixXd raw_LIP(bandsize, bandsize);
  for(uint16_t i=0; i<bandsize; i++){
    raw_LIP.row(i) = raw_LIP_row;
  }
  
  VectorXd col_major_vec_of_raw_LIP(bandsize*bandsize);
  VectorXd row_major_vec_of_raw_LIP(bandsize*bandsize);
  uint32_t idx = 0;
  for (uint16_t i=0; i<bandsize; ++i) {
    for (uint16_t j=0; j<bandsize; ++j) {
      col_major_vec_of_raw_LIP(idx) = raw_LIP(j, i);
      row_major_vec_of_raw_LIP(idx) = raw_LIP(i, j);
      ++idx;
    }
  } 
  //cout << col_major_vec_of_raw_LIP << endl << endl;
  //cout << row_major_vec_of_raw_LIP << endl << endl;
  
  MatrixXd LIP(bandsize*bandsize, 2);// LIP = List of Insignificant Pixels
  LIP.col(0) = col_major_vec_of_raw_LIP;
  LIP.col(1) = row_major_vec_of_raw_LIP;
  //cout << "LIP=\n" << LIP << endl << endl;
  
  cout << "initializise LIS" << endl;
  
  MatrixXd LIS(bandsize*bandsize, 3);// LIS = List of Insignificant Set
  LIS.col(0) = LIP.col(0);
  LIS.col(1) = LIP.col(1);
  LIS.col(2) = MatrixXd::Zero(bandsize*bandsize,1);
  //cout << "Before LIS=\n" << LIS << endl;
  
  uint8_t pstart = 0;
  uint8_t pend = (bandsize/2)-1;// minus one because idx begins at 0
  for(uint8_t i=0; i<bandsize/2; i++){
    LIS = EigenLibSupport::remove_rows(LIS, pstart, pend);
    
    uint8_t pdel;
    pdel = pend - pstart + 1;// plus one to make pdel points to the element just after the element pointed by pend, which was already deleted now
    
    pstart = pstart + bandsize - pdel;
    pend = pend + bandsize - pdel;
  }
  //cout << "After LIS=\n" << LIS << endl;
  
  cout << "initializise LSP" << endl;
  MatrixXd LSP;// LSP = List of Significant Pixels

  //---------------------------coding-----------------------------------
  cout << "coding ....\n";
  #define DEBUG_OFF
  
  uint64_t bitctr;// TODO is it bit counter?
  bitctr = 24;// 24 = 3 byte * 8 bit/byte = 24 bits; 3 for the first 3 column in bit_str
  
  uint64_t bits_LSP = 0;
  uint64_t bits_LIP = 0;
  uint64_t bits_LIS = 0;
  
  int64_t n;// n is counter for OUTERWHILE
  n = n_max;
  
  uint64_t if_ctr = 0;// "if" here refers to "if (tmp_LIS(i,2)==0) {}"
  uint64_t else_ctr = 0;// "else" here refers to "else {} of if (tmp_LIS(i,2)==0) {}"
  uint64_t outer_while_ctr = 0;
  uint64_t outer_while_ctr_target = 0;// Invalid target
  uint64_t base_outer_while_ctr = 0;// Invalid base
  
  #ifdef DEBUG_ON
  //outer_while_ctr_target = 11;// Valid target
  //base_outer_while_ctr = 2;// Valid base
  Debugger::load_outerwhile_param(base_outer_while_ctr, 
                                  &LSP, &LIP, &LIS, &bits_LSP, &bits_LIP, &bits_LIS,
                                  &bit_str, &bit_str_idx,
                                  &n, &bitctr, &if_ctr, &else_ctr, &outer_while_ctr);
  cout << "base_outer_while_ctr= " << base_outer_while_ctr << endl;
  #endif

  Debugger::reset(base_outer_while_ctr);
  
  while (bitctr < max_bits) {/////////////// OUTER_WHILE //////////////////////////////////////////////
    ++outer_while_ctr;
    string here_outerwhile = string("outerwhile-" + lexical_cast<string>(outer_while_ctr) + "/");
    cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
    cout << "outerwhile: iter= " << outer_while_ctr << ": BEGIN\n";
    
    string here_outerwhile_event_1 = here_outerwhile + "event-1/";
    
    // Sorting pass
    MatrixXd tmp_LIP = LIP;
    int64_t LIP_idx = -1;// TODO what does LIP_IDX point for in LIP?; -1 to synch with the Matlab's implementation
    
    for (uint64_t i=0; i<tmp_LIP.rows(); ++i) {
      cout << "for (uint64_t i=0; i<tmp_LIP.rows(); ++i), i= " << i << ": BEGIN\n";
      
      ++LIP_idx;
      if ((bitctr + 1) >= max_bits) {
        if (bitctr < max_bits) {
          bit_str.conservativeResize( bit_str.size()-1 );
        }
        cout << "((bitctr + 1) >= max_bits): RETURN\n";
        //#ifdef DEBUG_ON
        //BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_outerwhile_event_1), Debugger::msg.c_str());
        //#endif
          
        return bit_str;
      }
      
      double wavelet_img_character;
      wavelet_img_character = abs( wavelet_img(tmp_LIP(i,0),tmp_LIP(i,1)) );
      
      if ( wavelet_img_character >= pow(2, n) ) {
        cout << "if ( wavelet_img_character >= pow(2, n) ): BEGIN\n";
        
        spiht_enc_helper_4(1, &bit_str, &bit_str_idx, &bitctr, &bits_LIP);
        if (  wavelet_img(tmp_LIP(i,0),tmp_LIP(i,1))  >= 0  ) {
          bit_str(bit_str_idx) = 1;
        }
        else {
          bit_str(bit_str_idx) = 0;
        }
        
        ++bitctr;
        ++bit_str_idx;
        ++bits_LIP;
        
        if (LSP.size()==0) {
          LSP = tmp_LIP.row(i);
        }
        else {
          LSP.conservativeResize( LSP.rows()+1, LSP.cols() );
          LSP.row(LSP.rows()-1) = tmp_LIP.row(i);
        }
        LIP = EigenLibSupport::remove_row(LIP, LIP_idx);
        --LIP_idx;
        
        cout << "if ( wavelet_img_character >= pow(2, n) ): END\n";
      }
      else {
        cout << "else {} of if ( wavelet_img_character >= pow(2, n) ): BEGIN\n";
        
        spiht_enc_helper_4(0, &bit_str, &bit_str_idx, &bitctr, &bits_LIP);
        
        cout << "else {} of if ( wavelet_img_character >= pow(2, n) ): END\n";
      }
      
      cout << "for (uint64_t i=0; i<tmp_LIP.rows(); ++i), i= " << i << ": END\n";
    }// for (uint64_t i=0; i<tmp_LIP.rows(); ++i)
      
    #ifdef DEBUG_ON
    BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_outerwhile_event_1), Debugger::msg.c_str());    
    BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("tmp_LIP", tmp_LIP, here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LSP", LSP, here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_outerwhile_event_1), Debugger::msg.c_str());
    #endif
    
    MatrixXd tmp_LIS = LIS;
    int64_t LIS_idx = -1;// TODO this looks silly (as the matlab implementation), fix it! 
    uint64_t i = 0;// iterator variable for the innerwhile1: while (i < tmp_LIS.rows())
    
    uint64_t inner_while_1_ctr = 0;  
    while (i < tmp_LIS.rows()) {/////// INNER_WHILE_1 ////////////////////////////////////////////////
      ++inner_while_1_ctr;
      string here_innerwhile1 = string(here_outerwhile + "innerwhile1-" + lexical_cast<string>(inner_while_1_ctr) + "/");
      
      cout << "-------------------------------------------------------\n";
      cout << "outerwhile: iter= " << outer_while_ctr << " AT innerwhile1: iter= " << inner_while_1_ctr << ": BEGIN" << endl;
      
      string here_innerwhile1_event_1 = here_innerwhile1 + "event-1/";
      
      LIS_idx = LIS_idx + 1;
      
      if (tmp_LIS(i,2)==0) {
        cout << "if (tmp_LIS(i,2)==0): BEGIN\n";
        
        if (bitctr >= max_bits) {
          cout << "(bitctr >= max_bits): RETURN\n";
          //#ifdef DEBUG_ON
          //BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile1_event_1), Debugger::msg.c_str());
          //#endif
          
          return bit_str;
        }
        
        double max_d;
        max_d = get_descendant(tmp_LIS.row(i), wavelet_img);
        //Debugger::write_param("max_d_upper", EigenLibSupport::scalar2mat(max_d), here_innerwhile1_event_1);
        
        #ifdef DEBUG_ON
        double max_d_star;
        max_d_star = get_descendant(1, if_ctr);
        
        cout << "asserting (max_d == max_d_star)";
        BOOST_ASSERT_MSG(max_d == max_d_star, string("max_d != max_d_star AT IF{} UPPER: WANT: " + lexical_cast<string>(max_d_star) + " GOT: " + lexical_cast<string>(max_d)).c_str());
        cout << ": PASSED\n";
        //cout <<  "max_d UPPER: WANT: " << lexical_cast<string>(max_d_star) << " GOT: " << lexical_cast<string>(max_d) << endl;
        #endif
        
        if (max_d >= pow(2,n)) {
          cout << "if (max_d >= pow(2,n)): BEGIN\n";
          
          spiht_enc_helper_4(1, &bit_str, &bit_str_idx, &bitctr, &bits_LIS);
          
          // Go through 4 possible cases
          // TODO make use of get_pq()
          uint64_t x, y;
          x = tmp_LIS(i, 0);
          y = tmp_LIS(i, 1);
          
          for (uint8_t c=1; c<5; ++c) {// c for case iterator, $c \in {1, 2, 3, 4}$
            bool helper_2_flag;          
            helper_2_flag = spiht_enc_helper_2(bitctr, max_bits, &bit_str);
            if (helper_2_flag == true) {
              cout << "(helper_2_flag == true): RETURN\n";
              //#ifdef DEBUG_ON
              //BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile1_event_1), Debugger::msg.c_str());
              //#endif
        
              return bit_str;
            }
          
            uint64_t p, q;
            get_pq(c, x, y, &p, &q);
            
            spiht_enc_helper(wavelet_img, p, q, n, &LSP, &LIP, &bit_str, &bit_str_idx, &bitctr, &bits_LIS);
          }
          
          // Appendix step, TODO rationale?
          cout << "Appendix step: BEGIN:\n";
          
          uint64_t p, q;
          p = (2 * (2*x) - 1) + 3;// plus 3 for idx correction
          q = (2 * (2*y) - 1) + 3;
          
          if ( (p<wavelet_img.rows()) and (q<wavelet_img.rows()) ) { // Assume: wavelet_img.cols() == wavelet_img.rows()
            LIS.conservativeResize( LIS.rows()+1, LIS.cols() );
            LIS(LIS.rows()-1, 0) = tmp_LIS(i, 0);
            LIS(LIS.rows()-1, 1) = tmp_LIS(i, 1);
            LIS(LIS.rows()-1, 2) = 1;// The 3th column of LIS contains the type of descendant used in get_descendant()
            
            tmp_LIS.conservativeResize( tmp_LIS.rows()+1, tmp_LIS.cols() );
            tmp_LIS(tmp_LIS.rows()-1, 0) = tmp_LIS(i, 0);
            tmp_LIS(tmp_LIS.rows()-1, 1) = tmp_LIS(i, 1);
            tmp_LIS(tmp_LIS.rows()-1, 2) = 1;
          }
          LIS = EigenLibSupport::remove_row(LIS, LIS_idx);
          LIS_idx = LIS_idx - 1;
          
          cout << "if (max_d >= pow(2,n)): END\n";
        }// if (max_d >= pow(2,n))
        else {
          spiht_enc_helper_4(0, &bit_str, &bit_str_idx, &bitctr, &bits_LIS);
        } 
        
        // 
        ++if_ctr;
        
        cout << "if (tmp_LIS(i,2)==0): END\n";
      }// if (tmp_LIS(i,2)==0) 
      else {
        cout << "else {} of if (tmp_LIS(i,2)==0): BEGIN\n";
        
        if (bitctr >= max_bits) {
          cout << "(bitctr >= max_bits): RETURN\n";
          //#ifdef DEBUG_ON
          //BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile1_event_1), Debugger::msg.c_str());
          //#endif
          return bit_str;
        }
        
        double max_d;
        max_d = get_descendant(tmp_LIS.row(i), wavelet_img);
        //Debugger::write_param("max_d_bottom", EigenLibSupport::scalar2mat(max_d), here_innerwhile1_event_1 );
        
        #ifdef DEBUG_ON
        double max_d_star;
        max_d_star = get_descendant(2, else_ctr);
        
        cout << "asserting (max_d == max_d_star)\n";
        BOOST_ASSERT_MSG(max_d == max_d_star, string("max_d != max_d_star AT ELSE{} BOTTOM: WANT: " + lexical_cast<string>(max_d_star) + " GOT: " + lexical_cast<string>(max_d)).c_str());
        cout << ": PASSED\n";
        //cout <<  "max_d BOTTOM: WANT: " << lexical_cast<string>(max_d_star) << " GOT: " << lexical_cast<string>(max_d) << endl;
        #endif
        
        if (max_d >= pow(2,n)) {
          cout << "if (max_d >= pow(2,n)): BEGIN\n";
          
          spiht_enc_helper_4(1, &bit_str, &bit_str_idx, &bitctr);
          
          // Go through 4 cases, again, but simpler calculation
          uint64_t x, y;
          x = tmp_LIS(i, 0);
          y = tmp_LIS(i, 1);
          
          for (uint8_t c=1; c<5; ++c) {// c for case iterator, $c \in {1, 2, 3, 4}$
            uint64_t p, q;
            get_pq(c, x, y, &p, &q);
            
            spiht_enc_helper_3(p, q, &LIS);
            spiht_enc_helper_3(p, q, &tmp_LIS);
          }
          
          LIS = EigenLibSupport::remove_row(LIS, LIS_idx);
          --LIS_idx;
          
          cout << "if (max_d >= pow(2,n)): END\n";
        }
        else {
          cout << "else {} of if (max_d >= pow(2,n)): BEGIN\n";
          spiht_enc_helper_4(0, &bit_str, &bit_str_idx, &bitctr, &bits_LIS);
          cout << "else {} of if (max_d >= pow(2,n)): END\n";
        }
        
        //
        ++else_ctr;
        
        cout << "else {} of if (tmp_LIS(i,2)==0): END\n";
      }// else {} of if (tmp_LIS(i,2)==0
      
      #ifdef DEBUG_ON
      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LSP", LSP, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LIS", LIS, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("tmp_LIS", tmp_LIS, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile1_event_1), Debugger::msg.c_str());
      #endif
    
      // Increment the iterator idx of this while-loop:  while (i < tmp_LIS.rows() )
      ++i;
      
      cout << "outerwhile: iter= " << outer_while_ctr << " AT innerwhile1: iter= " << inner_while_1_ctr << ": END" << endl;
    }// while (i < tmp_LIS.rows() ) ... INNERWHILE1
    
    string here_outerwhile_event_2 = here_outerwhile + "event-2/";
    
    #ifdef DEBUG_ON
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LSP", LSP, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LIS", LIS, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("tmp_LIS", tmp_LIS, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_outerwhile_event_2), Debugger::msg.c_str());
    #endif
    
    // Refinement Pass
    //cout << "Refinement Pass\n";
    uint64_t LSP_idx = 0;
    
    double wavelet_img_character;
    wavelet_img_character = wavelet_img( LSP(LSP_idx,0), LSP(LSP_idx,1) );
        
    double LSP_character = abs( pow(2,n_max-n+1)*wavelet_img_character);
    LSP_character = floor(LSP_character);
    
    uint64_t inner_while_2_ctr = 0;
    while (  ( LSP_character >= pow(2,n_max+2) ) and ( LSP_idx < LSP.rows() )  ) { // INNER_WHILE_2 ////////////////////////////////////////
      ++inner_while_2_ctr;
      cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
      string here_innerwhile2 = string(here_outerwhile + "innerwhile2-" + lexical_cast<string>(inner_while_2_ctr) + "/");
      cout << "outerwhile: iter= " << outer_while_ctr << " AT innerwhile2: iter= " << inner_while_2_ctr << ": BEGIN" << endl;
      
      string here_innerwhile2_event_1 = here_innerwhile2 + "event-1/";
      
      //Debugger::write_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_innerwhile2_event_1);
      if (bitctr >= max_bits) {
        cout << "(bitctr >= max_bits): RETURN\n";
        //#ifdef DEBUG_ON
        //BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile2_event_1), Debugger::msg.c_str());
        //#endif
        
        return bit_str;
      }
      
      const uint8_t bitset_size = 100;// TODO why 100? higher?
      bitset<bitset_size> LSP_character_bin(LSP_character);
      
      bit_str(bit_str_idx) = LSP_character_bin[n_max + 2 - 1];// minus one because index in cpp begins at 0; TODO why plus 2?
      ++bitctr;
      ++bit_str_idx;
      ++LSP_idx;
      ++bits_LSP;
      
      if (LSP_idx < LSP.rows()) {
        wavelet_img_character = wavelet_img( LSP(LSP_idx,0), LSP(LSP_idx,1) );
        //Debugger::write_param("wavelet_img_character", EigenLibSupport::scalar2mat(wavelet_img_character), here_innerwhile2_event_1);

        LSP_character = abs( pow(2,n_max-n+1)*wavelet_img_character );
        LSP_character = floor(LSP_character);
      }

      #ifdef DEBUG_ON
      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_innerwhile2_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("bits_LSP", EigenLibSupport::scalar2mat(bits_LSP), here_innerwhile2_event_1
      ), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LSP_character", EigenLibSupport::scalar2mat(LSP_character), here_innerwhile2_event_1), Debugger::msg.c_str());// TODO is this really unncessary to assert this param? If we do, then at some point, this fails since the value of wavelet_img_character (due to numerical precision issues) is mismatched with the those of Matlab's run. This is justifiable because "bitset<bitset_size> LSP_character_bin(LSP_character);"; it takes the most significant bits
      //BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_innerwhile2_event_1), Debugger::msg.c_str());
      //BOOST_ASSERT_MSG(Debugger::debug_param("LSP_idx", EigenLibSupport::scalar2mat(LSP_idx), here_innerwhile2_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile2_event_1), Debugger::msg.c_str());
      #endif
      
      cout << "outerwhile: iter= " << outer_while_ctr << " AT innerwhile2: iter= " << inner_while_2_ctr << ": END" << endl;
    }// INNERWHILE2
    
    string here_outerwhile_event_3 = here_outerwhile + "event-3/";
    
    // Decrement the n; TODO elaborate the def of n
    --n;
    
    #ifdef DEBUG_ON
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bits_LIS", EigenLibSupport::scalar2mat(bits_LIS), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bits_LIP", EigenLibSupport::scalar2mat(bits_LIP), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bits_LSP", EigenLibSupport::scalar2mat(bits_LSP), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("n", EigenLibSupport::scalar2mat(n), here_outerwhile_event_3), Debugger::msg.c_str());    
    BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_outerwhile_event_3), Debugger::msg.c_str());
    
    Debugger::write_param("LSP", LSP, here_outerwhile_event_3);
    Debugger::write_param("LIS", LIS, here_outerwhile_event_3);
    Debugger::write_param("if_ctr", EigenLibSupport::scalar2mat(if_ctr), here_outerwhile_event_3);
    Debugger::write_param("else_ctr", EigenLibSupport::scalar2mat(else_ctr), here_outerwhile_event_3);
    #endif
    
    cout << "outerwhile: iter= " << outer_while_ctr << ": END  xxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
    if (outer_while_ctr==outer_while_ctr_target) {
      cout << "outer_while_ctr==outer_while_ctr_target: RETURN\n";
      return bit_str;
    }
  }// while (bitctr < max_bits) ... OUTERWHILE

  //////////////////////////////////////////////////////////////////
  cout << "spiht_enc(): END" << endl;
  
  return bit_str;
}

void ECGSPIHT::spiht_enc_helper(const Eigen::MatrixXd& wavelet_img, const uint8_t& x, const uint8_t& y, const int64_t& n, Eigen::MatrixXd* LSP, Eigen::MatrixXd* LIP, Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx, uint64_t* bitctr, uint64_t* bits_LIS) {
  using namespace std;
  cout << "spiht_enc_helper(): BEGIN\n";
  
  //cout << "x= " << (int)x << endl;
  //cout << "y= " << (int)y << endl;
  
  if ( abs(wavelet_img(x, y)) >= pow(2, n) ) {
    cout << "if ( abs(wavelet_img(x, y)) >= pow(2, n) ): BEGIN\n";
    
    if (LSP->size() == 0) {
      LSP->conservativeResize(1, 2);// yes, two columns
    }
    else {
      LSP->conservativeResize( LSP->rows()+1, LSP->cols() );// increment the size of rows by one
    }
    
    (*LSP)(LSP->rows()-1,0) = x;
    (*LSP)(LSP->rows()-1,1) = y;
    
    (*bit_str)(*bit_str_idx) = 1;
    
    ++(*bitctr);
    ++(*bit_str_idx);
    ++(*bits_LIS);
    
    if (wavelet_img(x, y) >= 0) {
      (*bit_str)(*bit_str_idx) = 1;
    }
    else {
      (*bit_str)(*bit_str_idx) = 0;
    }
    
    ++(*bitctr);
    ++(*bit_str_idx);
    ++(*bits_LIS);
    
    cout << "if ( abs(wavelet_img(x, y)) >= pow(2, n) ): END\n";
  }// if ( abs(wavelet_img(x, y)) >= pow(2, n) ) 
  else {
    cout << "else {} of if ( abs(wavelet_img(x, y)) >= pow(2, n) ): BEGIN\n";
    
    (*bit_str)(*bit_str_idx) = 0;
    
    ++(*bitctr);
    ++(*bit_str_idx);
    ++(*bits_LIS);
    
    LIP->conservativeResize( LIP->rows()+1, LIP->cols() );
    (*LIP)(LIP->rows()-1,0) = x;
    (*LIP)(LIP->rows()-1,1) = y;
    
    cout << "else {} of if ( abs(wavelet_img(x, y)) >= pow(2, n) ): END\n";
  }
  
  cout << "spiht_enc_helper(): END\n";
}

bool ECGSPIHT::spiht_enc_helper_2(const uint64_t& bitctr, const uint64_t& max_bits, Eigen::RowVectorXd* bit_str) {
  if ((bitctr + 1) >= max_bits) {
    if (bitctr < max_bits) {
      bit_str->conservativeResize( bit_str->size()-1 );
    }
    
    std::cout << "MAY IMMEDIATE RETURN bit_str\n";
    return true;
  }
  return false;
}

void ECGSPIHT::spiht_enc_helper_3(const uint8_t& x, const uint8_t& y, Eigen::MatrixXd* LIS) {
  // Assume: LIS is already filled (hence, not empty) up to this point
  LIS->conservativeResize( LIS->rows()+1, LIS->cols() );
  (*LIS)(LIS->rows()-1, 0) = x;
  (*LIS)(LIS->rows()-1, 1) = y;
  (*LIS)(LIS->rows()-1, 2) = 0;// descendant type \in {0,1}
}

void ECGSPIHT::spiht_enc_helper_4(const uint8_t bit_str_val, Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx, uint64_t* bitctr, uint64_t* bits_LIS_or_LIP) {
  using namespace std;
  
  (*bit_str)(*bit_str_idx) = bit_str_val;
  ++(*bitctr);
  ++(*bit_str_idx);
  if (bits_LIS_or_LIP != 0) {
    ++(*bits_LIS_or_LIP);
  }
}

double ECGSPIHT::get_descendant(const Eigen::RowVectorXd& LIS_row, const Eigen::MatrixXd& mat) {
  using namespace Eigen;
  using namespace std;
  using namespace boost;
  cout << "get_descendant(): BEGIN\n";
  cout << "input LIS_row= " << LIS_row << endl;
  //const string csv_dir = "/home/tor/robotics/prj/010/ws/lab1231-ecg-prj/ws/cpp/out/debug/S-in-while/";
  //boost::filesystem::remove_all( boost::filesystem::path(csv_dir) );
  //boost::filesystem::create_directories(csv_dir);
  
  // The modification is used to ease the matrix indexing since many indexing formulas below are really _unclear_ about where they come from.
  // As a result, we can treat all indexing formulas as _is_ (exactly the same with those in Matlab).
  // This modified matrix has one extra row and one extra col; the first column and the first row are invalid.
  MatrixXd mod_mat;
  mod_mat = EigenLibSupport::shift_mat(mat);
  
  //
  RowVectorXd S;// TODO More descriptive name?

  // +1 since matlab idx begins at 1 and we already have mod_mat, see above!
  // yes, truncate from double
  uint64_t val_1 = LIS_row(0) + 1;
  uint64_t val_2 = LIS_row(1) + 1; 
  //cout << "val_1= " << val_1 << endl;
  //cout << "val_2= " << val_2 << endl;
  
  uint64_t index = 0;// TODO More descriptive name? is it truly an index (as 0 is invalid idx in Matlab)? 
  uint64_t a = 0;// More descriptive name for "a"?
  uint64_t b = 0;// More descriptive name for "b"?
  
  uint64_t while_ctr = 0;
  while ( ((2*val_1-1)<(mod_mat.rows()-1)) and ((2*val_2-1)<(mod_mat.cols()-1)) ) {// Minus one for n_row and n_col calculation as weuse mod_mat. TODO where do the formulas come from?
    //cout << "while (  ( (2*val_1-1) < mat.rows() ) and ...): BEGIN iter= " << lexical_cast<string>(index+1) << endl;
    
    uint64_t a = val_1 - 1;// TODO why minus 1? 
    uint64_t b = val_2 - 1;// TODO why minus 1? 
  
    //cout << "construct row_idxes: BEGIN\n";
    const uint64_t row_idxes_start = 2 * (a+1) - 1;// TODO where do the formulas come from?
    const uint64_t row_idxes_end = 2 * (a+pow(2,index));// TODO where do the formulas come from?
    //cout << "row_idxes_start= " << row_idxes_start << endl;
    //cout << "row_idxes_end= " << row_idxes_end << endl;
    
    RowVectorXd row_idxes;
    row_idxes.setLinSpaced(row_idxes_end-row_idxes_start+1, row_idxes_start, row_idxes_end);
    row_idxes = modify_idxes(row_idxes, mat.rows());
    
    //cout << "construct col_idxed: BEGIN\n";
    const uint64_t col_idxes_start = 2 * (b+1) - 1;//TODO where do the formulas come from?
    const uint64_t col_idxes_end = 2 * (b+pow(2,index));//TODO where do the formulas come from?
    
    RowVectorXd col_idxes;
    col_idxes.setLinSpaced(col_idxes_end-col_idxes_start+1, col_idxes_start, col_idxes_end);
    col_idxes = modify_idxes(col_idxes, mat.cols());
    
    //cout << "construct local_S_mat: BEGIN\n";
    MatrixXd local_S_mat(row_idxes.size(),col_idxes.size());
    for (uint64_t i=0; i<row_idxes.size(); ++i) {
      for (uint64_t j=0; j<col_idxes.size(); ++j){
        uint64_t x = row_idxes(i);
        uint64_t y = col_idxes(j);
        
        BOOST_ASSERT_MSG(x>0, "ACCESS INVALID ELEMENT OF MOD_MAT");
        BOOST_ASSERT_MSG(y>0, "ACCESS INVALID ELEMENT OF MOD_MAT");

        local_S_mat(i,j) = mod_mat(x, y);
      }
    }
    
    //cout << "construct local_S_vec: BEGIN\n";
    RowVectorXd local_S_vec;
    local_S_vec = Map<RowVectorXd>(local_S_mat.data(), local_S_mat.size());// convert to a vector with column-major
    
    // 
    uint64_t init_appending_idx = S.size();
    S.conservativeResize( S.size()+local_S_vec.size() );

    for (uint64_t i=0, appending_idx=init_appending_idx; i<local_S_vec.size(); ++i,++appending_idx) {
      S(appending_idx) = local_S_vec(i);
    }
    
    // 
    index++;
    val_1 = 2 * a + 1;//TODO where do the formulas come from?
    val_2 = 2 * b + 1;//TODO where do the formulas come from?
    
    ////
    //MatrixXd S_mat(1, S.size());
    //S_mat.row(0) = S;
    //string S_csv_path = csv_dir + "S-" + lexical_cast<string>(while_ctr) + ".csv";
    //CSVIO::write(S_mat, S_csv_path);
    
    ++while_ctr;
    //cout << "while (  ( (2*val_1-1) < mat.rows() ) and ...): END \n";
  } //while (  ( (2*val_1-1) < mat.rows() ) and ( (2*val_2-1) < mat.rows() )   )
  BOOST_ASSERT_MSG(S.size() != 0, "S.size() == 0"); 
  
  // Assume that S.size() != 0
  RowVectorXd mod_S;
  if (LIS_row(2) == 1) {
    // TODO why remove 4 first elements? why 4?
    //S = S.tail( S.size()-4 );// THIS IS BUGGY
    
    const uint64_t first_idx = 4;
    mod_S.resize( S.size()-first_idx );
    for(uint64_t i=first_idx, target_idx= 0; i< S.size(); ++i, ++target_idx) {
      mod_S(target_idx) = S(i);
    }
  }
  else {
    mod_S = S;
  }

  //MatrixXd S_tail_mat(1, mod_S.size());     
  //S_tail_mat.row(0) = mod_S;
  //CSVIO::write(S_tail_mat, "/home/tor/robotics/prj/010/ws/lab1231-ecg-prj/ws/cpp/out/debug/S_tail_mat.csv");
  
  //
  for (uint64_t i=0; i<mod_S.size(); ++i){
    mod_S(i) = abs( mod_S(i) );
  }
  
  //
  MatrixXd::Index max_row, max_col;
  double descendant;
  descendant = mod_S.maxCoeff(&max_row, &max_col);
  
  //cout << "mod_S.size()= " << mod_S.size() << endl;
  //cout << "max_row= " << max_row << endl;
  //cout << "max_col= " << max_col << endl;
  //cout << "mod_S[max_col]= " << mod_S[max_col] << endl;
  //MatrixXd mod_S_mat(1, mod_S.size());
  //mod_S_mat.row(0) = mod_S;
  //CSVIO::write(mod_S_mat, "/home/tor/robotics/prj/010/ws/lab1231-ecg-prj/ws/cpp/out/debug/mod_S.csv");
  
  cout << "get_descendant(): END\n";
  return descendant;
}

double ECGSPIHT::get_descendant(const uint8_t& type, const uint64_t& ith) {
  using namespace std;
  cout << "get_descendant(LOOK UP): BEGIN\n";
  cout << "type= " << (int) type << endl;
  cout << "ith= " << ith << endl;
  
  std::string desc_csv;
  if (type==1)
    desc_csv = "../../octave/main/out/param/spiht-var/max_d_upper.csv";
  else if (type==2)
    desc_csv = "../../octave/main/out/param/spiht-var/max_d_bottom.csv";
   
  Eigen::MatrixXd desc;
  desc = CSVIO::load(desc_csv);
  //std::cout << "desc.cols()= " << desc.cols() << std::endl;
  assert( desc.size()!=0 );
  
  double val;
  val = desc(0, ith);
  //std::cout << "val= " << val << std::endl;
 
  cout << "get_descendant(LOOK UP): END\n";
  return val;
}

void ECGSPIHT::get_pq(const uint8_t& case_num, const uint64_t& x, const uint64_t& y, uint64_t* p, uint64_t* q) {
  // The varibles p and q are used to 1) in spiht_enc_helper() and 2) in modifying tmp_LIS
  // The formulas are copied from the Matlab implementation
  // Notice "+1" at the end of each formulas as indexes begin at 0
  switch(case_num) {
    case 1: {
      *p = (2 * x - 1) + 1;
      *q = (2 * y - 1) + 1;
      break;
    }
    case 2: {
      *p = (2 * x - 1) + 1;
      *q = (2 * y) + 1;
      break;
    }
    case 3: {
      *p = (2 * x) + 1;
      *q = (2 * y - 1) + 1;
      break;
    }
    case 4: {
      *p = (2 * x) + 1;
      *q = (2 * y) + 1;
      break;
    }
    default: {
      BOOST_ASSERT_MSG(false, std::string("ERROR: Unknown case").c_str());
    }
  }
}

Eigen::RowVectorXd ECGSPIHT::modify_idxes(const Eigen::RowVectorXd& idxes, const uint16_t& n_row) {
  using namespace std;
  //cout << "ECGSPIHT::modify_idxes(): BEGIN\n";
  //cout << "idxes.size()= " << idxes.size() << endl;
  
  uint16_t len_idxes = 0;
  for (uint64_t i=0; i<idxes.size(); ++i){
    if (idxes(i) <= n_row){
      len_idxes++;
    }
  }
  
  if (len_idxes < idxes.size()){
    return idxes.head(len_idxes);
  }
  
  //cout << "ECGSPIHT::modify_idxes(): END\n";
  return idxes;
}
