#include "finger.h"

extern OS_EVENT *Usart2_Event;
extern OS_EVENT *GetUser;
//extern uint8_t Rxfinger[300];
volatile unsigned char UART1_FINGERPRINT_RECEVICE_BUFFER[24];

//FINGERPRINT通信协议定义
unsigned char FP_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
unsigned char FP_Get_Img[6] = {0x01,0x00,0x03,0x01,0x0,0x05};    //获得指纹图像
unsigned char FP_GR_Match[6] = {0x01,0x00,0x03,0x03,0x00,0x07};  //核对两个buff区中的模板
unsigned char FP_Templete_Num[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数

unsigned char FP_Search[11]={0x01,0x0,0x08,0x1b,0x02,0x0,0x0,0x03,0xA1,0x0,0xca}; //搜索指纹搜索范围0 - 929
//unsigned char FP_Search[11]={0x01,0x0,0x08,0x04,0x01,0x0,0x0,0x03,0xA1,0x0,0xB2}; //搜索指纹搜索范围0 - 929
unsigned char FP_Img_To_Buffer1[7]={0x01,0x0,0x04,0x02,0x01,0x0,0x08}; //将图像放入到BUFFER1
unsigned char FP_Img_To_Buffer2[7]={0x01,0x0,0x04,0x02,0x02,0x0,0x09}; //将图像放入到BUFFER2
unsigned char FP_Reg_Model[6]={0x01,0x0,0x03,0x05,0x0,0x09}; //将BUFFER1跟BUFFER2合成特征模版
unsigned char FP_Delet_All_Model[6]={0x01,0x0,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版
unsigned char FP_GR_UpChar1[7] = {0x01,0x00,0x04,0x08,0x01,0x0,0x0e};  //上传特征值到上位机
unsigned char FP_GR_LoadChar[9] = {0x01,0x00,0x06,0x07,0x01,0x00,0x01,0x01,0x00}; //将Flash中的模板导入到buffer1中
volatile unsigned char  FP_Save_Finger[9]={0x01,0x00,0x06,0x06,0x02,0x00,0x01,0x00,0x0f};//将BUFFER1中的特征码存放到指定的位置
volatile unsigned char FP_Delete_Model[10]={0x01,0x00,0x07,0x0C,0x0,0x0,0x0,0x1,0x0,0x0}; //删除指定的模版

//volatile unsigned char FINGER_NUM;
/*-------------------功能函数------------------------------------*/

void NumberChangeToString(u8 Hchar,u8 Lchar,u8 arr[])
{
	u16 temp;
	temp = Hchar*256 + Lchar;
	arr[0] = temp/100 + 0x30;
	arr[1] = temp%100/10 + 0x30;
	arr[2] = temp%10 + 0x30;
	arr[3] = '\0';
}

void FINGERPRINT_Cmd_Load_Char(u8 Hchar,u8 Lchar)
{
	unsigned int i;
	u16 temp = 0;
	
	FP_GR_LoadChar[5] = Hchar;
	FP_GR_LoadChar[6] = Lchar;
	
	for (i = 0;i < 7;i ++)
	{
		temp +=  FP_GR_LoadChar[i];
	}
	FP_GR_LoadChar[7] = temp/256;
	FP_GR_LoadChar[8] = temp%256;

	for(i=0;i<6;i++) //发送包头
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);

	for(i=0;i<9;i++) //发送命令 0x1d
		FINGERPRINT_Usart_Send(USART2,FP_GR_LoadChar[i]);
}

void GetNewFingerID2(u8 *Hchar,u8 *Lchar)
{
	
	u16 temp = 0;
	uint8_t *TxBuffer1;
	u8 err;

	while(1)
	{
		FINGERPRINT_Cmd_Load_Char(temp/256,temp%256);
		TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
		if (TxBuffer1[9] == 0x0c)
		{
			//printf("no using id is:%d \r\n",temp);
			break;
		}
		temp ++;
	}
	*Hchar = temp / 256;
	*Lchar = temp % 256;
}

/*------------------ FINGERPRINT命令字 --------------------------*/

void FINGERPRINT_Usart_Send(USART_TypeDef *Usartx,u8 chr)
{
	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();
	USART_SendData(Usartx,chr);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);

	OS_EXIT_CRITICAL();
}

