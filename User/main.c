
/*******************************************
	*	@file ��  main.c
	* @author��  �޳�
	* @data�� 2023.01.15
	* @version��  v6.0
*******************************************/

/*
*************************************************************************
*                            ͷ�ļ�
*************************************************************************
*/

/* kernel ͷ�ļ� */
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

/*c��ͷ�ļ�*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>



/* ��������ͷ�ļ� */
#include "bsp_usart.h"
#include "bsp_timer.h"
#include "bsp_key.h"
#include "bsp_exti.h"
#include "bsp_interface.h"
#include "bsp_button.h"
#include "ili9486.h"
#include "bsp_spi.h"
#include "bsp_delay.h"
#include "GT30L32S4W.h"
#include "bsp_beep.h"
#include "as32-100-c.h"
#include "bsp_iwdg.h"
#include "DP_Print_inc.h"

extern vu16 a;


/*
*************************************************************************
*                         ����API    
*************************************************************************
*/

#if TEST_vApp_Hook
/*ջ������Ӻ���*/
void vApplicationStackOverflowHook(TaskHandle_t xTask,signed char *pcTaskName)
{
	printf("ջ�������������\r\n%s\r\n",pcTaskName);
}

#endif
/*
*************************************************************************
*                         ����    
*************************************************************************
*/


#define   TEST_PRINTF_CPU           0
#define   TEST_vApp_Hook            0
//������Ӵӻ���Ŀ
#define   timer_slave_max           6

//�������ȼ�
#define   KEY_Priority              8
#define   EXIT_Priority             9
#define   USART_Priority            10
#define   Interface_Priority        11
#define   Beep_Priority             7
#define   IWDG_Priority             13
#define   Print_Priority            6
/*
*************************************************************************
*                            �ں˶����� 
*************************************************************************
*/

EventGroupHandle_t EventGroupHandler = NULL;	//�¼���־����

#define EVENTBIT_0	(1<<0)                          //���⴫��������¼�
#define EVENTBIT_1	(1<<1)                          //������ӡ�¼�

SemaphoreHandle_t  BinarySem1_Handle = NULL;
SemaphoreHandle_t  BinarySem2_Handle = NULL;
SemaphoreHandle_t  BinarySem3_Handle = NULL;

/*
*************************************************************************
*                             ��������ʼ��
*************************************************************************
*/

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t KEY_Task_Handle = NULL;
static TaskHandle_t EXTIX_DealTask_Handler = NULL;
static TaskHandle_t USART2_DealTask_Handler = NULL;
static TaskHandle_t Face_SwitchTask_Handler = NULL;
static TaskHandle_t BEEP_CheckoutTask_Handler = NULL;
static TaskHandle_t IWDG_Task_Handle = NULL;
static TaskHandle_t CPU_Task_Handle = NULL;
static TaskHandle_t Print_Task_Handle = NULL;



/*
*************************************************************************
*                             �����ջ����
*************************************************************************
*/


/* �������������ջ */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* ��ʱ�����������ջ */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* ���崴������ջ */
static StackType_t AppTaskCreate_Stack[128];
/* ���尴������ջ */
static StackType_t KEY_Task_Stack[64];
/*
*************************************************************************
*                             ���ƿ�����
*************************************************************************
*/

/* ����������ƿ� */
static StaticTask_t Idle_Task_TCB;
/* ��ʱ��������ƿ� */
static StaticTask_t Timer_Task_TCB;
/* ���崴��������ƿ� */
static StaticTask_t AppTaskCreate_TCB;
/* ����������ƿ� */
static StaticTask_t KEY_Task_TCB;


/*
*************************************************************************
*                             ��������
*************************************************************************
*/


