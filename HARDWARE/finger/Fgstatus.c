#include "Fgstatus.h"

TakePost tp[10];

FATFS fs;
FIL fsrc;
FILINFO tfileinfo;
DIR tdir;
unsigned int br;
char path[16]={"PINF.txt"}; 
char path1[16]={"post.txt"};


int FBSTA_init(void)
{
	SD_Init();
	if (f_mount(0,&fs) != FR_OK)
	{return 0;}
	return 1;
}
int GetFreePostId(u8 *id)
{
	Post test;
	long p = 0;
	if (f_open(&fsrc,path1,FA_READ) != FR_OK)
		return 0;
	while(1)
	{
		f_lseek(&fsrc,(DWORD)(sizeof(Post)*p));
		if (f_read(&fsrc,&test,sizeof(Post),&br) != FR_OK)
		{
			f_close(&fsrc);
			return 0;
		}
		if (test.state != 1)
		{
			*id = p;
			break;
		}
		p ++;
//		printf("now p is : %d",test.state);
	}
	f_close(&fsrc);
	return 1;
}


void ChangeUsartToPost(u8 *Usart1buf,Post *p)
{
	char temp[4];
	u8 index = 0;
	
	temp[0] = Usart1buf[3];
	temp[1] = Usart1buf[4];
	temp[2] = Usart1buf[5];
	p->Admin = atol(temp);
	
	temp[0] = Usart1buf[7];
	temp[1] = Usart1buf[8];
	temp[2] = Usart1buf[9];
	p->friends[0] = atol(temp);
	
	temp[0] = Usart1buf[11];
	temp[1] = Usart1buf[12];
	temp[2] = Usart1buf[13];
	p->friends[1] = atol(temp);
	
	temp[0] = Usart1buf[15];
	temp[1] = Usart1buf[16];
	temp[2] = Usart1buf[17];
	p->friends[2] = atol(temp);
	
	Usart1buf += 19;
	while(*Usart1buf != '#')
	{
		p->Things[index] = *Usart1buf;
		index++;
		Usart1buf ++;
	}
	p->Things[index] = '\0';
	p->state = 1;
}

int Post_Clear(void)
{
	Post test;
	int index = 0;
	if (f_open(&fsrc,path1,FA_WRITE) != FR_OK)
		return 0;
	test.state = 0;
	for (index = 0;index < 100;index ++)
	{
		if (f_write(&fsrc,&test,sizeof(Post),&br) != FR_OK)
		{
			f_close(&fsrc);
			return 0;
		}	
	}
//	printf("index num is :%d \r\n",index);
	f_close(&fsrc);
	return 1;
}

int Post_list(u8 num)
{
	Post test;
	u8 index;
	if (f_open(&fsrc,path1,FA_OPEN_ALWAYS|FA_READ) != FR_OK)
		return 0;
	for (index = 0;index < num;index ++)
	{
		if (f_read(&fsrc,&test,sizeof(Post),&br) != FR_OK)
		{
			f_close(&fsrc);
			return 0;
		}
		
		if (test.state == 1)
		{
			printf("\r\nadmin:%d \r\n",test.Admin);
			printf("friend1:0x%X\r\n",test.friends[0]);
			printf("friend2:0x%X\r\n",test.friends[1]);
			printf("friend3:0x%X\r\n",test.friends[2]);
			printf("things:%s\r\n",test.Things);
			printf("status:%d\r\n",test.state);
			printf("PostId:%d\r\n",test.id);
		}
	}
	f_close(&fsrc);
	return 1;
}

