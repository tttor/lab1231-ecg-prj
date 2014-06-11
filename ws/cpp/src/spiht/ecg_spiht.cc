#include "ecg_spiht.h"

using namespace lab1231_ecg_prj;

std::vector<Eigen::RowVectorXd> ECGSPIHT::run_spiht(const Eigen::MatrixXd& wavelet_img, const uint64_t& CR, const uint64_t& res, const uint64_t n_frame, std::string out_dir) {
  using namespace std;
  using namespace boost;
  using Eigen::MatrixXd;
  using Eigen::RowVectorXd;
  
  cout << "run_spiht(): BEGIN" << endl;
  
  uint8_t n_lead = 8;
  uint64_t n_sample = 46260;
  
  uint64_t max_bits;
  max_bits = (n_sample*n_lead*res/CR) / n_frame;// Indeed: truncation!
  //cout << "max_bits= "<< max_bits << endl;
  
  vector<RowVectorXd> bit_str_all_frames(n_frame);
  
  const uint8_t target_frame = 1;
  
  for (uint8_t i=0; i<target_frame; ++i) {
    RowVectorXd bit_str;
    const uint8_t level = 7;// TODO max it flexible!

    bit_str = spiht_enc(wavelet_img, max_bits, level);
    //cout << "bit_str=\n" << bit_str << endl;
    cout << "bit_str.size()= " << bit_str.size() << endl;
    
    bit_str_all_frames.at(i) = bit_str;
  }
  cout << "bit_str_all_frames.size()= " << bit_str_all_frames.size() << endl;
  
  // Save
  for (uint8_t i=0; i<target_frame; ++i){
    if (!out_dir.empty()) {
      string bit_str_csv = string(out_dir + "bit_str.csv");
      cout << "bit_str_csv= " << bit_str_csv << endl;
      
      RowVectorXd bit_str;
      bit_str = bit_str_all_frames.at(i);
      
      MatrixXd bit_str_mat(1, bit_str.size());
      bit_str_mat.row(0) = bit_str;
      //cout << "bit_str_mat=\n" << bit_str_mat << endl;
      
      CSVIO::write(bit_str_mat, bit_str_csv);
    }
  }
  
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
  Debugger::reset();
  
  //-------------------------initialization------------------------------
  cout << "initialization" << endl;
  
  MatrixXd wavelet_img_tr;
  wavelet_img_tr = wavelet_img.transpose();
  
  double max_val;
  max_val = wavelet_img.rowwise().maxCoeff().maxCoeff();
  
  uint16_t n_max;// threshold to determine whether a wavelet coeff is significant
  n_max = floor( log2(abs(max_val)) );
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
  
  uint64_t bitctr;// TODO is it bit counter?
  bitctr = 24;// TODO why 24?
  
  uint64_t bits_LSP = 0;
  uint64_t bits_LIP = 0;
  uint64_t bits_LIS = 0;
  
  int64_t n;// TODO what is n?
  n = n_max;
  
  // TODO remove me!
  uint64_t if_counter = 0;
  uint64_t else_counter = 0;
  
  const uint64_t coding_while_iter_target = 4;
  uint64_t outer_while_ctr = 0;
  while (bitctr < max_bits) {/////////////// OUTER_WHILE //////////////////////////////////////////////
    ++outer_while_ctr;
    string here_outerwhile = string("outerwhile-" + lexical_cast<string>(outer_while_ctr) + "/");
    cout << "outerwhile: iter= " << outer_while_ctr << ": BEGIN xxxxxxxxxxxxxxxxxxxxxxxx\n";
    
    string here_outerwhile_event_1 = here_outerwhile + "event-1/";
    
    // Sorting pass
    MatrixXd tmp_LIP = LIP;
    int64_t LIP_idx = -1;// TODO what does LIP_IDX point for in LIP?; -1 to synch with the Matlab's implementation
        
    for (uint64_t i=0; i<tmp_LIP.rows(); ++i) {
      //cout << "for (uint2(10)=\n" << bit_str.head(10) << endl;
      
      ++LIP_idx;
      if ((bitctr + 1) >= max_bits) {
        if (bitctr < max_bits) {
          bit_str.conservativeResize( bit_str.size()-1 );
        }
        return bit_str;
      }
      
      double wavelet_img_character;
      wavelet_img_character = abs(wavelet_img(tmp_LIP(i,0),tmp_LIP(i,1)));
      //cout << "wavelet_img_character= " << wavelet_img_character << endl;
      
      if ( wavelet_img_character >= pow(2, n) ) {
        //cout << "if ( wavelet_img_character >= pow(2, n) ): BEGIN\n";
        
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
      }
      else {
        //cout << "else {} of if ( wavelet_img_character >= pow(2, n) ): BEGIN\n";
        
        spiht_enc_helper_4(0, &bit_str, &bit_str_idx, &bitctr, &bits_LIP);
      }
      
      //cout << "for (uint64_t i=0; i<tmp_LIP.rows(); ++i), i= " << i << ": END\n";
    }// for (uint64_t i=0; i<tmp_LIP.rows(); ++i)

    BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_outerwhile_event_1), Debugger::msg.c_str());    
    BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("tmp_LIP", tmp_LIP, here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LSP", LSP, here_outerwhile_event_1), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_outerwhile_event_1), Debugger::msg.c_str());
    
    MatrixXd tmp_LIS = LIS;
    int64_t LIS_idx = -1;// TODO this looks silly (as the matlab implementation), fix it! 
    uint64_t i = 0;// iterator variable for the innerwhile1: while (i < tmp_LIS.rows())
    
    uint64_t inner_while_1_ctr = 0;  
    while (i < tmp_LIS.rows()) {/////// INNER_WHILE_1 ////////////////////////////////////////////////
      ++inner_while_1_ctr;
      string here_innerwhile1 = string(here_outerwhile + "innerwhile1-" + lexical_cast<string>(inner_while_1_ctr) + "/");
      
      cout << "-------------------------------------------------------\n";
      cout << "innerwhile1: iter= " << inner_while_1_ctr << ": BEGIN" << endl;
      
      string here_innerwhile1_event_1 = here_innerwhile1 + "event-1/";
      
      LIS_idx = LIS_idx + 1;
      
      if (tmp_LIS(i,2)==0) {
        //cout << "if (tmp_LIS(i,2)==0): BEGIN\n";
        
        if (bitctr >= max_bits) {
          return bit_str;
        }
        
        double max_d;
        //max_d = get_descendant(tmp_LIS.row(i), wavelet_img);// TODO 
        max_d = get_descendant(1, if_counter);
        ++if_counter;
        
        if (max_d >= pow(2,n)) {
          //cout << "if (max_d >= pow(2,n)): BEGIN\n";
          
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
          
          //cout << "if (max_d >= pow(2,n)): END\n";
        }// if (max_d >= pow(2,n))
        else {
          spiht_enc_helper_4(0, &bit_str, &bit_str_idx, &bitctr, &bits_LIS);
        } 
        
        //cout << "if (tmp_LIS(i,2)==0): END\n";
      }// if (tmp_LIS(i,2)==0) 
      else {
        //cout << "else {} of if (tmp_LIS(i,2)==0): BEGIN\n";
        
        if (bitctr >= max_bits) {
          return bit_str;
        }
        
        double max_d;
        //max_d = get_descendant(tmp_LIS.row(i), wavelet_img);
        max_d = get_descendant(2, else_counter);
        ++else_counter;
        
        if (max_d >= pow(2,n)) {
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
        }
        else {
          spiht_enc_helper_4(0, &bit_str, &bit_str_idx, &bitctr, &bits_LIS);
        }
        //cout << "else {} of if (tmp_LIS(i,2)==0): END\n";
      }// else {} of if (tmp_LIS(i,2)==0
      
      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LSP", LSP, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LIS", LIS, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("tmp_LIS", tmp_LIS, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_innerwhile1_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile1_event_1), Debugger::msg.c_str());
    
      // Increment the iterator idx of this while-loop:  while (i < tmp_LIS.rows() )
      ++i;
    }// while (i < tmp_LIS.rows() ) ... INNERWHILE1
    
    string here_outerwhile_event_2 = here_outerwhile + "event-2/";

    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LSP", LSP, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LIS", LIS, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("tmp_LIS", tmp_LIS, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_outerwhile_event_2), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_outerwhile_event_2), Debugger::msg.c_str());
    
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
      string here_innerwhile2 = string(here_outerwhile + "innerwhile2-" + lexical_cast<string>(inner_while_2_ctr) + "/");
      //cout << "while (  ( LSP_character >= pow(2,n_max+2) ) and ( LSP_idx <= LSP.rows() )  ): BEGIN\n";
      //cout << "iteration_idx= " << inner_while_2_ctr << endl;
      
      string here_innerwhile2_event_1 = here_innerwhile2 + "event-1/";
      
      if (bitctr >= max_bits) {
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
        LSP_character = abs( pow(2,n_max-n+1)*wavelet_img_character );
        LSP_character = floor(LSP_character);
      }

      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_innerwhile2_event_1), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("bits_LSP", EigenLibSupport::scalar2mat(bits_LSP), here_innerwhile2_event_1
      ), Debugger::msg.c_str());
      BOOST_ASSERT_MSG(Debugger::debug_param("LSP_character", EigenLibSupport::scalar2mat(LSP_character), here_innerwhile2_event_1), Debugger::msg.c_str());    
      BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_innerwhile2_event_1), Debugger::msg.c_str());
      
    }// INNERWHILE2
    
    string here_outerwhile_event_3 = here_outerwhile + "event-3/";
    
    // Decrement the n; TODO elaborate the def of n
    --n;

    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str_idx", EigenLibSupport::scalar2mat(bit_str_idx), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bitctr", EigenLibSupport::scalar2mat(bitctr), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bits_LIS", EigenLibSupport::scalar2mat(bits_LIS), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bits_LIP", EigenLibSupport::scalar2mat(bits_LIP), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bits_LSP", EigenLibSupport::scalar2mat(bits_LSP), here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("n", EigenLibSupport::scalar2mat(n), here_outerwhile_event_3), Debugger::msg.c_str());    
    BOOST_ASSERT_MSG(Debugger::debug_param("LIP", LIP, here_outerwhile_event_3), Debugger::msg.c_str());
    BOOST_ASSERT_MSG(Debugger::debug_param("bit_str", bit_str, here_outerwhile_event_3), Debugger::msg.c_str());
    
    //cout << "if_counter= " << if_counter << endl;
    //cout << "else_counter= " << else_counter << endl;
    cout << "outerwhile: iter= " << outer_while_ctr << ": END  xxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
    if (outer_while_ctr==coding_while_iter_target) {
      return bit_str;
    }
  }// while (bitctr < max_bits) ... OUTERWHILE

  //////////////////////////////////////////////////////////////////
  cout << "spiht_enc(): END" << endl;
  return bit_str;
}

