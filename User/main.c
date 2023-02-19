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

//���Ա���
extern vu8 ek;

/*
*************************************************************************
*                            �ں˶����� 
*************************************************************************
*/

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
static TaskHandle_t EXTIX_DealTask_Handler= NULL;
static TaskHandle_t USART2_DealTask_Handler= NULL;
static TaskHandle_t Face_SwitchTask_Handler= NULL;

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


/*
*************************************************************************
*                             ȫ�ֱ���
*************************************************************************
*/

uint8_t Lap_stflag = 0;                     //��Ȧ��ʱ��ʼ������־
uint8_t Connection_count = 0;               //���Ӵӻ�ͨѶ����
uint8_t bxflag = 0;
uint16_t Showhigh = 260;                    //�ӻ������ʾλ��
uint16_t DataBuffer1[1024] = {0};           //���ٲ������ݻ�����
uint16_t DataBuffer2[1024] = {0};           //��Ȧ�������ݻ�����
uint16_t Data1_Count = 0;                  //���ٲ������ݼ�����
uint16_t Data2_Count = 0;                  //��Ȧ�������ݼ�����

extern vu32 Time3value;
extern vu8 Timer1flag;
extern vu8 EXTIX_Flag;



/*
*************************************************************************
*                             �������ȼ�
*************************************************************************
*/

#define   KEY_Priority              8
#define   EXIT_Priority             9
#define   USART_Priority            10
#define   Interface_Priority        11

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
	uart1_init(115200);
	uart2_init(115200);
	BEEP_GPIO_Config();
	Key_Init();
	Button_Init();
	
//	Exti_Init();
	TIMER1_Init(9999, 7199);                                                      //ʹ�ö�ʱ��1�Ĳ�����
	
	TIMER4_Init(9999, 7199);																											//(7199+1)��Ƶ��������4999+1����     1s
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
	ILI9486_showstring_Ch(50,200,(u8*)"�����ӵ�����",GB2312_32X32);
	delay_xms(500);
