#ifndef __SYSTICK_DELAY_H
#define __SYSTICK_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f30x.h"

/****************************************/
void SysTick_init(void);
void Delay_ms(uint32_t nTime_ms);
void Delay_10(uint32_t nTime_10us);
/****************************************/

#ifdef __cplusplus
}
#endif

#endif