#include "stm32f10x.h"
#include "lcd.h"
#include "lcd_font.h"
#include "lcdconf.h"

extern void LCD_WR_Data(unsigned int val);
extern unsigned int LCD_RD_data(void);
extern void LCD_Init1(void);


/****************************************************************************
* 名    称：void lcd_Initializtion()
* 功    能：初始化 lcd 控制器
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：lcd_Initializtion();
****************************************************************************/
void lcd_Initializtion()
{ 
   LCD_Init1();
}

/****************************************************************************
* 名    称：void lcd_SetCursor(u16 x,u16 y)
* 功    能：设置屏幕座标
* 入口参数：x      行座标
*           y      列座标
* 出口参数：无
* 说    明：
* 调用方法：lcd_SetCursor(10,10);
****************************************************************************/
void lcd_SetCursor(u16 x,u16 y)
{	
  *(__IO uint16_t *) (Bank1_LCD_C)= 0x200;
  *(__IO uint16_t *) (Bank1_LCD_D)= y;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x201;
  *(__IO uint16_t *) (Bank1_LCD_D)= 399-x;
}

/****************************************************************************
* 名    称：void lcd_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
* 功    能：设置窗口区域
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
* 出口参数：无
* 说    明：
* 调用方法：lcd_SetWindows(0,0,100,100)；
****************************************************************************/
void lcd_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{

  	
  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0210;	
  *(__IO uint16_t *) (Bank1_LCD_D)= StartY;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0211;	
  *(__IO uint16_t *) (Bank1_LCD_D)= EndX;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0212;	
  *(__IO uint16_t *) (Bank1_LCD_D)= 399-StartX;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0213;	
  *(__IO uint16_t *) (Bank1_LCD_D)= EndY;
}


/****************************************************************************
* 名    称：u16 lcd_GetPoint(u16 x,u16 y)
* 功    能：获取指定座标的颜色值
* 入口参数：x      行座标
*           y      列座标
* 出口参数：当前座标颜色值
* 说    明：
* 调用方法：i=lcd_GetPoint(10,10);
****************************************************************************/
u16 lcd_GetPoint(u16 x,u16 y)
{ 
  
  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0200;
  *(__IO uint16_t *) (Bank1_LCD_D)= y;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0201;
  *(__IO uint16_t *) (Bank1_LCD_D)= 399-x;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0202;

  //return (lcd_BGR2RGB(lcd_ReadData()));
  return (lcd_ReadData());

}

/****************************************************************************
* 名    称：void lcd_SetPoint(u16 x,u16 y,u16 point)
* 功    能：在指定座标画点
* 入口参数：x      行座标
*           y      列座标
*           point  点的颜色
* 出口参数：无
* 说    明：
* 调用方法：lcd_SetPoint(10,10,0x0fe0);
****************************************************************************/
void lcd_SetPoint(u16 x,u16 y,u16 point)
{
  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0200;	
  *(__IO uint16_t *) (Bank1_LCD_D)= y;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0201;	
  *(__IO uint16_t *) (Bank1_LCD_D)= 399-x;

  *(__IO uint16_t *) (Bank1_LCD_C)= 0x0202;	

  *(__IO uint16_t *) (Bank1_LCD_D)= point;
}

/****************************************************************************
* 名    称：void lcd_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* 功    能：在指定座标显示一个8x16点阵的ascii字符
* 入口参数：x          行座标
*           y          列座标
*           charColor  字符的颜色
*           bkColor    字符背景颜色
* 出口参数：无
* 说    明：显示范围限定为可显示的ascii码
* 调用方法：lcd_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void lcd_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;
  
  u8 tmp_char=0;

  for (i=0;i<16;i++)
  {
    tmp_char=ascii_8x16[((c-0x20)*16)+i];
    for (j=0;j<8;j++)
    {
      if ( (tmp_char >> 7-j) & 0x01 == 0x01)
        {
          lcd_SetPoint(x+j,y+i,charColor); // 字符颜色
        }
        else
        {
          lcd_SetPoint(x+j,y+i,bkColor); // 背景颜色
        }
    }
  }
}

/****************************************************************************
* 名    称：u16 lcd_BGR2RGB(u16 c)
* 功    能：RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* 入口参数：c      BRG 颜色值
* 出口参数：RGB 颜色值
* 说    明：内部函数调用
* 调用方法：
****************************************************************************/
u16 lcd_BGR2RGB(u16 c)
{
  u16  r, g, b;

  b = (c>>0)  & 0x1f;
  g = (c>>5)  & 0x3f;
  r = (c>>11) & 0x1f;
  
  //return( (b<<11) + (g<<5) + (r<<0) );
  return( (b<<11) + (g<<5) + (r<<0) );
}



/****************************************************************************
* 名    称：u16 lcd_ReadData(void)
* 功    能：读取控制器数据
* 入口参数：无
* 出口参数：返回读取到的数据
* 说    明：内部函数
* 调用方法：i=lcd_ReadData();
****************************************************************************/
u16 lcd_ReadData(void)
{
  u16 val=0;
  val=LCD_RD_data();
  return val;
}


/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/
