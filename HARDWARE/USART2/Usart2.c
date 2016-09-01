#include "Usart2.h"
 
uint8_t RxBuffer1[46] = {0};
__IO uint8_t RxfingerCounter = 0x00;
__IO uint8_t TxCounter1 = 0x00;
__IO uint8_t RxCounter1 = 0x00;
extern unsigned char FP_Pack_Head[6];  
extern OS_EVENT *Usart2_Event; 

typedef struct
{
	u8 buff[100];
	u8 head;
	u8 tail;
}Usart2_Buff;
Usart2_Buff usart2_Buff;
void NVIC_Configuration_USART2(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */  
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			     	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					
	NVIC_Init(&NVIC_InitStructure);
}

void Usart2GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         		 //USART2 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         	 	 //USART2 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 
	GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 
	
	NVIC_Configuration_USART2();
}


void USART2_Config(u32 bound){
	USART_InitTypeDef USART_InitStructure;
	Usart2GPIO_init();
	USART_InitStructure.USART_BaudRate = bound;					
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		
	USART_InitStructure.USART_StopBits = USART_StopBits_2;			
	USART_InitStructure.USART_Parity = USART_Parity_No;				
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					

	/* Configure USARTx */
	USART_Init(USART2, &USART_InitStructure);							

	/* Enable USARTx Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);					  

	/* Enable the USARTx */
	USART_Cmd(USART2, ENABLE);	
	
	
}

char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/****************************************************************************
* Ãû    ³Æ£ºvoid USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
* ¹¦    ÄÜ£º¸ñÊ½»¯´®¿ÚÊä³öº¯Êý
* Èë¿Ú²ÎÊý£ºUSARTx:  Ö¸¶¨´®¿Ú
			Data£º   ·¢ËÍÊý×é
			...:     ²»¶¨²ÎÊý
* ³ö¿Ú²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º¸ñÊ½»¯´®¿ÚÊä³öº¯Êý
        	"\r"	»Ø³µ·û	   USART_OUT(USARTx, "abcdefg\r")   
			"\n"	»»ÐÐ·û	   USART_OUT(USARTx, "abcdefg\r\n")
			"%s"	×Ö·û´®	   USART_OUT(USARTx, "×Ö·û´®ÊÇ£º%s","abcdefg")
			"%d"	Ê®½øÖÆ	   USART_OUT(USARTx, "a=%d",10)
* µ÷ÓÃ·½·¨£ºÎÞ 
****************************************************************************/
void USART2_OUT(uint8_t *Data,...){ 

	const char *s;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap, Data);

	while(*Data!='\0'){				                         
		if(*Data==0x5c){									  //'\'
			switch (*++Data){
				case 'r':							         
					USART_SendData(USART2, 0x0d);	   

					Data++;
					break;
				case 'n':							         
					USART_SendData(USART2, 0x0a);	
					Data++;
					break;
				
				default:
					Data++;
				    break;
			} 
		}
		else if(*Data=='%'){									  //
			switch (*++Data){				
				case 's':										  
                	s = va_arg(ap, const char *);
                	for ( ; *s; s++) {
                    	USART_SendData(USART2,*s);
						while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
            	case 'd':										 
                	d = va_arg(ap, int);
                	itoa(d, buf, 10);
                	for (s = buf; *s; s++) {
                    	USART_SendData(USART2,*s);
						while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
				default:
					Data++;
				    break;
			}		 
		}
		else USART_SendData(USART2, *Data++);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
	}
}

void USART2_Send(uint8_t *Data,u8 num)
{
	u8 index;
	for (index = 0;index < num;index ++)
	{
		USART_SendData(USART2, *Data++);	
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
	}
}

//void ClearFingerBuff(void)
//{
//	u16 a;
//	for (a = 0;a < 300;a ++)
//	{
//		Rxfinger[a] = 0;
//	}
//	RxfingerCounter = 0x00;
//}

//void ClearBuff()
//{
//	uint16_t a;
//	for (a = 0;a < 100;a ++)
//	{
//		usart2_Buff.buff[a] = 0x00;
//	}
//	usart2_Buff.head = 0;
//	usart2_Buff.tail = 0;
//}

//void addata(u8 data)
//{
//	usart2_Buff.buff[usart2_Buff.tail] = data;
//	usart2_Buff.tail = (usart2_Buff.tail ++) % 100;
//} 

//u8 Usart2_readata()
//{
//	u8 temp;
//	if (usart2_Buff.head != usart2_Buff.tail)
//	{
//		temp = usart2_Buff.buff[usart2_Buff.head];
//		usart2_Buff.head = (usart2_Buff.head ++) % 100;
//		return temp;
//	}
//	else
//	{
//	¯
//		return 0;
//	}
//	
//	return 0;
//}
/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval : None
  */
void USART2_IRQHandler(void)      
{
	u8 i;
	static u16 sum = 0;
#ifdef OS_TICKS_PER_SEC	 	
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	  
	{	
		RxBuffer1[RxCounter1++] = USART_ReceiveData(USART2);  
		
		if ((RxBuffer1[RxCounter1 -1] != FP_Pack_Head[RxCounter1 -1]) && (RxCounter1 <=6))
		{
			RxCounter1 = 0;
		}
		if (RxCounter1 >= 8)
		{
			if ((RxBuffer1[8] + 9) == RxCounter1)
			{
				for (i = 6;i < (RxCounter1 -1);i ++)
				{
					sum += RxBuffer1[i];
				}
				if (sum == RxBuffer1[RxCounter1 -1])
				{
					RxBuffer1[45] = RxCounter1;
					OSMboxPost(Usart2_Event,RxBuffer1);  
				}
					
				RxCounter1=0;
				sum = 0;
			}						
		}
	}

	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                 
	{ 
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					     
	}	 
#ifdef OS_TICKS_PER_SEC	 	
	OSIntExit();  											 
#endif
}