void ECGSPIHT::spiht_enc_helper(const Eigen::MatrixXd& wavelet_img, const uint8_t& x, const uint8_t& y, const int64_t& n, Eigen::MatrixXd* LSP, Eigen::MatrixXd* LIP, Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx, uint64_t* bitctr, uint64_t* bits_LIS) {
  using namespace std;
  //cout << "spiht_enc_helper(): BEGIN\n";
  
  //cout << "x= " << (int)x << endl;
  //cout << "y= " << (int)y << endl;
  
  if ( abs(wavelet_img(x, y)) >= pow(2, n) ) {
    //cout << "if ( abs(wavelet_img(x, y)) >= pow(2, n) ): BEGIN\n";
    
    // Assume: LSP is already filled (hence, not empty) up to this point
    LSP->conservativeResize( LSP->rows()+1, LSP->cols() );// increment the size of rows by one
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
    
    //cout << "if ( abs(wavelet_img(x, y)) >= pow(2, n) ): END\n";
  }// if ( abs(wavelet_img(x, y)) >= pow(2, n) ) 
  else {
    //cout << "else {} of if ( abs(wavelet_img(x, y)) >= pow(2, n) ): BEGIN\n";
    
    (*bit_str)(*bit_str_idx) = 0;
    
    ++(*bitctr);
    ++(*bit_str_idx);
    ++(*bits_LIS);
    
    LIP->conservativeResize( LIP->rows()+1, LIP->cols() );
    (*LIP)(LIP->rows()-1,0) = x;
    (*LIP)(LIP->rows()-1,1) = y;
    
    //cout << "else {} of if ( abs(wavelet_img(x, y)) >= pow(2, n) ): END\n";
  }
  
  //cout << "spiht_enc_helper(): END\n";
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
  (*LIS)(LIS->rows()-1, 2) = 0;// TODO why 0?
}

void ECGSPIHT::spiht_enc_helper_4(const uint8_t bit_str_val, Eigen::RowVectorXd* bit_str, uint16_t* bit_str_idx, uint64_t* bitctr, uint64_t* bits_LIS_or_LIP) {
  (*bit_str)(*bit_str_idx) = bit_str_val;
  ++(*bitctr);
  ++(*bit_str_idx);
  if (bits_LIS_or_LIP != 0) {
    ++(*bits_LIS_or_LIP);
  }
}

double ECGSPIHT::get_descendant(const Eigen::RowVectorXd& LIS_row, const Eigen::MatrixXd& matrix) {
  return 0.00;
}

double ECGSPIHT::get_descendant(const uint8_t& type, const uint64_t& ith) {
  //std::cout << "ith= " << ith << std::endl;
  
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
