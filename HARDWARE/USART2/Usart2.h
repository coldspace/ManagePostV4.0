#ifndef _USART2_H_
#define _USART2_H_
#include "includes.h"

void USART2_Config(u32 bound);
void USART2_OUT(uint8_t *Data,...);
void USART2_Send(uint8_t *Data,u8 num);
void ClearFingerBuff(void);

#endif
