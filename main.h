/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

//#define TEST_RX
//#define TEST_MOTOR
//#define DEMO
//#define SMALL_LCD
//#define DROP_CHECK_MULTI
#include "math.h"

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "SysTick_Delay.h"
#include "stm32f30x_it.h"
#include "UART_function.h"
#include "Circuler_Buffer.h"
#include "ADC_function.h"

#define Max 1000

/* Exported types ------------------------------------------------------------*/
typedef struct task
{
	bool Task_active;
	char Task_status;
	unsigned int Task_timeout;
}Task;

enum dropss
{
	dropss_nodrop,
	dropss_valid,
	dropss_invalid
};

/* Task creation */
enum task_ID
{
	task_Diagnose,          //tu chuan doan tinh trang
	task_SlotReset,         //reset all slot
	task_ReadTemp,				 	//doc nhiet do
	task_Comb_One_Single,	  //cai dat mot slot nao do la slot don
	task_Comb_One_Double,   //cai dat moot slot nao do la slot doi
	task_Comb_all_Single,   //cai dat tat ca cac slot la slot don
	task_Settemp,           //cai dat nhiet do may
	task_Settime_Defrost,   //cai dat thoi gian pha bang
	task_Settime_Working,   //cai dat thoi gian lam lanh
	task_Settime_Down,			//cai dat thoi gian nghi
	task_Light,             //dieu khien bat tat den
	task_Mode_OneBelt,      //cai dat mot slot kieu bang tai
	task_Mode_OneSpring,    //cai dat mot slot kieu lo xo
	task_Mode_AllBelt,      //cai dat tat ca cac slot kieu belt
	task_Mode_AllSpring,		//cai dat tat ca cac slot kieu lo xo
	task_Product_Dispense   //NHA HANG
};

enum task_Status
{
	task_status_stop,     //task active but not doing
	task_status_doing,    //task active and doing
	task_status_error     //task have problem
};

/* Exported constants --------------------------------------------------------*/
#define TEST
#define CNT_LED_TH 1000
#define CNT_MESS_TH 1000

/* Exported macro ------------------------------------------------------------*/
#define led      GPIO_ODR_5
#define CBuffer1_Size 256

#define CMD_START (unsigned char)0xAA            //byte start
#define CMD_DIAGNOSE (unsigned char)0x64         //code de chuan doan tinh trang cac thiet bi
#define CMD_SLOTRESET (unsigned char)0x65        //reset tat ca cac slot
#define CMD_READTEMP (unsigned char)0xDC         //doc nhiet do tu
#define CMD_COMB_ONE_SINGLE (unsigned char)0xC9  //set mot slot la slot don
#define CMD_COMB_ONE_DOUBLE (unsigned char)0xCA  //set mot slot la slot doi
#define CMD_COMB_ALL_SINGLE (unsigned char)0xCB  //set tat ca cac slot la slot don
#define CMD_SETTEMP (unsigned char)0xCF          //cai dat nhiet do        
#define CMD_SETTIME_DEFROST (unsigned char)0xD1  //cai dat thoi gian pha bang
#define CMD_SETTIME_WORKING (unsigned char)0xD2  //cai dat thoi gian cha cua may nen
#define CMD_SETTIME_DOWN (unsigned char)0xD3     //cai dat thoi gian nghi cua may nen
#define CMD_LIGHT (unsigned char)0xDD            //cai dat den led
#define CMD_MODE_ONE_BELT (unsigned char)0x68    //cai dat mot slot la kieu bang tai
#define CMD_MODE_ONE_SPRING (unsigned char)0x74  //cai dat mot slot la kieu lo xo
#define CMD_MODE_ALL_BELT (unsigned char)0x76    //cai dat tat ca cac slot la kieu bang tai
#define CMD_MODE_ALL_SPRING (unsigned char)0x75  //cai dat tat ca cac slot la kieu lo xo
#define CMD_STOP (unsigned char)0x55             //byte stop

#define CMD2_DISP_NOSENSOR (unsigned char)0x55
#define CMD2_DISP_SENSOR (unsigned char)0xAA
#define CMD2_LIGHT_ON (unsigned char)0xAA
#define CMD2_LIGHT_OFF (unsigned char)0x55
#define CMD2_DEFAULT (unsigned char)0x55

#define REP_B1_NORMAL 0x5D
#define REP_B1_FAIL 0x5C

#define REP_MOS_NORMAL 0x00
#define REP_PMOS_SHORT 0x10
#define REP_NMOS_SHORT 0x20
#define REP_MOTOR_SHORT 0x30
#define REP_MOTOR_OPEN 0x40
#define REP_MOTOR_TIMEOUT 0x50

#define REP_DROP_NORMAL 0x01
#define REP_DROP_WHENNON 0x02
#define REP_DROP_NON 0x03
#define REP_DROP_WHENMOTOR 0x04

#define MACHINE_TRACE 6
#define MACHINE_TRACE_SLOT 10
#define MACHINE_SLOT 60

#define ADC_ISR_PRIORITY 1
#define UART_ISR_PRIORITY 0
#define EXTI_ISR_PRIORITY 2

enum cmd_And2Con
{
	cmd_Start,
	cmd_Serial,
	cmd_Serial_buf,
	cmd_Code1,
	cmd_Code1_buf,
	cmd_Code2,
	cmd_Code2_buf,
	cmd_Stop
};

enum rep_Con2And
{
	rep_Serial,
	rep_Fault,
	rep_Code,
	rep_Dispense,
	rep_Checksum
};

/* "task_Product_Dispense" Task status */
enum product_Dispense_Status
{
	product_Dispense_relay1,
	product_Dispense_relay2
};


/* Private variables ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void GPIOx_bit_toggle(GPIO_TypeDef* GPIOx, uint32_t bit);

#endif 

