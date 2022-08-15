#include "SysTick_Delay.h"

volatile uint32_t us_10tick = 0;

/*******************************************************************/
void SysTick_Handler(void)
{
	/* System tick handler fcn */
	
    if(us_10tick != 0)
       us_10tick--;
}

/*******************************************************************/
void SysTick_init()
{
	/* Config systick */
	
    SystemCoreClockUpdate();                   //Update SystemCoreClock value
    SysTick_Config(SystemCoreClock/100000);    //Configure the SysTick timer to overflow every 10 us
}

/*******************************************************************/
void Delay_10(uint32_t nTime_10us)
{
	/* Delay 10*nTime_10us (us) */
	
	  us_10tick = nTime_10us;
	  while(us_10tick);
}

/*******************************************************************/
void Delay_ms(uint32_t nTime_ms)
{
	/* Delay nTime_ms (ms) */
	
	while(nTime_ms--)
    {  
        Delay_10(100);   //Delay 1ms
    }
}