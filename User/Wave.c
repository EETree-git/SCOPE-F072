#include "Wave.h"

extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

#define User_PI	3.1415926 / 180.0

/*
	Freq < 10k    			360点
	10k < Freq < 20k		180点
	20k < Freq < 50k		80点
	50k < Freq < 100k		40点
*/

uint16_t Wave_Result[360] = {0};//最终输出数组
int16_t Wave_Buffer[360] = {0};
uint16_t Wave_Point = 40;

void Get_TIM_Config(float Freq,uint16_t Point_Num,uint16_t *PSC,uint16_t *ARR)
{
	uint32_t Basic_Freq = System_Clock;
	uint32_t Prescaler = 1;
	
	Freq *= Point_Num;
	
	while((Basic_Freq / Prescaler) / Freq > 65535)
	{
		do
		{
			Prescaler++;
		}while(Basic_Freq % Prescaler);
	}
	
	*PSC = Prescaler;
	*ARR = (Basic_Freq / Prescaler) / Freq;
}

void Set_TIM3_Freq(float Wave_Freq)
{
	float Best_Freq = 100000,Now_Freq = 0;
	uint16_t PSC_Buffer = 0,ARR_Buffer = 0,Point_Max = 0;
	
	if(Wave_Freq <= 10000)
		Point_Max = 360;
	else if(Wave_Freq <= 20000)
		Point_Max = 180;
	else if(Wave_Freq <= 50000)
		Point_Max = 80;
	else
		Point_Max = 40;
	
	for(uint16_t i = Point_Max;i > Point_Max / 2;i--)
	{
		Get_TIM_Config(Wave_Freq,i,&PSC_Buffer,&ARR_Buffer);
		Now_Freq = ((System_Clock / PSC_Buffer) / ARR_Buffer) / i;
		
		if(fabs(Now_Freq - Wave_Freq) < Best_Freq)
		{
			Best_Freq = fabs(Now_Freq - Wave_Freq);
			Wave_Point = i;
		}
	}
	
	Get_TIM_Config(Wave_Freq,Wave_Point,&PSC_Buffer,&ARR_Buffer);
	TIM3->PSC = PSC_Buffer - 1;
	TIM3->ARR = ARR_Buffer - 1;
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void Stop_Generate(void)
{
	HAL_TIM_Base_Stop(&htim3);
	HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
}
/*------------------------------------------------------------------------------------------------------------------------------*/
//方波生成
void Generate_Square(uint16_t Point_Num,float VPP,float Offset,float Duty)
{
	uint16_t Duty_Pos = (uint16_t)((Point_Num * Duty) / 100.0);
	
	//实际测量误差补偿
	if (VPP >= 2.2)
		VPP -= 0.2;
	else if (VPP >= 1.7)
		VPP -= 0.15;
	else if (VPP >= 1.1)
		VPP -= 0.1;
	else if(VPP >= 0.8)
		VPP -= 0.05;
	
	VPP = (((VPP * 2.0) - 4.0125) / (-2.4294)) - 1.65;
	VPP = (4095.0 * VPP) / 3.3;
	
	Offset = ((Offset - 4.0125) / (-2.4294)) - 1.65;
	Offset = (4095.0 * Offset) / 3.3;
	
	for(uint16_t i = 0;i < Point_Num;i++)
	{
		if(i <= Duty_Pos)
			Wave_Buffer[i] = VPP + Offset + 2035 - (VPP / 2.0);
		else
			Wave_Buffer[i] = Offset + 2035 - (VPP / 2.0);
		
		if(Wave_Buffer[i] > 4095)
			Wave_Buffer[i] = 4095;
		if(Wave_Buffer[i] < 0)
			Wave_Buffer[i] = 0;
	}
	for(uint16_t i = 0;i < Point_Num;i++)
		Wave_Result[i] = Wave_Buffer[i];
}

void Set_Square(float Freq,float VPP,float Offset,float Duty)
{
	HAL_TIM_Base_Stop(&htim3);
	HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
	
	Set_TIM3_Freq(Freq);
	Generate_Square(Wave_Point,VPP,Offset,Duty);
	
	HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *)Wave_Result,Wave_Point,DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start(&htim3);
}
/*------------------------------------------------------------------------------------------------------------------------------*/
//三角波生成
void Generate_Saw_Tooth(uint16_t Point_Num,float VPP,float Offset,float Duty)
{
	uint16_t Duty_Pos = (uint16_t)((Point_Num * Duty) / 100.0);
	//实际测量误差补偿
	if(VPP >= 3.4)
		VPP += 0.5;
	else if(VPP >= 2.99)
		VPP += 0.3;
	else if(VPP >= 2.5)
		VPP += 0.2;
	else if(VPP >= 2.2)
		VPP += 0.1;
	else if(VPP >= 1.8)
		VPP += 0.08;
	else if(VPP >= 1.5)
		VPP += 0.06;
	else if(VPP >= 0.8)
		VPP += 0.04;
	else if(VPP >= 0.5)
		VPP += 0.02;
	
	VPP = (((VPP * 2.0) - 4.0125) / (-2.4294)) - 1.65;
	VPP = (4095.0 * VPP) / 3.3;
	
	Offset = ((Offset - 4.0125) / (-2.4294)) - 1.65;
	Offset = (4095.0 * Offset) / 3.3;
	
	for(uint16_t i = 0;i < Point_Num;i++)
	{
		uint16_t Pos = i;
		if(Duty == 0)
		{
			Pos = Point_Num - i - 1;
			Wave_Buffer[Pos] = ((1.0 / (Point_Num - 1.0)) * i * VPP) + Offset + 2035 - (VPP / 2.0);
		}
		else if(Duty == 100)
			Wave_Buffer[i] = ((1.0 / (Point_Num - 1.0)) * i * VPP) + Offset + 2035 - (VPP / 2.0);
		else
		{
			if(i <= Duty_Pos)
				Wave_Buffer[i] = ((1.0 / Duty_Pos) * i * VPP) + Offset + 2035 - (VPP / 2.0);
			else
				Wave_Buffer[i] = (1.0 - ((1.0 / (Point_Num - Duty_Pos - 1.0)) * (i - Duty_Pos))) * VPP + Offset + 2035 - (VPP / 2.0);
		}
		if(Wave_Buffer[Pos] > 4095)
			Wave_Buffer[Pos] = 4095;
		if(Wave_Buffer[Pos] < 0)
			Wave_Buffer[Pos] = 0;
	}
	for(uint16_t i = 0;i < Point_Num;i++)
		Wave_Result[i] = Wave_Buffer[i];
}

