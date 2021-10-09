#include "Menu.h"

/*--------------------------------------------------Oscilloscope--------------------------------------------------*/
extern ADC_HandleTypeDef hadc;
extern TIM_HandleTypeDef htim1;

uint16_t ADC_Scan_Samp[230][2] = {0};
int16_t Show_Buffer[230][2][2] = {0};
uint8_t Show_Cnt = 0;//切换缓冲区
uint16_t Scan_Cnt = 0;
uint16_t Scan_Cnt_Buffer = 0;
uint16_t Oscilloscope_Clean_Flag = 0;

void Oscilloscope_Init(void)
{
	Un_Scan_Init = 0;
	ADC_Samp_Init();
	Oscilloscope_UI();
}

void Oscilloscope_Deinit(void)
{
	HAL_TIM_Base_Stop(&htim1);
	HAL_TIM_Base_Stop_IT(&htim1);
	HAL_ADC_Stop_DMA(&hadc);
}

void Oscilloscope_UI(void)
{
	Show_Str(72,4,WHITE,BLACK,"Oscilloscope",0);
	LCD_DrawRectangle(5,22,235,206,WHITE);
}

void Oscilloscope_Show_Wave(void)
{
	float Vlotage_Bisic[7] = {0.05,0.1,0.2,0.5,1,2,5};
	static uint32_t Show_Wave_Tick = 0;
	static uint16_t Last_Cnt = 0;
	
	if(Oscilloscope_Clean_Flag == 1)
	{
		Oscilloscope_Clean_Flag = 0;
		
		LCD_Fill(6,23,234,205,BLACK);
		for(uint8_t i = 1;i < 10;i++)
		{
			LCD_DrawLine (5 + (i * 23), 23 , 5 + (i * 23), 205, GRAY);
			if(i < 8)
				LCD_DrawLine (6, 22 + (i * 23), 234, 22 + (i * 23), GRAY);
		}
		LCD_DrawLine (6, 114, 234, 114, LGRAY);
	}
	
	if(HAL_GetTick() >= Show_Wave_Tick && ADC_Samp_Ok == 1)
	{
		Show_Wave_Tick = HAL_GetTick() + 10;
		
		if(Scan_Show == 0)
		{
			if(Tri_Pos > ADC_Length - 231)
				Tri_Pos = ADC_Length - 231;
			
			//生成绘图数据
			for(uint16_t i = 0;i < 230;i++)
			{
				float Voltage_CH1 = (((ADC_Show[i + Tri_Pos][0] * 3.3) / 4096.0) - 1.645875) / 0.33;
				float Voltage_CH2 = (((ADC_Show[i + Tri_Pos][1] * 3.3) / 4096.0) - 1.645875) / 0.33;
				
				Show_Buffer[i][Show_Cnt][0] = (int32_t)(Voltage_CH1 * (23.0 / Vlotage_Bisic[Voltage_Div])) + 114;//通道1绘图数据
				Show_Buffer[i][Show_Cnt][1] = (int32_t)(Voltage_CH2 * (23.0 / Vlotage_Bisic[Voltage_Div])) + 114;//通道2绘图数据
				
				//限值->上限与下限
				if(Show_Buffer[i][Show_Cnt][0] < 23)	Show_Buffer[i][Show_Cnt][0] = 23;
				if(Show_Buffer[i][Show_Cnt][1] < 23)	Show_Buffer[i][Show_Cnt][1] = 23;
				if(Show_Buffer[i][Show_Cnt][0] > 205)	Show_Buffer[i][Show_Cnt][0] = 205;
				if(Show_Buffer[i][Show_Cnt][1] > 205)	Show_Buffer[i][Show_Cnt][1] = 205;
			}
			
			//绘图
			for(uint16_t i = 0;i < 228;i++)
			{
				Oscilloscope_DrawLine(i + 6,Show_Buffer[i][Show_Cnt ? 0 : 1][0],i + 7,Show_Buffer[i + 1][Show_Cnt ? 0 : 1][0],BLACK);
				Oscilloscope_DrawLine(i + 6,Show_Buffer[i][Show_Cnt ? 0 : 1][1],i + 7,Show_Buffer[i + 1][Show_Cnt ? 0 : 1][1],BLACK);
				LCD_DrawLine(i + 6,Show_Buffer[i][Show_Cnt][0],i + 7,Show_Buffer[i + 1][Show_Cnt][0],YELLOW);
				LCD_DrawLine(i + 6,Show_Buffer[i][Show_Cnt][1],i + 7,Show_Buffer[i + 1][Show_Cnt][1],CYAN);
			}
			Show_Cnt = ((Show_Cnt) ? (0) : (1));
		}
		else
		{
			Scan_Cnt_Buffer = Scan_Cnt;
			//生成绘图数据
			for(uint16_t i = 0;i < Scan_Cnt_Buffer;i++)
			{
				float Voltage_CH1 = (((ADC_Scan_Samp[i][0] * 3.3) / 4096.0) - 1.645875) / 0.33;
				float Voltage_CH2 = (((ADC_Scan_Samp[i][1] * 3.3) / 4096.0) - 1.645875) / 0.33;
				
				Show_Buffer[i][0][0] = (int32_t)(Voltage_CH1 * (23.0 / Vlotage_Bisic[Voltage_Div])) + 114;//通道1绘图数据
				Show_Buffer[i][0][1] = (int32_t)(Voltage_CH2 * (23.0 / Vlotage_Bisic[Voltage_Div])) + 114;//通道2绘图数据
				
				//限值->上限与下限
				if(Show_Buffer[i][0][0] < 23)		Show_Buffer[i][0][0] = 23;
				if(Show_Buffer[i][0][1] < 23)		Show_Buffer[i][0][1] = 23;
				if(Show_Buffer[i][0][0] > 205)	Show_Buffer[i][0][0] = 205;
				if(Show_Buffer[i][0][1] > 205)	Show_Buffer[i][0][1] = 205;
			}
			
			//绘图
			if(Scan_Cnt_Buffer > 2)
			{
				for(uint16_t i = 0;i < Scan_Cnt_Buffer - 2;i++)
				{
					if(i < Last_Cnt - 2)
					{
						Oscilloscope_DrawLine(i + 6,Show_Buffer[i][1][0],i + 7,Show_Buffer[i + 1][1][0],BLACK);
						Oscilloscope_DrawLine(i + 6,Show_Buffer[i][1][1],i + 7,Show_Buffer[i + 1][1][1],BLACK);
					}
					LCD_DrawLine(i + 6,Show_Buffer[i][0][0],i + 7,Show_Buffer[i + 1][0][0],YELLOW);
					LCD_DrawLine(i + 6,Show_Buffer[i][0][1],i + 7,Show_Buffer[i + 1][0][1],CYAN);
				}
				
				for(uint16_t i = 0;i < 230;i++)
				{
					Show_Buffer[i][1][0] = Show_Buffer[i][0][0];
					Show_Buffer[i][1][1] = Show_Buffer[i][0][1];
				}
			}
			Last_Cnt = Scan_Cnt_Buffer;
		}
		ADC_Samp_Ok = 0;
	}
}

