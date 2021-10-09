#ifndef __Menu_H
#define __Menu_H

#include "User.h"

extern int16_t Show_Buffer[230][2][2];
extern uint8_t Show_Cnt;

/*--------------------------------------------------Wave Generate--------------------------------------------------*/
extern Wave_Para Wave_Info;
extern uint8_t Wave_Vpp_Select;
extern uint8_t Wave_Freq_Select;
extern uint8_t Wave_Offset_Select;

extern uint8_t Wave_Freq_Level;
extern uint8_t Freq_Max_Num;
/*--------------------------------------------------Wave Generate--------------------------------------------------*/



/*--------------------------------------------------PWM Generate--------------------------------------------------*/
extern PWM_Type PWM_Info;
extern uint8_t PWM_Freq_Select;
extern uint8_t PWM_Duty_Select;

extern uint8_t PWM_Freq_Level;//Êä³öÆµÂÊµ²Î»
extern uint8_t PWM_Freq_Max_Num;
/*--------------------------------------------------PWM Generate--------------------------------------------------*/



/*----------------------------------------------------Power DC----------------------------------------------------*/
extern uint8_t Power_Select;
extern uint8_t Power_voltage_Select;
extern uint8_t Power_Out_Select;

extern float DC1_Voltage;
extern float DC2_Voltage;

extern uint8_t DC1_State;
extern uint8_t DC2_State;
/*----------------------------------------------------Power DC----------------------------------------------------*/

void Oscilloscope_UI(void);
void Oscilloscope_Show_Wave(void);
void Oscilloscope_Show_Information(uint8_t Select);
void Oscilloscope_Clean(void);
void Oscilloscope_Interrupt(void);
void Oscilloscope_Init(void);
void Oscilloscope_Deinit(void);

uint8_t Str_Get_Num(char *Str);
uint8_t Get_Str_Num_Pos(char *Str,uint8_t Num_Pos);
void Wave_Generate_UI(void);
void Wave_Show_Information(uint8_t Wave_Change);

void Draw_PWM_Picture(float Duty,uint16_t Color);
void PWM_Generate_UI(void);
void PWM_Show_Information(uint8_t Select);

void Power_DC_UI(void);
void Power_Show_Information(uint8_t Select);

void Spectrum_Init(void);
void Spectrum_Show_Wave(void);
void Spectrum_Show_Information(uint8_t Select);

#endif