static void AppTaskCreate(void);                                                /* ���ڴ������� */
static void BSP_Init(void);
static void KeyScan_Task(void* parameter);
static void EXTIX_DealTask(void* parameter);
static void USART2_DealTask(void* parameter);
static void TestData_Save(uint16_t Timevalue);
static void Face_SwitchTask(void* parameter);
static void BEEP_CheckoutTask(void* parameter);
static void IWDG_Task(void* pvParameters);
static void CPU_Task(void* pvParameters);                                        /* CPU_Task����ʵ�� */
inline static void Usart_Handle(uint8_t rd);
static void EM5820_Print_Task(void* parameter);

/*
*************************************************************************
*                             ȫ�ֱ���
*************************************************************************
*/

uint8_t Lap_stflag = 0;                     //��Ȧ��ʱ��ʼ������־
uint8_t Connection_count = 0;               //���Ӵӻ�ͨѶ����
uint8_t bxflag = 0;
uint16_t Showhigh = 150;                    //�ӻ������ʾλ��
uint16_t DataBuffer1[1024] = {0};           //���ٲ������ݻ�����
uint16_t DataBuffer2[1024] = {0};           //��Ȧ�������ݻ�����
uint16_t Data1_Count = 0;                  //���ٲ������ݼ�����
uint16_t Data2_Count = 0;                  //��Ȧ�������ݼ�����
uint8_t timer_slave[2][timer_slave_max] = {{0},{0,1,2,3,4,5}};
uint16_t timer_slaver_num[timer_slave_max] = {0};
uint8_t  slaver_flag = 0;
uint8_t  slaver_count = 0;
uint8_t  EM5820_flag = 1;
uint8_t  first_Print_flag = 0;
uint32_t Cur_Data = 0;
uint32_t Exti_Data[3] = {0};
//uint8_t  buchang_timesz[4] = {0};
uint8_t slaver_reset = 20;


extern vu32 buchang_time;
extern vu32 Time3value;
extern vu8 Timer1flag;
extern vu8 EXTIX_Flag;


/****************************************************************************/

/* ��ȡ����������ڴ� */

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
	                                 StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* ��ȡ��ʱ��������ڴ� */

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
	                                 StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/

int main(void)
{	
	BSP_Init();
	
  printf("�����ӵ����鶨ʱ����������!\r\n");	
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t	)AppTaskCreate,		  //������
															(const char* 	)"AppTaskCreate",		                //��������
															(uint32_t 		)128,	                              //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)3, 	                              //�������ȼ�
															(StackType_t*   )AppTaskCreate_Stack,	            //�����ջ
															(StaticTask_t*  )&AppTaskCreate_TCB);	            //������ƿ� 
  /* ����������� */          
	if(NULL != AppTaskCreate_Handle)                                              /* �����ɹ� */
    vTaskStartScheduler();    
	else
		printf("����ʧ��");
  
  while(1);   /* ��������ִ�е����� */    
}


