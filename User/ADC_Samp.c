#include "ADC_Samp.h"

/*---------------------------------------------------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;
extern TIM_HandleTypeDef htim1;
extern ADC_ChannelConfTypeDef sConfig;

uint16_t ADC_Show[ADC_Length][ADC_Channel] = {0};
uint16_t ADC_Value[ADC_Length][ADC_Channel] = {0};
uint16_t FFT_Samp[FFT_Length][ADC_Channel] = {0};
uint16_t ADC_Scan_Value[ADC_Channel] = {0};

ADC_Edge Tri_Edge = Edge_Rise;//上升沿触发
ADC_Ch Tri_Source = Ch1;//触发源

uint16_t ADC_Max[ADC_Channel] = {0};
uint16_t ADC_Min[ADC_Channel] = {0};
uint32_t ADC_Sum[ADC_Channel] = {0};
double ADC_Cycle[ADC_Channel] = {0};

uint16_t Tri_Voltage = 1955;//触发电压
uint16_t Best_Tri = 0;//最佳触发电压 VPP/2
uint16_t Tri_Pos = 0;//触发位置
uint8_t ADC_Samp_Ok = 0;//ADC采集完成标志

uint8_t Time_Div = 17;//示波器时间刻度
uint8_t Voltage_Div = 4;//示波器电压刻度
uint8_t Freq_Div = 7;//频谱频率刻度
uint8_t Freq_Voltage_Div = 5;//频谱电压刻度

uint8_t Scan_Show = 0;//扫描模式
uint8_t Un_Scan_Init = 0;

double Time_Arr[18] = 
{
	50,20,10,5,2,1,0.5,0.2,0.1,0.05,0.02,0.01,
	0.005,0.002,0.001,0.0005,0.0002,0.0001
};

uint32_t FFT_Select[8] = {100,500,1000,5000,10000,50000,100000,250000};
uint8_t FFT_State_Change = 0;
uint8_t FFT_Init = 0;

void User_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{
	HAL_ADC_Start_DMA(hadc,pData,Length);
	hadc->DMA_Handle->XferCpltCallback = ADC_DMA_Finish;
}

void Set_TIM1_Freq(double Freq)
{
	uint32_t Basic_Freq = System_Clock;
	uint32_t Prescaler = 1;
	
	while((Basic_Freq / Prescaler) / Freq > 65535)
	{
		do
		{
			Prescaler++;
		}while(Basic_Freq % Prescaler);
	}
	
	TIM1->PSC = Prescaler - 1;
	TIM1->ARR = ((Basic_Freq / Prescaler) / Freq) - 1;
}
/*------------------------------------------------------ ----Oscilloscope----------------------------------------------------------*/
void Get_Wave_Cycle(void)
{
	uint16_t Tri_Pos_Buffer[2] = {0};
	uint8_t Tri_Flag[2] = {0};
	
	memset(ADC_Cycle,0,sizeof(double) * 2);
  
	for(uint8_t Ch = 0;Ch < 2;Ch++)
	{
		for(uint16_t i = 0;i < ADC_Length;i++)
		{
			
			if(i < ADC_Length - 2 && i > 1)//查找触发位置
			{
				if(ADC_Show[i - 1][Ch] >= Tri_Voltage && ADC_Show[i + 1][Ch] <= Tri_Voltage)//检测到上升沿
				{
					if(ADC_Show[i - 2][Tri_Source] >= Tri_Voltage && ADC_Show[i + 2][Tri_Source] <= Tri_Voltage)
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
				else if(ADC_Show[i - 1][Ch] <= Tri_Voltage && ADC_Show[i + 1][Ch] >= Tri_Voltage && Tri_Flag[Ch] == 1)//检测到下降沿
					if(ADC_Show[i - 2][Tri_Source] <= Tri_Voltage && ADC_Show[i + 2][Tri_Source] >= Tri_Voltage)
						Tri_Flag[Ch] = 2;
				
			}
		}
	}
}

//求触发位置
void ADC_Data_Handle(void)
{
	ADC_Max[0] = 0;ADC_Max[1] = 0;
	ADC_Min[0] = 4095;ADC_Min[1] = 4095;
	
	uint8_t Tri_Flag = 0;
	for(uint16_t i = 0;i < ADC_Length;i++)
	{
		if(Tri_Flag == 0 && i < ADC_Length - 2 && i > 1)//查找触发位置
		{
			if(Tri_Edge == Edge_Rise)//上升沿触发
			{
				//硬件将输入信号反相 所以判断输入上升沿即判断输入数据下降沿
				if(ADC_Show[i - 1][Tri_Source] > Tri_Voltage && ADC_Show[i + 1][Tri_Source] < Tri_Voltage)
				{
					if(ADC_Show[i - 2][Tri_Source] > Tri_Voltage && ADC_Show[i + 2][Tri_Source] < Tri_Voltage)
					{
						Tri_Pos = i;
						Tri_Flag = 1;
					}
				}
			}
			else//下降沿触发
			{
				if(ADC_Show[i - 1][Tri_Source] < Tri_Voltage && ADC_Show[i + 1][Tri_Source] > Tri_Voltage)
				{
					if(ADC_Show[i - 2][Tri_Source] < Tri_Voltage && ADC_Show[i + 2][Tri_Source] > Tri_Voltage)
					{
						Tri_Pos = i;
						Tri_Flag = 1;
					}
				}
			}
		}
		
		if(ADC_Show[i][0] > ADC_Max[0])	ADC_Max[0] = ADC_Show[i][0];
		if(ADC_Show[i][1] > ADC_Max[1])	ADC_Max[1] = ADC_Show[i][1];
		if(ADC_Show[i][0] < ADC_Min[0])	ADC_Min[0] = ADC_Show[i][0];
		if(ADC_Show[i][1] < ADC_Min[1])	ADC_Min[1] = ADC_Show[i][1];
		
		ADC_Sum[0] += ADC_Show[i][0];
		ADC_Sum[1] += ADC_Show[i][1];
	}
	Get_Wave_Cycle();
	ADC_Sum[0] /= ADC_Length;
	ADC_Sum[1] /= ADC_Length;
	
	Best_Tri = (ADC_Max[Tri_Source] + ADC_Min[Tri_Source]) / 2.0;
	//Tri_Voltage = Best_Tri;
}

void Set_ADC_Samp_Rate(uint16_t Rate)
{
	static uint8_t Last_State = 0;
	
	Scan_Show = 0;
	if(Rate <= 7)
	{
		Scan_Show = 1;
		Un_Scan_Init = 0;
	}
	else
		Set_TIM1_Freq(229.0 / (Time_Arr[Rate] * 10.0));
	
	if(Last_State != Scan_Show)
	{
		Last_State = Scan_Show;
		Un_Scan_Init = 0;
	}
	
	if(Un_Scan_Init == 0)
	{
		Un_Scan_Init = 1;
		Oscilloscope_Clean();
		
		HAL_TIM_Base_Stop(&htim1);
		HAL_TIM_Base_Stop_IT(&htim1);
		HAL_ADC_Stop_DMA(&hadc);
		Set_TIM1_Freq(229.0 / (Time_Arr[Rate] * 10.0));
		if(Scan_Show == 0)
		{
			User_ADC_Start_DMA(&hadc,(uint32_t *)&ADC_Value,ADC_Length * ADC_Channel);
			HAL_TIM_Base_Start(&htim1);
		}
		else
		{
			User_ADC_Start_DMA(&hadc,(uint32_t *)&ADC_Scan_Value,2);
			HAL_TIM_Base_Start_IT(&htim1);
		}
	}
}

void ADC_Samp_Init(void)
{
	Set_ADC_Samp_Rate(Time_Div);
}
/*------------------------------------------------------ ----Oscilloscope----------------------------------------------------------*/


/*---------------------------------------------------------------FFT---------------------------------------------------------------*/
void Set_FFT_Samp_Rate(uint8_t Rate)
{
	static uint8_t Last_Rate = 0xFF;
	
	if(Last_Rate != Rate)
	{
		Last_Rate = Rate;
		FFT_Init = 1;
	}
	
	if(FFT_Init)
	{
		FFT_Init = 0;
		if(Rate < 7)
		{
			Set_TIM1_Freq(FFT_Select[Rate] * 2.0);
			
			if(FFT_State_Change)
			{
				FFT_State_Change = 0;
				HAL_TIM_Base_Stop(&htim1);
				HAL_ADC_Stop_DMA(&hadc);
				
				hadc.Init.ContinuousConvMode = DISABLE;
				hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_TRGO;
				HAL_ADC_Init(&hadc);
				sConfig.Channel = ADC_CHANNEL_6;
				HAL_ADC_ConfigChannel(&hadc, &sConfig);

				User_ADC_Start_DMA(&hadc,(uint32_t *)&FFT_Samp,FFT_Length * ADC_Channel);
				HAL_TIM_Base_Start(&htim1);
			}
		}
		else
		{
			HAL_TIM_Base_Stop(&htim1);
			HAL_ADC_Stop_DMA(&hadc);
			
			hadc.Init.ContinuousConvMode = ENABLE;
			hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
			HAL_ADC_Init(&hadc);
			sConfig.Channel = ADC_CHANNEL_6;
			HAL_ADC_ConfigChannel(&hadc, &sConfig);
			
			User_ADC_Start_DMA(&hadc,(uint32_t *)&FFT_Samp,FFT_Length * ADC_Channel);
			HAL_ADC_Start(&hadc);
			
			FFT_State_Change = 1;
		}
	}
}

void FFT_Samp_Init(void)
{
	FFT_State_Change = 1;
	FFT_Init = 1;
	Set_FFT_Samp_Rate(Freq_Div);
}

void FFT_Deinit(void)
{
	HAL_ADC_Stop_DMA(&hadc);
	HAL_ADC_Stop(&hadc);
	HAL_TIM_Base_Stop(&htim1);
	
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_TRGO;
	HAL_ADC_Init(&hadc);
	sConfig.Channel = ADC_CHANNEL_6;
	HAL_ADC_ConfigChannel(&hadc, &sConfig);
}

void FFT_ADC_Handle(void)
{
	for(uint16_t i = 0;i < FFT_Length;i++)
	{
		ADC_Sum[0] += ADC_Show[i][0];
		ADC_Sum[1] += ADC_Show[i][1];
	}
	ADC_Sum[0] /= FFT_Length;
	ADC_Sum[1] /= FFT_Length;
}
/*---------------------------------------------------------------FFT---------------------------------------------------------------*/
//ADC采集完成中断
void ADC_DMA_Finish(DMA_HandleTypeDef *hdma)
{
	if(ADC_Samp_Ok == 0 && Scan_Show == 0 && System == Oscilloscope)
	{
		memcpy(ADC_Show,ADC_Value,ADC_Length * ADC_Channel * sizeof(uint16_t));
		ADC_Data_Handle();//处理ADC数据
		ADC_Samp_Ok = 1;
	}
	if(ADC_Samp_Ok == 0 && System == Spectrum)
	{
		memcpy(ADC_Show,FFT_Samp,sizeof(FFT_Samp));
		FFT_ADC_Handle();//处理ADC数据
		ADC_Samp_Ok = 1;
	}
}












