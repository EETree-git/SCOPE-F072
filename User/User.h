#ifndef __User_H
#define __User_H

#define System_Clock	72000000

#include "main.h"
#include "math.h"
#include "stdio.h"
#include "string.h"

#include "Key.h"
#include "Wave.h"
#include "Menu.h"
#include "ST7789.h"
#include "ADC_Samp.h"
#include "Power_DC.h"
#include "FFT_Handle.h"

typedef enum
{
	Oscilloscope,
	Spectrum,
	FunctionGen,
	PWM_Generate,
	PowerDC
}System_State;

extern System_State System;

void User_Init(void);
void User_Loop(void);

#endif