/**********************************************************************
  * @ Taskname �� BSP_Task
  * @ brief��     ������ʼ��
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/



void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Delay_Init(F103SYSCLK);
	Key_Init();
	Button_Init();
  AS32_Init();BEEP_Init();
	Exti_Init();
	TIMER1_Init(499, 71);
//	TIMER4_Init(999, 72-1);																											//(7199+1)��Ƶ��������4999+1����     1ms
	TIMER3_Init(9999, 72-1);                                                       //(71+1)��Ƶ��������9999+1����       10ms
//  TIMER2_Init(9999, 7199);                                                      //(7199+1)��Ƶ��������4999+1����     1s

	SPI_ILI9486_Init();
	ILI9486_tftlcd_init();
	delay_xms(500);
	GT30L32S4W_Init();
	delay_xms(100);
	u32 addr = ASCII_Addr_Tans('A', ASCII_8X16);                                  //���ֿ�оƬ����һ�ζ�������ȥ����һ�ε����ݶ�ʧ
	u8 zdata[16];
	GT30L32S4W_Read(addr , zdata, ASCII_8X16);
  WWDG_Config(6,1000);                                                          //6.4s
	uart1_init(115200);	
	uart2_init(115200);
	uart3_init(115200);
	ILI9486_showstring_Ch(50,200,(u8*)"�����ӵ�����",GB2312_32X32);
	SelChineseChar();
	InitializePrint(); 
	delay_xms(500);
}

/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;     /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */	
  taskENTER_CRITICAL();           //�����ٽ���
	
	/* ����EventGroup */  
	EventGroupHandler = xEventGroupCreate();
	if(NULL != EventGroupHandler)
	printf("EventGroupHandler �¼������ɹ���\r\n");
	
	BinarySem1_Handle = xSemaphoreCreateBinary();
	BinarySem2_Handle = xSemaphoreCreateBinary();
	BinarySem3_Handle = xSemaphoreCreateBinary();
	if(NULL != BinarySem1_Handle){
		printf("BinarySem1_Handle ��ֵ�ź��������ɹ���\r\n");
	}
	if(NULL != BinarySem2_Handle){
		printf("BinarySem2_Handle ��ֵ�ź��������ɹ���\r\n");
	}
	if(NULL != BinarySem3_Handle){
		printf("BinarySem3_Handle ��ֵ�ź��������ɹ���\r\n");
	}	
	
#if TEST_PRINTF_CPU
  /* ����CPU_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )CPU_Task, /* ������ں��� */
                        (const char*    )"CPU_Task",/* �������� */
                        (uint16_t       )256,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )15,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&CPU_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����CPU_Task����ɹ�!\r\n");	
	else
		printf("CPU_Task���񴴽�ʧ��!\r\n");	
