#include "ST7789.h"
#include "oledfont_7789.h"
#include "stm32f0xx_hal_def.h"

#include "spi.h"

#define LCD_SPI hspi1
	   
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

HAL_StatusTypeDef User_SPI_Transmit_8Bit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size,uint32_t Out_Time)
{
	UNUSED(Out_Time);
	__HAL_LOCK(hspi);
	
	__IO uint8_t *dout= ((__IO uint8_t *)&hspi->Instance->DR);
	while (Size > 0U)
	{
		if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
		{
			*dout = *(pData++);
			Size--;
		}
	}
	
	__HAL_UNLOCK(hspi);
	
	return HAL_OK;
}

HAL_StatusTypeDef User_SPI_Transmit_16Bit(SPI_HandleTypeDef *hspi, uint16_t pData, uint16_t Size,uint32_t Out_Time)
{
	UNUSED(Out_Time);
	__HAL_LOCK(hspi);
	
	__IO uint16_t *Dout= ((__IO uint16_t *)&hspi->Instance->DR);
	
	while (Size > 0U)
	{
		if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
		{
			*Dout = pData;
			Size--;
		}
	}
	
	__HAL_UNLOCK(hspi);
	
	return HAL_OK;
}




/*****************************************************************************
 * @name       :void LCD_WR_REG(uint8_t data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(uint8_t data)
{ 
	LCD_DC_CLR;
	User_SPI_Transmit_8Bit(&LCD_SPI,&data,1,100);
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(uint8_t data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(uint8_t data)
{
	LCD_DC_SET;
	User_SPI_Transmit_8Bit(&LCD_SPI,&data,1,100);
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(uint16_t Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(uint16_t Data)
{
	uint16_t Buffer = (Data >> 8) | (Data << 8);
	LCD_DC_SET;
	
	User_SPI_Transmit_16Bit(&LCD_SPI,Buffer,1,100);
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(uint16_t x,uint16_t y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t Color)
{
	LCD_SetCursor(x,y);//设置光标位置 
	Lcd_WriteData_16Bit(Color); 
}

/*****************************************************************************
 * @name       :void LCD_Clear(uint16_t Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(uint16_t Color)
{
	uint16_t Buffer = (Color >> 8) | (Color << 8);
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	LCD_DC_SET;
	
	User_SPI_Transmit_16Bit(&LCD_SPI, Buffer, lcddev.width * lcddev.height, 100);
} 

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
	HAL_Delay(20);	
	LCD_RST_SET;
	HAL_Delay(20);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{
	__HAL_SPI_ENABLE(&LCD_SPI);
 	LCD_RESET(); //LCD 复位
//************* ST7789初始化**********//	
	LCD_WR_REG(0x36); 
	LCD_WR_DATA(0x00);

	LCD_WR_REG(0x3A); 
	LCD_WR_DATA(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x33);

	LCD_WR_REG(0xB7); 
	LCD_WR_DATA(0x35);  

	LCD_WR_REG(0xBB);
	LCD_WR_DATA(0x19);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA(0x12);   

	LCD_WR_REG(0xC4);
	LCD_WR_DATA(0x20);  

	LCD_WR_REG(0xC6); 
	LCD_WR_DATA(0x0F);    

	LCD_WR_REG(0xD0); 
	LCD_WR_DATA(0xA4);
	LCD_WR_DATA(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0xD0);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x2B);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x54);
	LCD_WR_DATA(0x4C);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x23);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA(0xD0);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x51);
	LCD_WR_DATA(0x2F);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x23);

	LCD_WR_REG(0x21); 

	LCD_WR_REG(0x11); 
	//Delay (120); 

	LCD_WR_REG(0x29); 	
  LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
	
	LCD_Clear(BLACK);//清全屏黑色
	//LCD_Clear(WHITE);//清全屏白色
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
{
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA((xStar+lcddev.xoffset)>>8);
	LCD_WR_DATA(xStar+lcddev.xoffset);		
	LCD_WR_DATA((xEnd+lcddev.xoffset)>>8);
	LCD_WR_DATA(xEnd+lcddev.xoffset);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA((yStar+lcddev.yoffset)>>8);
	LCD_WR_DATA(yStar+lcddev.yoffset);		
	LCD_WR_DATA((yEnd+lcddev.yoffset)>>8);
	LCD_WR_DATA(yEnd+lcddev.yoffset);

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(uint8_t direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(uint8_t direction)
{ 
			lcddev.setxcmd=0x2A;
			lcddev.setycmd=0x2B;
			lcddev.wramcmd=0x2C;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			lcddev.xoffset=0;
			lcddev.yoffset=0;
			LCD_WriteReg(0x36,0);//BGR==1,MY==0,MX==0,MV==0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			lcddev.xoffset=0;
			lcddev.yoffset=0;
			LCD_WriteReg(0x36,(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;
      lcddev.xoffset=0;
			lcddev.yoffset=80;			
			LCD_WriteReg(0x36,(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			lcddev.xoffset=80;
			lcddev.yoffset=0;
			LCD_WriteReg(0x36,(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:break;
	}		
}









/*******************************************************************
 * @name       :void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
 * @date       :2018-08-09 
 * @function   :fill the specified area
 * @parameters :sx:the bebinning x coordinate of the specified area
                sy:the bebinning y coordinate of the specified area
								ex:the ending x coordinate of the specified area
								ey:the ending y coordinate of the specified area
								color:the filled color value
 * @retvalue   :None
********************************************************************/
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
	uint16_t Buffer = (color >> 8) | (color << 8);
	
	uint16_t width=ex-sx + 1; 		//得到填充的宽度
	uint16_t height=ey-sy + 1;		//高度
	LCD_SetWindows(sx,sy,ex,ey);//设置显示窗口
	
	LCD_DC_SET;
	User_SPI_Transmit_16Bit(&LCD_SPI, Buffer, width * height, 100);
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口设置为全屏
}

/*******************************************************************
 * @name       :void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 * @date       :2018-08-09 
 * @function   :Draw a line between two points
 * @parameters :x1:the bebinning x coordinate of the line
                y1:the bebinning y coordinate of the line
								x2:the ending x coordinate of the line
								y2:the ending y coordinate of the line
 * @retvalue   :None
********************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t Color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{
		LCD_DrawPoint(uRow,uCol,Color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

/*****************************************************************************
 * @name       :void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 * @date       :2018-08-09 
 * @function   :Draw a rectangle
 * @parameters :x1:the bebinning x coordinate of the rectangle
                y1:the bebinning y coordinate of the rectangle
								x2:the ending x coordinate of the rectangle
								y2:the ending y coordinate of the rectangle
 * @retvalue   :None
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color)
{
	LCD_DrawLine(x1,y1,x2,y1,Color);
	LCD_DrawLine(x1,y1,x1,y2,Color);
	LCD_DrawLine(x1,y2,x2,y2,Color);
	LCD_DrawLine(x2,y1,x2,y2,Color);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle (uint16_t x0, uint16_t y0, uint8_t r,uint16_t color)
{
  int a, b;
  int di;
  a = 0;
  b = r;
  di = 3 - (r << 1);             //判断下个点位置的标志
  while (a <= b)
  {
    LCD_DrawPoint (x0 - b, y0 - a, color);             //3
    LCD_DrawPoint (x0 + b, y0 - a, color);             //0
    LCD_DrawPoint (x0 - a, y0 + b, color);             //1
    LCD_DrawPoint (x0 - a, y0 - b, color);             //2
    LCD_DrawPoint (x0 + b, y0 + a, color);             //4
    LCD_DrawPoint (x0 + a, y0 - b, color);             //5
    LCD_DrawPoint (x0 + a, y0 + b, color);             //6
    LCD_DrawPoint (x0 - b, y0 + a, color);
    a++;
    //使用Bresenham算法画圆
    if (di < 0) di += 4 * a + 6;
    else
    {
      di += 10 + 4 * (a - b);
      b--;
    }
    LCD_DrawPoint (x0 + a, y0 + b, color);
  }
}

//画1/4圆
void Draw_4_Circle(uint16_t x0, uint16_t y0, uint8_t r,uint8_t Dir,uint16_t color)
{
	int x = r;		
	int y = 0;
	int F = -2 * r + 3;
	
	while(x >= y)
	{
		if(Dir == 0)//左上
		{
			LCD_DrawPoint(x0-x, y0-y, color);
			LCD_DrawPoint(x0-y, y0-x, color);
		}
		
		else if(Dir == 1)//左下
		{
			LCD_DrawPoint(x0-x, y0+y, color);
			LCD_DrawPoint(x0-y, y0+x, color);
		}
		
		else if(Dir == 2)//右上
		{
			LCD_DrawPoint(x0+y, y0-x, color);
			LCD_DrawPoint(x0+x, y0-y, color);
		}
		
		else if(Dir == 3)//右下
		{
			LCD_DrawPoint(x0+x, y0+y, color);
			LCD_DrawPoint(x0+y, y0+x, color);
		}
		
		if(F >= 0)
		{
			x--;
			F -= 4 * x;
		}
		y++;
		F += 4 * y + 2;
	}
}

/*****************************************************************************
 * @name       :void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, uint8_t num,uint8_t size,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display a single English character
 * @parameters :x:the bebinning x coordinate of the Character display position
                y:the bebinning y coordinate of the Character display position
								fc:the color value of display character
								bc:the background color of display character
								num:the ascii code of display character(0~94)
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, char num,uint8_t mode)
{ 
	uint8_t temp;
	uint8_t pos,t;
	
	num=num-' ';//得到偏移后的值
	LCD_SetWindows(x,y,x+7,y+15);//设置单个文字显示窗口
	if(!mode) //非叠加方式
	{		
		for(pos=0;pos<16;pos++)
		{
			temp=asc2_1608[num][pos];//调用1608字体
			for(t=0;t<8;t++)
			{                 
				if(temp&0x01)
					Lcd_WriteData_16Bit(fc); 
				else
					Lcd_WriteData_16Bit(bc); 
				temp>>=1; 
			}
		}	
	}
	else//叠加方式
	{
		for(pos=0;pos<16;pos++)
		{
			temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<8;t++)
			{              
				if(temp&0x01)
					LCD_DrawPoint(x+t,y+pos,fc);//画一个点    
				temp>>=1; 
			}
		}
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏    	   	 	  
}

/*****************************************************************************
 * @name       :void Show_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display Chinese and English strings
 * @parameters :x:the bebinning x coordinate of the Chinese and English strings
                y:the bebinning y coordinate of the Chinese and English strings
								fc:the color value of Chinese and English strings
								bc:the background color of Chinese and English strings
								str:the start address of the Chinese and English strings
								size:the size of Chinese and English strings
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/	   		   
void Show_16x32_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char str)
{
	LCD_SetWindows(x,y,x+15,y+31);//设置单个文字显示窗口
	
	uint16_t Char = str - ' ';
		
	for(uint16_t Y = 0;Y < 32;Y++)
	{
		for(uint16_t X = 0;X < 16;X++)
		{
			if(Font16x32[(Char * 64) + (X / 8) + (Y * 2)] & (0x80 >> (X % 8)))
				Lcd_WriteData_16Bit(fc); 
			else
				Lcd_WriteData_16Bit(bc); 
		}
	}
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏    	
}

void Show_16x32_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *str)
{
	while(*str != '\0')
	{
		Show_16x32_Char(x,y,fc,bc,*str);
		str++;
		x += 16;
	}
}

void Show_60x60_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char str)
{
	uint16_t Char = 0;
	
	LCD_SetWindows(x,y,x+29,y+59);//设置单个文字显示窗口
	
	if(str >= '0' && str <= '9')
		Char = str - '0';
	else if(str == 'D')
		Char = 10;
	else if(str == 'C')
		Char = 11;
	else
		Char = 12;
		
	for(uint16_t Y = 0;Y < 60;Y++)
	{
		for(uint16_t X = 0;X < 30;X++)
		{
			if(Font60x60[(Char * 240) + (X / 8) + (Y * 4)] & (0x80 >> (X % 8)))
				Lcd_WriteData_16Bit(fc); 
			else
				Lcd_WriteData_16Bit(bc); 
		}
	}
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏    	
}

void Show_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *str,uint8_t mode)
{					
	uint16_t x0=x;							  	  
	while(*str!=0)//数据未结束
	{ 
		if(x>(lcddev.width-16/2)||y>(lcddev.height-16)) 
			return;          
		if(*str==0x0D)//换行符号
		{         
			y+=16;
			x=x0;
			str++; 
		}  
		else
		{
			LCD_ShowChar(x,y,fc,bc,*str,mode);
			x+=8; //字符,为全字的一半 
		} 
		str++; 
	}
}

uint8_t Judge_Back_Ground(int16_t Pos_x,int16_t Pos_y)
{
	if(Pos_y == 114)
		return 2;
	if((Pos_x - 5) % 23 == 0 || (Pos_y - 22) % 23 == 0)
		return 1;
	return 0;
}

//示波器画背景线函数 如果是栅格的画就写栅格颜色 否则写黑色
void Oscilloscope_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t Color)
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{
		if(Judge_Back_Ground(uRow,uCol) == 1)
			LCD_DrawPoint(uRow,uCol,GRAY);//画点 
		else if(Judge_Back_Ground(uRow,uCol) == 2)
			LCD_DrawPoint(uRow,uCol,LGRAY);//画点 
		else
			LCD_DrawPoint(uRow,uCol,Color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

//画一个边角是圆弧的方形
void Draw_Circle_Square(uint16_t X,uint16_t Y,uint16_t End_X,uint16_t End_Y,uint16_t R,uint16_t Color)
{
	LCD_DrawLine(X + R,Y,End_X - R,Y,Color);
	LCD_DrawLine(X + R,End_Y,End_X - R,End_Y,Color);
	LCD_DrawLine(X,Y + R,X,End_Y - R,Color);
	LCD_DrawLine(End_X,Y + R,End_X,End_Y - R,Color);
	
	Draw_4_Circle(X + R,Y + R,R,0,Color);
	Draw_4_Circle(X + R,End_Y - R,R,1,Color);
	Draw_4_Circle(End_X - R,Y + R,R,2,Color);
	Draw_4_Circle(End_X - R,End_Y - R,R,3,Color);
}

void Show_BitMap_Pic(uint16_t X,uint16_t Y,uint16_t Width,uint16_t Height,uint16_t Back,uint16_t Color,const uint8_t *Pic)
{
	uint16_t Back_Buffer = (Back >> 8) | (Back << 8);
	uint16_t Color_Buffer = (Color >> 8) | (Color << 8);
	LCD_SetWindows(X,Y,X + Width-1,Y + Height-1);
	LCD_DC_SET;
	for(uint16_t y = 0;y < Height;y++)
	{
		for(uint16_t x = 0;x < Width;x++)
		{
			if(Pic[x / 8 + (y * (Width / 8))] & (0x80 >> x % 8))
				User_SPI_Transmit_16Bit(&LCD_SPI, Color_Buffer, 1,100);
			else
				User_SPI_Transmit_16Bit(&LCD_SPI, Back_Buffer, 1,100);
		}
	}
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
}

//打印三角波图像
void Wave_Saw_Tooth_Pic(uint16_t Color,uint16_t Back)
{
	Show_BitMap_Pic(15,35,64,64,Back,Color,Saw_Tooth_Pic);
}

//打印方波图像
void Wave_Square_Pic(uint16_t Color,uint16_t Back)
{
	Show_BitMap_Pic(88,35,64,64,Back,Color,Square_Pic);
}

//打印正弦波图像
void Wave_Sine_Pic(uint16_t Color,uint16_t Back)
{
	Show_BitMap_Pic(161,35,64,64,Back,Color,Sine_Pic);
}

//打印三角形
void Print_Delta(uint16_t x,uint16_t y,uint8_t Levle,uint16_t Color)
{
	for(int8_t Y = Levle - 1;Y >= 0;Y--)
	{
		for(int8_t X = 0;X < (Y * 2) + 1;X++)
		{
			LCD_DrawPoint(x + X,y,Color);
		}
		x++;y++;
	}
}





