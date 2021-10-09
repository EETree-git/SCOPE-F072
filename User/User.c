#include "User.h"


/*
	当前系统时钟为72M
	
	SPI原预分频为 SPI_BAUDRATEPRESCALER_4
	PLL锁相环原PLLMUL为 RCC_PLL_MUL6
*/
/*-----------------------------------------------------------------------------------------------------------------*/
System_State System = Oscilloscope;

uint8_t Page_Init = 1;
uint8_t System_Select[5] = {0};

uint8_t LCD_Clean_Flag = 0;
uint8_t Wave_Change_Flag = 1;//上电默认开启信号发生器
uint8_t PWM_Change_Flag = 1;//上电默认开启PWM发生器
uint8_t Power_DC_Flag = 1;//上电默认开启PowerDC输出

void System_Change_State(System_State State)
{
	System = State;
	System_Select[State] = 0;
	LCD_Clean_Flag = 1;
	Page_Init = 1;
	
	//信号发生器光标归位
	Wave_Vpp_Select = 0;
	Wave_Freq_Select = 0;
	Wave_Offset_Select = 0;
	
	//PWM发生器光标归位
	PWM_Freq_Select = 0;
	PWM_Duty_Select = 0;
	
	//Power DC输出光标归位
	Power_Select = 0;
	Power_voltage_Select = 0;
	Power_Out_Select = 0;
}


void LCD_Handle(void)
{
	switch(System)
	{
		case Oscilloscope:
		{
			if(Page_Init)
			{
				Page_Init = 0;
				Oscilloscope_Init();
			}
			Oscilloscope_Show_Wave();
			Oscilloscope_Show_Information(System_Select[Oscilloscope]);
			break;
		}
		
		case Spectrum:
			if(Page_Init)
			{
				Page_Init = 0;
				Spectrum_Init();
			}
			Spectrum_Show_Wave();
			Spectrum_Show_Information(System_Select[Spectrum]);
			break;
		
		case FunctionGen:
			if(Page_Init)
			{
				Page_Init = 0;
				Wave_Generate_UI();
			}
			Wave_Show_Information(System_Select[FunctionGen]);
			break;
			
		case PWM_Generate:
			if(Page_Init)
			{
				Page_Init = 0;
				PWM_Generate_UI();
			}
			PWM_Show_Information(System_Select[PWM_Generate]);
			break;
		
		case PowerDC:
			if(Page_Init)
			{
				Page_Init = 0;
				Power_DC_UI();
			}
			Power_Show_Information(System_Select[PowerDC]);
			break;
		
		default:
			break;
	}
}