#endif	
	
  /* ����KeyScan_Task���� */
	KEY_Task_Handle = xTaskCreateStatic((TaskFunction_t	)KeyScan_Task,		            //������
															(const char* 	)"KeyScan_Task",		                    //��������
															(uint32_t 		)64,					                      //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)KEY_Priority, 				                        //�������ȼ�
															(StackType_t*   )KEY_Task_Stack,	                //�����ջ
															(StaticTask_t*  )&KEY_Task_TCB);	                //������ƿ�													
															
															
	if(NULL != KEY_Task_Handle)                                                   /* �����ɹ� */
		printf("KeyScan_Task���񴴽��ɹ�!\n");
	else
		printf("KeyScan_Task���񴴽�ʧ��!\n");
	
	  /* ����EXTIX_DealTask���� */
	xReturn = xTaskCreate((TaskFunction_t	)EXTIX_DealTask,		               //������
															(const char* 	)"EXTIX_DealTask",		         //��������
															(uint16_t 		)512,					                           //�����ջ��С
															(void* 		  	)NULL,  				                         //���ݸ��������Ĳ���
															(UBaseType_t 	)EXIT_Priority, 				                             //�������ȼ�
															(TaskHandle_t*  )&EXTIX_DealTask_Handler);	           //������ƿ�ָ��   
	
	if(pdPASS == xReturn)/* �����ɹ� */
		printf("EXTIX_DealTask���񴴽��ɹ�!\r\n");
	else
		printf("EXTIX_DealTask���񴴽�ʧ��!\r\n");	
	
	
	  /* ����USART2_DealTask���� */
	xReturn = xTaskCreate((TaskFunction_t	)USART2_DealTask,		               //������
															(const char* 	)"USART2_DealTask",		         //��������
															(uint16_t 		)512,					                           //�����ջ��С
															(void* 		  	)NULL,  				                         //���ݸ��������Ĳ���
															(UBaseType_t 	)USART_Priority, 				                  //�������ȼ�
															(TaskHandle_t*  )&USART2_DealTask_Handler);	           //������ƿ�ָ��   
	
	if(pdPASS == xReturn)/* �����ɹ� */
		printf("USART2_DealTask���񴴽��ɹ�!\r\n");
	else
		printf("USART2_DealTask���񴴽�ʧ��!\r\n");
	
	
	  /* ����Face_SwitchTask���� */
	xReturn = xTaskCreate((TaskFunction_t	)Face_SwitchTask,		               //������
															(const char* 	)"Face_Task",		         //��������
															(uint16_t 		)512,					                           //�����ջ��С
															(void* 		  	)NULL,  				                         //���ݸ��������Ĳ���
															(UBaseType_t 	)Interface_Priority, 				                  //�������ȼ�
															(TaskHandle_t*  )&Face_SwitchTask_Handler);	           //������ƿ�ָ��   
	
	if(pdPASS == xReturn)/* �����ɹ� */
		printf("Face_SwitchTask���񴴽��ɹ�!\r\n");
	else
		printf("Face_SwitchTask���񴴽�ʧ��!\r\n");

		  /* ����BEEP_CheckoutTask���� */
	xReturn = xTaskCreate((TaskFunction_t	)BEEP_CheckoutTask,		               //������
															(const char* 	)"BEEP_Task",		         //��������
															(uint16_t 		)512,					                           //�����ջ��С
															(void* 		  	)NULL,  				                         //���ݸ��������Ĳ���
															(UBaseType_t 	)Beep_Priority, 				                  //�������ȼ�
															(TaskHandle_t*  )&BEEP_CheckoutTask_Handler);	           //������ƿ�ָ��   
	
	if(pdPASS == xReturn)/* �����ɹ� */
		printf("BEEP_CheckoutTask���񴴽��ɹ�!\r\n");
	else
		printf("BEEP_CheckoutTask���񴴽�ʧ��!\r\n");
	
	/* ����IWDG_Task���� */
	xReturn = xTaskCreate((TaskFunction_t	)IWDG_Task,		         							//������
												(const char* 	)"IWDG_Task",		         								//��������
												(uint16_t 		)128,					                     		//�����ջ��С
												(void* 		  	)NULL,				                       //���ݸ��������Ĳ���
												(UBaseType_t 	)IWDG_Priority, 				                         //�������ȼ�
												(TaskHandle_t*  )&IWDG_Task_Handle);	   						//������ƿ�ָ��   
	if(pdPASS == xReturn)   /* �����ɹ� */
		printf("IWDG_Task���񴴽��ɹ�!\r\n");
	else
		printf("IWDG_Task���񴴽�ʧ��!\r\n");		
	
	/* ����EM5820_Print_Task���� */
	xReturn = xTaskCreate((TaskFunction_t	)EM5820_Print_Task,		         							//������
												(const char* 	)"EM5820_Print_Task",		         								//��������
												(uint16_t 		)128,					                     		//�����ջ��С
												(void* 		  	)NULL,				                       //���ݸ��������Ĳ���
												(UBaseType_t 	)Print_Priority, 				             //�������ȼ�
												(TaskHandle_t*  )&Print_Task_Handle);	   						//������ƿ�ָ��   
	if(pdPASS == xReturn)   /* �����ɹ� */
		printf("EM5820_Print_Task���񴴽��ɹ�!\r\n");
	else
		printf("EM5820_Print_Task���񴴽�ʧ��!\r\n");	

  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();                //�˳��ٽ���
}