//FINGERPRINT_获得指纹图像命令
void FINGERPRINT_Cmd_Get_Img(void)
{
	unsigned char i;

	for(i=0;i<6;i++) //发送包头
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);

	for(i=0;i<6;i++) //发送命令 0x1d
		FINGERPRINT_Usart_Send(USART2,FP_Get_Img[i]);
}

//讲图像转换成特征码存放在Buffer1中
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
	unsigned char i;

	for(i=0;i<6;i++)    //发送包头
	{
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);   
	}

	for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
	{
		FINGERPRINT_Usart_Send(USART2,FP_Img_To_Buffer1[i]);
	}
}

//将图像转换成特征码存放在Buffer2中
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
	unsigned char i;
	for(i=0;i<6;i++)    //发送包头
	{
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);   
	}

	for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
	{
		FINGERPRINT_Usart_Send(USART2,FP_Img_To_Buffer2[i]);
	}
}

void FINGERPRINT_Cmd_GR_Match(void)
{
	unsigned char i;
	for(i=0;i<6;i++)    //发送包头
	{
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]); 
	}

	for(i=0;i<6;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
	{
		FINGERPRINT_Usart_Send(USART2,FP_GR_Match[i]);
	}
}
//将BUFFER1 跟 BUFFER2 中的特征码合并成指纹模版
void FINGERPRINT_Cmd_Reg_Model(void)
{
	unsigned char i;    

	for(i=0;i<6;i++) //包头
	{
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);   
	}

	for(i=0;i<6;i++) //命令合并指纹模版
	{
		FINGERPRINT_Usart_Send(USART2,FP_Reg_Model[i]);   
	}

}

//删除指纹模块里的所有指纹模版
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
	unsigned char i;    

	for(i=0;i<6;i++) //包头
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);   

	for(i=0;i<6;i++) //命令合并指纹模版
		FINGERPRINT_Usart_Send(USART2,FP_Delet_All_Model[i]);   
}

//删除指纹模块里的指定指纹模版
void FINGERPRINT_Cmd_Delete_Model(unsigned int uiID_temp)
{
	volatile unsigned int uiSum_temp = 0;
	unsigned char i;    

	FP_Delete_Model[4]=(uiID_temp&0xFF00)>>8;
	FP_Delete_Model[5]=(uiID_temp&0x00FF);

	for(i=0;i<8;i++)
		uiSum_temp = uiSum_temp + FP_Delete_Model[i];

	//UART0_Send_Byte(uiSum_temp);	

	FP_Delete_Model[8]=(uiSum_temp&0xFF00)>>8;
	FP_Delete_Model[9]=uiSum_temp&0x00FF;


	for(i=0;i<6;i++) //包头
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);   

	for(i=0;i<10;i++) //命令合并指纹模版
		FINGERPRINT_Usart_Send(USART2,FP_Delete_Model[i]);   
}

//获得指纹模板数量
void FINGERPRINT_Cmd_Get_Templete_Num(void)
{  
	unsigned int i;

	for(i=0;i<6;i++) //包头
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);

	//发送命令 0x1d
	for(i=0;i<6;i++)
		FINGERPRINT_Usart_Send(USART2,FP_Templete_Num[i]);
}

//搜索全部用户999枚
void FINGERPRINT_Cmd_Search_Finger(void)
{
	unsigned char i;	   
	for(i=0;i<6;i++)   //发送命令搜索指纹库
	{
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);   
	}

	for(i=0;i<11;i++)
	{
		FINGERPRINT_Usart_Send(USART2,FP_Search[i]);   
	}
}

void FP_GR_UpChar()
{
	unsigned char i;	   
	for(i=0;i<6;i++)   //发送命令搜索指纹库
	{
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);   
	}

	for(i=0;i<7;i++)
	{
		FINGERPRINT_Usart_Send(USART2,FP_GR_UpChar1[i]);   
	}
}

