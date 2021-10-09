#ifndef __Wave_H
#define __Wave_H

typedef enum
{
	Saw_Tooth,
	Square,
	Sine
}Wave_Type;

typedef struct
{
	Wave_Type Wave;
	float Freq;
	float VPP;
	float Offset;
	float Duty;
}Wave_Para;

typedef struct
{
	float Freq;
	float Duty;
}PWM_Type;

#include "User.h"

void Stop_Generate(void);
void Set_Sine_Wave(float Freq,float VPP,float Offset);
void Set_Saw_Tooth(float Freq,float VPP,float Offset,float Duty);
void Set_Square(float Freq,float VPP,float Offset,float Duty);
void Set_Wave_Out(Wave_Para Wave);

void PWM_Generate_Out(PWM_Type PWM);
void PWM_Picture_ReDraw(uint8_t State);

#endif