/**********************************************************************
  * @ Taskname �� KeyScan_Task
  * @ brief��     ����ɨ��
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/

static void KeyScan_Task(void* parameter)
{
	uint8_t KeyValue;
	for(;;)
	{
		KeyValue = 0xff;
		KeyValue = Key_Scan(0);
	  if(KeyValue != 0xff){
			Button_Dealing(KeyValue);
		}
		vTaskDelay(50);
	}
}


/**********************************************************************
  * @ Taskname �� EXTIX_DealTask
  * @ brief��     �����ⲿ�ж�
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/
static void EXTIX_DealTask(void* parameter)
{
	uint8_t cnt,i = 0;
	static uint8_t n = 0;
	char str[20] = { 0 };
	for(;;)
	{
		xSemaphoreTake(BinarySem1_Handle,portMAX_DELAY);
		EXTIX_Flag = 0;
		while(i < 10)                                                                //����˲�����Ҫ�ķ�10ms
		{
			if(EXTI_READ() == 0) cnt++;
			vTaskDelay(1);
			i++;
		}
		if(cnt > 5)                                                                 //�����Ч�������������趨ֵ�������ж��ź�
		{
			//��Ȧ��ʱģʽ
			if(1 == InterfaceFlag)
			{
				if(0 == Lap_stflag){
					n = 0;
					Timer3_Open();
					Lap_stflag = 1;
					Time3value = 0;
					sprintf(str, "%s", "��ʱ��");
					ILI9486_clear_screen(130, 137, 150, 30);
					ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
					ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
					if(first_Print_flag == 0){
						xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);                //��λ������ӡ�¼�						
					}
				}//��ӻ�
				else
				{
					if(1 == n){Exti_Data[1] = Exti_Data[0];}
					Exti_Data[0] = Time3value;
					TIM_SetCounter(TIM3,0);
					Time3value = 0;
					sprintf(str, "%s", "��ʱ����");
					ILI9486_clear_screen(130, 137, 150, 30);
					ILI9486_clear_screen(200, 200, 90, 40);//�����ǰ��������
					ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
					ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);           
					Show_Data(Exti_Data[0] , 200);
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
					if(Data1_Count <= 1000){
						TestData_Save(Exti_Data[0]);
					}
					vTaskDelay(1000);                                //������ʱ������Ϊ��һ�ε�Ȧ��ʱ�������˻�����ʱ��ʾ���������ͺ���ʾ����ʱ��������  
					if(1 == InterfaceFlag)
					{
						sprintf(str, "%s", "��ʱ��");
						ILI9486_clear_screen(130, 137, 150, 30);
						ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
						ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
						if(n == 1){
							ILI9486_clear_screen(200, 240, 90, 40);
							Show_Data(Exti_Data[1] , 240);
						}
						if(0 == n){n++;}
					}
				}
			}
			//��ӻ�ģʽ
			else if(3 == InterfaceFlag)
			{
				uint8_t j = 0;
				Exti_Close();
				Timer3_Open();
				for(j = 0;j < slaver_count;j++){
					timer_slaver_num[j] = 0;
				}
				slaver_count = 0;
				USART_Cmd(USART2, ENABLE);		//������
				sprintf(str, "%s", "��ʱ��");
				ILI9486_clear_screen(130, 137, 150, 30);
				ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
				ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
				if(first_Print_flag == 0){
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);                //��λ������ӡ�¼�						
				}				
			}
		}
		else
		{
			sprintf(str, "%s", "��ʱ����");
			ILI9486_clear_screen(130, 137, 150, 30);
			ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
			ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
			vTaskDelay(1000);                                                    //��Ҫ��
		}
	}
}

/**********************************************************************
  * @ Taskname �� USART2_DealTask
  * @ brief��     ������ͨѶ
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/
static void USART2_DealTask(void* parameter)
{
	uint8_t rdata = 0;
	for(;;)
	{
		xSemaphoreTake(BinarySem2_Handle,portMAX_DELAY);
//		printf("Sem");
		rdata = USART_ReceiveData(USART2);
		if(3 == InterfaceFlag)                              //��ӻ�����
		{
			Usart_Handle(rdata);
		}
		else if((2 == InterfaceFlag) && (Connection_count <= timer_slave_max))
		{
			if(1 == Timer1flag)
			{
				uint8_t s1 = 0;
				printf("�ӻ����ӳ�ʱ\r\n");
				slaver_flag = rdata&0x0F;
				Timer1_Off();
				ILI9486_showstring_Ch(20, Showhigh, (u8*)"�ӻ�����ʧ��", GB2312_24X24);
				s1 = slaver_flag + 0x30;
				ILI9486_showstring_En(180, Showhigh, (u8*)&s1, ASCII_12X24);
				Showhigh += 40;
				Timer1flag = 0;
				bxflag = 0;
			}
			else
			{
				if(0 == bxflag)
				{
					if((rdata&0xF0) == 0xa0)
					{
//						printf("0x%x\r\n",rdata);
						slaver_flag = rdata&0x0F;
//						printf("%d\r\n",slaver_flag);
						for(uint8_t i = 0; i<10 ;i++)
						{
							USART_SendData(USART2 ,(0xb0+slaver_flag));
							vTaskDelay(50);
						}
						bxflag = 1;
						Timer1_Open();//�򿪶�ʱ����ʱ
					}
				}
				else if(1 == bxflag)
				{
//					printf("0x%x\r\n",rdata);
//					printf("0x%x\r\n",0xc0+slaver_flag);
					if(rdata == (0xc0+slaver_flag))
					{
						uint8_t j;
						bool z = 0;
//						printf("0x%x\r\n",rdata);
						uint8_t s2 = 0;
						Timer1_Off();
						if(Connection_count > 0)
						{
							for(j = 0; j<Connection_count; j++)
							{
								if(slaver_flag == timer_slave[0][j])
								{
									z = 1;
									break;
								}
							}
						}
						if(0 == z)
						{
							uint8_t n = Connection_count++;
							timer_slave[0][n--] = slaver_flag;
//							printf("%d\r\n",Connection_count);
							ILI9486_showstring_Ch(20, Showhigh, (u8*)"�����Ӵӻ���", GB2312_24X24);
							s2 = slaver_flag + 0x30;
							ILI9486_showstring_En(160, Showhigh, (u8*)&s2, ASCII_12X24); 
							Showhigh += 40;
							//ʱ��ͬ������δ������
						}
						bxflag = 0;
					}
				}
			}
		  USART_Cmd(USART2, ENABLE);
		}
		else if(Connection_count > timer_slave_max){
				ILI9486_showstring_Ch(20, Showhigh, (u8*)"�ӻ�������������", GB2312_24X24);
		}
	}
}


///**********************************************************************
//  * @ Taskname �� Buchang_time_Handle
//  * @ brief��     ͬ���ӻ�ʱ��
//  * @ param    �� �� 
//  * @ retval   �� ��
//  ********************************************************************/


