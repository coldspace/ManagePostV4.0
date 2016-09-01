#ifndef _FGSTATUS_H_
#define _FGSTATUS_H_

#include "includes.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "math.h"
#include "usart.h"
#include "delay.h"

typedef struct{
	u16 Admin;
	u16 friends[3];
	char Things[30];
	u8 id;
	u8 state;
}Post;

typedef struct{
	char name[20];
	u8 phone[12];
	u8 state;	 // 使用标志位
	u16 id;		 // 存放的位置
	u8 sex;      // 0 为男，1为女
}PersonInf;

/*
*取快递的所有信息
*邮件名称，取快递id 姓名 
*后期可以添加时间等信息
*/
typedef struct{
	Post post;
	PersonInf PInfAdmin;
	PersonInf PInfUser;
	u8 takestate;
}TakePost;


int FBSTA_init(void);
//int FBSTA_clear(void);
//int GetNewFingerID(u8 *Hchar,u8 *Lchar);
//int SetFingerIDStatus(u8 Hchar,u8 Lchar);
//int ClearFingerIDStatus(u8 Hchar,u8 Lchar);

int Post_Clear(void);
int FB_AddPost(u8 *Usart1buf);
int Post_delete(u8 *Usart1buf);
int Post_delete1(u16 id,u16 user);
int Post_Change(u8 *Usart1buf);
int FB_SeachPost(u16 user);
int Post_list(u8 num);

/*
*人物信息储存
*/
int Person_clear(void);
int Person_Search(u16 id,char *p);
int Person_Add(u16 id,u8 *p);
int Person_delete(u16 id);
int Person_list(u16 num);

#endif