//串口删除函数
int Post_delete(u8 *Usart1buf)
{
	Post test;
	char temp[4];
	u16 id;
	
	temp[0] = Usart1buf[3];
	temp[1] = Usart1buf[4];
	temp[2] = Usart1buf[5];
	temp[3] = '\0';
	id = atol(temp);
	
	if (f_open(&fsrc,path1,FA_WRITE) != FR_OK)
		return 0;
	test.state = 0;
	f_lseek(&fsrc,sizeof(Post) * id);
	if (f_write(&fsrc,&test,sizeof(Post),&br) != FR_OK)
	{
		f_close(&fsrc);
		return 0;
	}	
	f_close(&fsrc);
	temp[0] = id/100 + 0x30;
	temp[1] = id%100/10 + 0x30;
	temp[2] = id%100%10 + 0x30;
	temp[3] = '\0';
	printf("05#%s#\r\n",temp);
	return 1;
}

//单片机删除函数
int Post_delete1(u16 id,u16 user)
{
	Post test;
	char temp[4];
	char temp2[4];
	
	if (f_open(&fsrc,path1,FA_WRITE) != FR_OK)
		return 0;
	test.state = 0;
	f_lseek(&fsrc,sizeof(Post) * id);
	if (f_write(&fsrc,&test,sizeof(Post),&br) != FR_OK)
	{
		f_close(&fsrc);
		return 0;
	}	
	f_close(&fsrc);
	
	temp[0] = id/100 + 0x30;
	temp[1] = id%100/10 + 0x30;
	temp[2] = id%100%10 + 0x30;
	temp[3] = '\0';
	
	temp2[0] = user/100 + 0x30;
	temp2[1] = user%100/10 + 0x30;
	temp2[2] = user%100%10 + 0x30;
	temp2[3] = '\0';
	printf("06#%s#%s#\r\n",temp,temp2);
	
	return 1;
}

int Post_Change(u8 *Usart1buf)
{
	char temp[4];
	Post p;
	u8 id;
	
	ChangeUsartToPost(Usart1buf,&p);
	
	Usart1buf += 19;
	while(*Usart1buf != '#')
		Usart1buf ++;
	Usart1buf ++;
	temp[0] = *Usart1buf;
	temp[1] = *(Usart1buf+1);
	temp[2] = *(Usart1buf+2);
	temp[3] = '\0';
	id = atol(temp);
	//printf("id:%d  temp:%s\r\n",id,temp);
	p.id = id;
	
	if (f_open(&fsrc,path1,FA_WRITE) != FR_OK)
		return 0;
	f_lseek(&fsrc,sizeof(Post) * id);
	if (f_write(&fsrc,&p,sizeof(Post),&br) != FR_OK)
	{
		f_close(&fsrc);
		return 0;
	}	
	f_close(&fsrc);
	temp[0] = id/100 + 0x30;
	temp[1] = id%100/10 + 0x30;
	temp[2] = id%100%10 + 0x30;
	temp[3] = '\0';
	printf("04#%s#\r\n",temp);
	return 1;
}

int FB_AddPost(u8 *Usart1buf)
{
	Post test;
	char temp[4];
	OS_CPU_SR cpu_sr = 0;
	ChangeUsartToPost(Usart1buf,&test);
	GetFreePostId(&test.id);
	
//	OS_ENTER_CRITICAL();
//	printf("admin is:%d \r\n",test.Admin);
//	printf("admin is:%d \r\n",test.friends[0]);
//	printf("admin is:%d \r\n",test.friends[1]);
//	printf("admin is:%d \r\n",test.friends[2]);
//	printf("admin is:%d \r\n",test.id);
//	printf("admin is:%d \r\n",test.state);
//	printf("admin is:%s \r\n",test.Things);
//	OS_EXIT_CRITICAL();
	
	OS_ENTER_CRITICAL();
	if (f_open(&fsrc,path1,FA_WRITE) != FR_OK)
		return 0;
	f_lseek(&fsrc,sizeof(Post)*test.id);
	if (f_write(&fsrc,&test,sizeof(Post),&br) != FR_OK)
	{
		f_close(&fsrc);
		return 0;
	}
	f_close(&fsrc);
	
	temp[0] = test.id/100 + 0x30;
	temp[1] = test.id%100/10 + 0x30;
	temp[2] = test.id%100%10 + 0x30;
	temp[3] = '\0';
	printf("03#%s#\r\n",temp);
	OS_EXIT_CRITICAL();
	return 1;
}

