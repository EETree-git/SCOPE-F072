#include "FFT_Handle.h"

float32_t FFT_Handle_Buffer[FFT_Length * 2] = {0};
float32_t FFT_Max;

uint32_t FFT_Max_Index;

float32_t FFT_Freq_Sum;
float32_t FFT_Freq[2];

void Process_FFT_Data(void)
{
	float Vlotage_Bisic[7] = {0.05,0.1,0.2,0.5,1,2,5};
	
	/*-----------------------------------------------------Channel 1-----------------------------------------------------*/
	FFT_Max_Index = 0;
	
	for(uint16_t i = 0;i < 256;i++)
	{
		FFT_Handle_Buffer[i * 2] = (float)ADC_Show[i][0] - (float)ADC_Sum[0];
		FFT_Handle_Buffer[i * 2 + 1] = 0;
	}
	
	arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_Handle_Buffer, 0, 1);
	arm_cmplx_mag_f32(FFT_Handle_Buffer,FFT_Handle_Buffer,256);
	arm_max_f32(FFT_Handle_Buffer,FFT_Length / 2 ,&FFT_Max, &FFT_Max_Index);
	
	if(FFT_Max_Index >= 2)
	{
		FFT_Freq_Sum = FFT_Handle_Buffer[FFT_Max_Index - 2] + 
									 FFT_Handle_Buffer[FFT_Max_Index - 1] + 
									 FFT_Handle_Buffer[FFT_Max_Index] + 
									 FFT_Handle_Buffer[FFT_Max_Index + 1] + 
									 FFT_Handle_Buffer[FFT_Max_Index + 2] ;
		FFT_Freq[0] = (((FFT_Max_Index - 2) * FFT_Handle_Buffer[FFT_Max_Index - 2]) + 
									 ((FFT_Max_Index - 1) * FFT_Handle_Buffer[FFT_Max_Index - 1]) + 
									 ((FFT_Max_Index)		 * FFT_Handle_Buffer[FFT_Max_Index]) + 
									 ((FFT_Max_Index + 2) * FFT_Handle_Buffer[FFT_Max_Index + 2]) + 
									 ((FFT_Max_Index + 1) * FFT_Handle_Buffer[FFT_Max_Index + 1])) / FFT_Freq_Sum;
		FFT_Freq[0] *= (FFT_Select[Freq_Div] / 128.0);
	}
	else
		FFT_Freq[0] = 0;
	
	//生成绘图数据
	for(uint16_t i = 0;i < 230;i++)
	{
		uint16_t Pos = i * (127.0 / 229.0);
		float Now_FFT = (FFT_Handle_Buffer[Pos] / 64.0) + ADC_Sum[0];
		float Voltage = (((Now_FFT * 3.3) / 4096.0) - 1.645875) / 0.33;
		
		Show_Buffer[i][Show_Cnt][0] = 114 - (int32_t)(Voltage * (23.0 / Vlotage_Bisic[Freq_Voltage_Div]));//通道1绘图数据
		
		//限值->上限与下限
		if(Show_Buffer[i][Show_Cnt][0] < 23)		Show_Buffer[i][Show_Cnt][0] = 23;
		if(Show_Buffer[i][Show_Cnt][0] > 114)		Show_Buffer[i][Show_Cnt][0] = 114;
	}
	
	
	/*-----------------------------------------------------Channel 2-----------------------------------------------------*/
	FFT_Max_Index = 0;
	
	for(uint16_t i = 0;i < 256;i++)
	{
		FFT_Handle_Buffer[i * 2] = (float)ADC_Show[i][1] - (float)ADC_Sum[1];
		FFT_Handle_Buffer[i * 2 + 1] = 0;
	}
	
	arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_Handle_Buffer, 0, 1);
	arm_cmplx_mag_f32(FFT_Handle_Buffer,FFT_Handle_Buffer,256);
	arm_max_f32(FFT_Handle_Buffer,FFT_Length / 2 ,&FFT_Max, &FFT_Max_Index);
	
	if(FFT_Max_Index >= 2)
	{
		FFT_Freq_Sum = FFT_Handle_Buffer[FFT_Max_Index - 2] + 
									 FFT_Handle_Buffer[FFT_Max_Index - 1] + 
									 FFT_Handle_Buffer[FFT_Max_Index] + 
									 FFT_Handle_Buffer[FFT_Max_Index + 1] + 
									 FFT_Handle_Buffer[FFT_Max_Index + 2] ;
		FFT_Freq[1] = (((FFT_Max_Index - 2) * FFT_Handle_Buffer[FFT_Max_Index - 2]) + 
									 ((FFT_Max_Index - 1) * FFT_Handle_Buffer[FFT_Max_Index - 1]) + 
									 ((FFT_Max_Index)		 * FFT_Handle_Buffer[FFT_Max_Index]) + 
									 ((FFT_Max_Index + 2) * FFT_Handle_Buffer[FFT_Max_Index + 2]) + 
									 ((FFT_Max_Index + 1) * FFT_Handle_Buffer[FFT_Max_Index + 1])) / FFT_Freq_Sum;
		FFT_Freq[1] *= (FFT_Select[Freq_Div] / 128.0);
	}
	else
		FFT_Freq[1] = 0;
	
	//生成绘图数据
	for(uint16_t i = 0;i < 230;i++)
	{
		uint16_t Pos = i * (127.0 / 229.0);
		float Now_FFT = (FFT_Handle_Buffer[Pos] / 64.0) + ADC_Sum[1];
		float Voltage = (((Now_FFT * 3.3) / 4096.0) - 1.645875) / 0.33;
		
		Show_Buffer[i][Show_Cnt][1] = 205 - (int32_t)(Voltage * (23.0 / Vlotage_Bisic[Freq_Voltage_Div]));//通道1绘图数据
		
		//限值->上限与下限
		if(Show_Buffer[i][Show_Cnt][1] < 115)		Show_Buffer[i][Show_Cnt][1] = 115;
		if(Show_Buffer[i][Show_Cnt][1] > 205)		Show_Buffer[i][Show_Cnt][1] = 205;
	}
}