void Oscilloscope_Show_Information(uint8_t Select)
{
	static uint32_t Oscilloscope_Show_Tick = 0;
	
	char Str[31] = {0},Str_Buffer[31] = {0};
	char Time_Str[18][6] = 
	{
		"50s","20s","10s","5s","2s","1s","500ms","200ms","100ms","50ms","20ms",
		"10ms","5ms","2ms","1ms","500us","200us","100us"
	};
	char Voltage_Str[7][6] = {"50mV","100mV","200mV","500mV","1V","2V","5V"};
	double Max_Voltage[2] = {0},Min_Voltage[2] = {0},Wave_Freq[2] = {0};
	
	
	for(uint8_t i = 0;i < 2;i++)
	{
		Max_Voltage[i] = -((((ADC_Max[i] * 3.3) / 4096.0) - 1.645875) / 0.33);
		Min_Voltage[i] = -((((ADC_Min[i] * 3.3) / 4096.0) - 1.645875) / 0.33);
	}
	
	//打印电压刻度
	sprintf(Str,"%-5s",Voltage_Str[Voltage_Div]);
	if(Select == 1)
		Show_Str(5,4,BLACK,WHITE,Str,0);
	else
		Show_Str(5,4,WHITE,BLACK,Str,0);
	
	//打印时间刻度
	sprintf(Str,"%5s",Time_Str[Time_Div]);
	if(Select == 2)
		Show_Str(195,4,BLACK,WHITE,Str,0);
	else
		Show_Str(195,4,WHITE,BLACK,Str,0);
	
	
	if(HAL_GetTick() >= Oscilloscope_Show_Tick)//200ms更新一次示波器信息
	{
		Oscilloscope_Show_Tick = HAL_GetTick() + 200;
		
		//因输入反相 所以最大值实际是最小值
		//显示最大值
		for(uint8_t i = 0;i < 2;i++)
		{
			if(Min_Voltage[i] >= 1.0 || Min_Voltage[i] <= -1.0)
				sprintf(Str,"Max=%.1lfV",Min_Voltage[i]);
			else
				sprintf(Str,"Max=%.0lfmV",Min_Voltage[i] * 1000.0);
			sprintf(Str_Buffer,"%-10s",Str);
			
			if(i == 0)
				Show_Str(5,207,YELLOW,BLACK,Str_Buffer,0);
			else
				Show_Str(5,224,CYAN,BLACK,Str_Buffer,0);
		}
		
		//显示最小值
		for(uint8_t i = 0;i < 2;i++)
		{
			if(Max_Voltage[i] >= 1.0 || Max_Voltage[i] <= -1.0)
				sprintf(Str,"Min=%.1lfV",Max_Voltage[i]);
			else
				sprintf(Str,"Min=%.0lfmV",Max_Voltage[i] * 1000.0);
			sprintf(Str_Buffer,"%-10s",Str);
			
			if(i == 0)
				Show_Str(88,207,YELLOW,BLACK,Str_Buffer,0);
			else
				Show_Str(88,224,CYAN,BLACK,Str_Buffer,0);
		}
		
		//显示频率
		for(uint8_t i = 0;i < 2;i++)
		{
			if(ADC_Cycle[i])
				Wave_Freq[i] = 1.0 / ADC_Cycle[i];
			else
				Wave_Freq[i] = 0;
			
			if(Wave_Freq[i])
			{
				if(Wave_Freq[i] >= 1000000)
					sprintf(Str,"F=%.1lfM",Wave_Freq[i] / 1000000.0);
				else if(Wave_Freq[i] >= 1000)
					sprintf(Str,"F=%.1lfK",Wave_Freq[i] / 1000.0);
				else if(Wave_Freq[i] > 0.99)
					sprintf(Str,"F=%.0lfHz",Wave_Freq[i]);
				else
					sprintf(Str,"F=%.2lfHz",Wave_Freq[i]);
				sprintf(Str_Buffer,"%-9s",Str);
			}
			else
				sprintf(Str_Buffer,"F=  --  ");
			
			if(i == 0)
				Show_Str(171,207,YELLOW,BLACK,Str_Buffer,0);
			else
				Show_Str(171,224,CYAN,BLACK,Str_Buffer,0);
		}
	}
}