int FB_SeachPost(u16 user)
{
	Post test;
	u8 index;
	u8 i = 0;
	for (index = 0;index < 10;index ++)
		tp[index].takestate = 0;
	if (f_open(&fsrc,path1,FA_READ) != FR_OK)
		return 0;
	for (index = 0;index < 100;index ++)
	{
		if (f_read(&fsrc,&test,sizeof(Post),&br) != FR_OK)
		{
			f_close(&fsrc);
			return 0;
		}
		if (test.state)
		{
			if ((test.Admin == user) || \
				(test.friends[0] == user) || \
			(test.friends[1] == user) || \
			(test.friends[2] == user))
			{
				tp[i].post = test;
				tp[i].takestate = 1;
				i ++;
			}
		}
	}	
	f_close(&fsrc);
	
	for (index = 0;index < 10;index ++)
	{
		if (tp[index].takestate)
		{
			Person_Search(tp[index].post.Admin,tp[index].PInfAdmin.name);
			Person_Search(user,tp[index].PInfUser.name);
		}
	}
	
	if (!tp[0].takestate)
	{
		return 0;
	}
	return 1;
}

/*
*
*人物信息更新  测试成功
*
*/
int Person_clear(void)
{
	PersonInf test;
	int index = 0;
	if (f_open(&fsrc,path,FA_WRITE) != FR_OK)
		return 0;
	test.state = 0;
//	strcpy(test.name,"zhenwen");   // 明天注释这里
	for (index = 0;index < 1000;index ++)
	{
		if (f_write(&fsrc,&test,sizeof(PersonInf),&br) != FR_OK)
		{
			f_close(&fsrc);
			return 0;
		}	
	}
	f_close(&fsrc);
	return 1;
}

/*
*
*人物信息更新  测试成功
*
*/
int Person_Search(u16 id,char *p)
{
	PersonInf test;
	p = p;
	if (f_open(&fsrc,path,FA_OPEN_ALWAYS|FA_READ) != FR_OK)
		return 0;
	f_lseek(&fsrc,sizeof(PersonInf) * id);
	if (f_read(&fsrc,&test,sizeof(PersonInf),&br) != FR_OK)
	{
		f_close(&fsrc);
		return 0;
	}
//	printf("\r\nperson's id is:%d\r\n",test.id);
//	printf("person's state is:%d\r\n",test.state);
//	printf("person's name is :%s\r\n",test.name);
	if (!test.state)
	{
		f_close(&fsrc);
		return 0;
	}
	
	f_close(&fsrc);
	if (test.id == id)
	{
		strcpy(p,test.name);
	}
	else
	{
		return 0;
	}
	return 1;
}

/*
*
*人物信息更新  测试成功
*
*/
int Person_Add(u16 id,u8 *p)
{
	PersonInf test;

	if (f_open(&fsrc,path,FA_WRITE) != FR_OK)
		return 0;

	test.state = 1;
	strcpy(test.name,p);
	test.id = id;
	f_lseek(&fsrc,sizeof(PersonInf) * id);
	if (f_write(&fsrc,&test,sizeof(PersonInf),&br) != FR_OK)
	{
		f_close(&fsrc);
		return 0;
	}
	f_close(&fsrc);
	return 1;
}

int Person_delete(u16 id)
{
	PersonInf test;
	int index = 0;
	if (f_open(&fsrc,path,FA_WRITE) != FR_OK)
		return 0;
	test.state = 0;
	f_lseek(&fsrc,sizeof(PersonInf) * id);
	if (f_write(&fsrc,&test,sizeof(PersonInf),&br) != FR_OK)
	{
		f_close(&fsrc);
		return 0;
	}
	f_close(&fsrc);
	return 1;
}

