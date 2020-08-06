#ifndef STEP_COUNTING_H__
#define STEP_COUNTING_H__

#define SAMPLING_METHOD_1BY1
#define STEP_THD 0.05

#define FILTER_N 12
float Filter(char DIR) ;

#define FILTER_N_3AIX 36
void Filter_3aix(float * filterSum_X, float * filterSum_Y, float * filterSum_Z)  ;


#endif