//滚动波形处理 获取频率等信息
void Scan_Wave_Handle(void)
{
	uint16_t Tri_Pos_Buffer[2] = {0};
	uint8_t Tri_Flag[2] = {0};
	
	memset(ADC_Cycle,0,sizeof(double) * 2);
	ADC_Max[0] = 0;ADC_Max[1] = 0;
	ADC_Min[0] = 4095;ADC_Min[1] = 4095;
	
	for(uint16_t i = 0;i < 230;i++)
	{
		if(ADC_Scan_Samp[i][0] > ADC_Max[0])	ADC_Max[0] = ADC_Scan_Samp[i][0];
		if(ADC_Scan_Samp[i][1] > ADC_Max[1])	ADC_Max[1] = ADC_Scan_Samp[i][1];
		if(ADC_Scan_Samp[i][0] < ADC_Min[0])	ADC_Min[0] = ADC_Scan_Samp[i][0];
		if(ADC_Scan_Samp[i][1] < ADC_Min[1])	ADC_Min[1] = ADC_Scan_Samp[i][1];
	}
	
	for(uint8_t Ch = 0;Ch < 2;Ch++)
	{
		for(uint16_t i = 0;i < 230;i++)
		{
			
			if(i < ADC_Length - 2 && i > 1)//查找触发位置
			{
				if(ADC_Scan_Samp[i - 1][Ch] >= Tri_Voltage && ADC_Scan_Samp[i + 1][Ch] <= Tri_Voltage)//检测到上升沿
				{
					if(ADC_Scan_Samp[i - 2][Tri_Source] >= Tri_Voltage && ADC_Scan_Samp[i + 2][Tri_Source] <= Tri_Voltage)
					{
						if(Tri_Flag[Ch] == 0)
						{
							Tri_Pos_Buffer[Ch] = i;
							Tri_Flag[Ch] = 1;
						}
						else if(Tri_Flag[Ch] == 2)
						{
							ADC_Cycle[Ch] = ((Time_Arr[Time_Div] * 10.0) / 229.0) * (i - Tri_Pos_Buffer[Ch]);
							break;
						}
					}
				}
				else if(ADC_Scan_Samp[i - 1][Ch] <= Tri_Voltage && ADC_Scan_Samp[i + 1][Ch] >= Tri_Voltage && Tri_Flag[Ch] == 1)//检测到下降沿
					if(ADC_Scan_Samp[i - 2][Tri_Source] <= Tri_Voltage && ADC_Scan_Samp[i + 2][Tri_Source] >= Tri_Voltage)
						Tri_Flag[Ch] = 2;
				
			}
		}
	}
}

