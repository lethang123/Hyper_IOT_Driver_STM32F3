#include "ADC_function.h"

/******************************************************************************/
/* To config for ADC module, some process must be follow 
- before "ADEN" bit is set, config:
  + select, enable ADC clock in RCC registers
	+ select diffrential mode 
	+ init calib ADC 
	+ and other selection in ADC common register
	
- after "ADEN" bit is set, config:
  In ADCx_CFGR register
  + analog watchdog chennel 
	+ autoinject mode 
	+ queue contaxt 
	+ discontinuous mode 
	+ autodelay mode 
	+ continuous conversion mode 
	+ overrun mode 
	+ external trigger 
	+ shift left or right 
	+ data resolution
	+ DMA config 
	
	In ADCx_SMPR register 
	+ ADC sampling time config
	
	In ADCx_TR register 
	+ ADC watchdog threshold 
	
	In ADCx_SQR register 
	+ ADC sequence conversion
	
	In ADCx_OFR register 
	+ offset config 
	
	In ADCx_IER register 
	+ ADC interrupt enable */
	
	/* To config ADC module in dual mode, some rule must be follow: 
	   - In ADC regular simutanuous mode, the length of ADC conversion time
	of master group and slave must same same each other and length of one
	comversion in master group mode must longer than length of one conversion
	in slave group otherwise, ADC result would be incorrect
	
	   - In ADC interleaved mode, the length of delay time after sampling
	phase of master conversion must be canculate to one ADC don't start
	conversion while other ADC still insampling phase. if continuos mode used,
	the length of sampling phase of two ADC must be equal */

/******************************************************************************/
/**
 * @brief Init for ADC1 module in idependent mode
 * 
 * @note This function use for ADC1 but can for ADC2 (but must change parementer) 
 */
void ADC_Indep_Init(ADC_TypeDef* ADCx)
{
	/*--------------------------------------------------------------------------*/
	ADC_InitTypeDef ADC_Init_Struct;
  ADC_CommonInitTypeDef ADC_Common_Struct;	
	NVIC_InitTypeDef ADC_Interrupt_Struct;
	
	/*--------------------------------------------------------------------------*/
	/* ADC begin and clock config */
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);                           //ADC clock prescaler
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);               //Enable ADC clock if ADC clock sync AHB 
	ADC_VoltageRegulatorCmd(ADCx, ENABLE);                            //ADC vol regulator
	Delay_10(2);
	
	CLEAR_BIT(ADCx->CR, ADC_CR_ADCALDIF);                             //Select calibration mode
	SET_BIT(ADCx->CR, ADC_CR_ADCAL);                                  //Start calibration
	while(READ_BIT(ADCx->CR, ADC_CR_ADCAL));                          //Wairt process
	
	/*--------------------------------------------------------------------------*/
	/* ADC common mode config */
	ADC_Common_Struct.ADC_Clock = ADC_Clock_AsynClkMode;              //ADC clock source select 
	ADC_Common_Struct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA mode select for dual ADC
	ADC_Common_Struct.ADC_DMAMode = ADC_DMAMode_Circular;             //DMA mdoe select for single ADC               
	ADC_Common_Struct.ADC_Mode = ADC_Mode_Independent;                //ADC mode dual or single
	ADC_Common_Struct.ADC_TwoSamplingDelay = 2;                       //Delay between 2 sample in anternate trigger mode
	ADC_CommonInit(ADCx, &ADC_Common_Struct);
	
	/*--------------------------------------------------------------------------*/
	/* ADC module enable */
	ADC_Cmd(ADCx, ENABLE);
	while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_RDY));
	
	/*--------------------------------------------------------------------------*/
	/* ADC1 init */
	ADC_Init_Struct.ADC_AutoInjMode = ADC_AutoInjec_Disable;                          //Auto injected group conversion mode
	ADC_Init_Struct.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;          //Regular group continuous conversion mode
	ADC_Init_Struct.ADC_DataAlign = ADC_DataAlign_Right;                              //ADC data type
	ADC_Init_Struct.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_13;         //External trigger event select
	ADC_Init_Struct.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;       //External trigger edge select 
	ADC_Init_Struct.ADC_NbrOfRegChannel = 1;                                          //Number of conversion channel in regular group
	ADC_Init_Struct.ADC_OverrunMode = ADC_OverrunMode_Disable;                        //ADC result overrun mode
	ADC_Init_Struct.ADC_Resolution = ADC_Resolution_12b;                              //ADC resolution
	ADC_Init(ADCx, &ADC_Init_Struct);
	
	/*--------------------------------------------------------------------------*/
	/* Auto delay mode must be enable if using ADC modedule 
	in idependent mode and multi channel converting*/
	ADC_AutoDelayCmd(ADCx, DISABLE);                                                    //This mode used conjungtion with 
	                                                                                   //ADC continuous mode to handle ADC convertion     
	/*--------------------------------------------------------------------------*/
	/* Analog watchdog config (if used) */
  ADC_AnalogWatchdogCmd(ADCx, ADC_AnalogWatchdog_AllRegEnable);	                     //Chon nhom kenh duoc ch phep
	ADC_AnalogWatchdog1ThresholdsConfig(ADCx, 4095, 0);                                //Cai dat nguong so 1 
	ADC_AnalogWatchdog1SingleChannelConfig(ADCx, ADC_Channel_1);                       //Chon kenh cu the
	
	/*--------------------------------------------------------------------------*/
	/* ADC DMA request enable end interrupt config */
	ADC_ITConfig(ADC1, ADC_IT_EOC | ADC_IT_EOS, ENABLE);
	
	/* Interrupt config if need */
	ADC_Interrupt_Struct.NVIC_IRQChannel = ADC1_2_IRQn;                      //Interrupt channel position
	ADC_Interrupt_Struct.NVIC_IRQChannelPreemptionPriority = 0;              //First priority
	ADC_Interrupt_Struct.NVIC_IRQChannelSubPriority = ADC_ISR_PRIORITY;      //Second priority
	ADC_Interrupt_Struct.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&ADC_Interrupt_Struct);
	
	/*--------------------------------------------------------------------------*/
	/* ADC1 regular group channels select and config, each channel need 
	one saperate follwing function to config */
	ADC_RegularChannelConfig(ADCx, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);
