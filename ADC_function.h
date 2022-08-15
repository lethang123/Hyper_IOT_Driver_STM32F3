#ifndef __ADC_FUNCTION_H
#define __ADC_FUNCTION_H

/******************************Include*****************************************/
#include "stm32f30x.h"
#include "SysTick_Delay.h"
#include "User_GPIO.h"

/******************************************************************************/

/******************************************************************************/
void ADC_Indep_Init(ADC_TypeDef* ADCx);
void ADC1_Indep_Init(ADC_TypeDef* ADCx);

#endif /* __MAIN_H */