//void Buchang_time_Handle(void)
//{
//	uint8_t i = 0;
//	for(i = 0;i<4;i++)
//	{
//		USART_SendData(USART2 ,(0xb0+slaver_flag));
//		buchang_timesz[i] = buchang_time>>(8*i);
//	}
//	for(i = 0;i<4;i++)
//	{
//		USART_SendData(USART2 ,buchang_timesz[i]);
//	}
//	//�����ַ���
//	//�����ַ�����buchang_time���д�С�Ƚϣ��ó��������˴����������ʱ��
//	//���䲹��ʱ�䷢�͸��ӻ����ôӻ�����ʱ��
//}
/**********************************************************************
  * @ Taskname �� Face_SwitchTask
  * @ brief��     ������ʾ����
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/
static void Face_SwitchTask(void* parameter)
{
	for(;;)
	{
		if(0 == InterfaceFlag)
		{
			if(EXTIX_Flag == 1){
				EXTIX_Flag = 0;
				xSemaphoreTake(BinarySem1_Handle,portMAX_DELAY);
			}
			USART_Cmd(USART2, DISABLE);
			MainMenu();//������
			xEventGroupClearBits(EventGroupHandler,EVENTBIT_0);
			BEEP(0);
			Showhigh = 150;
			first_Print_flag = 0;
		}
		else if(1 == InterfaceFlag)
		{
			printf("��Ȧ��ʱ����\n\r");
			Lap_Test_Menu();//��Ȧ��ʱ����
			xEventGroupSetBits(EventGroupHandler,EVENTBIT_0);
			Connection_Count_Show();
		}
		else if(2 == InterfaceFlag)
		{
			printf("�ӻ�����������\n\r");
			USART_Cmd(USART2, ENABLE);//�򿪴���
			for(uint8_t t = 0;t<10;t++){
				USART_SendData(USART2 ,slaver_reset);
				vTaskDelay(5);
			}
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			Slaver_Check_Interface();//�ӻ�����������
			xEventGroupClearBits(EventGroupHandler,EVENTBIT_0);
		}
		else if(3 == InterfaceFlag)
		{
			printf("��ӻ����ٲ��Խ���\n\r");
			USART_Cmd(USART2, DISABLE);//�رմ���
			Multi_slave_function_interface();//��ӻ����ٲ��Խ���
			xEventGroupSetBits(EventGroupHandler,EVENTBIT_0);
			Connection_Count_Show();
		}
		else if(4 == InterfaceFlag)
		{
			Exti_Close();
			Show_LapRecords(DataBuffer1, Data1_Count);
			xEventGroupClearBits(EventGroupHandler,EVENTBIT_0);
			BEEP(0);
		}
		else if(5 == InterfaceFlag)
		{
			Exti_Close();
			Show_AcceleratedRecords(DataBuffer2, Data2_Count);
			xEventGroupClearBits(EventGroupHandler,EVENTBIT_0);
			BEEP(0);
		}
		xSemaphoreTake(BinarySem3_Handle,portMAX_DELAY);
	}
}

/**********************************************************************
  * @ APIname �� TestData_Save
  * @ brief��     ��������
  * @ param    �� Timevalue 
  * @ retval   �� ��
  ********************************************************************/
