#ifndef __USER_GPIO_H
#define __USER_GPIO_H

#include "main.h"

#define SLOT0 GPIO_Pin_12
#define SLOT1 GPIO_Pin_11
#define SLOT2 GPIO_Pin_10
#define SLOT3 GPIO_Pin_9
#define SLOT4 GPIO_Pin_8
#define SLOT5 GPIO_Pin_15
#define SLOT6 GPIO_Pin_14
#define SLOT7 GPIO_Pin_13
#define SLOT8 GPIO_Pin_12
#define SLOT9 GPIO_Pin_11

#define SLOT0_P GPIOA
#define SLOT1_P GPIOA
#define SLOT2_P GPIOA
#define SLOT3_P GPIOA
#define SLOT4_P GPIOA
#define SLOT5_P GPIOB
#define SLOT6_P GPIOB
#define SLOT7_P GPIOB
#define SLOT8_P GPIOB
#define SLOT9_P GPIOB

#define TRACE0 (uint16_t)GPIO_Pin_15
#define TRACE1 (uint16_t)GPIO_Pin_3
#define TRACE2 (uint16_t)GPIO_Pin_4
#define TRACE3 (uint16_t)GPIO_Pin_5
#define TRACE4 (uint16_t)GPIO_Pin_6
#define TRACE5 (uint16_t)GPIO_Pin_7

#define TRACE0_P GPIOA
#define TRACE1_P GPIOB
#define TRACE2_P GPIOB
#define TRACE3_P GPIOB
#define TRACE4_P GPIOB
#define TRACE5_P GPIOB

#define UART1_TX GPIO_Pin_9
#define UART1_RX GPIO_Pin_8
#define UART2_TX GPIO_Pin_2
#define UART2_RX GPIO_Pin_3

#define DROPSS_IN GPIO_Pin_0
#define DROPSS_OUT GPIO_Pin_1

#define HEAT GPIO_Pin_13       //Heat relay control pin
#define COOL GPIO_Pin_14       //Cool air relay control pin
#define FAN GPIO_Pin_15        //Fan control pin

#define DOOR GPIO_Pin_0        //Door status input pin
#define LED GPIO_Pin_1         //LED pin

#define LIGHT GPIO_Pin_2
#define DOOR_HEAT GPIO_Pin_10

void GPIO_USART2_Init(void);
void GPIO_USART1_Init(void);
void GPIO_ADC_Init();
void GPIO_IO_Init();

#endif /* __USER_GPIO_H */