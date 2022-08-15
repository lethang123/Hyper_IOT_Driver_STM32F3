#include "stm32f30x_it.h"

Circuler_Buffer_type Buff;
volatile uint8_t Buff_RX;
unsigned int cnt_Mess = 0;
unsigned int cnt_Motor = 0;
unsigned int ADC_product_Dispense[3] = {0};
unsigned int cnt_Dropss_Disable = 0;
unsigned int cnt_Global = 0;
bool Task_Operation = false;
int mask_cnt = 0;
char mask = 0;

unsigned int phase = 0;
unsigned int phase_1 = 0;
unsigned int phase_2 = 0;
unsigned int phase_3 = 0;
unsigned int phase_4 = 0;

unsigned int timeout = 0;

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
/******************************************************************************/

/****************************TIM6_DAC1_IRQHandler******************************/
// Add by Toan - Control freezer
int dem = 0;
int dem1 = 0;
int check_hex = 0;
int check_light = 0;
// Add by Toan - Control freezer
void TIM6_DAC1_IRQHandler()
{
   if(TIM_GetITStatus(TIM6, TIM_IT_Update) != 0)
   {
     TIM_ClearITPendingBit(TIM6 , TIM_IT_Update);
		 cnt_Global++;
		 
		 dem++;
		 dem1++;
		 check_hex++;
		 check_light++;
		 
		 if(cnt_Dropss_Disable)
			 cnt_Dropss_Disable++;
		 
		 if(Task_Operation == true)
			 timeout++;
		 
		 if(mask == 1)
			mask_cnt++;
		 
		 
		 if(phase == 1)
		 {
			 phase_1++;
		 }
		 
		 if(phase == 2)
		 {
			 phase_2++;
		 }
		 
		 if(phase == 3)
		 {
			 phase_3++;
		 }
		 
		 if(phase == 4)
		 {
			 phase_4++;
		 }
	 }
}


/******************************************************************************/
void USART1_IRQHandler()
{
	 static unsigned char i = 0;
	 static unsigned char Start_count = 0;
   if(USART_GetITStatus(USART1, USART_IT_RXNE) != 0)
   {
      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
   }
}

/******************************************************************************/
void USART2_IRQHandler()
{
   if(USART_GetITStatus(USART2, USART_IT_RXNE) != 0)
   {
		 USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		 Buff_RX = USART2->RDR;
		 Circular_Buffer_Put(&Buff, Buff_RX);
   }
}

/******************************************************************************/
void ADC1_2_IRQHandler()
{
	static char cnt_MotorCurrent = 0;
	
	if(ADC_GetITStatus(ADC2, ADC_IT_EOC))
	{
		char Arr[7];
		
		ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);
		ADC_product_Dispense[cnt_MotorCurrent] = ADC_GetConversionValue(ADC2);

		cnt_MotorCurrent++;
		if(cnt_MotorCurrent > 2)
			cnt_MotorCurrent = 0;
	}

	if(ADC_GetITStatus(ADC2, ADC_IT_EOS))
	{
		ADC_ClearITPendingBit(ADC2, ADC_IT_EOS);
	}
}

/******************************************************************************/
void EXTI0_IRQHandler()
{
	if(READ_BIT(EXTI->PR, EXTI_PR_PR0))
	{
		SET_BIT(EXTI->PR, EXTI_PR_PR0);
		CLEAR_BIT(EXTI->EMR, EXTI_EMR_MR0);
		if(cnt_Dropss_Disable == 0)
			cnt_Dropss_Disable++;
	}
}