static void TestData_Save(uint16_t Timevalue)
{
	if(1 == InterfaceFlag){
		DataBuffer1[Data1_Count++] = Timevalue;
	}
	else if(3 == InterfaceFlag){
		DataBuffer2[Data2_Count++] = Timevalue;
	}
}

/**********************************************************************
  * @ APIname �� Usart_Handle
  * @ brief��     ����ͨѶ����
  * @ param    �� rd
  * @ retval   �� ��
  ********************************************************************/


inline static void Usart_Handle(uint8_t rd)
{
	uint8_t j = 0,n = 5;
	bool m = 0;
	uint8_t slaver_display;
	if((rd&0x0f) >0&&(rd&0x0f)<=timer_slave_max)
	{
//		printf("���ܴӻ�����\r\n");
		for(j = 0; j<slaver_count; j++){
			if((rd&0x0f) == timer_slaver_num[j]){
//				printf("�ӻ��ظ�����\r\n");
				m = 1;
				break;
			}
		}
		if(m == 0)
		{
			if((Time3value-(rd&0xf0)*5) <= 60000){//�������
				Cur_Data = Time3value-((rd&0xf0)>>4)*5;
			}
			else{
				printf("Time3value = %d,rd = %d\r\n",Time3value,((rd&0xf0)>>4)*5);
				Cur_Data = 0;
			}	
			for(uint8_t i = 0;i < Connection_count;i++){
//				printf("slaver_namenum = %d\r\n",rd&0x0f);
				if((rd&0x0f) == timer_slave[0][i]){
					slaver_display = timer_slave[1][i];
				}
			}
			ILI9486_clear_screen(200, 190+40*slaver_display, 90, 30);
			Show_Data(Cur_Data , 190+40*slaver_display);									//������ʾ
			xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
			if(Data2_Count <= 1000){
				TestData_Save(Cur_Data);
			}
			timer_slaver_num[slaver_count] = (rd&0x0f);
//			printf("%d\r\n",timer_slaver_num[slaver_count]);
			while(n--){
				USART_SendData(USART2,(rd&0x0f)+0xf0);               /*���ӻ�����ֹͣ�ź�*/		
				vTaskDelay(1);
			}
			slaver_count++;
		}
//		printf("slaver_count = %d\r\n",slaver_count);
		USART_Cmd(USART2, ENABLE);
	}
	else
	{
		Cur_Data = 1110;                                    //1.11s��ʾ���ݴ���
		Show_Data(Cur_Data , 190+40*slaver_count);                 //������ʾ
		slaver_count++;
	}
	if(slaver_count >= Connection_count)
	{
		USART_Cmd(USART2, DISABLE);
//		printf("Connection_count = %d,slaver_count = %d\r\n",Connection_count,slaver_count);
		Exti_Open();
		Timer3_Off();
		Time3value = 0;
		ILI9486_clear_screen(130, 137, 150, 30);
		ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
		ILI9486_showstring_Ch(141, 140, (u8*)"��ʱ����", GB2312_24X24);
		vTaskDelay(1000);
		ILI9486_clear_screen(130, 137, 150, 30);
		ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
		ILI9486_showstring_Ch(141, 140, (u8*)"׼����ʱ", GB2312_24X24);
		for(j = 0;j < slaver_count;j++){
			timer_slaver_num[j] = 0;
		}
		slaver_count = 0; 
	}
}



