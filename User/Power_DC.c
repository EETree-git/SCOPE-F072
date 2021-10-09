#include "Power_DC.h"

float Voltage_DC1 = 3.3;
float Voltage_DC2 = 3.3;

void Set_DC1_Out(double Voltage)
{
	Voltage = (Voltage - 6.985714285714286) / (-4.0);
	TIM15->CCR2 = (uint16_t)((Voltage / 3.3) * 1000.0);
	TIM15->CNT = 0;
}

void Set_DC2_Out(double Voltage)
{
	Voltage = (Voltage - 6.985714285714286) / (-4.0);
	TIM15->CCR1 = (uint16_t)((Voltage / 3.3) * 1000.0);
	TIM15->CNT = 0;
}
