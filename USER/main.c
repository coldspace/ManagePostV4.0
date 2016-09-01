#include "includes.h" 		//ucos���
#include "ff.h"				//fatfs�ļ�ϵͳ���
#include "led.h"			//led��ƣ���������ucos�Ƿ���������
#include "delay.h"			//��ʱ����
#include "usart.h"			//����1
#include "Usart2.h"			//����2
#include "finger.h"			//ָ��ģ������
#include "Fgstatus.h"
#include "MYUI.h"

OS_EVENT *Usart1_Event;
OS_EVENT *Usart2_Event;
OS_EVENT *GetUser;
BUTTON_Handle  _ahButton[3];

extern TakePost tp[10];

void TakePostFun(void);

#define START_TASK_PRIO   19  
#define START_STK_SIZE    128
OS_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *pdata);

#define LED0_TASK_PRIO    5
#define LED0_STK_SIZE     64
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_tesk(void *pdata);

#define LED1_TASK_PRIO    6
#define LED1_STK_SIZE     64
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_tesk(void *pdata);

#define Usart1_TASK_PRIO   7
#define Usart1_TASK_SIZE   640
__align(8) OS_STK Usart1_TASK_STK[Usart1_TASK_SIZE];
void Usart1_task(void *pdata);

#define Usart2_TASK_PRIO   14
#define Usart2_TASK_SIZE   2048
__align(8) OS_STK Usart2_TASK_STK[Usart2_TASK_SIZE];
void Usart2_task(void *pdata);

#define UI_TASK_PRIO 15
#define UI_TASK_SIZE 2048
__align(8) OS_STK UI_TASK_STK[UI_TASK_SIZE];
void UI_TASK(void *pdata);

#define Post_TASK_PRIO 16
#define Post_TASK_SIZE 2048
__align(8) OS_STK Post_TASK_STK[Post_TASK_SIZE];
void Post_TASK(void *pdata);

int main(void)
{	
	delay_init();	    	 //��ʱ������ʼ��	  
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(9600);
	USART2_Config(57600);
	BSP_Init();
	GUI_Init(); 
	FBSTA_init();
//	Post_Clear();
//	Person_clear();
	WM_SetBkWindowColor(GUI_BLACK);
	OSInit();
	Usart1_Event = OSMboxCreate((void *)0);
	Usart2_Event = OSMboxCreate((void *)0);
	GetUser = OSMboxCreate((void *)0);
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart();
}

void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	pdata = pdata;
	OSStatInit();
	
	OS_ENTER_CRITICAL();
	OSTaskCreate(led0_tesk,(void*)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);
	OSTaskCreate(led1_tesk,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(Usart1_task,(void*)0,(OS_STK*)&Usart1_TASK_STK[Usart1_TASK_SIZE-1],Usart1_TASK_PRIO);
	OSTaskCreate(Usart2_task,(void*)0,(OS_STK*)&Usart2_TASK_STK[Usart2_TASK_SIZE-1],Usart2_TASK_PRIO);
	OSTaskCreate(UI_TASK,(void*)0,(OS_STK*)&UI_TASK_STK[UI_TASK_SIZE-1],UI_TASK_PRIO);
	OSTaskCreate(Post_TASK,(void*)0,(OS_STK*)&Post_TASK_STK[Post_TASK_SIZE-1],Post_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);
	OS_EXIT_CRITICAL();
}

void led0_tesk(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		LED0 = 0;
		OSTimeDlyHMSM(0,0,1,0);
		LED0 = 1;
		OSTimeDlyHMSM(0,0,1,0);
	}
}

void led1_tesk(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		LED1 = 0;
		OSTimeDlyHMSM(0,0,2,0);
		LED1 = 1;
		OSTimeDlyHMSM(0,0,2,0);
	}
}

void Usart1_task(void *pdata)
{
	INT8U err;
	u8 *Usart1buf;
	u8 com;
	pdata = pdata;
	while(1)
	{
		Usart1buf = OSMboxPend(Usart1_Event,0,&err);
		com = ((Usart1buf[0] - 0x30) * 10) + (Usart1buf[1] - 0x30); //���ַ�ת��Ϊ��ֵ
		OSTaskSuspend(Usart2_TASK_PRIO);
		switch (com)
		{
			case 1:FP_add_new_user(Usart1buf);break;
			case 2:delete_finger(Usart1buf);break;
			
			case 3:FB_AddPost(Usart1buf);break;
			case 4:Post_Change(Usart1buf);break;
			case 5:Post_delete(Usart1buf);break;
			
			/*һ������ȫ��Ϊ����ʹ��*/
			case 6:Post_list(10);break;  //�����г��ʼ��б�
			case 7:Person_list(10);break;//�����г�������Ϣ�б�
			case 8:Person_clear();break; //ɾ������������Ϣ
			case 9:Post_Clear();break;	 //ɾ�������ʼ���Ϣ
			case 10:FINGERPRINT_Cmd_Delete_All_Model();break; //ɾ������ģ��
		//	case 11:GetNewFingerID2(&Usart1buf[0],&Usart1buf[1]);break;//  �õ�����ID     ������Ч
		}
		OSTaskResume(Usart2_TASK_PRIO);
	}
}

void Usart2_task(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		OSTaskSuspend(Usart1_TASK_PRIO);
		Seach_finger();
		OSTaskResume(Usart1_TASK_PRIO);
		OSTimeDlyHMSM(0,0,1,0);
	}
}

void UI_TASK(void *pdata)
{
	pdata = pdata;
	GUI_CURSOR_Show(); 
	while(1)
	{
		MainTask();
		OSTimeDlyHMSM(0,0,0,5);
	}
}

void Post_TASK(void* pdata)
{
	pdata = pdata;
	while(1)
	{
		TakePostFun();
		GUI_TOUCH_Exec();
		OSTimeDlyHMSM(0,0,0,10);
	}
}

void TakePostFun(void)
{
	u16 *UserId;
	u8 err;
	UserId = OSMboxPend(GetUser,1,&err);
	if (err == OS_ERR_TIMEOUT)
		return;
	if (!FB_SeachPost(*UserId))
	{
		return;
	}
	
	OSTaskSuspend(Usart2_TASK_PRIO);
	DisPost(UserId);
}
