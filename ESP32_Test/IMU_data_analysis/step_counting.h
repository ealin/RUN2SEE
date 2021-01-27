#ifndef STEP_COUNTING_H__
#define STEP_COUNTING_H__

#define SAMPLING_METHOD_1BY1
#define STEP_THD 0.05

#define FILTER_N 12
float Filter(char DIR) ;

#define FILTER_N_3AIX 36
void Filter_3aix(float * filterSum_X, float * filterSum_Y, float * filterSum_Z)  ;


void reset_step_number(void) ;
int get_step_number(void) ;

// return 0 means no new step, else return step number
int sampling_new_step(void) ;


#ifdef TARGET_M5STACK_GARY
  //=======data collection functions====
  //  

  // return 0 if open file in SD-card error
  int start_collection(void) ;

  void stop_collection(void) ;
  void sample_and_save_one_record(void) ;
  
  //======= M5Stack-core Buzzer frequency definition  ====
  //  
  #define NOTE_D0 -1
  #define NOTE_D1 294
  #define NOTE_D2 330
  #define NOTE_D3 350
  #define NOTE_D4 393
  #define NOTE_D5 441
  #define NOTE_D6 495
  #define NOTE_D7 556

  #define NOTE_DL1 147
  #define NOTE_DL2 165
  #define NOTE_DL3 175
  #define NOTE_DL4 196
  #define NOTE_DL5 221
  #define NOTE_DL6 248
  #define NOTE_DL7 278

  #define NOTE_DH1 589
  #define NOTE_DH2 661
  #define NOTE_DH3 700
  #define NOTE_DH4 786
  #define NOTE_DH5 882
  #define NOTE_DH6 990
  #define NOTE_DH7 112

#endif

#endif