void Oscilloscope_Interrupt(void)
{
	if(Scan_Show == 1)
	{
		if(Scan_Cnt >= 230)
		{
			for(uint16_t i = 0;i < 229;i++)
			{
				ADC_Scan_Samp[i][0] = ADC_Scan_Samp[i + 1][0];
				ADC_Scan_Samp[i][1] = ADC_Scan_Samp[i + 1][1];
			}
			
			Scan_Cnt = 229;
		}
		
		ADC_Scan_Samp[Scan_Cnt][0] = ADC_Scan_Value[0];
		ADC_Scan_Samp[Scan_Cnt][1] = ADC_Scan_Value[1];
		
		ADC_Samp_Ok = 1;
		Scan_Cnt = ((Scan_Cnt < 230) ? (Scan_Cnt + 1) : (Scan_Cnt));
		if(Scan_Cnt >= 230)
		{
			Scan_Wave_Handle();
		}
	}
}

void Oscilloscope_Clean(void)
{
	Oscilloscope_Clean_Flag = 1;
	
	Scan_Cnt = 0;//滚动坐标
	Scan_Cnt_Buffer = 0;
	for(uint8_t i = 0;i < 230;i++)
		for(uint8_t j = 0;j < 2;j++)
			for(uint8_t k = 0;k < 2;k++)
				Show_Buffer[i][j][k] = 114;
	memset(ADC_Scan_Samp,0,sizeof(ADC_Scan_Samp));
}
/*--------------------------------------------------Oscilloscope--------------------------------------------------*/




/*--------------------------------------------------Wave Generate--------------------------------------------------*/
Wave_Para Wave_Info = {Sine,10000,2,0,50};
uint8_t Wave_Vpp_Select = 0;
uint8_t Wave_Freq_Select = 0;
uint8_t Wave_Offset_Select = 0;

uint8_t Wave_Freq_Level = 0;//输出频率挡位
uint8_t Freq_Max_Num = 0;


uint8_t Str_Get_Num(char *Str)
{
	uint8_t Num_Count = 0;
	for(uint8_t i = 0;i < strlen(Str);i++)
	{
		if(Str[i] >= '0' && Str[i] <= '9')
			Num_Count++;
	}
	
	return Num_Count;
}

uint8_t Get_Str_Num_Pos(char *Str,uint8_t Num_Pos)
{
	uint8_t Num_Count = 0;
	
	for(uint8_t i = 0;i < strlen(Str);i++)
	{
		if(Str[i] >= '0' && Str[i] <= '9')
		{
			if(Num_Count == Num_Pos)
				return i;
			Num_Count++;
		}
	}
	
	return 0xFF;
}


void Wave_Generate_UI(void)
{
	Show_Str(48,4,WHITE,BLACK,"Function Generator",0);
	Draw_Circle_Square(5,25,234,234,20,WHITE);
	LCD_DrawLine(6,111,233,111,WHITE);
	LCD_DrawLine(6,152,233,152,WHITE);
	LCD_DrawLine(6,193,233,193,WHITE);
	
	Show_Str(15,123,WHITE,BLACK,"Amplitude",0);
	Show_Str(15,164,WHITE,BLACK,"Frequency",0);
	Show_Str(15,205,WHITE,BLACK,"DC Offset",0);
}

