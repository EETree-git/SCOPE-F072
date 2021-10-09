#ifndef __Key_H
#define __Key_H

#include "User.h"

#define Key_Num		5

#define Long_Press_Time		1000//长按时间 (这个时间必须大于Key_Ok)
#define Cont_Click_Time		300//连续单击超时
#define Long_Tri					100//长按成立后每隔Long_Tri(ms)触发一次
#define Key_Ok						10//按键消抖时间

//原理图 KeyL实际是右边 KeyR实际是左边
#define Key1	0
#define Key2	1
#define KeyL	4
#define KeyO	3
#define KeyR	2

typedef struct
{
	int8_t State;
	int8_t Flag;
	int32_t Start;
	int32_t Count;
	
	int32_t Cont_Click_Buffer;
	int32_t Cont_Click_Tick;
	int32_t Key_Tri_Tick;
}Key_Type;

typedef struct
{
	int8_t Rise;//上升沿 按下触发一次
	int8_t Fall;//下降沿 松开触发一次
	int8_t Long_Press;//长按 触发一次
	int8_t Long_Tri_Press;//长按成立后连续触发置1
	
	int32_t Click_Cnt;//连续单击
	int32_t Press_Time;//按键按下到松开的时间
}Key_State_Type;

void Get_Key(Key_Type *Key);

int8_t Get_Rise(uint8_t Key_ID);
int8_t Get_Fall(uint8_t Key_ID);
int8_t Get_Long_Press(uint8_t Key_ID);
int8_t Get_Long_Tri(uint8_t Key_ID);
int32_t Get_Cont_Click(uint8_t Key_ID);
int32_t Get_Press_Time(uint8_t Key_ID);

#endif
