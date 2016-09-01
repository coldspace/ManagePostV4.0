#if !defined( _BSP_H )
#define _BSP_H
#include "includes.h"
//#include "cpu.h"
typedef  unsigned  long      CPU_INT32U;  
void RCC_Configuration(void);

void GPIO_Configuration(void);

void NVIC_Configuration1(void);

CPU_INT32U  BSP_CPU_ClkFreq (void);

//INT32U  OS_CPU_SysTickClkFreq (void);

void  OS_CPU_SysTickInit(void);

void BSP_Init(void);


#ifdef  DEBUG
void assert_failed(u8* file, u32 line)
#endif



#endif