void Wave_Show_Information(uint8_t Wave_Change)
{
	char Str[31] = {0};
	char Str_Buffer[31] = {0};
	
	static uint8_t Last_Freq_Level = 0;
	static uint8_t Last_Freq_Max = 0;
	
	//Set Wave
	if(Wave_Info.Wave == Saw_Tooth)
		Wave_Saw_Tooth_Pic(ORANGE,(Wave_Change == 0) ? (WHITE) : (GRAY));
	else
		Wave_Saw_Tooth_Pic(ORANGE,BLACK);
	
	if(Wave_Info.Wave == Square)
		Wave_Square_Pic(ORANGE,(Wave_Change == 0) ? (WHITE) : (GRAY));
	else
		Wave_Square_Pic(ORANGE,BLACK);
	
	if(Wave_Info.Wave == Sine)
		Wave_Sine_Pic(ORANGE,(Wave_Change == 0) ? (WHITE) : (GRAY));
	else
		Wave_Sine_Pic(ORANGE,BLACK);
	
	//Set Vpp
	if(Wave_Info.VPP <= 0.0001 && Wave_Info.VPP >= -0.0001)
		Wave_Info.VPP = 0;
	
	sprintf(Str,"%.1lfV",Wave_Info.VPP);
	sprintf(Str_Buffer,"%-5s",Str);
	if(Wave_Change == 1)
	{
		for(uint8_t i = 0;i < strlen(Str_Buffer);i++)
		{
			if(Get_Str_Num_Pos(Str_Buffer,Wave_Vpp_Select) == i)
				LCD_ShowChar(100 + i * 8,123,BLACK,ORANGE,Str_Buffer[i],0);
			else
				LCD_ShowChar(100 + i * 8,123,ORANGE,BLACK,Str_Buffer[i],0);
		}
	}
	else
		Show_Str(100,123,ORANGE,BLACK,Str,0);
	
	//Set Freq
	if(Wave_Info.Freq < 1.0)
	{
		Wave_Freq_Level = 0;
		sprintf(Str,"%.1lfHz",Wave_Info.Freq);
	}
	else if(Wave_Info.Freq < 1000.0)
	{
		Wave_Freq_Level = 1;
		Wave_Info.Freq = (uint32_t)Wave_Info.Freq;
		sprintf(Str,"%.0lfHz",Wave_Info.Freq);
	}
	else if(Wave_Info.Freq < 5000.0)
	{
		Wave_Freq_Level = 2;
		Wave_Info.Freq = ((uint32_t)(Wave_Info.Freq / 10.0) * 1000.0) / 100.0;//去除超出的小数部分
		sprintf(Str,"%.2lfkHz",Wave_Info.Freq / 1000.0);
	}
	else if(Wave_Info.Freq < 50000.0)
	{
		Wave_Freq_Level = 3;
		Wave_Info.Freq = ((uint32_t)(Wave_Info.Freq / 100.0) * 1000.0) / 10.0;//去除超出的小数部分
		sprintf(Str,"%.1lfkHz",Wave_Info.Freq / 1000.0);
	}
	else
	{
		Wave_Freq_Level = 4;
		Wave_Info.Freq = (uint32_t)(Wave_Info.Freq / 1000.0) * 1000.0;//去除超出的小数部分
		sprintf(Str,"%.0lfkHz",Wave_Info.Freq / 1000.0);
	}
	sprintf(Str_Buffer,"%-15s",Str);
	
	Freq_Max_Num = Str_Get_Num(Str_Buffer) - 1;
	
	if(Last_Freq_Max != Freq_Max_Num)
	{
		Last_Freq_Max = Freq_Max_Num;
		if(Last_Freq_Level != Wave_Freq_Level)
			Last_Freq_Level = Wave_Freq_Level;
		else
			Wave_Freq_Select = 0;
	}
	if(Last_Freq_Level != Wave_Freq_Level)
	{
		Last_Freq_Level = Wave_Freq_Level;
		Wave_Freq_Select = 0;
	}
	
	if(Wave_Change == 2)
	{
		for(uint8_t i = 0;i < strlen(Str_Buffer);i++)
		{
			if(Get_Str_Num_Pos(Str_Buffer,Wave_Freq_Select) == i)
				LCD_ShowChar(100 + i * 8,164,BLACK,ORANGE,Str_Buffer[i],0);
			else
				LCD_ShowChar(100 + i * 8,164,ORANGE,BLACK,Str_Buffer[i],0);
		}
	}
	else
		Show_Str(100,164,ORANGE,BLACK,Str,0);
	
	//Set Offset
	if(Wave_Info.Offset <= 0.0001 && Wave_Info.Offset >= -0.0001)
		Wave_Info.Offset = 0;
	
	sprintf(Str,"%.1lfV",Wave_Info.Offset);
	sprintf(Str_Buffer,"%-5s",Str);
	
	if(Wave_Change == 3)
	{
		for(uint8_t i = 0;i < strlen(Str_Buffer);i++)
		{
			if(Get_Str_Num_Pos(Str_Buffer,Wave_Offset_Select) == i)
				LCD_ShowChar(100 + i * 8,205,BLACK,ORANGE,Str_Buffer[i],0);
			else
				LCD_ShowChar(100 + i * 8,205,ORANGE,BLACK,Str_Buffer[i],0);
		}
	}
	else
		Show_Str(100,205,ORANGE,BLACK,Str,0);
}
/*--------------------------------------------------Wave Generate--------------------------------------------------*/




/*--------------------------------------------------PWM Generate--------------------------------------------------*/
PWM_Type PWM_Info = {1000,0.5};

uint8_t PWM_Freq_Select = 0;
uint8_t PWM_Duty_Select = 0;

uint8_t PWM_Freq_Level = 0;//输出频率挡位
uint8_t PWM_Freq_Max_Num = 0;

void Draw_PWM_Picture(float Duty,uint16_t Color)
{
	uint16_t High_Length = Duty * 120;
	uint16_t Low_Length = 120 - High_Length;
	
	LCD_DrawLine(92,48,92,108,Color);//上升沿
	LCD_DrawLine(92,48,92 + High_Length,48,Color);//高电平
	LCD_DrawLine(92 + High_Length,48,92 + High_Length,108,Color);//下降沿
	LCD_DrawLine(92 + High_Length,108,92 + High_Length + Low_Length,108,Color);//低电平
	LCD_DrawLine(212,48,212,108,Color);//上升沿
	
	LCD_DrawLine(85,108,92,108,Color);
	LCD_DrawLine(212,48,219,48,Color);
}

