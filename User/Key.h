#ifndef __Key_H
#define __Key_H

#include "User.h"

#define Key_Num		5

#define Long_Press_Time		1000//����ʱ�� (���ʱ��������Key_Ok)
#define Cont_Click_Time		300//����������ʱ
#define Long_Tri					100//����������ÿ��Long_Tri(ms)����һ��
#define Key_Ok						10//��������ʱ��

//ԭ��ͼ KeyLʵ�����ұ� KeyRʵ�������
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
	int8_t Rise;//������ ���´���һ��
	int8_t Fall;//�½��� �ɿ�����һ��
	int8_t Long_Press;//���� ����һ��
	int8_t Long_Tri_Press;//��������������������1
	
	int32_t Click_Cnt;//��������
	int32_t Press_Time;//�������µ��ɿ���ʱ��
}Key_State_Type;

void Get_Key(Key_Type *Key);

int8_t Get_Rise(uint8_t Key_ID);
int8_t Get_Fall(uint8_t Key_ID);
int8_t Get_Long_Press(uint8_t Key_ID);
int8_t Get_Long_Tri(uint8_t Key_ID);
int32_t Get_Cont_Click(uint8_t Key_ID);
int32_t Get_Press_Time(uint8_t Key_ID);

#endif