void FINGERPRINT_Cmd_Save_Finger( unsigned char ucH_Char,unsigned char ucL_Char )
{
	unsigned long temp = 0;
	unsigned char i;

	//SAVE_FINGER[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置

	FP_Save_Finger[5] = ucH_Char;
	FP_Save_Finger[6] = ucL_Char;

	for(i=0;i<7;i++)   //计算校验和
		temp = temp + FP_Save_Finger[i];

	FP_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
	FP_Save_Finger[8]= temp & 0x0000FF;

	for(i=0;i<6;i++)    
		FINGERPRINT_Usart_Send(USART2,FP_Pack_Head[i]);        //发送包头

	for(i=0;i<9;i++)  
		FINGERPRINT_Usart_Send(USART2,FP_Save_Finger[i]);      //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
}

/*添加新的用户*/
unsigned char FP_add_new_user(u8 *Usart1buf)
{
	OS_CPU_SR cpu_sr = 0;
	u8 ucH_user,ucL_user;
	uint8_t *TxBuffer1;
	u8 NameIndex = 0;
	INT8U err;
	u8 UserName[20] = {0};
	u8 arr[4];
	Usart1buf++;
	Usart1buf++;	//这个时候的指针指向#
	Usart1buf++;  
	while(*Usart1buf != '#')
	{
		UserName[NameIndex] = *Usart1buf;
		NameIndex ++;
		Usart1buf ++;
		if (NameIndex >= 19)
			break;
	}
	UserName[19] = '\0';   //最后一位要是结束符
	
	do
	{
		FINGERPRINT_Cmd_Get_Img();
		TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
		OSTimeDlyHMSM(0,0,0,500);
		Printf_error(*(TxBuffer1 + 9));
	}while (*(TxBuffer1 + 9) != 0x00);
	
	FINGERPRINT_Cmd_Img_To_Buffer1();
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
	Printf_error(*(TxBuffer1 + 9));
	OSTimeDlyHMSM(0,0,1,0);
	
	OS_ENTER_CRITICAL();
	printf ("再次按下指纹\r\n");
	OS_EXIT_CRITICAL();
	
	do
	{
		FINGERPRINT_Cmd_Get_Img();
		TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
		OSTimeDlyHMSM(0,0,0,500);
		Printf_error(*(TxBuffer1 + 9));
	}while (*(TxBuffer1 + 9) != 0x00);
	
	FINGERPRINT_Cmd_Img_To_Buffer2();
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
//	Printf_error(*(TxBuffer1 + 9));
	
	FINGERPRINT_Cmd_GR_Match();
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
//	Printf_error(*(TxBuffer1 + 9));
	if ((*(TxBuffer1 + 9)) != 0x00)
	{
		return 0;
	}
	
	FINGERPRINT_Cmd_Reg_Model();//转换成特征码
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
//	Printf_error(*(TxBuffer1 + 9));
	
	GetNewFingerID2(&ucH_user,&ucL_user);
	
	FINGERPRINT_Cmd_Save_Finger(ucH_user,ucL_user);                		         
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
//	Printf_error(*(TxBuffer1 + 9));
	if ((*(TxBuffer1 + 9)) == 0x00)
	{
//		SetFingerIDStatus(ucH_user,ucL_user);
		NumberChangeToString(ucH_user,ucL_user,arr);
		Person_Add((ucH_user<<8)+ucL_user,UserName);
		OS_ENTER_CRITICAL();
		printf ("01#%s#%s \r\n",arr,UserName);
		OS_EXIT_CRITICAL();
	}
	OSTimeDlyHMSM(0,0,0,500);
	return 1;	
}