void PWM_Generate_UI(void)
{
	Show_Str(80,4,WHITE,BLACK,"PWM Signal",0);
	Draw_Circle_Square(5,25,234,234,10,WHITE);
	LCD_DrawLine(6,130,233,130,WHITE);
	
	Show_Str(15,153,WHITE,BLACK,"Frequency",0);
	Show_Str(15,195,WHITE,BLACK,"Duty Cycle",0);
	
	Show_Str(10,40,WHITE,BLACK,"3.3V",0);
	Show_Str(10,100,WHITE,BLACK,"  0V",0);
	
	for(uint8_t i = 45;i < 75;i++)
	{
		if(i % 6 >= 3)
		{
			LCD_DrawPoint(i,48,WHITE);//画点
			LCD_DrawPoint(i,108,WHITE);
		}
	}
	
	Draw_PWM_Picture(PWM_Info.Duty,WHITE);
}

void PWM_Show_Information(uint8_t Select)
{
	char Str[31] = {0};
	char Str_Buffer[31] = {0};
	
	static uint8_t Last_Freq_Level = 0;
	static uint8_t Last_Freq_Max = 0;
	
	//Set Freq
	if(PWM_Info.Freq < 1.0)
	{
		PWM_Freq_Level = 0;
		sprintf(Str,"%.1lfHz",PWM_Info.Freq);
	}
	else if(PWM_Info.Freq < 1000.0)
	{
		PWM_Freq_Level = 1;
		PWM_Info.Freq = (uint32_t)PWM_Info.Freq;
		sprintf(Str,"%.0lfHz",PWM_Info.Freq);
	}
	else if(PWM_Info.Freq < 10000.0)
	{
		PWM_Freq_Level = 2;
		PWM_Info.Freq = ((uint32_t)(PWM_Info.Freq / 10.0) * 1000.0) / 100.0;//去除超出的小数部分
		sprintf(Str,"%.2lfkHz",PWM_Info.Freq / 1000.0);
	}
	else if(PWM_Info.Freq < 50000.0)
	{
		PWM_Freq_Level = 3;
		PWM_Info.Freq = ((uint32_t)(PWM_Info.Freq / 100.0) * 1000.0) / 10.0;//去除超出的小数部分
		sprintf(Str,"%.1lfkHz",PWM_Info.Freq / 1000.0);
	}
	else
	{
		PWM_Freq_Level = 4;
		PWM_Info.Freq = (uint32_t)(PWM_Info.Freq / 1000.0) * 1000.0;//去除超出的小数部分
		sprintf(Str,"%.0lfkHz",PWM_Info.Freq / 1000.0);
	}
	sprintf(Str_Buffer,"%-15s",Str);
	
	PWM_Freq_Max_Num = Str_Get_Num(Str_Buffer) - 1;
	
	if(Last_Freq_Max != PWM_Freq_Max_Num)
	{
		Last_Freq_Max = PWM_Freq_Max_Num;
		if(Last_Freq_Level != PWM_Freq_Level)
			Last_Freq_Level = PWM_Freq_Level;
		else
			PWM_Freq_Select = 0;
	}
	if(Last_Freq_Level != PWM_Freq_Level)
	{
		Last_Freq_Level = PWM_Freq_Level;
		PWM_Freq_Select = 0;
	}
	
	if(Select == 1)
	{
		for(uint8_t i = 0;i < strlen(Str_Buffer);i++)
		{
			if(Get_Str_Num_Pos(Str_Buffer,PWM_Freq_Select) == i)
				LCD_ShowChar(100 + i * 8,153,BLACK,VIOLET,Str_Buffer[i],0);
			else
				LCD_ShowChar(100 + i * 8,153,VIOLET,BLACK,Str_Buffer[i],0);
		}
	}
	else
		Show_Str(100,153,VIOLET,BLACK,Str,0);
	
	sprintf(Str,"%.0lf%%",PWM_Info.Duty * 100.0);
	sprintf(Str_Buffer,"%-15s",Str);
	//Set Duty
	if(Select == 2)
	{
		for(uint8_t i = 0;i < strlen(Str_Buffer);i++)
		{
			if(Get_Str_Num_Pos(Str_Buffer,PWM_Duty_Select) == i)
				LCD_ShowChar(100 + i * 8,195,BLACK,VIOLET,Str_Buffer[i],0);
			else
				LCD_ShowChar(100 + i * 8,195,VIOLET,BLACK,Str_Buffer[i],0);
		}
	}
	else
		Show_Str(100,195,VIOLET,BLACK,Str,0);
}
/*--------------------------------------------------PWM Generate--------------------------------------------------*/




