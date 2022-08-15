#include "UART_function.h"

/******************************************************************************/
/**
  * @brief  Init UART module for nucleo64 kit (UART2)
  * @param  
  * @retval 
  */
void USARTx_Init(USART_TypeDef* USARTx)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	if(USARTx == USART2)
	{
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	}
	
	/* USARTx configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled */
	
	USART_DeInit(USARTx);
  USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = (USART_Mode_Tx | USART_Mode_Rx);
	USART_Init(USARTx, &USART_InitStructure);
	
	/* USART interrupt config */
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_ISR_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USARTx, ENABLE);
}

/******************************************************************************/
void USARTx_TX_Char(USART_TypeDef* USARTx, char data)
{
	/* Transmit one char and wairt until compeate */
	
	while((READ_BIT(USARTx->ISR, USART_ISR_TXE)==0)
	&&(READ_BIT(USARTx->ISR, USART_ISR_TC)==0));
	CLEAR_BIT(USARTx->ICR, USART_ICR_TCCF);
	USARTx->TDR = (data & (uint16_t)0x01FF); 
}

/******************************************************************************/
void USARTx_TX_Cir(USART_TypeDef* USARTx, uint8_t *data, uint16_t *Tail_Ptr, uint16_t Buffer_size)
{
	/* Transmit one char and wairt until compeate with input param is a pointer */
	
	while((READ_BIT(USARTx->ISR, USART_ISR_TXE)==0)
	&&(READ_BIT(USARTx->ISR, USART_ISR_TC)==0));
	CLEAR_BIT(USARTx->ICR, USART_ICR_TCCF);
	USARTx->TDR = *data; 
	*Tail_Ptr = (*Tail_Ptr + 1)% Buffer_size;
}

/******************************************************************************/
void USARTx_TX(USART_TypeDef* USARTx, char data)
{
	/* Transmit one char */
	
	while(!READ_BIT(USARTx->ISR, USART_ISR_TXE));
	USARTx->TDR = data;
}

/******************************************************************************/
void USARTx_TX_Str(USART_TypeDef* USARTx, char *string)
{
	/* Transmit one string and wairt until compleate */
	
	while(*string)
	USARTx_TX(USARTx, *string++);
	while(!READ_BIT(USARTx->ISR, USART_ISR_TC));
	SET_BIT(USARTx->ICR, USART_ICR_TCCF);
}

/******************************************************************************/
uint16_t USARTx_RX_CheckData(USART_TypeDef* USARTx)
{
	/* Cheack receiver byte, if one of error occur 
	cancel receiver byte and transmit "!" to warning*/
	
	if((USART2->ICR&0xFFFFFFF0)!=0)
	{
		USART2->ISR &= 0xFFFFFFF0;
		return RX_fault;
	}
	else
		return USART2->RDR;
}

/******************************************************************************/
void USARTx_TX_IntNumber(USART_TypeDef* USARTx, unsigned char *Start_add)
{
	USARTx_TX_Char(USARTx, *Start_add);
	USARTx_TX_Char(USARTx, *(Start_add+1));
}

/******************************************************************************/
void USARTx_TX_FloatNumber(USART_TypeDef* USARTx, unsigned char *Start_add)
{
	USARTx_TX_Char(USARTx, *Start_add);
	USARTx_TX_Char(USARTx, *(Start_add+1));
	USARTx_TX_Char(USARTx, *(Start_add+2));
	USARTx_TX_Char(USARTx, *(Start_add+3));
}