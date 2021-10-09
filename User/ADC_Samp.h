#ifndef __ADC_Samp_H
#define __ADC_Samp_H

#define ADC_Channel		2
#define ADC_Length		700

typedef enum
{
	Edge_Fall,
	Edge_Rise,
}ADC_Edge;

typedef enum
{
	Ch1,
	Ch2,
}ADC_Ch;

#include "User.h"

extern uint16_t ADC_Show[ADC_Length][ADC_Channel];
extern uint16_t ADC_Scan_Value[ADC_Channel];
extern uint16_t ADC_Max[ADC_Channel];
extern uint16_t ADC_Min[ADC_Channel];
extern uint32_t ADC_Sum[ADC_Channel];

extern ADC_Edge Tri_Edge;//������Ե
extern ADC_Ch Tri_Source;//����Դ

extern uint16_t Tri_Voltage;//������ѹ
extern uint16_t Best_Tri;//��Ѵ�����ѹ VPP/2
extern uint16_t Tri_Pos;//����λ��
extern uint8_t ADC_Samp_Ok;//ADC�ɼ���ɱ�־

extern uint8_t Time_Div;//ʾ����ʱ��̶�
extern uint8_t Voltage_Div;//ʾ������ѹ�̶�
extern uint8_t Freq_Div;//Ƶ��Ƶ�ʿ̶�
extern uint8_t Freq_Voltage_Div;//Ƶ�׵�ѹ�̶�

extern double ADC_Cycle[ADC_Channel];//���벨������

extern uint8_t Scan_Show;

extern double Time_Arr[18];
extern uint8_t Un_Scan_Init;

extern uint32_t FFT_Select[8];

void ADC_Samp_Init(void);
void Set_ADC_Samp_Rate(uint16_t Rate);

void FFT_Samp_Init(void);
void Set_FFT_Samp_Rate(uint8_t Rate);
void FFT_Deinit(void);

void ADC_DMA_Finish(DMA_HandleTypeDef *hdma);

#endif
