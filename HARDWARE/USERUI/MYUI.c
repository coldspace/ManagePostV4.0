/*
******************************************************************
**                      uCGUIBuilder                            **
**                  Version:   4.0.0.0                          **
**                     2012 / 04                               **
**                   CpoyRight to: wyl                          **
**              Email:ucguibuilder@163.com                        **
**          This text was Created by uCGUIBuilder               **
******************************************************************/

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"

#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"

#include "MYUI.h"
#include "Fgstatus.h"
#include "finger.h"

extern int MULTIEDIT_AddText(MULTIEDIT_HANDLE hObj, const char * s);
//EventsFunctionList
void CloseClicked(WM_MESSAGE * pMsg);
void NextClicked(WM_MESSAGE * pMsg);
void EnterClicked(WM_MESSAGE * pMsg);
void PreviousClicked(WM_MESSAGE * pMsg);
//EndofEventsFunctionList

/*********************************************************************
*
*       static data
*
**********************************************************************
*/
extern TakePost tp[10];
extern OS_EVENT *GetUser;
WM_HWIN msg;
u16 userid = 0;
u16 PostIndex = 0;

/*********************************************************************
*
*       deifine function
*
**********************************************************************
*/

void EnterClicked(WM_MESSAGE * pMsg)
{
	WM_HWIN hwin = pMsg->hWin;
	
	if (!tp[PostIndex].post.state)
	{
		MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"have been taken!!\r\n");
		return ;
	}
	MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"confirm finger!!\r\n");
	MULTIEDIT_AddText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"please place finger\r\n");
	
	if (CompleteFinger(userid))
	{
		MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"Fingerprint matching success\r\n");
		if(Post_delete1(tp[PostIndex].post.id,userid))
		{
			MULTIEDIT_AddText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"Receive delivery success");
			tp[PostIndex].post.state = 0;
		}
		else
			MULTIEDIT_AddText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"Receive delivery faile");
	}
	else
	{
		MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"Does not match the fingerprint");
		return;
	}
}

void PreviousClicked(WM_MESSAGE * pMsg)
{
	WM_HWIN hwin = pMsg->hWin;
	if (PostIndex > 0)
	{
		PostIndex --;
		MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"");
	}
	else
		MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"this is the first post");
}

void NextClicked(WM_MESSAGE * pMsg)
{
	WM_HWIN hwin = pMsg->hWin;
	if (tp[PostIndex+1].takestate)
	{
		PostIndex ++;
		MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"");
	}
	else
		MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"no more post");
}

void CloseClicked(WM_MESSAGE * pMsg)
{
	WM_HWIN hwin = pMsg->hWin;
	u8 index;
	PostIndex = 0;
	MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"return");
	CHECKBOX_Uncheck(WM_GetDialogItem(hwin,GUI_ID_CHECK0));
	CHECKBOX_Uncheck(WM_GetDialogItem(hwin,GUI_ID_CHECK1));
	MULTIEDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0),"");
	for (index = 0;index < 10;index ++)
		tp[index].takestate = 0;
	
	EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT0),"");
	EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT1),"");
	EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT2),"");
	EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT3),"");
	
	WM_DisableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON0));
	WM_DisableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON1));
	WM_DisableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON2));
	WM_DisableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON3));
	
	OSTaskResume(14);
}


/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "Caption",           0,                       0,  0,  400,240,FRAMEWIN_CF_MOVEABLE,0},
    { TEXT_CreateIndirect,      "USER:",             GUI_ID_TEXT0,            0,  9,  85, 24, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT0,            114,9,  137,24, 0,0},
    { BUTTON_CreateIndirect,    "Previous",          GUI_ID_BUTTON0,          4,  179,100,44, 0,0},
    { BUTTON_CreateIndirect,    "Enter",             GUI_ID_BUTTON1,          110,179,100,44, 0,0},
    { BUTTON_CreateIndirect,    "Next",              GUI_ID_BUTTON2,          216,179,100,44, 0,0},
    { BUTTON_CreateIndirect,    "Close",             GUI_ID_BUTTON3,          319,179,75, 44, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT1,            114,51, 137,24, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT2,            114,96, 137,24, 0,0},
    { TEXT_CreateIndirect,      "GOODS:",            GUI_ID_TEXT1,            0,  51, 85, 24, 0,0},
    { TEXT_CreateIndirect,      "ID:",               GUI_ID_TEXT2,            0,  96, 85, 24, 0,0},
    { TEXT_CreateIndirect,      "OWNER:",            GUI_ID_TEXT3,            0,  142,85, 24, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT3,            114,142,137,24, 0,0},
    { CHECKBOX_CreateIndirect,  "Valid",             GUI_ID_CHECK0,           264,130,130,15, 0,0},
    { CHECKBOX_CreateIndirect,  "Invalid",           GUI_ID_CHECK1,           264,151,130,15, 0,0},
    { TEXT_CreateIndirect,      "Status:",           GUI_ID_TEXT4,            264,109,73, 15, 0,0},
    { TEXT_CreateIndirect,      "Information:",      GUI_ID_TEXT5,            264,9,  73, 15, 0,0},
    { MULTIEDIT_CreateIndirect,  NULL,               GUI_ID_MULTIEDIT0,       264,30, 119,73, 0,0}
};



/*****************************************************************
**      FunctionName:void PaintDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_PAINT
*****************************************************************/

void PaintDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
    GUI_SetColor(0x000000);
    GUI_DrawLine(257,-1,257,172);
    GUI_SetColor(0x000000);
    GUI_DrawLine(-3,172,393,172);
}



/*****************************************************************
**      FunctionName:void InitDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_INIT_DIALOG
*****************************************************************/

void InitDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	msg = hWin;
    //
    //FRAMEWIN
    //
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_AddMaxButton(hWin, FRAMEWIN_BUTTON_RIGHT, 1);
    FRAMEWIN_AddMinButton(hWin, FRAMEWIN_BUTTON_RIGHT, 2);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font16_1);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT0),GUI_TA_VCENTER|GUI_TA_CENTER);
    //
    //GUI_ID_EDIT0
    //
    EDIT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_EDIT0),0,0xf5f5f5);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT0),&GUI_Font16_1);
    //
    //GUI_ID_BUTTON0
    //
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON0),&GUI_Font16_1);
    WM_DisableWindow(WM_GetDialogItem(hWin,GUI_ID_BUTTON0));
    //
    //GUI_ID_BUTTON1
    //
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON1),&GUI_Font16_1);
    WM_DisableWindow(WM_GetDialogItem(hWin,GUI_ID_BUTTON1));
    //
    //GUI_ID_BUTTON2
    //
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON2),&GUI_Font16_1);
    WM_DisableWindow(WM_GetDialogItem(hWin,GUI_ID_BUTTON2));
    //
    //GUI_ID_BUTTON3
    //
    BUTTON_SetFont(WM_GetDialogItem(hWin,GUI_ID_BUTTON3),&GUI_Font16_1);
    WM_DisableWindow(WM_GetDialogItem(hWin,GUI_ID_BUTTON3));
    //
    //GUI_ID_EDIT1
    //
    EDIT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_EDIT1),0,0xf5f5f5);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT1),&GUI_Font16_1);
    //
    //GUI_ID_EDIT2
    //
    EDIT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_EDIT2),0,0xf5f5f5);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT2),&GUI_Font16_1);
    //
    //GUI_ID_TEXT1
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font16_1);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT1),GUI_TA_VCENTER|GUI_TA_CENTER);
    //
    //GUI_ID_TEXT2
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2),&GUI_Font16_1);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT2),GUI_TA_VCENTER|GUI_TA_CENTER);
    //
    //GUI_ID_TEXT3
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3),&GUI_Font16_1);
    TEXT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_TEXT3),GUI_TA_VCENTER|GUI_TA_CENTER);
    //
    //GUI_ID_EDIT3
    //
    EDIT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_EDIT3),0,0xf5f5f5);
    EDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_EDIT3),&GUI_Font16_1);
    //
    //GUI_ID_MULTIEDIT0
    //
    MULTIEDIT_SetAutoScrollH(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1);
    MULTIEDIT_SetAutoScrollV(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1);
    WM_DisableWindow(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0));
    //
    //GUI_ID_CHECK0
    //
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK0),&GUI_Font16_1);
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK0),"Valid");
    WM_DisableWindow(WM_GetDialogItem(hWin,GUI_ID_CHECK0));
    //
    //GUI_ID_CHECK1
    //
    CHECKBOX_SetFont(WM_GetDialogItem(hWin,GUI_ID_CHECK1),&GUI_Font16_1);
    CHECKBOX_SetText(WM_GetDialogItem(hWin,GUI_ID_CHECK1),"Invalid");
    WM_DisableWindow(WM_GetDialogItem(hWin,GUI_ID_CHECK1));
    //
    //GUI_ID_TEXT4
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT4),&GUI_Font16_1);
    //
    //GUI_ID_TEXT5
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT5),&GUI_Font16_1);
	
	MULTIEDIT_SetMaxNumChars(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0), 100);
}




/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
	u16 temp;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialog(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            PreviousClicked(pMsg);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            EnterClicked(pMsg);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            NextClicked(pMsg);
                            break;
                    }
                    break;
                case GUI_ID_BUTTON3:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_CLICKED:
                            CloseClicked(pMsg);
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
	temp = userid;
	DisPost(&temp);
}

/*********************************************************************
*
*       My Function
*
**********************************************************************/

void DisPost(u16 *UserId)
{
	WM_HWIN hwin = msg;
	OS_CPU_SR cpu_sr = 0;
	char str[10];
	userid = *UserId;
	
	if (tp[PostIndex].takestate)
	{	
		EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT0),tp[PostIndex].PInfUser.name);
		EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT1),tp[PostIndex].post.Things);
		itoa(tp[PostIndex].post.id,str,10);
		EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT2),str);
		EDIT_SetText(WM_GetDialogItem(hwin,GUI_ID_EDIT3),tp[PostIndex].PInfAdmin.name);
		
		if (tp[PostIndex].post.state)
		{
			CHECKBOX_Check(WM_GetDialogItem(hwin,GUI_ID_CHECK0));
			CHECKBOX_Uncheck(WM_GetDialogItem(hwin,GUI_ID_CHECK1));
		}
		else
		{
			CHECKBOX_Check(WM_GetDialogItem(hwin,GUI_ID_CHECK1));
			CHECKBOX_Uncheck(WM_GetDialogItem(hwin,GUI_ID_CHECK0));
		}
		
		WM_EnableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON0));
		WM_EnableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON1));
		WM_EnableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON2));
		WM_EnableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON3));
		OS_ENTER_CRITICAL();
//		printf("run to 2  %s\r\n",tp[PostIndex].post.Things);
		OS_EXIT_CRITICAL();
	}
	else
		WM_DisableWindow(WM_GetDialogItem(hwin,GUI_ID_BUTTON1));
}

/*********************************************************************
*
*       MainTask
*
**********************************************************************/
void MainTask(void) 
{ 
    GUI_Init();
    WM_SetDesktopColor(GUI_WHITE);      /* Automacally update desktop window */
    WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
    //PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    //FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
    //PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    //BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
    //CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
    //DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
    //SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
    //SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
    //HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
    //RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	while(1)
	{
        GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
	}
}