//	ADC_RegularChannelConfig(ADCx, ADC_Channel_2, 2, ADC_SampleTime_7Cycles5);
//	ADC_RegularChannelConfig(ADCx, ADC_Channel_4, 3, ADC_SampleTime_7Cycles5);
}

/******************************************************************************/
/**
 * @brief Init for ADC1 module in idependent mode
 * 
 * @note This function use for ADC1 but can for ADC2 (but must change parementer) 
 */
void ADC1_Indep_Init(ADC_TypeDef* ADCx)
{
	/*--------------------------------------------------------------------------*/
	ADC_InitTypeDef ADC_Init_Struct;
  ADC_CommonInitTypeDef ADC_Common_Struct;	
	NVIC_InitTypeDef ADC_Interrupt_Struct;
	
	/*--------------------------------------------------------------------------*/
	/* ADC begin and clock config */
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);                           //ADC clock prescaler
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);               //Enable ADC clock if ADC clock sync AHB 
	ADC_VoltageRegulatorCmd(ADCx, ENABLE);                            //ADC vol regulator
	Delay_10(2);
	
	CLEAR_BIT(ADCx->CR, ADC_CR_ADCALDIF);                             //Select calibration mode
	SET_BIT(ADCx->CR, ADC_CR_ADCAL);                                  //Start calibration
	while(READ_BIT(ADCx->CR, ADC_CR_ADCAL));                          //Wairt process
	/*--------------------------------------------------------------------------*/
	
	/* ADC common mode config */
	ADC_Common_Struct.ADC_Clock = ADC_Clock_AsynClkMode;              //ADC clock source select 
	ADC_Common_Struct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA mode select for dual ADC
	ADC_Common_Struct.ADC_DMAMode = ADC_DMAMode_Circular;             //DMA mdoe select for single ADC               
	ADC_Common_Struct.ADC_Mode = ADC_Mode_Independent;                //ADC mode dual or single
	ADC_Common_Struct.ADC_TwoSamplingDelay = 2;                       //Delay between 2 sample in anternate trigger mode
	ADC_CommonInit(ADCx, &ADC_Common_Struct);
	
	/*--------------------------------------------------------------------------*/
	/* ADC module enable */
	ADC_Cmd(ADCx, ENABLE);
	while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_RDY));
	
	/*--------------------------------------------------------------------------*/
	/* ADC1 init */
	ADC_Init_Struct.ADC_AutoInjMode = ADC_AutoInjec_Disable;                          //Auto injected group conversion mode
	ADC_Init_Struct.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;          //Regular group continuous conversion mode
	ADC_Init_Struct.ADC_DataAlign = ADC_DataAlign_Right;                              //ADC data type
	ADC_Init_Struct.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_13;         //External trigger event select
	ADC_Init_Struct.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;       //External trigger edge select 
	ADC_Init_Struct.ADC_NbrOfRegChannel = 1;                                          //Number of conversion channel in regular group
	ADC_Init_Struct.ADC_OverrunMode = ADC_OverrunMode_Disable;                        //ADC result overrun mode
	ADC_Init_Struct.ADC_Resolution = ADC_Resolution_12b;                              //ADC resolution
	ADC_Init(ADCx, &ADC_Init_Struct);
	
	/*--------------------------------------------------------------------------*/
	/* Auto delay mode must be enable if using ADC modedule 
	in idependent mode and multi channel converting*/
	ADC_AutoDelayCmd(ADCx, DISABLE);                                                    //This mode used conjungtion with 
	                                                                                   //ADC continuous mode to handle ADC convertion     
	/*--------------------------------------------------------------------------*/
	/* Analog watchdog config (if used) */
//  ADC_AnalogWatchdogCmd(ADCx, ADC_AnalogWatchdog_AllRegEnable);	                     //Chon nhom kenh duoc ch phep
//	ADC_AnalogWatchdog1ThresholdsConfig(ADCx, 4095, 0);                                //Cai dat nguong so 1 
//	ADC_AnalogWatchdog1SingleChannelConfig(ADCx, ADC_Channel_11);                       //Chon kenh cu the
	
	/*--------------------------------------------------------------------------*/
	/* ADC DMA request enable end interrupt config */
	//ADC_ITConfig(ADC1, ADC_IT_EOC , ENABLE);
	
	/* Interrupt config if need */
//	ADC_Interrupt_Struct.NVIC_IRQChannel = ADC1_2_IRQn;                      //Interrupt channel position
//	ADC_Interrupt_Struct.NVIC_IRQChannelPreemptionPriority = 0;              //First priority
//	ADC_Interrupt_Struct.NVIC_IRQChannelSubPriority = ADC_ISR_PRIORITY;      //Second priority
//	ADC_Interrupt_Struct.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&ADC_Interrupt_Struct);
	
	/*--------------------------------------------------------------------------*/
	/* ADC1 regular group channels select and config, each channel need 
	one saperate follwing function to config */
	ADC_RegularChannelConfig(ADCx, ADC_Channel_12, 1, ADC_SampleTime_61Cycles5);  
}