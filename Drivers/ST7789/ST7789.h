#ifndef __LCD_7789_H
#define __LCD_7789_H

#include "User.h"

//LCD重要参数集
typedef struct  
{										    
	uint16_t width;			//LCD 宽度
	uint16_t height;			//LCD 高度
	uint16_t id;				  //LCD ID
	uint8_t  dir;			  //横屏还是竖屏控制：0，竖屏；1，横屏。	
	uint16_t	 wramcmd;		//开始写gram指令
	uint16_t  setxcmd;		//设置x坐标指令
	uint16_t  setycmd;		//设置y坐标指令	
  uint8_t   xoffset;    
  uint8_t	 yoffset;
}_lcd_dev; 	

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
/////////////////////////////////////用户配置区///////////////////////////////////	 
#define USE_HORIZONTAL  	 0 //定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转

//////////////////////////////////////////////////////////////////////////////////	  
//定义LCD的尺寸
#define LCD_W 240
#define LCD_H 240

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
//GPIO置位（拉高）
#define	LCD_DC_SET	LCD_DC_GPIO_Port->BSRR = LCD_DC_Pin
#define	LCD_RST_SET	LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin

//GPIO复位（拉低）							    
#define	LCD_DC_CLR	LCD_DC_GPIO_Port->BRR = LCD_DC_Pin
#define	LCD_RST_CLR	LCD_RST_GPIO_Port->BRR = LCD_RST_Pin

#define RGB16Dec2Hex(r,g,b) (uint32_t)((r<<16)|(g<<8)|(b))

#define RGB16Hex2RGB565(rgb) (uint16_t)(((rgb&0xf80000)>>8) | ((rgb&0x00fc00)>>5) | ((rgb&0x0000f8)>>3))

//画笔颜色
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //棕色
#define BRRED 			0XFC07 //棕红色
#define GRAY  			0X8430 //灰色
#define GRAY0       0xEF7D 
#define GRAY1       0x8410      	//灰色1      00000 000000 00000
#define GRAY2       0x4208
#define ORANGE			RGB16Hex2RGB565(RGB16Dec2Hex(255,128,0))
#define VIOLET			RGB16Hex2RGB565(RGB16Dec2Hex(255,0,255))
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	0X841F //浅绿色
#define LIGHTGRAY     0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 		0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE      	0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)
	    															  
void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t Color);	 
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);
void LCD_direction(uint8_t direction);

void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
void Lcd_WriteData_16Bit(uint16_t Data);
uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y);//读点
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t Color);//画点
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t Color);//画线
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);//画框
void Draw_Circle (uint16_t x0, uint16_t y0, uint8_t r,uint16_t color);//画圆
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, char num,uint8_t mode);
void Show_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *str,uint8_t mode);
void Oscilloscope_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t Color);

void Draw_4_Circle(uint16_t x0, uint16_t y0, uint8_t r,uint8_t Dir,uint16_t color);
void Draw_Circle_Square(uint16_t X,uint16_t Y,uint16_t End_X,uint16_t End_Y,uint16_t R,uint16_t Color);
void Show_BitMap_Pic(uint16_t X,uint16_t Y,uint16_t Width,uint16_t Height,uint16_t Back,uint16_t Color,const uint8_t *Pic);
void Wave_Generate_Frame_Pic(void);
void Wave_Saw_Tooth_Pic(uint16_t Color,uint16_t Back);
void Wave_Square_Pic(uint16_t Color,uint16_t Back);
void Wave_Sine_Pic(uint16_t Color,uint16_t Back);


void Show_60x60_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char str);//显示30x60的字符
void Show_16x32_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char str);//显示16x32的字符
void Show_16x32_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *str);//显示16x32的字符串
void Print_Delta(uint16_t x,uint16_t y,uint8_t Levle,uint16_t Color);//打印一个倒角三角形
#endif

