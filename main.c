/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern Circuler_Buffer_type Buff;
extern volatile uint8_t Buff_RX;
static uint8_t volatile Buff_Array[CBuffer1_Size];
extern unsigned int cnt_Dropss_Disable;
extern unsigned int cnt_Global;
extern unsigned int ADC_product_Dispense[3];
extern bool Task_Operation;
unsigned int ADC_result;
extern int mask_cnt;
extern char mask;

extern unsigned int phase;
extern unsigned int phase_1;
extern unsigned int phase_2;
extern unsigned int phase_3;
extern unsigned int phase_4;

extern unsigned int timeout;

/* Task creation */
Task task[16];
/* Private function prototypes -----------------------------------------------*/
/******************************************************************************/
void GPIOx_bit_toggle(GPIO_TypeDef* GPIOx, uint32_t bit)
{
  if(READ_BIT(GPIOx->ODR, bit))
	{
		 CLEAR_BIT(GPIOx->ODR, bit);
	}
	else
	{
		 SET_BIT(GPIOx->ODR, bit);
	}
}

/* Private functions ---------------------------------------------------------*/
void Task_init(Task *task)
{
	task[task_Diagnose].Task_active = false;
	task[task_Diagnose].Task_status = task_status_stop;
	task[task_Diagnose].Task_timeout = 5000;
	
	task[task_SlotReset].Task_active = false;
	task[task_SlotReset].Task_status = task_status_stop;
	task[task_SlotReset].Task_timeout = 5000;
	
	task[task_ReadTemp].Task_active = false;
	task[task_ReadTemp].Task_status = task_status_stop;
	task[task_ReadTemp].Task_timeout = 5000;
	
	task[task_Comb_One_Single].Task_active = false;
	task[task_Comb_One_Single].Task_status = task_status_stop;
	task[task_Comb_One_Single].Task_timeout = 5000;
	
	task[task_Comb_One_Double].Task_active = false;
	task[task_Comb_One_Double].Task_status = task_status_stop;
	task[task_Comb_One_Double].Task_timeout = 5000;
	
	task[task_Comb_all_Single].Task_active = false;
	task[task_Comb_all_Single].Task_status = task_status_stop;
	task[task_Comb_all_Single].Task_timeout = 5000;
	
	task[task_Settemp].Task_active = false;
	task[task_Settemp].Task_status = task_status_stop;
	task[task_Settemp].Task_timeout = 5000;
	
	task[task_Settime_Defrost].Task_active = false;
	task[task_Settime_Defrost].Task_status = task_status_stop;
	task[task_Settime_Defrost].Task_timeout = 5000;
	
	task[task_Settime_Working].Task_active = false;
	task[task_Settime_Working].Task_status = task_status_stop;
	task[task_Settime_Working].Task_timeout = 5000;
	
	task[task_Settime_Down].Task_active = false;
	task[task_Settime_Down].Task_status = task_status_stop;
	task[task_Settime_Down].Task_timeout = 5000;
	
	task[task_Light].Task_active = false;
	task[task_Light].Task_status = task_status_stop;
	task[task_Light].Task_timeout = 5000;
	
	task[task_Mode_OneBelt].Task_active = false;
	task[task_Mode_OneBelt].Task_status = task_status_stop;
	task[task_Mode_OneBelt].Task_timeout = 5000;
	
	task[task_Mode_OneSpring].Task_active = false;
	task[task_Mode_OneSpring].Task_status = task_status_stop;
	task[task_Mode_OneSpring].Task_timeout = 5000;
	
	task[task_Mode_AllBelt].Task_active = false;
	task[task_Mode_AllBelt].Task_status = task_status_stop;
	task[task_Mode_AllBelt].Task_timeout = 5000;
	
	task[task_Mode_AllSpring].Task_active = false;
	task[task_Mode_AllSpring].Task_status = task_status_stop;
	task[task_Mode_AllSpring].Task_timeout = 5000;
	
	task[task_Product_Dispense].Task_active = false;
	task[task_Product_Dispense].Task_status = task_status_stop;
	task[task_Product_Dispense].Task_timeout = 5000;
}

/**********************************************************************************************/
void TIMER6_timebase()
{
	NVIC_InitTypeDef NVIC_Timer6;
  TIM_TimeBaseInitTypeDef TimeBase_Timer6;
	
	TIM_DeInit(TIM6);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/*--------------------------------------------------------------------------------*/
	/* Time Base configuration */
	TimeBase_Timer6.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBase_Timer6.TIM_Prescaler = 63;
	TimeBase_Timer6.TIM_Period = 999;
	TIM_TimeBaseInit(TIM6, &TimeBase_Timer6);
	
	/*--------------------------------------------------------------------------------*/
	/* Interrupt config if need */
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_Timer6.NVIC_IRQChannel = TIM6_DAC1_IRQn;
	NVIC_Timer6.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Timer6.NVIC_IRQChannelSubPriority = 0;
	NVIC_Timer6.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Timer6);
	TIM_SetCounter(TIM6, 0);
	SET_BIT(TIM6->EGR, TIM_EGR_UG);                    //Update registers when compleate setup
	SET_BIT(TIM6->CR1, TIM_CR1_CEN);                   //Enable counter clock
}

/**********************************************************************************************/
bool check_Time(unsigned int *cnt_local, unsigned int cnt_global, unsigned int thresholed)
{
	unsigned int result;
	
	if(cnt_global >= * cnt_local)
		result = cnt_global - *cnt_local;
	else
		result = *cnt_local - cnt_global;
	
	if(result > thresholed)
	{
		*cnt_local = cnt_global;
		return true;
	}
	else
		return false;
}

/**********************************************************************************************/
bool check_TaskActive(Task *task, char begin)
{
	bool havetask;
	
	if(begin == 0x01)
	{
		for(int i = 0; i < 16; i++)
		{
			if(task[i].Task_active == true)
			{
				havetask = false;
				break;
			}
			else
				havetask = true;
		}
	}
	else if(begin == 0x02)
	{
		for(int i = 0; i < 16; i++)
		{
			if(task[i].Task_active == true)
			{
				havetask = true;
				break;
			}
			else
				havetask = false;
		}
	}
	else if(begin == 0x03)
	{
		for(int i = 0; i < 16; i++)
		{
			if(task[i].Task_status == task_status_doing)
			{
				havetask = true;
				break;
			}
			else
				havetask = false;
		}
	}
	
	return havetask;
}


/**********************************************************************************************/
bool check_Mottor_Toggle(unsigned int ADC_value, unsigned int th_down)
{
	bool result = false;
	static char godown_cnt = 0;
	
	if(ADC_value < th_down)
	{
		godown_cnt++;
		if(godown_cnt > 3)
		{
			godown_cnt = 0;
			result = true;
		}
	}
	else
		godown_cnt = 0;
	
	return result;
}

