#ifndef __FFT_Handle_H
#define __FFT_Handle_H

#include "User.h"

#include "arm_common_tables.h"
#include "arm_const_structs.h"
#include "arm_math.h"

#define FFT_Length	256

extern float32_t FFT_Freq[2];

void Process_FFT_Data(void);

#endif
