#include <M5StickC.h>        // Hardware-specific library
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
