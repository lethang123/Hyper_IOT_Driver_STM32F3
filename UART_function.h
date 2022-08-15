#ifndef __UART_FUNCTION_H
#define __UART_FUNCTION_H

#include "main.h"
#include "User_GPIO.h"

/******************************************************************************/
#define RX_fault 0x21
#define BACKSPACE 0x7f
#define ENTER 0x0D
#define USART2_Buff_Size 10
/******************************************************************************/
#define UART2_TX_Pin GPIO_Pin_2 
#define UART2_RX_Pin GPIO_Pin_3 
#define UART2_TX_Port GPIOA
#define UART2_RX_Port GPIOA

/******************************************************************************/
void USARTx_Init();
void USARTx_TX_Char(USART_TypeDef* USARTx, char data);
void USARTx_TX_Cir(USART_TypeDef* USARTx, uint8_t *data, uint16_t *Tail_Ptr, uint16_t Buffer_size);
void USARTx_TX(USART_TypeDef* USARTx, char data);
void USARTx_TX_Str(USART_TypeDef* USARTx, char *string);
uint16_t USARTx_RX_CheckData(USART_TypeDef* USARTx);
void USARTx_TX_IntNumber(USART_TypeDef* USARTx, unsigned char *Start_add);
void USARTx_TX_FloatNumber(USART_TypeDef* USARTx, unsigned char *Start_add);

#endif 
