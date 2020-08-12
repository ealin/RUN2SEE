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

  
  // return 0 if open file in SD-card error
  int start_collection(void) ;

  void stop_collection(void) ;
  void sample_and_save_one_record(void) ;
  

#endif

#endif