void Key_Handle(void)
{
	static Key_Type Key[5] = {0};
	Get_Key(Key);
	
	switch(System)
	{
		case Oscilloscope:
		{
			if(Get_Rise(Key2))
				System_Select[Oscilloscope] = ((System_Select[Oscilloscope] < 2) ? (System_Select[Oscilloscope] + 1) : (0));
			
			if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))
			{
				if(System_Select[Oscilloscope] == 1)//电压刻度-
				{
					Voltage_Div = ((Voltage_Div > 0) ? (Voltage_Div - 1) : (Voltage_Div));
					if(Scan_Show)
						Oscilloscope_Clean();
				}
				else if(System_Select[Oscilloscope] == 2)//时间刻度-
				{
					Time_Div = ((Time_Div > 0) ? (Time_Div - 1) : (Time_Div));
					Set_ADC_Samp_Rate(Time_Div);
				}
			}
			
			if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))
			{
				if(System_Select[Oscilloscope] == 1)//电压刻度+
				{
					Voltage_Div = ((Voltage_Div < 6) ? (Voltage_Div + 1) : (Voltage_Div));
					if(Scan_Show)
						Oscilloscope_Clean();
				}
				else if(System_Select[Oscilloscope] == 2)//时间刻度+
				{
					Time_Div = ((Time_Div < 17) ? (Time_Div + 1) : (Time_Div));
					Set_ADC_Samp_Rate(Time_Div);
				}
			}
			
			if(Get_Rise(Key1))
			{
				Oscilloscope_Deinit();
				System_Change_State(Spectrum);
			}
			break;
		}
		
		case Spectrum:
		{
			if(Get_Rise(Key2))
				System_Select[Spectrum] = ((System_Select[Spectrum] < 2) ? (System_Select[Spectrum] + 1) : (0));
			
			if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))
			{
				if(System_Select[Spectrum] == 1)//电压刻度-
					Freq_Voltage_Div = ((Freq_Voltage_Div > 0) ? (Freq_Voltage_Div - 1) : (Freq_Voltage_Div));
				else if(System_Select[Spectrum] == 2)//频率刻度-
				{
					Freq_Div = ((Freq_Div > 0) ? (Freq_Div - 1) : (Freq_Div));
					Set_FFT_Samp_Rate(Freq_Div);
				}
			}
			
			if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))
			{
				if(System_Select[Spectrum] == 1)//电压刻度+
					Freq_Voltage_Div = ((Freq_Voltage_Div < 6) ? (Freq_Voltage_Div + 1) : (Freq_Voltage_Div));
				else if(System_Select[Spectrum] == 2)//频率刻度+
				{
					Freq_Div = ((Freq_Div < 7) ? (Freq_Div + 1) : (Freq_Div));
					Set_FFT_Samp_Rate(Freq_Div);
				}
			}
			
			if(Get_Rise(Key1))
			{
				FFT_Deinit();
				System_Change_State(FunctionGen);
			}
			break;
		}
		
		case FunctionGen:
		{
			float Voltage_Plus[2] = {1.0,0.1};
			
			if(Get_Rise(Key2))
			{
				System_Select[FunctionGen] = ((System_Select[FunctionGen] < 3) ? (System_Select[FunctionGen] + 1) : (0));
				Wave_Vpp_Select = 0;
				Wave_Freq_Select = 0;
				Wave_Offset_Select = 0;
			}
			
			switch(System_Select[FunctionGen])
			{
				case 0://调节输出波形
				{
					if(Get_Rise(KeyR))
					{
						Wave_Info.Wave = (Wave_Type)((Wave_Info.Wave < 2) ? (Wave_Info.Wave + 1) : (Wave_Info.Wave));
						Wave_Change_Flag = 1;
					}
					if(Get_Rise(KeyL))
					{
						Wave_Info.Wave = (Wave_Type)((Wave_Info.Wave > 0) ? (Wave_Info.Wave - 1) : (Wave_Info.Wave));
						Wave_Change_Flag = 1;
					}
					break;
				}
				
				case 1://调节输出VPP
				{
					if(Get_Rise(KeyO))
						Wave_Vpp_Select = ((Wave_Vpp_Select < 1) ? (Wave_Vpp_Select + 1) : (0));
					
					if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))
					{
						if(Wave_Info.VPP + Voltage_Plus[Wave_Vpp_Select] <= 4.0)
							Wave_Info.VPP += Voltage_Plus[Wave_Vpp_Select];
						Wave_Change_Flag = 1;
					}
					
					if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))
					{
						if(Wave_Info.VPP - Voltage_Plus[Wave_Vpp_Select] >= -4.0)
							Wave_Info.VPP -= Voltage_Plus[Wave_Vpp_Select];
						Wave_Change_Flag = 1;
					}
					break;
				}
				
				case 2://调节输出Freq
				{
					float Freq_Plus_Num[5][4] = 
					{
						{0.1,1},
						{1,10,100},
						{10,100,1000},
						{100,1000,10000},
						{1000,10000},
					};
					
					if(Wave_Freq_Select > Freq_Max_Num)
						Wave_Freq_Select = 0;
					
					if(Get_Rise(KeyO))
						Wave_Freq_Select = ((Wave_Freq_Select < Freq_Max_Num) ? (Wave_Freq_Select + 1) : (0));
					
					if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))
					{
						if(Wave_Info.Freq + Freq_Plus_Num[Wave_Freq_Level][Freq_Max_Num - Wave_Freq_Select] <= 100000.0)
							Wave_Info.Freq += Freq_Plus_Num[Wave_Freq_Level][Freq_Max_Num - Wave_Freq_Select];
						Wave_Change_Flag = 1;
					}
					
					if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))
					{
						if(Wave_Info.Freq - Freq_Plus_Num[Wave_Freq_Level][Freq_Max_Num - Wave_Freq_Select] >= 0.05)
							Wave_Info.Freq -= Freq_Plus_Num[Wave_Freq_Level][Freq_Max_Num - Wave_Freq_Select];
						else if(Wave_Info.Freq == 1)
							Wave_Info.Freq -= 0.1;
						Wave_Change_Flag = 1;
					}
					
					break;
				}
				
				case 3://调节输出Offset
				{
					if(Get_Rise(KeyO))
						Wave_Offset_Select = ((Wave_Offset_Select < 1) ? (Wave_Offset_Select + 1) : (0));
				
					if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))
					{
						if(Wave_Info.Offset + Voltage_Plus[Wave_Offset_Select] <= 4.0)
							Wave_Info.Offset += Voltage_Plus[Wave_Offset_Select];
						Wave_Change_Flag = 1;
					}
					
					if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))
					{
						if(Wave_Info.Offset - Voltage_Plus[Wave_Offset_Select] >= -4.0)
							Wave_Info.Offset -= Voltage_Plus[Wave_Offset_Select];
						Wave_Change_Flag = 1;
					}
					break;
				}
				
				default:
					break;
			}
			
			if(Get_Rise(Key1))
				System_Change_State(PWM_Generate);
			break;
		}
		
		case PWM_Generate:
		{
			if(Get_Rise(Key2))
			{
				System_Select[PWM_Generate] = ((System_Select[PWM_Generate] < 2) ? (System_Select[PWM_Generate] + 1) : (0));
				PWM_Freq_Select = 0;
				PWM_Duty_Select = 0;
			}
			
			if(System_Select[PWM_Generate] == 1)
			{
				float Freq_Plus_Num[5][4] = 
				{
					{0.1,1},
					{1,10,100},
					{10,100,1000},
					{100,1000,10000},
					{1000,10000,100000},
				};
				
				if(PWM_Freq_Select > PWM_Freq_Max_Num)
					PWM_Freq_Select = 0;
				
				if(Get_Rise(KeyO))
					PWM_Freq_Select = ((PWM_Freq_Select < PWM_Freq_Max_Num) ? (PWM_Freq_Select + 1) : (0));
				
				if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))
				{
					if(PWM_Info.Freq + Freq_Plus_Num[PWM_Freq_Level][PWM_Freq_Max_Num - PWM_Freq_Select] <= 720000.0)
						PWM_Info.Freq += Freq_Plus_Num[PWM_Freq_Level][PWM_Freq_Max_Num - PWM_Freq_Select];
					PWM_Change_Flag = 1;
				}
				
				if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))
				{
					if(PWM_Info.Freq - Freq_Plus_Num[PWM_Freq_Level][PWM_Freq_Max_Num - PWM_Freq_Select] >= 0.05)
						PWM_Info.Freq -= Freq_Plus_Num[PWM_Freq_Level][PWM_Freq_Max_Num - PWM_Freq_Select];
					else if(PWM_Info.Freq == 1)
						PWM_Info.Freq -= 0.1;
					PWM_Change_Flag = 1;
				}
			}
			else if(System_Select[PWM_Generate] == 2)
			{
				uint8_t Duty_Max_Num = ((PWM_Info.Duty  > 0.06) ? (1) : (0));
				float Duty_Plus_Num[2] = {0.05,0.1};
				
				if(PWM_Duty_Select > Duty_Max_Num)
					PWM_Duty_Select = 0;
				
				if(Get_Rise(KeyO))
					PWM_Duty_Select = ((PWM_Duty_Select < Duty_Max_Num) ? (PWM_Duty_Select + 1) : (0));
				
				if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))//Duty+
				{
					if(PWM_Info.Duty + Duty_Plus_Num[Duty_Max_Num - PWM_Duty_Select] <= 0.96)
						PWM_Info.Duty += Duty_Plus_Num[Duty_Max_Num - PWM_Duty_Select];
					PWM_Change_Flag = 1;
				}
				
				if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))//Duty-
				{
					if(PWM_Info.Duty - Duty_Plus_Num[Duty_Max_Num - PWM_Duty_Select] >= 0.04)
						PWM_Info.Duty -= Duty_Plus_Num[Duty_Max_Num - PWM_Duty_Select];
					PWM_Change_Flag = 1;
				}
			}
			if(Get_Rise(Key1))
				System_Change_State(PowerDC);
			
			break;
		}
		
		case PowerDC:
		{
			float Voltage_Plus[2] = {1.0,0.1};
			float *Channel[2] = {&DC1_Voltage,&DC2_Voltage};
			uint8_t *Channel_State[2] = {&DC1_State,&DC2_State};
			
			if(Get_Rise(Key2))
			{
				System_Select[PowerDC] = ((System_Select[PowerDC] < 3) ? (System_Select[PowerDC] + 1) : (0));
				Power_voltage_Select = 0;
				Power_Out_Select = 0;
			}
			
			if(System_Select[PowerDC] == 1)
			{
				if(Get_Rise(KeyR))
					Power_Select = ((Power_Select < 1) ? (Power_Select + 1) : (Power_Select));
				if(Get_Rise(KeyL))
					Power_Select = ((Power_Select > 0) ? (Power_Select - 1) : (Power_Select));
			}
			else if(System_Select[PowerDC] == 2)
			{
				if(Get_Rise(KeyO))
					Power_voltage_Select = ((Power_voltage_Select < 1) ? (Power_voltage_Select + 1) : (0));
				
				if(Get_Rise(KeyR) || Get_Long_Tri(KeyR))
				{
					if(*Channel[Power_Select] + Voltage_Plus[Power_voltage_Select] <= 4.0)
					{
						*Channel[Power_Select] += Voltage_Plus[Power_voltage_Select];
						Power_DC_Flag = 1;
					}
				}
				
				if(Get_Rise(KeyL) || Get_Long_Tri(KeyL))
				{
					if(*Channel[Power_Select] - Voltage_Plus[Power_voltage_Select] >= -4.0)
					{
						*Channel[Power_Select] -= Voltage_Plus[Power_voltage_Select];
						Power_DC_Flag = 1;
					}
				}
			}
			else if(System_Select[PowerDC] == 3)
			{
				if(Get_Rise(KeyL))
				{
					*Channel_State[Power_Select] = 1;
					Power_DC_Flag = 1;
				}
				
				if(Get_Rise(KeyR))
				{
					*Channel_State[Power_Select] = 0;
					Power_DC_Flag = 1;
				}
			}
			
			if(Get_Rise(Key1))
				System_Change_State(Oscilloscope);
			break;
		}
		
		default:
			break;
	}
}

void User_Init(void)
{
	LCD_Init();
}

void User_Loop(void)
{
	LCD_Handle();
	if(LCD_Clean_Flag)
	{
		LCD_Clean_Flag = 0;
		LCD_Clear(BLACK);//清屏
	}
	if(Wave_Change_Flag)
	{
		Wave_Change_Flag = 0;
		Set_Wave_Out(Wave_Info);//生成波形
	}
	if(PWM_Change_Flag)
	{
		PWM_Change_Flag = 0;
		PWM_Generate_Out(PWM_Info);
		PWM_Picture_ReDraw(System);
	}
	if(Power_DC_Flag)
	{
		Power_DC_Flag = 0;
		Set_DC1_Out((DC1_State) ? (DC1_Voltage) : (0.0));
		Set_DC2_Out((DC2_State) ? (DC2_Voltage) : (0.0));
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		Oscilloscope_Interrupt();
	}
	if(htim->Instance == TIM7)
	{
		Key_Handle();
	}
}