/*----------------------------------------------------Power DC----------------------------------------------------*/
uint8_t Power_Select = 0;
uint8_t Power_voltage_Select = 0;
uint8_t Power_Out_Select = 0;

float DC1_Voltage = -1.6;
float DC2_Voltage = 2.3;
uint8_t DC1_State = 1;
uint8_t DC2_State = 1;

void Power_DC_UI(void)
{
	Draw_Circle_Square(0,0,239,239,20,WHITE);
	LCD_DrawLine(119,0,119,239,WHITE);
	LCD_DrawLine(0,90,239,90,WHITE);
	
	Show_60x60_Char(16,20,RED,BLACK,'D');
	Show_60x60_Char(46,20,RED,BLACK,'C');
	Show_60x60_Char(76,20,RED,BLACK,'1');
	
	Show_60x60_Char(135,20,BLUE,BLACK,'D');
	Show_60x60_Char(165,20,BLUE,BLACK,'C');
	Show_60x60_Char(195,20,BLUE,BLACK,'2');
}

void Power_Show_Information(uint8_t Select)
{
	char Str[31] = {0};
	char Str_Buffer[31] = {0};
	uint16_t X_Start = 0;
	
	Print_Delta(45,8,15,(Select == 1) ? ((Power_Select == 0) ? (WHITE) : (BLACK)) : (BLACK));
	Print_Delta(165,8,15,(Select == 1) ? ((Power_Select == 1) ? (WHITE) : (BLACK)) : (BLACK));
	
	sprintf(Str,"%.1lfV",DC1_Voltage);
	if(DC1_Voltage < 0)
	{
		sprintf(Str_Buffer,"%s",Str);//居中
		X_Start = 20;
	}
	else
	{
		sprintf(Str_Buffer," %s ",Str);//居中
		X_Start = 11;
	}
	
	if(Select == 2 && Power_Select == 0)//选中DC1电压
	{
		for(uint8_t i = 0;i < strlen(Str_Buffer);i++)
		{
			if(Get_Str_Num_Pos(Str_Buffer,Power_voltage_Select) == i)
				Show_16x32_Char(X_Start + i * 16,111,BLACK,RED,Str_Buffer[i]);
			else
				Show_16x32_Char(X_Start + i * 16,111,RED,BLACK,Str_Buffer[i]);
		}
	}
	else
		Show_16x32_Str(X_Start,111,RED,BLACK,Str_Buffer);
	
	
	
	sprintf(Str,"%.1lfV",DC2_Voltage);
	if(DC2_Voltage < 0)
	{
		sprintf(Str_Buffer,"%s",Str);//居中
		X_Start = 140;
	}
	else
	{
		sprintf(Str_Buffer," %s ",Str);//居中
		X_Start = 131;
	}
	
	if(Select == 2 && Power_Select == 1)//选中DC2电压
	{
		for(uint8_t i = 0;i < strlen(Str_Buffer);i++)
		{
			if(Get_Str_Num_Pos(Str_Buffer,Power_voltage_Select) == i)
				Show_16x32_Char(X_Start + i * 16,111,BLACK,BLUE,Str_Buffer[i]);
			else
				Show_16x32_Char(X_Start + i * 16,111,BLUE,BLACK,Str_Buffer[i]);
		}
	}
	else
		Show_16x32_Str(X_Start,111,BLUE,BLACK,Str_Buffer);
	
	
	if(Select == 3)
	{
		if(Power_Select == 0)
		{
			Show_16x32_Str(12,186,((DC1_State) ? (WHITE) : (GRAY)),BLACK,"ON");
			Show_16x32_Str(60,186,((DC1_State) ? (GRAY) : (WHITE)),BLACK,"OFF");
		}
		else if(Power_Select == 1)
		{
			Show_16x32_Str(132,186,((DC2_State) ? (WHITE) : (GRAY)),BLACK,"ON");
			Show_16x32_Str(180,186,((DC2_State) ? (GRAY) : (WHITE)),BLACK,"OFF");
		}
	}
	else
	{
		Show_16x32_Str(132,186,((DC2_State) ? (BLUE) : (GRAY)),BLACK,"ON");
		Show_16x32_Str(180,186,((DC2_State) ? (GRAY) : (BLUE)),BLACK,"OFF");
		
		Show_16x32_Str(12,186,((DC1_State) ? (RED) : (GRAY)),BLACK,"ON");
		Show_16x32_Str(60,186,((DC1_State) ? (GRAY) : (RED)),BLACK,"OFF");
	}
}
/*----------------------------------------------------Power DC----------------------------------------------------*/


