#include "User_GPIO.h"

/******************************************************************************/
void GPIO_IO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/* UART GPIO pin config */
	GPIO_InitStructure.GPIO_Pin = DROPSS_OUT | SLOT0 | SLOT1 | SLOT2 | SLOT3 | SLOT4 | TRACE0;           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FAN | COOL | HEAT;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LIGHT | DOOR_HEAT | SLOT5 | SLOT6 | SLOT7 | SLOT8 | SLOT9 | 
	                              TRACE1 | TRACE2 | TRACE3 | TRACE4 | TRACE5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DOOR;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DROPSS_IN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	EXTI_InitStruct.EXTI_Line = EXTI_Line0;			
//  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  
//  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStruct); 
//	
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
//  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStruct.NVIC_IRQChannelSubPriority = EXTI_ISR_PRIORITY;
//  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStruct);
}

/******************************************************************************/
/**
  * @brief  Initializes the GPIO pin for USART1 module
  * @param  None
  * @retval None
  */
void GPIO_USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* UART GPIO pin config */
	GPIO_InitStructure.GPIO_Pin = UART2_TX | UART2_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* UART pin alternate function config */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);    
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
}

void GPIO_USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* UART TX GPIO pin config */
	GPIO_InitStructure.GPIO_Pin = UART1_TX | UART1_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* UART pin alternate function config */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_7);    
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_7);
}

/******************************************************************************/
/**
  * @brief  Initializes the GPIO pin for ADC1 module
  * @param  None
  * @retval None
  */
void GPIO_ADC_Init()
{
	GPIO_InitTypeDef ADC_IO_Init_Struct;
	
	ADC_IO_Init_Struct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_7;
//	ADC_IO_Init_Struct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	ADC_IO_Init_Struct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOA, &ADC_IO_Init_Struct);
	ADC_IO_Init_Struct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &ADC_IO_Init_Struct);
}