void Set_Saw_Tooth(float Freq,float VPP,float Offset,float Duty)
{
	HAL_TIM_Base_Stop(&htim3);
	HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
	
	Set_TIM3_Freq(Freq);
	Generate_Saw_Tooth(Wave_Point,VPP,Offset,Duty);
	
	HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *)Wave_Result,Wave_Point,DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start(&htim3);
}
/*------------------------------------------------------------------------------------------------------------------------------*/
//正弦波生成
void Generate_Sine(uint16_t Point_Num,float VPP,float Offset)
{
	VPP = ((VPP - 4.0125) / (-2.4294)) - 1.65;
	VPP = (4095.0 * VPP) / 3.3;
	
	Offset = ((Offset - 4.0125) / (-2.4294)) - 1.65;
	Offset = (4095.0 * Offset) / 3.3;
	for(uint16_t i = 0;i < Point_Num;i++)
	{
		Wave_Buffer[i] = (sin(User_PI * i * (360.0 / Point_Num)) * VPP) + 2035 + Offset;
		
		if(Wave_Buffer[i] > 4095)
			Wave_Buffer[i] = 4095;
		if(Wave_Buffer[i] < 0)
			Wave_Buffer[i] = 0;
	}
	for(uint16_t i = 0;i < Point_Num;i++)
		Wave_Result[i] = Wave_Buffer[i];
}

void Set_Sine_Wave(float Freq,float VPP,float Offset)
{
	HAL_TIM_Base_Stop(&htim3);
	HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
	
	Set_TIM3_Freq(Freq);
	Generate_Sine(Wave_Point,VPP,Offset);
	
	HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *)Wave_Result,Wave_Point,DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start(&htim3);
}
/*------------------------------------------------------------------------------------------------------------------------------*/
void Set_Wave_Out(Wave_Para Wave)
{
	switch(Wave.Wave)
	{
		case Sine:
			Set_Sine_Wave(Wave.Freq,Wave.VPP,Wave.Offset);
			break;
		
		case Saw_Tooth:
			Set_Saw_Tooth(Wave.Freq,Wave.VPP,Wave.Offset,Wave.Duty);
			break;
		
		case Square:
			Set_Square(Wave.Freq,Wave.VPP,Wave.Offset,Wave.Duty);
			break;
		
		default:
			break;
	}
}

void PWM_Picture_ReDraw(uint8_t State)
{
	static float Last_Duty = 0;
	
	if(Last_Duty != PWM_Info.Duty)
	{
		if(State == PWM_Generate)
		{
			Draw_PWM_Picture(Last_Duty,BLACK);
			Draw_PWM_Picture(PWM_Info.Duty,WHITE);
		}
		
		Last_Duty = PWM_Info.Duty;
	}
}

void PWM_Generate_Out(PWM_Type PWM)
{
	uint32_t Basic_Freq = System_Clock;
	uint32_t Prescaler = 1;
	float ARR = 0;
	
	while((Basic_Freq / Prescaler) / PWM.Freq > 65535)
	{
		do
		{
			Prescaler++;
		}while(Basic_Freq % Prescaler);
	}
	
	ARR = (Basic_Freq / Prescaler) / PWM.Freq;
	
	TIM2->PSC = Prescaler - 1;
	TIM2->ARR = (uint32_t)ARR - 1;
	TIM2->CCR3 = ARR * PWM.Duty;
	TIM2->CNT = 0;
}