/*----------------------------------------------------Spectrum----------------------------------------------------*/
void Spectrum_UI(void)
{
	Show_Str(88,4,WHITE,BLACK,"Spectrum",0);
	LCD_DrawRectangle(5,22,235,206,WHITE);
	
	LCD_Fill(6,23,234,205,BLACK);
	for(uint8_t i = 1;i < 10;i++)
	{
		LCD_DrawLine (5 + (i * 23), 23 , 5 + (i * 23), 205, GRAY);
		if(i < 8)
			LCD_DrawLine (6, 22 + (i * 23), 234, 22 + (i * 23), GRAY);
	}
	LCD_DrawLine (6, 114, 234, 114, LGRAY);
}

void Spectrum_Init(void)
{
	FFT_Samp_Init();
	Spectrum_UI();
}

void Spectrum_Show_Wave(void)
{
	if(ADC_Samp_Ok)
	{
		Process_FFT_Data();//处理FFT数据
		for(uint16_t i = 0;i < 228;i++)
		{
			Oscilloscope_DrawLine(i + 6,Show_Buffer[i][Show_Cnt ? 0 : 1][0],i + 7,Show_Buffer[i + 1][Show_Cnt ? 0 : 1][0],BLACK);
			Oscilloscope_DrawLine(i + 6,Show_Buffer[i][Show_Cnt ? 0 : 1][1],i + 7,Show_Buffer[i + 1][Show_Cnt ? 0 : 1][1],BLACK);
			LCD_DrawLine(i + 6,Show_Buffer[i][Show_Cnt][0],i + 7,Show_Buffer[i + 1][Show_Cnt][0],YELLOW);
			LCD_DrawLine(i + 6,Show_Buffer[i][Show_Cnt][1],i + 7,Show_Buffer[i + 1][Show_Cnt][1],CYAN);
		}
		ADC_Samp_Ok = 0;
		Show_Cnt = ((Show_Cnt) ? (0) : (1));
	}
}

void Spectrum_Show_Information(uint8_t Select)
{
	char Str[31] = {0},Str_Buffer[31] = {0};
	char Freq_Str[8][7] = { "100Hz","500Hz","1kHz","5kHz","10kHz","50kHz","100kHz","250kHz"};
	char Freq_Div_Str[8][10] = 
	{
		"10Hz/Div","50Hz/Div","100Hz/Div","500Hz/Div","1kHz/Div","5kHz/Div","10kHz/Div","25kHz/Div"
	};
	char Voltage_Str[7][6] = {"50mV","100mV","200mV","500mV","1V","2V","5V"};
	
	//打印电压刻度
	sprintf(Str,"%-5s",Voltage_Str[Freq_Voltage_Div]);
	if(Select == 1)
		Show_Str(5,4,BLACK,WHITE,Str,0);
	else
		Show_Str(5,4,WHITE,BLACK,Str,0);
	
	//打印时间刻度
	sprintf(Str,"%6s",Freq_Str[Freq_Div]);
	if(Select == 2)
		Show_Str(187,4,BLACK,WHITE,Str,0);
	else
		Show_Str(187,4,WHITE,BLACK,Str,0);
	
	sprintf(Str,"%-10s",Freq_Div_Str[Freq_Div]);
	Show_Str(5,207,WHITE,BLACK,Str,0);
	
	//打印通道1频率
	if(FFT_Freq[0] >= 1000000)
		sprintf(Str,"Freq=%.1lfMHz",FFT_Freq[0] / 1000000.0);
	else if(FFT_Freq[0] >= 1000)
		sprintf(Str,"Freq=%.1lfkHz",FFT_Freq[0] / 1000.0);
	else if(FFT_Freq[0] > 0.99)
		sprintf(Str,"Freq=%.0lfHz",FFT_Freq[0]);
	else
		sprintf(Str,"Freq=%.2lfHz",FFT_Freq[0]);
	sprintf(Str_Buffer,"%-14s",Str);
	
	Show_Str(5,224,YELLOW,BLACK,Str_Buffer,0);
	
	//打印通道2频率
	if(FFT_Freq[1] >= 1000000)
		sprintf(Str,"Freq=%.1lfMHz",FFT_Freq[1] / 1000000.0);
	else if(FFT_Freq[1] >= 1000)
		sprintf(Str,"Freq=%.1lfkHz",FFT_Freq[1] / 1000.0);
	else if(FFT_Freq[1] > 0.99)
		sprintf(Str,"Freq=%.0lfHz",FFT_Freq[1]);
	else
		sprintf(Str,"Freq=%.2lfHz",FFT_Freq[1]);
	sprintf(Str_Buffer,"%-14s",Str);
	
	Show_Str(120,224,CYAN,BLACK,Str_Buffer,0);
}
/*----------------------------------------------------Spectrum----------------------------------------------------*/