/**********************************************************************
  * @ Taskname �� BEEP_CheckoutTask
  * @ brief��     ��⼤���ƽ
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/
static void BEEP_CheckoutTask(void* parameter)
{
	static uint8_t beep_status = 0;
	EventBits_t r_event;
	for(;;)
	{
		r_event = xEventGroupWaitBits(EventGroupHandler,EVENTBIT_0,pdFALSE,pdTRUE,portMAX_DELAY); //pdFALSE����������¼���־λ
		if((r_event&EVENTBIT_0) == EVENTBIT_0)
		{
			beep_status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
			if(beep_status == 0){
				BEEP(1);
				vTaskDelay(200);
			}
			else{
				BEEP(0);
				vTaskDelay(1);		
			}
		}
	}
}

/**********************************************************************
  * @ Taskname �� IWDG_Task
  * @ brief��     ��ֹ�����ܷ�
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/

static void IWDG_Task(void* pvParameters)
{
	for(;;)
	{
//		printf("IWDG_ReloadCounter \r\n");
		IWDG_ReloadCounter();
		vTaskDelay(500);
	}
}

/**********************************************************************
  * @ Taskname �� EM5820_Print_Task
  * @ brief��     Ƕ��ʽ������ӡ
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/
static void EM5820_Print_Task(void* parameter)
{
	uint8_t buf[100]={0};
	static uint16_t print_count = 1;
	EventBits_t r_event;
	for(;;)
	{
		r_event = xEventGroupWaitBits(EventGroupHandler,EVENTBIT_1,pdTRUE,pdTRUE,portMAX_DELAY); //pdFALSE����������¼���־λ
		if((r_event&EVENTBIT_1) == EVENTBIT_1){
			if(EM5820_flag == 0)              //����������ӡ
			{
				print_count++;
				if(first_Print_flag == 0){
					first_Print_flag = 1;
					print_count = 0;
					if(InterfaceFlag == 1){
						InitializePrint();
						Print_ASCII(" ��Ȧ��������:");
						select_lines(1);   /* ����1�� */
					}
					else if(InterfaceFlag == 3){
						InitializePrint();
						Print_ASCII(" �����������:");
						select_lines(1);
					}
				}
				else if(InterfaceFlag == 1){
					InitializePrint();
					sprintf((char *)buf,"��Ȧ-Data%d  :  %.2fs",print_count,(float)Exti_Data[0]/100);
					Print_ASCII(buf);
					select_lines(1);
				}
				else if(InterfaceFlag == 3){
					InitializePrint();
					sprintf((char *)buf,"����-Data%d  :  %.2fs",print_count,(float)Cur_Data/100);
					Print_ASCII(buf);
					select_lines(1);
				}
			}
		}
	}
}


