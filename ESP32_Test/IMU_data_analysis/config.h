#ifndef _CONFIG_H__
#define _CONFIG_H__

#define _DEBUG_LOG_ 0
#define _CALCULATE_SAMPLING_TIME_ 1

//#define TARGET_M5STICK_C
#define TARGET_M5STACK_GARY

// !!!! define must ahead #include <M5Stack.h>
#ifdef TARGET_M5STACK_GARY
  #define M5STACK_MPU6886 
  #include <M5Stack.h>
#endif

#ifdef TARGET_M5STICK_C
   #include <M5StickC.h>        // Hardware-specific library
#endif


#endif