int Person_list(u16 num)
{
	PersonInf test;
	u8 index;
	if (f_open(&fsrc,path,FA_READ) != FR_OK)
		return 0;
	for (index = 0;index < num;index ++)
	{
		if (f_read(&fsrc,&test,sizeof(PersonInf),&br) != FR_OK)
		{
			f_close(&fsrc);
			return 0;
		}
		
		if (test.state)
		{
			printf("\r\nperson's id is:%d\r\n",test.id);
			printf("person's name is :%s\r\n",test.name);
		}
	}
	f_close(&fsrc);
	return 1;
}

//int GetNewFingerID(u8 *Hchar,u8 *Lchar)
//{
//	u8 index,indey,num;
//	u16 temp;
//	u16 buffer;
//	OS_CPU_SR cpu_sr = 0;
//	OS_ENTER_CRITICAL();
//	if (f_open(&fsrc,path,FA_OPEN_ALWAYS|FA_READ) != FR_OK)
//	{
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//	
//	for (index = 0;index < 4;index ++)
//	{
//		for (indey = 0;indey < 16;indey ++)
//		{
//			if (f_read(&fsrc,&buffer,2,&br) != FR_OK)
//			{
//				f_close(&fsrc);
//				OS_EXIT_CRITICAL();
//				return 0;
//			}
//			if (buffer != 0xffff)
//			{
//				temp = buffer;
//				for (num = 0;num < 16;num ++)
//				{
//					if (!(temp & 0x01))
//					{
//						*Hchar = index;
//						*Lchar = indey * 16 + num;
//						f_close(&fsrc);
//						OS_EXIT_CRITICAL();
//						return 1;
//					}			
//					temp >>= 1;
//				}
//			}
//		}
//	}
//	f_close(&fsrc);
//	OS_EXIT_CRITICAL();
//	return 0;
//}

//int ClearFingerIDStatus(u8 Hchar,u8 Lchar)
//{
//	u16 buffer;
//	OS_CPU_SR cpu_sr = 0;
//	OS_ENTER_CRITICAL();
//	if (f_open(&fsrc,path,FA_OPEN_ALWAYS|FA_READ) != FR_OK)
//	{
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//	f_lseek(&fsrc,(Hchar*16 + (Lchar / 16)) * 2);
//	if (f_read(&fsrc,&buffer,2,&br) != FR_OK)
//	{
//		f_close(&fsrc);
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//	f_close(&fsrc);
//	
//	if (f_open(&fsrc,path,FA_WRITE) != FR_OK)
//	{
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//		
//	f_lseek(&fsrc,(Hchar*16 + (Lchar / 16)) * 2);
//	buffer &= (~(0x0001 << (Lchar % 16)));
//	if (f_write(&fsrc,&buffer,2,&br) != FR_OK)
//	{
//		f_close(&fsrc);
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//	f_close(&fsrc);
//	OS_EXIT_CRITICAL();
//	return 1;
//}

//int SetFingerIDStatus(u8 Hchar,u8 Lchar)
//{
//	u16 buffer;
//	OS_CPU_SR cpu_sr = 0;
//	OS_ENTER_CRITICAL();
//	if (f_open(&fsrc,path,FA_OPEN_ALWAYS|FA_READ) != FR_OK)
//	{
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//	f_lseek(&fsrc,(Hchar*16 + (Lchar / 16)) * 2);
//	if (f_read(&fsrc,&buffer,2,&br) != FR_OK)
//	{
//		f_close(&fsrc);
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//	f_close(&fsrc);
//	
//	if (f_open(&fsrc,path,FA_WRITE) != FR_OK)
//	{
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//		
//	f_lseek(&fsrc,(Hchar*16 + (Lchar / 16)) * 2);
//	buffer = buffer | (0x0001 << (Lchar % 16));
//	if (f_write(&fsrc,&buffer,2,&br) != FR_OK)
//	{
//		f_close(&fsrc);
//		OS_EXIT_CRITICAL();
//		return 0;
//	}
//	f_close(&fsrc);
//	OS_EXIT_CRITICAL();
//	return 1;
//}
