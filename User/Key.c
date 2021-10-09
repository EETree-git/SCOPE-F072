#include "Key.h"

Key_State_Type Key_State[Key_Num] = {0};

void Get_Key_State(Key_Type *Key)
{
	Key[0].State = HAL_GPIO_ReadPin(Key_1_GPIO_Port,Key_1_Pin);
	Key[1].State = HAL_GPIO_ReadPin(Key_2_GPIO_Port,Key_2_Pin);
	Key[2].State = HAL_GPIO_ReadPin(Key_L_GPIO_Port,Key_L_Pin);
	Key[3].State = HAL_GPIO_ReadPin(Key_O_GPIO_Port,Key_O_Pin);
	Key[4].State = HAL_GPIO_ReadPin(Key_R_GPIO_Port,Key_R_Pin);
}

void Get_Key(Key_Type *Key)
{
	Get_Key_State(Key);
	for(uint8_t i = 0;i < Key_Num;i++)
	{
		if(Key[i].State)
		{
			if(Key[i].Flag == 0)
			{
				Key[i].Flag = 1;
				Key[i].Start = HAL_GetTick();
				
				Key_State[i].Fall = 0;
				Key_State[i].Press_Time = 0;
			}
			
			if(Key[i].Flag)
				Key[i].Count = HAL_GetTick() - Key[i].Start;
			
			if(Key[i].Flag == 1 && Key[i].Count >= Key_Ok)//触发一次上升沿事件
			{
				Key[i].Flag = 2;
				Key_State[i].Rise = 1;
				
				if(Key[i].Cont_Click_Buffer == 0)
					Key[i].Cont_Click_Buffer = 1;
				else if(HAL_GetTick() - Key[i].Cont_Click_Tick <= Cont_Click_Time)//距离上一次单击不超时
					Key[i].Cont_Click_Buffer++;
				
				Key[i].Cont_Click_Tick = HAL_GetTick();
			}
			
			if(Key[i].Flag == 2 && Key[i].Count >= Long_Press_Time)//长按成立 触发一次长按事件
			{
				Key[i].Flag = 3;
				Key_State[i].Long_Press = 1;
				
				Key[i].Key_Tri_Tick = HAL_GetTick() + Long_Tri;
			}
			
			if(Key[i].Count >= Long_Press_Time && HAL_GetTick() >= Key[i].Key_Tri_Tick)//长按成立后连续触发
			{
				Key[i].Key_Tri_Tick = HAL_GetTick() + Long_Tri;
				Key_State[i].Long_Tri_Press = 1;
			}
		}
		else
		{
			if(HAL_GetTick() - Key[i].Cont_Click_Tick > Cont_Click_Time)//连续点击超时
			{
				Key_State[i].Click_Cnt = Key[i].Cont_Click_Buffer;
				Key[i].Cont_Click_Buffer = 0;
			}
			
			if(Key[i].Flag)
			{
				Key[i].Flag = 0;
				
				Key_State[i].Press_Time = Key[i].Count;
				Key_State[i].Fall = 1;
			}
			
			Key_State[i].Rise = 0;
			Key_State[i].Long_Press = 0;
			Key_State[i].Long_Tri_Press = 0;
		}
	}
}

int8_t Get_Rise(uint8_t Key_ID)
{
	int8_t Value = Key_State[Key_ID].Rise;
	Key_State[Key_ID].Rise = 0;
	return Value;
}

int8_t Get_Fall(uint8_t Key_ID)
{
	int8_t Value = Key_State[Key_ID].Fall;
	Key_State[Key_ID].Fall = 0;
	return Value;
}

int8_t Get_Long_Press(uint8_t Key_ID)
{
	int8_t Value = Key_State[Key_ID].Long_Press;
	Key_State[Key_ID].Long_Press = 0;
	return Value;
}

int8_t Get_Long_Tri(uint8_t Key_ID)
{
	int8_t Value = Key_State[Key_ID].Long_Tri_Press;
	Key_State[Key_ID].Long_Tri_Press = 0;
	return Value;
}

int32_t Get_Cont_Click(uint8_t Key_ID)
{
	int32_t Value = Key_State[Key_ID].Click_Cnt;
	Key_State[Key_ID].Click_Cnt = 0;
	return Value;
}

int32_t Get_Press_Time(uint8_t Key_ID)
{
	int32_t Value = Key_State[Key_ID].Press_Time;
	Key_State[Key_ID].Press_Time = 0;
	return Value;
}
