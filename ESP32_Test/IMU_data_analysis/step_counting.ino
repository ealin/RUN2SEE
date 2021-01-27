

#include "config.h"
#include "step_counting.h"



float Filter(char DIR)   // DIR may be 'x','y','z' 
{
    float accX, accY, accZ;

    // 平均滤波法
    float filterSum = 0.0;
    
    for(int i = 0; i < FILTER_N; i++) 
    {
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        if(DIR == 'x')
          filterSum += accX ;
        else if(DIR == 'y')
          filterSum += accY ;
        else
          filterSum += accZ ;

        delay(5);
    }
    
    return (float)(filterSum / FILTER_N);
}


void Filter_3aix(float * filterSum_X, float * filterSum_Y, float * filterSum_Z)  
{
    float accX, accY, accZ;

    // 平均滤波法
    
    for(int i = 0; i < FILTER_N_3AIX; i++) 
    {
        M5.IMU.getAccelData(&accX, &accY, &accZ);

        *filterSum_X = *filterSum_X + accX ;
        *filterSum_Y = *filterSum_Y + accY ;
        *filterSum_Z = *filterSum_Z + accZ ;

        delay(5);
    }

    *filterSum_X = *filterSum_X * 10 / FILTER_N_3AIX ;
    *filterSum_Y = *filterSum_Y * 10 / FILTER_N_3AIX ;
    *filterSum_Z = *filterSum_Z * 10 / FILTER_N_3AIX ;
    
}


static int stepNumber = 0;

void reset_step_number(void)
{
 stepNumber = 0 ; 
}

int get_step_number(void)
{
 return stepNumber ; 
}

// return 0 means no new step, else return step number
int sampling_new_step(void)
{
//~~~~~~~~~~~STEP COUNTER - BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   static float oldAccx, oldAccy, oldAccz, oldDot;
   int new_step_flasg = 0 ;

#ifdef SAMPLING_METHOD_1BY1
   float accx = Filter('x') ;  // 读取滤波后的加速度 x 分量
   float accy = Filter('y') ;
   float accz = Filter('z') ;
#else
   float accx ; // 读取滤波后的加速度 x 分量
   float accy ;
   float accz ;
   Filter_3aix(&accx, &accy, &accz) ;
#endif
   
   float dot = (oldAccx * accx)+(oldAccy * accy)+(oldAccz * accz);

   float oldAcc = abs(sqrt(oldAccx * oldAccx + oldAccy * oldAccy + oldAccz * oldAccz));
   float newAcc = abs(sqrt(accx * accx + accy * accy + accz * accz));
   dot /= (oldAcc * newAcc);  // 计算加速度变化程度

   #if _DEBUG_LOG_ == 1
     Serial.printf("dot:%5.3f\noldDot:%5.3f\n", dot, oldDot);
   #endif

   if(abs(dot - oldDot) >= STEP_THD) 
   {
        // 变化程度超过阈值，则判定步数增加; 并打印
        stepNumber += 1;
        new_step_flasg = stepNumber ;
        
   }
    
   oldAccx = accx;
   oldAccy = accy;
   oldAccz = accz;
   oldDot = dot;
//~~~~~~~~~~~STEP COUNTER - END~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  return new_step_flasg ;
}
