#ifndef __LCD_7789_H
#define __LCD_7789_H

#include "User.h"

//LCD��Ҫ������
typedef struct  
{										    
	uint16_t width;			//LCD ���
	uint16_t height;			//LCD �߶�
	uint16_t id;				  //LCD ID
	uint8_t  dir;			  //���������������ƣ�0��������1��������	
	uint16_t	 wramcmd;		//��ʼдgramָ��
	uint16_t  setxcmd;		//����x����ָ��
	uint16_t  setycmd;		//����y����ָ��	
  uint8_t   xoffset;    
  uint8_t	 yoffset;
}_lcd_dev; 	

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
/////////////////////////////////////�û�������///////////////////////////////////	 
#define USE_HORIZONTAL  	 0 //����Һ����˳ʱ����ת���� 	0-0����ת��1-90����ת��2-180����ת��3-270����ת

//////////////////////////////////////////////////////////////////////////////////	  
//����LCD�ĳߴ�
#define LCD_W 240
#define LCD_H 240

////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���---------------- 
//GPIO��λ�����ߣ�
#define	LCD_DC_SET	LCD_DC_GPIO_Port->BSRR = LCD_DC_Pin
#define	LCD_RST_SET	LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin

//GPIO��λ�����ͣ�							    
#define	LCD_DC_CLR	LCD_DC_GPIO_Port->BRR = LCD_DC_Pin
#define	LCD_RST_CLR	LCD_RST_GPIO_Port->BRR = LCD_RST_Pin

#define RGB16Dec2Hex(r,g,b) (uint32_t)((r<<16)|(g<<8)|(b))

#define RGB16Hex2RGB565(rgb) (uint16_t)(((rgb&0xf80000)>>8) | ((rgb&0x00fc00)>>5) | ((rgb&0x0000f8)>>3))

//������ɫ
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
#define BROWN 			0XBC40 //��ɫ
#define BRRED 			0XFC07 //�غ�ɫ
#define GRAY  			0X8430 //��ɫ
#define GRAY0       0xEF7D 
#define GRAY1       0x8410      	//��ɫ1      00000 000000 00000
#define GRAY2       0x4208
#define ORANGE			RGB16Hex2RGB565(RGB16Dec2Hex(255,128,0))
#define VIOLET			RGB16Hex2RGB565(RGB16Dec2Hex(255,0,255))
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	0X841F //ǳ��ɫ
#define LIGHTGRAY     0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 		0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE      	0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE          0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	    															  
void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t Color);	 
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);
void LCD_direction(uint8_t direction);

void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
void Lcd_WriteData_16Bit(uint16_t Data);
uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y);//����
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t Color);//����
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t Color);//����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);//����
void Draw_Circle (uint16_t x0, uint16_t y0, uint8_t r,uint16_t color);//��Բ
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


void Show_60x60_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char str);//��ʾ30x60���ַ�
void Show_16x32_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char str);//��ʾ16x32���ַ�
void Show_16x32_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *str);//��ʾ16x32���ַ���
void Print_Delta(uint16_t x,uint16_t y,uint8_t Levle,uint16_t Color);//��ӡһ������������
#endif

