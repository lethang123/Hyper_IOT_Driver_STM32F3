/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CIRCULER_BUFFER_H
#define __CIRCULER_BUFFER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include <stdio.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
typedef struct 
{
	volatile uint8_t *Buffer; //Buffer data
	uint16_t Head;            //Head pointer to put data in buffer
	uint16_t Tail;            //Tail pointer to get data from buffer
	uint16_t Size;            //Size of buffer
	bool Data_loss;           //Flag to note that data loss when put it in buffer 
}Circuler_Buffer_type;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Circular_Buff_Init(Circuler_Buffer_type *CBuffer, volatile uint8_t *array, uint16_t size);
void Circular_Buff_Reset(Circuler_Buffer_type *CBuffer);
bool Circular_Buff_Full(Circuler_Buffer_type *CBuffer);
bool Circular_Buff_Empty(Circuler_Buffer_type *CBuffer);
void Circular_Buffer_Put(Circuler_Buffer_type *CBuffer, uint8_t data);
void Circular_Buffer_PutBackSpace(Circuler_Buffer_type *CBuffer);
bool Circular_Buffer_Get(Circuler_Buffer_type *CBuffer, uint8_t *data);

#endif 