#ifndef _FINGER_H_
#define _FINGER_H_

#include "includes.h"
#include "Usart2.h"
#include "Fgstatus.h"

void FINGERPRINT_Cmd_Get_Img(void);
void FINGERPRINT_Cmd_Img_To_Buffer1(void);
void FINGERPRINT_Cmd_Img_To_Buffer2(void);
void FINGERPRINT_Cmd_GR_Match(void);
void FINGERPRINT_Cmd_Reg_Model(void);
void FINGERPRINT_Cmd_Delete_All_Model(void);
void FINGERPRINT_Cmd_Delete_Model(unsigned int uiID_temp);
void FINGERPRINT_Cmd_Get_Templete_Num(void);
void FINGERPRINT_Cmd_Search_Finger(void);
void FINGERPRINT_Cmd_Search_Finger_Admin(void);
void FINGERPRINT_Cmd_Save_Finger( unsigned char ucH_Char,unsigned char ucL_Char );
void FINGERPRINT_Recevice_Data(unsigned char ucLength);

void Printf_error(u8 data);
//unsigned char FP_add_new_user(unsigned char ucH_user,unsigned char ucL_user);
void FINGERPRINT_Usart_Send(USART_TypeDef *Usartx,u8 chr);
unsigned char FP_add_new_user(u8 *Usart1buf);
unsigned char Seach_finger(void);
void FP_UpChar(void);
void GetNewFingerID2(u8 *Hchar,u8 *Lchar);
int delete_finger(u8 *Usart1buf);
int CompleteFinger(u16 userid);
#endif