//	Timer2_Open();                                                                //�򿪶�ʱ��2��ʱι��
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
	
	BinarySem1_Handle = xSemaphoreCreateBinary();
	BinarySem2_Handle = xSemaphoreCreateBinary();
	BinarySem3_Handle = xSemaphoreCreateBinary();
	if(NULL != BinarySem1_Handle){
		printf("BinarySem1_Handle ��ֵ�ź��������ɹ���");
	}
	if(NULL != BinarySem2_Handle){
		printf("BinarySem2_Handle ��ֵ�ź��������ɹ���");
	}
	if(NULL != BinarySem3_Handle){
		printf("BinarySem3_Handle ��ֵ�ź��������ɹ���");
	}	
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
															(const char* 	)"Face_SwitchTask",		         //��������
															(uint16_t 		)512,					                           //�����ջ��С
															(void* 		  	)NULL,  				                         //���ݸ��������Ĳ���
															(UBaseType_t 	)Interface_Priority, 				                  //�������ȼ�
															(TaskHandle_t*  )&Face_SwitchTask_Handler);	           //������ƿ�ָ��   
	
	if(pdPASS == xReturn)/* �����ɹ� */
		printf("Face_SwitchTask���񴴽��ɹ�!\r\n");
	else
		printf("Face_SwitchTask���񴴽�ʧ��!\r\n");


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
	while(1)
	{
		KeyValue = 0xff;
		KeyValue = Key_Scan(0);
	  if(KeyValue != 0xff)
		{
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
	static uint32_t Exti_Data[3] = {0};
	static uint8_t n = 0;
	char str[20] = { 0 };
	//	static u8 k = 0;
	while(1)
	{
		xSemaphoreTake(BinarySem1_Handle,portMAX_DELAY);
//		k++;
		EXTIX_Flag = 0;
		BEEP(1);
		while(i < 10)                                                                //����˲�����Ҫ�ķ�10ms
		{
			if(EXTI_READ() == 0) cnt++;
			vTaskDelay(1);
			i++;
		}
		BEEP(0);
		if(cnt > 5)                                                                 //�����Ч�������������趨ֵ�������ж��ź�
		{
			//��Ȧ��ʱģʽ
			if(1 == InterfaceFlag)
			{
				if(0 == Lap_stflag)
				{
					n = 0;
					Timer3_Open();
					Lap_stflag = 1;
					Time3value = 0;
					sprintf(str, "%s", "��ʱ��");
					ILI9486_clear_screen(130, 137, 150, 30);
					ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
					ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
				}
				else
				{
					if(1 == n){Exti_Data[1] = Exti_Data[0];}
					Exti_Data[0] = Time3value;//ek
					TIM_SetCounter(TIM3,0);
					Time3value = 0;
					sprintf(str, "%s", "��ʱ����");
					ILI9486_clear_screen(130, 137, 150, 30);//�����ǰ��������
					ILI9486_clear_screen(200, 200, 90, 40); 
					ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
					ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);           
					Show_Data(Exti_Data[0] , 200);
					if(Data1_Count <= 1000)
					{
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
							Show_Data(Exti_Data[1] , 240);}
						if(0 == n){n++;}
					}
				}
			}
			//����ģʽ
			else if(3 == InterfaceFlag)
			{
				Exti_Close();
				USART_Cmd(USART2, ENABLE);		//������
				sprintf(str, "%s", "��ʱ��");
				ILI9486_clear_screen(130, 137, 150, 30);
				ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
				ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);            
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
	uint8_t rdata,slaver_flag= 0;
	static uint8_t i = 0;
	uint32_t Cur_Data = 0;
	char str[30] = {0};
	while(1)
	{
		xSemaphoreTake(BinarySem2_Handle,portMAX_DELAY);
		rdata = USART_ReceiveData(USART2);
		if(3 == InterfaceFlag)                              //��ӻ�����
		{
			Connection_count--;
			if((rdata&0x0f) == (PASSWORD1&0x0f)) 
			{
				Cur_Data = Time3value+(rdata&0xf0)*5;          
				Show_Data(Cur_Data , 200+40*i);									//������ʾ
				if(Data2_Count <= 1000)
				{
					TestData_Save(Cur_Data);
				}
				USART_SendData(USART2,PASSWORD1);               /*���ӻ�����ֹͣ�ź�*/
				i++;
			}
			else
			{
				Cur_Data = 1110;                                    //1.11s��ʾ���ݴ���
				Show_Data(Cur_Data , 200+40*i);                 //������ʾ
				USART_SendData(USART2,PASSWORD1);               /*���ӻ�����ֹͣ�ź�*/
				i++;
			}
			if(Connection_count == i+1)
			{
				Exti_Open();
				Timer3_Off();
				sprintf(str, "%s", "��ʱ����");
				ILI9486_clear_screen(130, 137, 150, 30);
				ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
				ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
				vTaskDelay(2000);
				sprintf(str, "%s", "׼����ʱ");
				ILI9486_clear_screen(130, 137, 150, 30);
				ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
				ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
				i = 0;
			}
		}
		else if(2 == InterfaceFlag)
		{
			if(1 == Timer1flag)
			{
				Timer1_Off();
				sprintf(str, "%s%d", "�ӻ�����ʧ��: ",slaver_flag);
				ILI9486_showstring_Ch(20, Showhigh, (u8*)str, GB2312_24X24);
				Showhigh += 40;
				Timer1flag = 0;
				bxflag = 0;
			}
			else
			{
				if(0 == bxflag)
				{
					if(rdata&0xF0 == 0xa0)
					{
						slaver_flag = rdata&0x0F;
						for(uint8_t i = 0; i<10 ;i++)
						{
							USART_SendData(USART2 , (0xb0+slaver_flag));
							vTaskDelay(50);
						}  
						bxflag = 1;
						Timer1_Open();//�򿪶�ʱ����ʱ
					}				
				}
				else if(1 == bxflag)
				{
					if(rdata == (0xc0+slaver_flag))
					{
						Timer1_Off();
						Connection_count++;
						sprintf(str, "%s%d", "�����Ӵӻ�: ",slaver_flag);
						ILI9486_showstring_Ch(20, Showhigh, (u8*)str, GB2312_24X24);
						Showhigh += 40;
						bxflag = 0;
					}		
				}
			}
		}
	}
}
/**********************************************************************
  * @ Taskname �� Face_SwitchTask
  * @ brief��     ������ʾ����
  * @ param    �� �� 
  * @ retval   �� ��
  ********************************************************************/
static void Face_SwitchTask(void* parameter)
{
	while(1)
	{
		if(0 == InterfaceFlag)
		{
//			if(EXTIX_Flag == 1)
//			{
//				EXTIX_Flag = 0;
////				xSemaphoreTake(BinarySem1_Handle,portMAX_DELAY);
//			}
			USART_Cmd(USART2, DISABLE);
			MainMenu();//������
		}
		else if(1 == InterfaceFlag)
		{
			printf("��Ȧ��ʱ����\n\r");
			Lap_Test_Menu();//��Ȧ��ʱ����
			Connection_Count_Show();
		}
		else if(2 == InterfaceFlag)
		{
			Slaver_Check_Interface();//�ӻ�����������
			USART_Cmd(USART2, ENABLE);//�򿪴���
		}
		else if(3 == InterfaceFlag)
		{
			USART_Cmd(USART2, DISABLE);//�رմ���
			Multi_slave_function_interface();//��ӻ����ٲ��Խ���
			Connection_Count_Show();
		}
		else if(4 == InterfaceFlag)
		{
			Exti_Close();
			Show_LapRecords(DataBuffer1, Data1_Count);
		}
		else if(5 == InterfaceFlag)
		{
			Exti_Close();
			Show_AcceleratedRecords(DataBuffer2, Data2_Count);
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
	if(1 == InterfaceFlag)
	{
		DataBuffer1[Data1_Count++] = Timevalue;
	}
	else if(3 == InterfaceFlag)
	{
		DataBuffer2[Data2_Count++] = Timevalue;
	}
}