int delete_finger(u8 *Usart1buf)
{
	char temp[4];
	u16 id;
	uint8_t *TxBuffer1;
	INT8U err;
	OS_CPU_SR cpu_sr = 0;
	
	temp[0] = Usart1buf[3];
	temp[1] = Usart1buf[4];
	temp[2] = Usart1buf[5];
	temp[3] = '\0';
	id = atol(temp);
	
	FINGERPRINT_Cmd_Delete_Model(id);
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
	
	if ((*(TxBuffer1 + 9)) == 0x00)
	{
//		SetFingerIDStatus(ucH_user,ucL_user);
		Person_delete(id);
		OS_ENTER_CRITICAL();
		printf("02#%s#",temp);
		OS_EXIT_CRITICAL();
	}
	else
		return 0;
	return 1;
}

unsigned int pageid;
unsigned char Seach_finger(void)
{
	OS_CPU_SR cpu_sr = 0;
	uint8_t *TxBuffer1;
	INT8U err;
	char Pername[20];
	unsigned int MatchScore;

	FINGERPRINT_Cmd_Get_Img();
	TxBuffer1 = OSMboxPend(Usart2_Event,100,&err);
//	Printf_error(*(TxBuffer1 + 9));
	
	if ((*(TxBuffer1 + 9) != 0x00))
		return 0;
	
	FINGERPRINT_Cmd_Img_To_Buffer2();
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
//	Printf_error(*(TxBuffer1 + 9));
	
	FINGERPRINT_Cmd_Search_Finger();
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
	Printf_error(*(TxBuffer1 + 9));
	
	if ((*(TxBuffer1 + 9)) == 0x00)
	{
		pageid = ((*(TxBuffer1 + 10)) << 8) + (*(TxBuffer1 + 11));
		MatchScore = ((*(TxBuffer1 + 12)) << 8) + (*(TxBuffer1 + 13));
		Person_Search(pageid,Pername);
		OS_ENTER_CRITICAL();
		
		printf ("finger id is %d \r\n",pageid);
		printf ("finger MatchScore is %d \r\n",MatchScore);
		printf ("finger name is :%s\r\n",Pername);
		OS_EXIT_CRITICAL();
		
		OSMboxPost(GetUser,&pageid);   //获取用户成功
	}
	else
		return 0;
	
	return 1;
}

int CompleteFinger(u16 userid)
{
//	u8 ucH_user,ucL_user;
	uint8_t *TxBuffer1;
	INT8U err;
	u16 temp;
	do
	{
		FINGERPRINT_Cmd_Get_Img();
		TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
		OSTimeDlyHMSM(0,0,0,500);
//		Printf_error(*(TxBuffer1 + 9));
	}while (*(TxBuffer1 + 9) != 0x00);
	
	if ((*(TxBuffer1 + 9) != 0x00))
		return 0;
	
	FINGERPRINT_Cmd_Img_To_Buffer2();
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
	
	FINGERPRINT_Cmd_Search_Finger();
	TxBuffer1 = OSMboxPend(Usart2_Event,0,&err);
//	Printf_error(*(TxBuffer1 + 9));
	
	if ((*(TxBuffer1 + 9)) == 0x00)
	{
		temp = ((*(TxBuffer1 + 10)) << 8) + (*(TxBuffer1 + 11));
		if (temp == userid)
			return 1;
	}
	
	return 0;
}

void Printf_error(u8 data)
{
	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();
	switch(data)
	{
		case 0x00: printf("OK\r\n");break;
		case 0x01: printf("package error\r\n");break;
		case 0x02: printf("no finger\r\n");break;
		case 0x03: printf("save finger error\r\n");break;
		case 0x04: printf("finger is too dry\r\n");break;
		case 0x05: printf("finger is too wet\r\n");break;
		case 0x06: printf("finger is too disorder\r\n");break;
		case 0x07: printf("finger's area is too small\r\n");break;
		case 0x08: printf("Does not match the fingerprint\r\n");break;
		case 0x09: printf("No search the fingerprint\r\n");break;
		case 0x0a: printf("Synthesis of eigenvalues error\r\n");break;
		case 0x0b: printf("Out of range\r\n");break;
		case 0x0c: printf("read fingerprint maps database error\r\n");break;
		case 0x0d: printf("error1\r\n");break;
		case 0x0e: printf("error2\r\n");break;
		default:printf("ErrorAll\r\n");break;
	}
	OS_EXIT_CRITICAL();
}

