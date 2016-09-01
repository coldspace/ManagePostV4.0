#include "includes.h"
#ifdef GLOBALS 
#define EXT
#else
#define EXT extern 
#endif

#define LED_LED1_ON()   GPIO_SetBits(GPIOB, GPIO_Pin_5 );  	       //LED1  ��
#define LED_LED1_OFF()  GPIO_ResetBits(GPIOB, GPIO_Pin_5 ); 	   //LED1  ��

#define TP_CS()  GPIO_ResetBits(GPIOB,GPIO_Pin_7)	  			   //����Ƭѡʹ��
#define TP_DCS() GPIO_SetBits(GPIOB,GPIO_Pin_7)					   //����Ƭѡ��ֹ

#define ETH_CS()  GPIO_ResetBits(GPIOA,GPIO_Pin_4)	  			   //����Ƭѡʹ��
#define ETH_DCS() GPIO_SetBits(GPIOA,GPIO_Pin_4)				   //����Ƭѡ��ֹ

EXT unsigned long fm_ch[50];
EXT unsigned char dat[4096];
EXT volatile unsigned long FM_FREQ;
EXT volatile unsigned long FM_PLL;
EXT volatile unsigned char PLL_HIGH,num;
EXT volatile unsigned char PLL_LOW;
EXT volatile unsigned char rec_f,ch; 
EXT volatile unsigned short keymsg;
EXT unsigned char SST25_buffer[4096],fac_id,dev_id;
/* TEA5767д��ַ */
#define Tea5767_WriteAddress1    0xc0
/* TEA5767����ַ */
#define Tea5767_ReadAddress1     0xc1
EXT OS_EVENT* _itMBOX;
//EXT BUTTON_Handle   _ahButton[3];