/**********************************************************************************************/
void Con2And_send(unsigned char* and2con, unsigned char Serial, unsigned char Fault, unsigned char Code, unsigned char Dispense, bool drop)
{
	and2con[rep_Serial] = Serial;
	and2con[rep_Fault] = Fault;
	and2con[rep_Code] = Code;
	if(drop)
		and2con[rep_Dispense] = Dispense;
	else
		and2con[rep_Dispense] = 0x00;
	
	and2con[rep_Checksum] = and2con[rep_Serial] + and2con[rep_Fault] + and2con[rep_Code] + and2con[rep_Dispense];
	
	for(int i = rep_Serial; i <= rep_Checksum; i++)
	{
		USARTx_TX_Char(USART2, and2con[i]);
		and2con[i] = 0;
	}
}

/* Main functions ------------------------------------------------------------*/
int main(void)
{
	char Data;
	static unsigned char Sequence = 0;
	static unsigned char cmd_MesDone = 0;
	unsigned char And2Con[8];
	unsigned char Con2And[5];
	unsigned int Slot[10] = {SLOT0, SLOT1, SLOT2, SLOT3, SLOT4, SLOT5, SLOT6, SLOT7, SLOT8, SLOT9};
	GPIO_TypeDef * Slot_P[10] = {SLOT0_P, SLOT1_P, SLOT2_P, SLOT3_P, SLOT4_P, SLOT5_P, SLOT6_P, SLOT7_P, SLOT8_P, SLOT9_P};
	unsigned int Trace[6] = {TRACE0, TRACE1, TRACE2, TRACE3, TRACE4, TRACE5};
	GPIO_TypeDef * Trace_P[6] = {TRACE0_P, TRACE1_P, TRACE2_P, TRACE3_P, TRACE4_P, TRACE5_P};
	static char Dropss = dropss_nodrop;
	unsigned int slot;
	unsigned int trace;
	
	GPIO_InitTypeDef GPIO_led_init;
	
  GPIO_DeInit(GPIOC);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOA);  
	
	  /* GPIO Periph clock enable */
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN);  
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOBEN);
  SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOCEN); 
	SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOFEN); 
	
	GPIO_led_init.GPIO_Pin = GPIO_Pin_5;
  GPIO_led_init.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_led_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_led_init.GPIO_OType = GPIO_OType_PP;
  GPIO_led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
  //GPIO_Init(GPIOA, &GPIO_led_init);
	
	GPIO_ResetBits(Trace_P[0], Trace[0]);
  
  /* Basic function config */	
  SysTick_init();
	/* Init interrupt group */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* Peripheral function config */
	GPIO_IO_Init();
	GPIO_USART2_Init();
	GPIO_ADC_Init();
	Circular_Buff_Init(&Buff, Buff_Array, CBuffer1_Size);
	ADC_Indep_Init(ADC2);
	ADC1_Indep_Init(ADC1);
  USARTx_Init(USART2);
	Task_init(task);
	
	// Add by Toan - Control freezer
	char Display[20];
	extern int dem;
	extern int dem1;
	extern int check_hex;
	extern int check_light;
	
	int Status_check = 0;
	int time_active_avai = 5;  // chu ky dinh san theo phut
	int time_rest_avai = 5;
	int time_active_require = 5;  // chu ky yeu cau theo phut
	int time_rest_require = 5;
	
	int time_temp_active = 10; // chu ky dinh san theo phut
	int time_temp_rest = 10;
	int temp_require;

	int active = 0;
	float sum = 0;
	double sum1 = 0;
	double Voltage_1 = 0;
	double Voltage_2 = 0;
	double Vol_R0 = 0; 
	double Divide_R0 = 0;
	double Rx = 0;
	double A_NTC = 1.009249522e-03;
	double B_NTC = 2.378405444e-04;
	double C_NTC = 2.019202697e-07;
	double T1 = 0;
	double T2 = 0;
	char Temp_C = 0;
	// Add by Toan - Control freezer
	
	for(char i = 10; i > 0 ; i--)
	{
		GPIOx_bit_toggle(GPIOF, GPIO_Pin_1);
	  Delay_ms(200);
	}	
	
	TIMER6_timebase();

	/**********************************************************************************************/
  while(1)
  {
		/* For check drop sensor 2 */
		static unsigned int count_on = 0;
		static unsigned int count_off = 0;
		static unsigned char count_drop = 0;
		
		static unsigned int cnt_led = 0;
		if(check_Time(&cnt_led, cnt_Global, CNT_LED_TH))
		{
			GPIOx_bit_toggle(GPIOF, GPIO_Pin_1);
			//USARTx_TX_Str(USART2, "Drop sensor\r\n");
		}

		/**********************************************************************************************/
		#ifdef DROP_CHECK_MULTI
		/* Check Drop sensor */
		if(READ_BIT(GPIOA->IDR, GPIO_IDR_0) == 0)
		{
			if(cnt_Dropss_Disable > 5)
			{
				cnt_Dropss_Disable = 0;
				EXTI->IMR |= EXTI_IMR_MR0;
				if(task[task_Product_Dispense].Task_status == task_status_doing)
				{
					Dropss = dropss_valid;
				}
				else if(task[task_Product_Dispense].Task_status == task_status_stop)
				{
					Dropss = dropss_invalid;
					#ifdef DEMO
					static unsigned int cnt_dropss  = 0;
					if(check_Time(&cnt_dropss, cnt_Global, 500))
						USARTx_TX_Str(USART2, "Drop sensor invalid\r\n");
					#else
					static bool first_time = false;
					static unsigned int cnt_dropss  = 0;
					Con2And[rep_Serial] = 0x00;
					Con2And[rep_Fault] = REP_B1_FAIL;
					Con2And[rep_Code] = REP_B1_FAIL;
					Con2And[rep_Dispense] = REP_DROP_WHENNON;
					Con2And[rep_Checksum] = Con2And[rep_Serial] + Con2And[rep_Fault] + Con2And[rep_Code] + Con2And[rep_Dispense];
					
					if(first_time == false)
					{
						for(int i = rep_Serial; i <= rep_Checksum; i++)
							USARTx_TX_Char(USART2, Con2And[i]);
						first_time = true;
					}
					else
					{
						if(check_Time(&cnt_dropss, cnt_Global, 2000))
						{
							for(int i = rep_Serial; i <= rep_Checksum; i++)
								USARTx_TX_Char(USART2, Con2And[i]);
							first_time = false;
						}
					}
					#endif
				}
			}
		}
		
		#else
		/* Check Drop sensor 2 */
		if(READ_BIT(GPIOA->IDR, GPIO_IDR_0) == 0)
		{
			if(check_Time(&count_on, cnt_Global, 5000))
			{
				if(task[task_Product_Dispense].Task_status == task_status_doing)
				{
					Dropss = dropss_valid;
					count_drop++;
				}
				else if(task[task_Product_Dispense].Task_status == task_status_stop)
				{
					Dropss = dropss_invalid;
					count_drop = 0;
					Con2And_send(Con2And, 0x55, 0x55, 0x55, 0x55, true);
				}
			}
		}
		else
		{
			if(check_Time(&count_off, cnt_Global, 50))
			{
				count_drop = 0;
			}
		}
		
		//Count number of time sensor detect
//		if(count_drop == 1)
//		{
//			static unsigned int count = 0;
//			Dropss = dropss_valid;
//			
//			if(check_Time(&count, cnt_Global, 500))
//				USARTx_TX_Str(USART2, "Dropss valid\r\n");
//		}
//		else if(count_drop > 1)
//		{
//			static unsigned int count = 0;
//			Dropss = dropss_invalid;
//			
//			if(check_Time(&count, cnt_Global, 500))
//				USARTx_TX_Str(USART2, "Dropss invalid - drop multi\r\n");
//		}
		#endif
		
		/**********************************************************************************************/
		/* Check Message valid or not */
		if(check_hex == 10 && Status_check == 1)
		{
			for(int i = cmd_Start; i < cmd_Stop; i++)
			{
				And2Con[i] = 0;
			}
			Sequence = cmd_Start;
			check_hex = 0;
			Status_check = 0;
		}
		
		if(Circular_Buffer_Get(&Buff, &Data) && (!cmd_MesDone))
		{
			switch (Sequence)
			{
				case cmd_Start:
					if(Data == CMD_START)
					{
						And2Con[Sequence] = Data;
						Sequence = cmd_Serial;
					}
					check_hex = 0;
					Status_check = 1;
					break;
					
				case cmd_Serial:
					And2Con[Sequence] = Data;
					Sequence = cmd_Serial_buf;
					Status_check = 1;
					break;
				
				case cmd_Serial_buf:
					And2Con[Sequence] = Data;
					Sequence = cmd_Code1;
					Status_check = 1;
					break;
				
				case cmd_Code1:
					And2Con[Sequence] = Data;
					Sequence = cmd_Code1_buf;
					Status_check = 1;
					break;
				
				case cmd_Code1_buf:
					And2Con[Sequence] = Data;
					Sequence = cmd_Code2;
					Status_check = 1;
					break;
				
				case cmd_Code2:
					And2Con[Sequence] = Data;
					Sequence = cmd_Code2_buf;
					Status_check = 1;
					break;
				
				case cmd_Code2_buf:
					And2Con[Sequence] = Data;
					Sequence = cmd_Stop;
					Status_check = 1;
					break;
				
				case cmd_Stop:
					if(Data == CMD_STOP)
					{
						And2Con[Sequence] = Data;
						//Checksum
						if(((And2Con[cmd_Serial] + And2Con[cmd_Serial_buf]) == 255) && 
							((And2Con[cmd_Code1] + And2Con[cmd_Code1_buf]) == 255) &&
						  ((And2Con[cmd_Code2] + And2Con[cmd_Code2_buf]) == 255))
							cmd_MesDone = 1;
						  Status_check = 0;
					}
					else
					{
						for(int i = cmd_Start; i < cmd_Stop; i++)
							And2Con[i] = 0;
					}
					Sequence = cmd_Start;
					break;
			}
		}
		
		/**********************************************************************************************/
		/* Classify message */
		#ifdef TEST_RX
		if(cmd_MesDone)
		{
			cmd_MesDone = 0;
			USARTx_TX_Str(USART2, "Mess is:\r\n");
			
			//Check serial number
			if(And2Con[cmd_Serial] == 0)
				USARTx_TX_Str(USART2, "Serial number correct\r\n");
			else
				USARTx_TX_Str(USART2, "Serial number incorrect\r\n");
		
			//Check if is product dispense
			if((0x01 <= And2Con[cmd_Code1]) && (And2Con[cmd_Code1] <= 0x50))
			{
				char Arr[5];
				sprintf(Arr, "%d", And2Con[cmd_Code1]);
				USARTx_TX_Str(USART2, "CODE1 - Product dispense: ");
				USARTx_TX_Str(USART2, Arr);
				USARTx_TX_Str(USART2, "\r\n");
				if(And2Con[cmd_Code2] == 0x55)
					USARTx_TX_Str(USART2, "CODE2 - Without drop sensor function\r\n");
				else if(And2Con[cmd_Code2] == 0xAA)
					USARTx_TX_Str(USART2, "CODE2 - With drop sensor function\r\n");
				else
					USARTx_TX_Str(USART2, "CODE2 - Error\r\n");
			}
			
			//Check if is other mess
			else
			{
				//Check code1
				switch (And2Con[cmd_Code1])
				{
					//SELF DIAGNOSE
					case CMD_DIAGNOSE:
					{
						USARTx_TX_Str(USART2, "CODE1 - Driver dianose\r\n");
						if(And2Con[cmd_Code2] == CMD2_DEFAULT)
							USARTx_TX_Str(USART2, "CODE2 - Default\r\n");
						else
							USARTx_TX_Str(USART2, "CODE2 - incorrent\r\n");
						break;
					}
					
					//ALL SLOT RESET
					case CMD_SLOTRESET:
					{
						USARTx_TX_Str(USART2, "CODE1 - Slot reset\r\n");
						if(And2Con[cmd_Code2] == CMD2_DEFAULT)
							USARTx_TX_Str(USART2, "CODE2 - Default\r\n");
						else
							USARTx_TX_Str(USART2, "CODE2 - incorrent\r\n");
						break;
					}
					
					//READ TEMPERATURE
					case CMD_READTEMP:
					{
						USARTx_TX_Str(USART2, "CODE1 - Read Temperature\r\n");
						if(And2Con[cmd_Code2] == CMD2_DEFAULT)
							USARTx_TX_Str(USART2, "CODE2 - Default\r\n");
						else
							USARTx_TX_Str(USART2, "CODE2 - incorrent\r\n");
						break;
					}
					
					//SET ONE SOT IS SINGLE SLOT
					case CMD_COMB_ONE_SINGLE:
					{
						USARTx_TX_Str(USART2, "CODE1 - setup one slot is single\r\n");
						if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
							USARTx_TX_Str(USART2, "CODE2 - incorrent slot number\r\n");
						else
						{
							char Arr[50];
							sprintf(Arr, "CODE2 - Set slot %d is single mode\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//SET ONE SLOT IS DOUBLE SLOT
					case CMD_COMB_ONE_DOUBLE:
					{
						USARTx_TX_Str(USART2, "CODE1 - setup one slot is double\r\n");
						if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
							USARTx_TX_Str(USART2, "CODE2 - incorrent slot number");
						else
						{
							char Arr[50];
							sprintf(Arr, "Set slot %d is double mode\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//SET ALL SLOT IS SINGLE SLOT
					case CMD_COMB_ALL_SINGLE:
					{
						USARTx_TX_Str(USART2, "CODE1 - setup all slot is single\r\n");
						if(And2Con[cmd_Code2] == CMD2_DEFAULT)
							USARTx_TX_Str(USART2, "CODE2 - Default\r\n");
						else
							USARTx_TX_Str(USART2, "CODE2 - incorrent\r\n");
						break;
					}
					
					//SET TEMPERATURE
					case CMD_SETTEMP:
					{
						USARTx_TX_Str(USART2, "CODE1 - Set temperature\r\n");
						if(And2Con[cmd_Code2] > 10)
							USARTx_TX_Str(USART2, "CODE2 - incorrent temperature\r\n");
						else
						{
							char Arr[50];
							sprintf(Arr, "CODE2 - Set temperature to %d degree\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//SET DEFROST TIME
					case CMD_SETTIME_DEFROST:
					{
						USARTx_TX_Str(USART2, "CODE1 - Set time defrost\r\n");
						if(And2Con[cmd_Code2] > 50)
							USARTx_TX_Str(USART2, "CODE2 - incorrent defrost time\r\n");
						else
						{
							char Arr[50];
							sprintf(Arr, "CODE2 - Set defrost time is %d minute\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//SET WORKING TIME
					case CMD_SETTIME_WORKING:
					{
						USARTx_TX_Str(USART2, "CODE1 - Set time working\r\n");
						if(And2Con[cmd_Code2] > 20)
							USARTx_TX_Str(USART2, "CODE2 - incorrent working time\r\n");
						else
						{
							char Arr[50];
							sprintf(Arr, "CODE2 - Set working time is %d minute\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//SET DOWN TIME
					case CMD_SETTIME_DOWN:
					{
						USARTx_TX_Str(USART2, "CODE1 - Set time Down\r\n");
						if(And2Con[cmd_Code2] > 50)
							USARTx_TX_Str(USART2, "CODE2 - incorrent down time\r\n");
						else
						{
							char Arr[50];
							sprintf(Arr, "CODE2 - Set down time is %d minute\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//OPEN CLOSE LED LIGHT
					case CMD_LIGHT:
					{
						USARTx_TX_Str(USART2, "CODE1 - Setup light led\r\n");
						if(And2Con[cmd_Code2] == CMD2_LIGHT_ON)
							USARTx_TX_Str(USART2, "CODE2 - Turn on light led\r\n");
						else if(And2Con[cmd_Code2] == CMD2_LIGHT_OFF)
							USARTx_TX_Str(USART2, "CODE2 - Turn off light led\r\n");
						else
							USARTx_TX_Str(USART2, "CODE2 - incorrent led control\r\n");
						break;
					}
					
					//SET ONE SLOT IS BELT MODE
					case CMD_MODE_ONE_BELT:
					{
						USARTx_TX_Str(USART2, "CODE1 - Setup one slot is belt\r\n");
						if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
							USARTx_TX_Str(USART2, "CODE2 - incorrent slot number\r\n");
						else
						{
							char Arr[50];
							sprintf(Arr, "CODE2 - Set slot %d is belt mode\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//SET ONE SLOT IS SPRING MODE
					case CMD_MODE_ONE_SPRING:
					{
						USARTx_TX_Str(USART2, "CODE1 - Setup one slot is spring\r\n");
						if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
							USARTx_TX_Str(USART2, "CODE2 - incorrent slot number\r\n");
						else
						{
							char Arr[50];
							sprintf(Arr, "CODE2 - Set slot %d is spring mode\r\n", And2Con[cmd_Code2]);
							USARTx_TX_Str(USART2, Arr);
						}
						break;
					}
					
					//SET ALL SLOT IS BELT MODE
					case CMD_MODE_ALL_BELT:
					{
						USARTx_TX_Str(USART2, "CODE1 - Setup all slot is belt\r\n");
						if(And2Con[cmd_Code2] == CMD2_DEFAULT)
							USARTx_TX_Str(USART2, "CODE2 - Default\r\n");
						else
							USARTx_TX_Str(USART2, "CODE2 - incorrent\r\n");
						break;
					}
					
					//SET ALL SLOT IS SPRING MODE
					case CMD_MODE_ALL_SPRING:
					{
						USARTx_TX_Str(USART2, "CODE1 - Setup all slot is spring\r\n");
						if(And2Con[cmd_Code2] == CMD2_DEFAULT)
							USARTx_TX_Str(USART2, "CODE2 - Default\r\n");
						else
							USARTx_TX_Str(USART2, "CODE2 - incorrent\r\n");
						break;
					}
					
					//Code 1 error
					default:
						USARTx_TX_Str(USART2, "CODE1 - Error\r\n");
				}
			}
		}
		
		else
		{
			if(check_Time(&cnt_Mess, CNT_MESS_TH))
				USARTx_TX_Str(USART2, "Dont have mess or mess fail\r\n");
		}
		
		#else
		if(cmd_MesDone && (Task_Operation == false))
		{
			cmd_MesDone = 0;
			
			//Check serial number
			if(And2Con[cmd_Serial] == 0)
			{
				Con2And[rep_Serial] = 0x00;
			}
			else
				Con2And[rep_Fault] = REP_B1_FAIL;
				
			//Check code1 if not product dispense
			switch (And2Con[cmd_Code1])
			{
				//SELF DIAGNOSE
				case CMD_DIAGNOSE:
				{
					if(And2Con[cmd_Code2] == CMD2_DEFAULT)
						task[task_Diagnose].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//ALL SLOT RESET
				case CMD_SLOTRESET:
				{
					if(And2Con[cmd_Code2] == CMD2_DEFAULT)
						task[task_SlotReset].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//READ TEMPERATURE
				case CMD_READTEMP:
				{
					if(And2Con[cmd_Code2] == CMD2_DEFAULT)
						task[task_ReadTemp].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//SET ONE SOT IS SINGLE SLOT
				case CMD_COMB_ONE_SINGLE:
				{
					USARTx_TX(USART2, 0x55);
					if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
						task[task_Comb_One_Single].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//SET ONE SLOT IS DOUBLE SLOT
				case CMD_COMB_ONE_DOUBLE:
				{
					if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
						task[task_Comb_One_Double].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//SET ALL SLOT IS SINGLE SLOT
				case CMD_COMB_ALL_SINGLE:
				{
					if(And2Con[cmd_Code2] == CMD2_DEFAULT)
						task[task_Comb_all_Single].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//SET TEMPERATURE
				case CMD_SETTEMP:
				{
					if(And2Con[cmd_Code2] > 50)
						Con2And[rep_Fault] = REP_B1_FAIL;
					else
						task[task_Settemp].Task_active = check_TaskActive(&task[16], 0x01);
					break;
				}
				
				//SET DEFROST TIME
				case CMD_SETTIME_DEFROST:
				{
					if(And2Con[cmd_Code2] > 50)
						Con2And[rep_Fault] = REP_B1_FAIL;
					else
						task[task_Settime_Defrost].Task_active = check_TaskActive(&task[16], 0x01);
					break;
				}
				
				//SET WORKING TIME
				case CMD_SETTIME_WORKING:
				{
					if(And2Con[cmd_Code2] > 100)
						Con2And[rep_Fault] = REP_B1_FAIL;
					else{
						task[task_Settime_Working].Task_active = check_TaskActive(&task[16], 0x01);
					}
					break;
				}
				
				//SET DOWN TIME
				case CMD_SETTIME_DOWN:
				{
					if(And2Con[cmd_Code2] > 50)
						Con2And[rep_Fault] = REP_B1_FAIL;
					else{
						task[task_Settime_Down].Task_active = check_TaskActive(&task[16], 0x01);
					}
					break;
				}
				
				//OPEN CLOSE LED LIGHT
				case CMD_LIGHT:
				{
					task[task_Light].Task_active = check_TaskActive(&task[16], 0x01);
					break;
				}
				
				//SET ONE SLOT IS BELT MODE
				case CMD_MODE_ONE_BELT:
				{
					if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
						task[task_Mode_OneBelt].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//SET ONE SLOT IS SPRING MODE
				case CMD_MODE_ONE_SPRING:
				{
					if((And2Con[cmd_Code2] < 0x01) || (And2Con[cmd_Code2] > 0x50))
						task[task_Mode_OneSpring].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//SET ALL SLOT IS BELT MODE
				case CMD_MODE_ALL_BELT:
				{
					if(And2Con[cmd_Code2] == CMD2_DEFAULT)
						task[task_Mode_AllBelt].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//SET ALL SLOT IS SPRING MODE
				case CMD_MODE_ALL_SPRING:
				{
					if(And2Con[cmd_Code2] == CMD2_DEFAULT)
						task[task_Mode_AllSpring].Task_active = check_TaskActive(&task[16], 0x01);
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
					break;
				}
				
				//Check if is product dispense
				default:
				{
					//Check if is product dispense
					if((0x00 <= And2Con[cmd_Code1]) && (And2Con[cmd_Code1]) <= 0x50)
					{
						if(And2Con[cmd_Code1] >= MACHINE_TRACE_SLOT)
						{
							trace = And2Con[cmd_Code1]/MACHINE_TRACE_SLOT;    //open motor
							slot = And2Con[cmd_Code1] - (trace*MACHINE_TRACE_SLOT);
						}
						else
						{
							trace = 0;
							slot = And2Con[cmd_Code1];
						}
						task[task_Product_Dispense].Task_active = check_TaskActive(&task[16], 0x01);
					}
					//Code 1 error
					else
						Con2And[rep_Fault] = REP_B1_FAIL;
				}
			}
			
			if(Con2And[rep_Fault] == REP_B1_FAIL)
				Con2And_send(Con2And, 0x00, REP_B1_FAIL, 0x00, REP_DROP_NORMAL, false);
		}
		#endif
		
		/**********************************************************************************************/
		/* Task operation */
		if(((Task_Operation == false) && check_TaskActive(&task[16], 0x02)) || check_TaskActive(&task[16], 0x03))
		{
			/*--------------------------------------------------------------------------------*/
			/* Product dispense */
			if(task[task_Product_Dispense].Task_active)
			{
				Task_Operation = true;
				static bool NoDrop = false;
				static char ghim = 0;
				
				/* task doing */
				if(task[task_Product_Dispense].Task_status == task_status_doing)
				{		
					#ifdef DEMO
					static unsigned int cnt_PD = 0;
					if(check_Time(&cnt_PD, cnt_Global, 1000))
					{
						char Arr1[4];
						char Arr2[4];
						sprintf(Arr1, "%d", trace);
						sprintf(Arr2, "%d", slot);
						
						USARTx_TX_Str(USART2, "\r\nTrace: ");
						USARTx_TX_Str(USART2, Arr1);
						USARTx_TX_Str(USART2, "\r\n");
						USARTx_TX_Str(USART2, "Slot: ");
						USARTx_TX_Str(USART2, Arr2);
						USARTx_TX_Str(USART2, "\r\n");
						
						USARTx_TX_Str(USART2, "Prodcut dispense\r\n");
						USARTx_TX_Str(USART2, "Prodcut dispense task doing\r\n");
						
						if(Dropss == dropss_valid)
							USARTx_TX_Str(USART2, "Drop sensor detected\r\n");
						else if(Dropss == dropss_nodrop)
							USARTx_TX_Str(USART2, "Drop sensor still don't detect\r\n");
					}
					#else
					static unsigned char Motor_toggle = 0;
					
					if(ghim == 0)
					{
						GPIO_SetBits(Slot_P[slot], Slot[slot]);
						GPIO_SetBits(Trace_P[trace], Trace[trace]);

						ADC_StartConversion(ADC2);
						if(mask == 0)
							mask = 1;
						while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
						ADC_result = ADC_GetConversionValue(ADC2);
					}
					
					//Check motor start phase
					if(mask == 1)
					{
						if(ADC_result > 4095)
						{
							GPIO_ResetBits(Slot_P[slot], Slot[slot]);
							GPIO_ResetBits(Trace_P[trace], Trace[trace]);
						}
						else if(mask_cnt > 400)
						{
							mask_cnt = 0;
							mask = 2;
						}
					}
					
					//Check motor posotion stop
					else if(((mask == 2) && check_Mottor_Toggle(ADC_result, 200)) || (ghim == 1))
					{
						if(ghim == 0)
							Motor_toggle++;
						
						if((Motor_toggle >= 1) && (ghim == 0))
						{
							Motor_toggle = 0;

							GPIO_ResetBits(Trace_P[trace], Trace[trace]);
							GPIO_ResetBits(Slot_P[slot], Slot[slot]);
							
							#ifdef SMALL_LCD
							if((Dropss == dropss_nodrop) && (ghim == 0))
								ghim = 1;
							#else
							ghim = 1;
							#endif
						}
						
						if(ghim == 1)
						{
							#ifdef SMALL_LCD
							if(phase == 0)
								phase = 1;

							switch(phase)
							{
								case 1:
									if(phase_1 >= 500)
									{
										phase_1 = 0;
										phase = 2;
										GPIO_SetBits(Slot_P[slot], Slot[slot]);
										GPIO_SetBits(Trace_P[trace], Trace[trace]);
									}
									break;
								
								case 2:
									if(phase_2 >= 200)
									{
										phase_2 = 0;
										phase = 3;
										GPIO_ResetBits(Slot_P[slot], Slot[slot]);
										GPIO_ResetBits(Trace_P[trace], Trace[trace]);
									}
									break;
								
								case 3:
									if(phase_3 >= 500)
									{
										phase_3 = 0;
										phase = 4;
										GPIO_SetBits(Slot_P[slot], Slot[slot]);
										GPIO_SetBits(Trace_P[trace], Trace[trace]);
									}
									break;
									
								case 4:
									if(phase_4 >= 200)
									{
										phase_4 = 0;
										phase = 0;
										ghim = 0;
										GPIO_ResetBits(Slot_P[slot], Slot[slot]);
										GPIO_ResetBits(Trace_P[trace], Trace[trace]);
										
										unsigned char drop = 0;
										if(Dropss == dropss_valid)
											drop = REP_DROP_NORMAL;
										else 
											drop = REP_DROP_NON;
										
										Con2And_send(Con2And, 0x00, REP_B1_NORMAL, 0x00, drop, true);
										
										mask = 0;
										timeout = 0;
										task[task_Product_Dispense].Task_status = task_status_stop;
										task[task_Product_Dispense].Task_active = false;
										Task_Operation = false;
									}
									break;
							}
							#else
							ghim = 0;
							unsigned char drop = 0;
							if(Dropss == dropss_valid)
								drop = REP_DROP_NORMAL;
							else 
								drop = REP_DROP_NON;
							
							Con2And_send(Con2And, 0x00, REP_B1_NORMAL, 0x00, drop, true);
							drop = 0x00;
							
							mask = 0;
							timeout = 0;
							task[task_Product_Dispense].Task_status = task_status_stop;
							task[task_Product_Dispense].Task_active = false;
							Task_Operation = false;
							#endif
						}
					}
					
					//Check motor shorst circuit and motor open circuit
					else if(mask == 2)
					{		
						//Check motor shorst circuit
						if(ADC_result > 4095)
						{
							GPIO_ResetBits(Slot_P[slot], Slot[slot]);
							GPIO_ResetBits(Trace_P[trace], Trace[trace]);
							
							unsigned char drop = 0;
							if(Dropss == dropss_valid)
								drop = REP_DROP_NORMAL;
							else 
								drop = REP_DROP_NON;
							Con2And_send(Con2And, 0x00, REP_B1_FAIL, REP_MOTOR_SHORT, 0x00, false);
							drop = 0x00;
							
							mask = 0;
							timeout = 0;
							task[task_Product_Dispense].Task_status = task_status_stop;
							task[task_Product_Dispense].Task_active = false;
							Task_Operation = false;
						}
						//Check motor open circuit
						else if(ADC_result < 100)
						{
							GPIO_ResetBits(Slot_P[slot], Slot[slot]);
							GPIO_ResetBits(Trace_P[trace], Trace[trace]);
							
							unsigned char drop = 0;
							if(Dropss == dropss_valid)
								drop = REP_DROP_NORMAL;
							else 
								drop = REP_DROP_NON;
							
							Con2And_send(Con2And, 0x00, REP_B1_FAIL, REP_MOTOR_OPEN, 0x00, false);
							drop = 0x00;
							
							mask = 0;
							timeout = 0;
							task[task_Product_Dispense].Task_status = task_status_stop;
							task[task_Product_Dispense].Task_active = false;
							Task_Operation = false;
						}
					}
					#endif

					/* Check task timeout */
					if(timeout > task[task_Product_Dispense].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Prodcut dispense task timeout!!!!\r\n");
						if(Dropss == dropss_nodrop)
							USARTx_TX_Str(USART2, "Error: Drop sensor don't detect item\r\n");
						#else
						
						GPIO_ResetBits(Trace_P[trace], Trace[trace]);
						GPIO_ResetBits(Slot_P[slot], Slot[slot]);
						
						unsigned char drop = 0;
						if(Dropss == dropss_valid)
							drop = REP_DROP_NORMAL;
						else 
							drop = REP_DROP_NON;
						
						Con2And_send(Con2And, 0x00, REP_B1_FAIL, REP_MOTOR_TIMEOUT, drop, true);
						drop = 0x00;
						#endif
						
						mask = 0;
						timeout = 0;
						task[task_Product_Dispense].Task_status = task_status_stop;
						task[task_Product_Dispense].Task_active = false;
						Task_Operation = false;
					}
				}
				
				/* if task not doing preveus, start task */
				else if(task[task_Product_Dispense].Task_status == task_status_stop)
				{
					task[task_Product_Dispense].Task_status = task_status_doing;
					Dropss = dropss_nodrop;
				}
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Diagnose */
			else if(task[task_Diagnose].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Diagnose].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_Diagnose = 0;
					if(check_Time(&cnt_Diagnose, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task diaglose\r\n");
						USARTx_TX_Str(USART2, "Diaglose task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Diagnose].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Diaglose task  task timeout\r\n");
						#endif
						
						timeout = 0;
						task[task_Diagnose].Task_status = task_status_stop;
						task[task_Diagnose].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Diagnose].Task_status == task_status_stop)
					task[task_Diagnose].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* SlotReset */
			else if(task[task_SlotReset].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_SlotReset].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_Slotreset = 0;
					if(check_Time(&cnt_Slotreset, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Slotreset\r\n");
						USARTx_TX_Str(USART2, "Slotreset task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_SlotReset].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Slotreset task timeout\r\n");
						#endif
						
						timeout = 0;
						task[task_SlotReset].Task_status = task_status_stop;
						task[task_SlotReset].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_SlotReset].Task_status == task_status_stop)
					task[task_SlotReset].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* ReadTemp */
			else if(task[task_ReadTemp].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_ReadTemp].Task_status == task_status_doing)
				{
					static unsigned long sum = 0;
					#ifdef DEMO
					static unsigned int cnt_Led = 0;
					if(check_Time(&cnt_Led, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Readtemp\r\n");
						USARTx_TX_Str(USART2, "Readtemp task doing\r\n");
					}
					#else
					static unsigned char cnt_temp_ADC = 0;
		
					cnt_temp_ADC++;
					
					if(cnt_temp_ADC > 50)
					{
						cnt_temp_ADC = 0;
						
//						char Arr[6];
//						sprintf(Arr,"%d",Temp_C);
//						USARTx_TX_Str(USART2, Arr);
//						USARTx_TX_Str(USART2, "\r\n");
						
						if((Temp_C < 1) || (Temp_C > 100))
							Con2And_send(Con2And, 0x00, REP_B1_FAIL, 0x00, 0x00, false);
						else
							Con2And_send(Con2And, 0x00, REP_B1_NORMAL, Temp_C, 0x00, false);
													
						timeout = 0;
						task[task_ReadTemp].Task_status = task_status_stop;
						task[task_ReadTemp].Task_active = false;
						Task_Operation = false;
					}
					#endif

					/* Check task timeout */
					if(timeout > task[task_ReadTemp].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Readtemp task timeout!!!\r\n");
						#endif
						
						Con2And_send(Con2And, 0x00, REP_B1_FAIL, REP_MOTOR_TIMEOUT, 0x00, false);
						
						timeout = 0;
						task[task_ReadTemp].Task_status = task_status_stop;
						task[task_ReadTemp].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_ReadTemp].Task_status == task_status_stop)
					task[task_ReadTemp].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup one slot is single */
			else if(task[task_Comb_One_Single].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Comb_One_Single].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_COS = 0;
					if(check_Time(&cnt_COS, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Setup one slot is single\r\n");
						USARTx_TX_Str(USART2, "Setup one slot is single task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Comb_One_Single].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Setup one slot is single task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Comb_One_Single].Task_status = task_status_stop;
						task[task_Comb_One_Single].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Comb_One_Single].Task_status == task_status_stop)
					task[task_Comb_One_Single].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup one slot is double */
			else if(task[task_Comb_One_Double].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Comb_One_Double].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_COD = 0;
					if(check_Time(&cnt_COD, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Setup one slot is Double\r\n");
						USARTx_TX_Str(USART2, "Setup one slot is Double task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Comb_One_Double].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Setup one slot is single task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Comb_One_Double].Task_status = task_status_stop;
						task[task_Comb_One_Double].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Comb_One_Double].Task_status == task_status_stop)
					task[task_Comb_One_Double].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup all slot is single */
			else if(task[task_Comb_all_Single].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Comb_all_Single].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_CAS  = 0;
					if(check_Time(&cnt_CAS, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Setup all slot is single\r\n");
						USARTx_TX_Str(USART2, "Setup all slot is single task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Comb_all_Single].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Setup all slot is single task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Comb_all_Single].Task_status = task_status_stop;
						task[task_Comb_all_Single].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Comb_all_Single].Task_status == task_status_stop)
					task[task_Comb_all_Single].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup temperature */
			else if(task[task_Settemp].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Settemp].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_Settemp = 0;
					if(check_Time(&cnt_Settemp, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Set temperature\r\n");
						USARTx_TX_Str(USART2, "Set temperature task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Settemp].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Set temperature task timeout!!!\r\n");
						active = 1;
						temp_require = And2Con[cmd_Code2];
						dem = 0;
						#endif
						
						Con2And_send(Con2And, 0x00, REP_MOS_NORMAL, 0x00, 0x00, false);
						
						temp_require = And2Con[cmd_Code2];
						active  = 1;
						dem = 0;
						timeout = 0;
						task[task_Settemp].Task_status = task_status_stop;
						task[task_Settemp].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Settemp].Task_status == task_status_stop)
					task[task_Settemp].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup defrost time */
			else if(task[task_Settime_Defrost].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Settime_Defrost].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_SD = 0;
					if(check_Time(&cnt_SD, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Set time defrost\r\n");
						USARTx_TX_Str(USART2, "Set time defrost task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Settime_Defrost].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Set time defrost task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Settime_Defrost].Task_status = task_status_stop;
						task[task_Settime_Defrost].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Settime_Defrost].Task_status == task_status_stop)
					task[task_Settime_Defrost].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup working time */
			else if(task[task_Settime_Working].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Settime_Working].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_SW = 0;
					if(check_Time(&cnt_SW, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Setup working time\r\n");
						USARTx_TX_Str(USART2, "Setup working time task doing\r\n");
					}
					#else
					
					#endif

					/* Check task timeout */
					if(timeout > task[task_Settime_Working].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Setup working time task timeout!!!\r\n");
						time_active_require = And2Con[cmd_Code2];
						#else
						time_active_require = And2Con[cmd_Code2];
						Con2And_send(Con2And, 0x00, REP_MOS_NORMAL, 0x00, 0x00, false);
						#endif
						
						timeout = 0;
						task[task_Settime_Working].Task_status = task_status_stop;
						task[task_Settime_Working].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Settime_Working].Task_status == task_status_stop)
					task[task_Settime_Working].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup down time */
			else if(task[task_Settime_Down].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Settime_Down].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_ST = 0;
					if(check_Time(&cnt_ST, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task Setup down time\r\n");
						USARTx_TX_Str(USART2, "Setup down time task doing\r\n");
					}
					#else
					
					#endif

					/* Check task timeout */
					if(timeout > task[task_Settime_Down].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Setup down time timeout!!!\r\n");
						time_rest_require = And2Con[cmd_Code2];
						#else
						time_rest_require = And2Con[cmd_Code2];
						Con2And_send(Con2And, 0x00, REP_MOS_NORMAL, 0x00, 0x00, false);
						#endif
						
						timeout = 0;
						task[task_Settime_Down].Task_status = task_status_stop;
						task[task_Settime_Down].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Settime_Down].Task_status == task_status_stop)
					task[task_Settime_Down].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup light */
			else if(task[task_Light].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Light].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_Light = 0;
					if(check_Time(&cnt_Light, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task on off light\r\n");
						USARTx_TX_Str(USART2, "On off light task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Light].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "On off light task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Light].Task_status = task_status_stop;
						task[task_Light].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Light].Task_status == task_status_stop)
					task[task_Light].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup one slot is belt mode */
			else if(task[task_Mode_OneBelt].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Mode_OneBelt].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_MOB = 0;
					if(check_Time(&cnt_MOB, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task on off light\r\n");
						USARTx_TX_Str(USART2, "On off light task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Mode_OneBelt].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "On off light task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Mode_OneBelt].Task_status = task_status_stop;
						task[task_Mode_OneBelt].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Mode_OneBelt].Task_status == task_status_stop)
					task[task_Mode_OneBelt].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup one slot is spring mode */
			else if(task[task_Mode_OneSpring].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Mode_OneSpring].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned cnt_MOS = 0;
					if(check_Time(&cnt_MOS, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task set one slot spring\r\n");
						USARTx_TX_Str(USART2, "Set one slot spring task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Mode_OneSpring].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Set one slot spring task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Mode_OneSpring].Task_status = task_status_stop;
						task[task_Mode_OneSpring].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Mode_OneSpring].Task_status == task_status_stop)
					task[task_Mode_OneSpring].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup all slot is belt mode */
			else if(task[task_Mode_AllBelt].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Mode_AllBelt].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_MAB = 0;
					if(check_Time(&cnt_MAB, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task set all slot belt\r\n");
						USARTx_TX_Str(USART2, "Set all slot belt task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Mode_AllBelt].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Set all slot belt task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Mode_AllBelt].Task_status = task_status_stop;
						task[task_Mode_AllBelt].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Mode_AllBelt].Task_status == task_status_stop)
					task[task_Mode_AllBelt].Task_status = task_status_doing;
			}
			
			/*--------------------------------------------------------------------------------*/
			/* Setup all slot is spring mode */
			else if(task[task_Mode_AllSpring].Task_active)
			{
				Task_Operation = true;
				
				/* task doing */
				if(task[task_Mode_AllSpring].Task_status == task_status_doing)
				{
					
					#ifdef DEMO
					static unsigned int cnt_MAS = 0;
					if(check_Time(&cnt_MAS, cnt_Global, 1000))
					{
						USARTx_TX_Str(USART2, "Task set all slot spring\r\n");
						USARTx_TX_Str(USART2, "Set all slot spring task doing\r\n");
					}
					#else
					#endif

					/* Check task timeout */
					if(timeout > task[task_Mode_AllSpring].Task_timeout)
					{
						#ifdef DEMO
						USARTx_TX_Str(USART2, "Set all slot spring task timeout!!!\r\n");
						#endif
						
						timeout = 0;
						task[task_Mode_AllSpring].Task_status = task_status_stop;
						task[task_Mode_AllSpring].Task_active = false;
						Task_Operation = false;
					}
				}
				/* if task not doing preveus, start task */
				else if(task[task_Mode_AllSpring].Task_status == task_status_stop)
					task[task_Mode_AllSpring].Task_status = task_status_doing;
			}
		}
		
		/*--------------------------------------------------------------------------------*/
		// Bat quat 
		GPIO_SetBits(GPIOC, FAN);
		if(check_light == 5*1000)
		{
			GPIO_SetBits(GPIOB ,LIGHT);
		}
		if(check_light == 10*1000)
		{
			GPIO_ResetBits(GPIOB, LIGHT);
			check_light = 0;
		}
		/*--------------------------------------------------------------------------------*/
		// PT100 - Toan
//		if(dem1 == 1*1000)
//		{
//			for(int i = 0; i < 200; i++)
//			{
//				ADC_StartConversion(ADC1);
//				float value_adc1 = ADC_GetConversionValue(ADC1);
//				while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
//				sum += value_adc1;
//			}
//			sum1 = (float) sum/200;
//			Voltage_1 = (float)sum1 * 3.3;
//			Voltage_2 = (float)Voltage_1/4096; 
//			/********/
//			Vol_R0 = (float)5 - Voltage_2;  
//			Divide_R0 = (float)Vol_R0 / 10000;
//			Rx = (float)Voltage_2 / Divide_R0;
//			T1 = A_NTC + B_NTC*log(Rx)+ C_NTC*log(Rx)*log(Rx)*log(Rx);
//			T2 = 1 / T1;
//			Temp_C = T2 - 273.15;
//			/********/
//			//sprintf(Display,"Temp_NTC:%d\n",Temp_C);
//			USARTx_TX_Str(USART2,Display);
//			sum = 0;
//			dem1 = 0;
//		}
//		
//		// Add by Toan - Control freezer
//		if(time_active_require != time_active_avai)
//		{    
//			time_active_avai = time_active_require;
//			time_temp_active = time_active_require;
//			//USARTx_TX_Str(USART2,"Da chinh sua thoi gian bat\n");
//		}
//		
//		if(time_rest_require != time_rest_avai)
//		{
//			time_rest_avai = time_rest_require;
//			time_temp_rest = time_rest_require;
//			//USARTx_TX_Str(USART2,"Da chinh sua thoi gian nghi\n");
//		}
//		
//		if(active == 0)
//		{
//			if(dem == 1)
//			{ // tat may lanh
//				GPIO_ResetBits(GPIOC,COOL);
//				//USARTx_TX_Str(USART2, "Chu ky - Tat may lanh\n");
//			}
//			if(dem == time_rest_avai * 60 * 1000)
//			{ // bat may lanh
//				GPIO_SetBits(GPIOC,COOL); 
//				//USARTx_TX_Str(USART2, "Chu ky - Bat may lanh\n");
//			}
//			if(dem >= (time_rest_avai + time_active_avai)* 60 * 1000)
//			{
//				dem = 0;
//			}
//		}
//		
//		if(active == 1)
//		{
//			if(temp_require < Temp_C)
//			{
//				if(dem == 1*1000)
//				{
//					GPIO_SetBits(GPIOC,COOL);   // bat may lanh
//					//USARTx_TX_Str(USART2, "Nho hon - Bat may lanh\n");
//				}
//				if(dem == time_temp_active * 60 * 1000)
//				{
//					GPIO_ResetBits(GPIOC,COOL);
//					//USARTx_TX_Str(USART2, "Nho hon - Tat may lanh\n");
//				}
//				if(dem >= ((time_temp_active + time_temp_rest)* 60 * 1000))
//				{
//					dem = 0;
//				}
//			}
//			if(temp_require > Temp_C)
//			{
//				GPIO_ResetBits(GPIOC,COOL); // tat may lanh
////				if ( dem == 2 * 1000){
////					//USARTx_TX_Str(USART2, "Lon hon -  Tat may lanh\n");
////					dem = 0;
////				}
//			}
//			if((Temp_C - 1 <= temp_require) && (temp_require <= Temp_C + 1))
//			{
//				active = 0;
//					//USARTx_TX_Str(USART2, "Tro ve ban dau");
//			}
//		}
		// Add by Toan - Control freezer
		
			
		#ifdef TEST_MOTOR
		//GPIOx_bit_toggle(GPIOF, GPIO_Pin_1);
		
		if(check_Time(&cnt_Motor, 5000))
			GPIO_ResetBits(GPIOA, TRACE1);
		//Delay_ms(5000);

		if(check_Time(&cnt_Motor, 5000))
			GPIO_SetBits(GPIOA, TRACE1);
		//Delay_ms(5000);
		#endif
  }
}


