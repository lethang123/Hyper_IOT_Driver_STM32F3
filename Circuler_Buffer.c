#include "Circuler_Buffer.h"

/******************************************************************************/
/**
 * @brief        CIRCULAR BUFFER initialize circular buffer
 * 
 * @param[in]    *CBuffer  address of circular buffer
 * @param[in]    *array    address array save data    
 * @param[in]     size     size of buffer
 * @note
 */
void Circular_Buff_Init(Circuler_Buffer_type *CBuffer, volatile uint8_t *array, uint16_t size)
{
	CBuffer->Buffer = array; //"Buffer" is a ptr, so this instruction mean that ptr point to start add of "array" 
	CBuffer->Head = 0;
	CBuffer->Tail = 0;
	CBuffer->Size = size;
	CBuffer->Data_loss = false;
}

/******************************************************************************/
/**
 * @brief        CIRCULAR BUFFER initialize circular buffer
 * 
 * @param[in]    *CBuffer  address of circular buffer
 * @note
 */
void Circular_Buff_Reset(Circuler_Buffer_type *CBuffer)
{
	CBuffer->Head = 0;
	CBuffer->Tail = 0;
	CBuffer->Data_loss = false;
}

/******************************************************************************/
/**
 * @brief        CIRCULAR BUFFER initialize circular buffer
 * 
 * @param[in]    *CBuffer  address of circular buffer
 * @note
 */
bool Circular_Buff_Full(Circuler_Buffer_type *CBuffer)
{
	return(((CBuffer->Head+1)%CBuffer->Size)==CBuffer->Tail);
}

/******************************************************************************/
/**
 * @brief        CIRCULAR BUFFER initialize circular buffer
 * 
 * @param[in]    *CBuffer  address of circular buffer
 * @note
 */
bool Circular_Buff_Empty(Circuler_Buffer_type *CBuffer)
{
	return(CBuffer->Head == CBuffer->Tail);
}

/******************************************************************************/
/**
 * @brief        CIRCULAR BUFFER put data to buffer
 * 
 * @param[in]    *CBuffer  address of circular buffer
 * @param[in]    data is data put in buffer
 * @note
 */
void Circular_Buffer_Put(Circuler_Buffer_type *CBuffer, uint8_t data)
{
	if(!Circular_Buff_Full(CBuffer))
	{
		CBuffer->Buffer[CBuffer->Head] = data;
		CBuffer->Head = (CBuffer->Head + 1)%CBuffer->Size;
	}
	if(CBuffer->Head == CBuffer->Tail)       //this instructions make some confusing, this is to check
	{                                        //after each time we push data in to buffer, but before push
		CBuffer->Tail = (CBuffer->Tail + 1)%CBuffer->Size;   //data into buffer, we have been checked buffer
		CBuffer->Data_loss = true;                           //full or not.
	}
}

/******************************************************************************/
/**
 * @brief        Delate previus element of buffer if received packspace or delete command
                 This function may be used in project, that have interface with human           
 * 
 * @param[in]    *CBuffer  address of circular buffer
 * @param[in]    data is data put in buffer
 * @note
 */
void Circular_Buffer_PutBackSpace(Circuler_Buffer_type *CBuffer)
{
	if(CBuffer->Head > CBuffer->Tail)
	  CBuffer->Head = CBuffer->Head - 1;
		CBuffer->Buffer[CBuffer->Head] = 0;
}

/******************************************************************************/
/**
 * @brief        CIRCULAR BUFFER Get data from buffer
 * 
 * @param[in]    *CBuffer  address of circular buffer
 * @param[in]    *data get from buffer
 * @note
 */
bool Circular_Buffer_Get(Circuler_Buffer_type *CBuffer, uint8_t *data)
{
  if(!Circular_Buff_Empty(CBuffer)&&(CBuffer->Data_loss == false))
	{
		*data = CBuffer->Buffer[CBuffer->Tail];
		CBuffer->Tail = (CBuffer->Tail + 1)%CBuffer->Size;
		return true;
	}
	else
	{
		return false;
	}
}