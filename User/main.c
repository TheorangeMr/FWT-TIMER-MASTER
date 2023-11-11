
/*******************************************
	*	@file ：  main.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v6.0
*******************************************/

/*
*************************************************************************
*                            头文件
*************************************************************************
*/

/* kernel 头文件 */
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

/*c库头文件*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>



/* 外设驱动头文件 */
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
*                         调试API    
*************************************************************************
*/

#if TEST_vApp_Hook
/*栈溢出钩子函数*/
void vApplicationStackOverflowHook(TaskHandle_t xTask,signed char *pcTaskName)
{
	printf("栈溢出的任务名：\r\n%s\r\n",pcTaskName);
}

#endif
/*
*************************************************************************
*                         定义    
*************************************************************************
*/


#define   TEST_PRINTF_CPU           0
#define   TEST_vApp_Hook            0
//最大连接从机数目
#define   timer_slave_max           6

//任务优先级
#define   KEY_Priority              8
#define   EXIT_Priority             9
#define   USART_Priority            10
#define   Interface_Priority        11
#define   Beep_Priority             7
#define   IWDG_Priority             13
#define   Print_Priority            6
/*
*************************************************************************
*                            内核对象句柄 
*************************************************************************
*/

EventGroupHandle_t EventGroupHandler = NULL;	//事件标志组句柄

#define EVENTBIT_0	(1<<0)                          //激光传感器检测事件
#define EVENTBIT_1	(1<<1)                          //热敏打印事件

SemaphoreHandle_t  BinarySem1_Handle = NULL;
SemaphoreHandle_t  BinarySem2_Handle = NULL;
SemaphoreHandle_t  BinarySem3_Handle = NULL;

/*
*************************************************************************
*                             任务句柄初始化
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
*                             任务堆栈声明
*************************************************************************
*/


/* 闲置任务任务堆栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务任务堆栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* 定义创建任务栈 */
static StackType_t AppTaskCreate_Stack[128];
/* 定义按键任务栈 */
static StackType_t KEY_Task_Stack[64];
/*
*************************************************************************
*                             控制块声明
*************************************************************************
*/

/* 闲置任务控制块 */
static StaticTask_t Idle_Task_TCB;
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;
/* 定义创建任务控制块 */
static StaticTask_t AppTaskCreate_TCB;
/* 按键任务控制块 */
static StaticTask_t KEY_Task_TCB;


/*
*************************************************************************
*                             函数声明
*************************************************************************
*/


static void AppTaskCreate(void);                                                /* 用于创建任务 */
static void BSP_Init(void);
static void KeyScan_Task(void* parameter);
static void EXTIX_DealTask(void* parameter);
static void USART2_DealTask(void* parameter);
static void TestData_Save(uint16_t Timevalue);
static void Face_SwitchTask(void* parameter);
static void BEEP_CheckoutTask(void* parameter);
static void IWDG_Task(void* pvParameters);
static void CPU_Task(void* pvParameters);                                        /* CPU_Task任务实现 */
inline static void Usart_Handle(uint8_t rd);
static void EM5820_Print_Task(void* parameter);

/*
*************************************************************************
*                             全局变量
*************************************************************************
*/

uint8_t Lap_stflag = 0;                     //单圈计时起始计数标志
uint8_t Connection_count = 0;               //连接从机通讯计数
uint8_t bxflag = 0;
uint16_t Showhigh = 150;                    //从机检测显示位置
uint16_t DataBuffer1[1024] = {0};           //加速测试数据缓存区
uint16_t DataBuffer2[1024] = {0};           //单圈测试数据缓存区
uint16_t Data1_Count = 0;                  //加速测试数据计数器
uint16_t Data2_Count = 0;                  //单圈测试数据计数器
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

/* 获取空闲任务的内存 */

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
	                                 StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* 获取定时器任务的内存 */

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
	                                 StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/

int main(void)
{	
	BSP_Init();
	
  printf("飞翼车队电气组定时器函数调试!\r\n");	
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t	)AppTaskCreate,		  //任务函数
															(const char* 	)"AppTaskCreate",		                //任务名称
															(uint32_t 		)128,	                              //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)3, 	                              //任务优先级
															(StackType_t*   )AppTaskCreate_Stack,	            //任务堆栈
															(StaticTask_t*  )&AppTaskCreate_TCB);	            //任务控制块 
  /* 启动任务调度 */          
	if(NULL != AppTaskCreate_Handle)                                              /* 创建成功 */
    vTaskStartScheduler();    
	else
		printf("创建失败");
  
  while(1);   /* 正常不会执行到这里 */    
}


/**********************************************************************
  * @ Taskname ： BSP_Task
  * @ brief：     驱动初始化
  * @ param    ： 无 
  * @ retval   ： 无
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
//	TIMER4_Init(999, 72-1);																											//(7199+1)分频，计数（4999+1）次     1ms
	TIMER3_Init(9999, 72-1);                                                       //(71+1)分频，计数（9999+1）次       10ms
//  TIMER2_Init(9999, 7199);                                                      //(7199+1)分频，计数（4999+1）次     1s

	SPI_ILI9486_Init();
	ILI9486_tftlcd_init();
	delay_xms(500);
	GT30L32S4W_Init();
	delay_xms(100);
	u32 addr = ASCII_Addr_Tans('A', ASCII_8X16);                                  //对字库芯片进行一次读操作，去除第一次的数据丢失
	u8 zdata[16];
	GT30L32S4W_Read(addr , zdata, ASCII_8X16);
  WWDG_Config(6,1000);                                                          //6.4s
	uart1_init(115200);	
	uart2_init(115200);
	uart3_init(115200);
	ILI9486_showstring_Ch(50,200,(u8*)"飞翼车队电气组",GB2312_32X32);
	SelChineseChar();
	InitializePrint(); 
	delay_xms(500);
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;     /* 定义一个创建信息返回值，默认为pdPASS */	
  taskENTER_CRITICAL();           //进入临界区
	
	/* 创建EventGroup */  
	EventGroupHandler = xEventGroupCreate();
	if(NULL != EventGroupHandler)
	printf("EventGroupHandler 事件创建成功！\r\n");
	
	BinarySem1_Handle = xSemaphoreCreateBinary();
	BinarySem2_Handle = xSemaphoreCreateBinary();
	BinarySem3_Handle = xSemaphoreCreateBinary();
	if(NULL != BinarySem1_Handle){
		printf("BinarySem1_Handle 二值信号量创建成功！\r\n");
	}
	if(NULL != BinarySem2_Handle){
		printf("BinarySem2_Handle 二值信号量创建成功！\r\n");
	}
	if(NULL != BinarySem3_Handle){
		printf("BinarySem3_Handle 二值信号量创建成功！\r\n");
	}	
	
#if TEST_PRINTF_CPU
  /* 创建CPU_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )CPU_Task, /* 任务入口函数 */
                        (const char*    )"CPU_Task",/* 任务名字 */
                        (uint16_t       )256,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )15,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&CPU_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建CPU_Task任务成功!\r\n");	
	else
		printf("CPU_Task任务创建失败!\r\n");	
#endif	
	
  /* 创建KeyScan_Task任务 */
	KEY_Task_Handle = xTaskCreateStatic((TaskFunction_t	)KeyScan_Task,		            //任务函数
															(const char* 	)"KeyScan_Task",		                    //任务名称
															(uint32_t 		)64,					                      //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)KEY_Priority, 				                        //任务优先级
															(StackType_t*   )KEY_Task_Stack,	                //任务堆栈
															(StaticTask_t*  )&KEY_Task_TCB);	                //任务控制块													
															
															
	if(NULL != KEY_Task_Handle)                                                   /* 创建成功 */
		printf("KeyScan_Task任务创建成功!\n");
	else
		printf("KeyScan_Task任务创建失败!\n");
	
	  /* 创建EXTIX_DealTask任务 */
	xReturn = xTaskCreate((TaskFunction_t	)EXTIX_DealTask,		               //任务函数
															(const char* 	)"EXTIX_DealTask",		         //任务名称
															(uint16_t 		)512,					                           //任务堆栈大小
															(void* 		  	)NULL,  				                         //传递给任务函数的参数
															(UBaseType_t 	)EXIT_Priority, 				                             //任务优先级
															(TaskHandle_t*  )&EXTIX_DealTask_Handler);	           //任务控制块指针   
	
	if(pdPASS == xReturn)/* 创建成功 */
		printf("EXTIX_DealTask任务创建成功!\r\n");
	else
		printf("EXTIX_DealTask任务创建失败!\r\n");	
	
	
	  /* 创建USART2_DealTask任务 */
	xReturn = xTaskCreate((TaskFunction_t	)USART2_DealTask,		               //任务函数
															(const char* 	)"USART2_DealTask",		         //任务名称
															(uint16_t 		)512,					                           //任务堆栈大小
															(void* 		  	)NULL,  				                         //传递给任务函数的参数
															(UBaseType_t 	)USART_Priority, 				                  //任务优先级
															(TaskHandle_t*  )&USART2_DealTask_Handler);	           //任务控制块指针   
	
	if(pdPASS == xReturn)/* 创建成功 */
		printf("USART2_DealTask任务创建成功!\r\n");
	else
		printf("USART2_DealTask任务创建失败!\r\n");
	
	
	  /* 创建Face_SwitchTask任务 */
	xReturn = xTaskCreate((TaskFunction_t	)Face_SwitchTask,		               //任务函数
															(const char* 	)"Face_Task",		         //任务名称
															(uint16_t 		)512,					                           //任务堆栈大小
															(void* 		  	)NULL,  				                         //传递给任务函数的参数
															(UBaseType_t 	)Interface_Priority, 				                  //任务优先级
															(TaskHandle_t*  )&Face_SwitchTask_Handler);	           //任务控制块指针   
	
	if(pdPASS == xReturn)/* 创建成功 */
		printf("Face_SwitchTask任务创建成功!\r\n");
	else
		printf("Face_SwitchTask任务创建失败!\r\n");

		  /* 创建BEEP_CheckoutTask任务 */
	xReturn = xTaskCreate((TaskFunction_t	)BEEP_CheckoutTask,		               //任务函数
															(const char* 	)"BEEP_Task",		         //任务名称
															(uint16_t 		)512,					                           //任务堆栈大小
															(void* 		  	)NULL,  				                         //传递给任务函数的参数
															(UBaseType_t 	)Beep_Priority, 				                  //任务优先级
															(TaskHandle_t*  )&BEEP_CheckoutTask_Handler);	           //任务控制块指针   
	
	if(pdPASS == xReturn)/* 创建成功 */
		printf("BEEP_CheckoutTask任务创建成功!\r\n");
	else
		printf("BEEP_CheckoutTask任务创建失败!\r\n");
	
	/* 创建IWDG_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t	)IWDG_Task,		         							//任务函数
												(const char* 	)"IWDG_Task",		         								//任务名称
												(uint16_t 		)128,					                     		//任务堆栈大小
												(void* 		  	)NULL,				                       //传递给任务函数的参数
												(UBaseType_t 	)IWDG_Priority, 				                         //任务优先级
												(TaskHandle_t*  )&IWDG_Task_Handle);	   						//任务控制块指针   
	if(pdPASS == xReturn)   /* 创建成功 */
		printf("IWDG_Task任务创建成功!\r\n");
	else
		printf("IWDG_Task任务创建失败!\r\n");		
	
	/* 创建EM5820_Print_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t	)EM5820_Print_Task,		         							//任务函数
												(const char* 	)"EM5820_Print_Task",		         								//任务名称
												(uint16_t 		)128,					                     		//任务堆栈大小
												(void* 		  	)NULL,				                       //传递给任务函数的参数
												(UBaseType_t 	)Print_Priority, 				             //任务优先级
												(TaskHandle_t*  )&Print_Task_Handle);	   						//任务控制块指针   
	if(pdPASS == xReturn)   /* 创建成功 */
		printf("EM5820_Print_Task任务创建成功!\r\n");
	else
		printf("EM5820_Print_Task任务创建失败!\r\n");	

  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();                //退出临界区
}

/**********************************************************************
  * @ Taskname ： KeyScan_Task
  * @ brief：     按键扫描
  * @ param    ： 无 
  * @ retval   ： 无
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
  * @ Taskname ： EXTIX_DealTask
  * @ brief：     处理外部中断
  * @ param    ： 无 
  * @ retval   ： 无
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
		while(i < 10)                                                                //软件滤波，需要耗费10ms
		{
			if(EXTI_READ() == 0) cnt++;
			vTaskDelay(1);
			i++;
		}
		if(cnt > 5)                                                                 //如果有效采样次数大于设定值，则是中断信号
		{
			//单圈计时模式
			if(1 == InterfaceFlag)
			{
				if(0 == Lap_stflag){
					n = 0;
					Timer3_Open();
					Lap_stflag = 1;
					Time3value = 0;
					sprintf(str, "%s", "计时中");
					ILI9486_clear_screen(130, 137, 150, 30);
					ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
					ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
					if(first_Print_flag == 0){
						xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);                //置位热敏打印事件						
					}
				}//多从机
				else
				{
					if(1 == n){Exti_Data[1] = Exti_Data[0];}
					Exti_Data[0] = Time3value;
					TIM_SetCounter(TIM3,0);
					Time3value = 0;
					sprintf(str, "%s", "计时结束");
					ILI9486_clear_screen(130, 137, 150, 30);
					ILI9486_clear_screen(200, 200, 90, 40);//清除当前测试数据
					ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
					ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);           
					Show_Data(Exti_Data[0] , 200);
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
					if(Data1_Count <= 1000){
						TestData_Save(Exti_Data[0]);
					}
					vTaskDelay(1000);                                //两秒延时处理是为了一次单圈计时结束后人机交互时显示‘结束’和后显示‘计时’做缓冲  
					if(1 == InterfaceFlag)
					{
						sprintf(str, "%s", "计时中");
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
			//多从机模式
			else if(3 == InterfaceFlag)
			{
				uint8_t j = 0;
				Exti_Close();
				Timer3_Open();
				for(j = 0;j < slaver_count;j++){
					timer_slaver_num[j] = 0;
				}
				slaver_count = 0;
				USART_Cmd(USART2, ENABLE);		//开串口
				sprintf(str, "%s", "计时中");
				ILI9486_clear_screen(130, 137, 150, 30);
				ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
				ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
				if(first_Print_flag == 0){
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);                //置位热敏打印事件						
				}				
			}
		}
		else
		{
			sprintf(str, "%s", "计时错误");
			ILI9486_clear_screen(130, 137, 150, 30);
			ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
			ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
			vTaskDelay(1000);                                                    //需要改
		}
	}
}

/**********************************************************************
  * @ Taskname ： USART2_DealTask
  * @ brief：     处理串口通讯
  * @ param    ： 无 
  * @ retval   ： 无
  ********************************************************************/
static void USART2_DealTask(void* parameter)
{
	uint8_t rdata = 0;
	for(;;)
	{
		xSemaphoreTake(BinarySem2_Handle,portMAX_DELAY);
//		printf("Sem");
		rdata = USART_ReceiveData(USART2);
		if(3 == InterfaceFlag)                              //多从机测试
		{
			Usart_Handle(rdata);
		}
		else if((2 == InterfaceFlag) && (Connection_count <= timer_slave_max))
		{
			if(1 == Timer1flag)
			{
				uint8_t s1 = 0;
				printf("从机连接超时\r\n");
				slaver_flag = rdata&0x0F;
				Timer1_Off();
				ILI9486_showstring_Ch(20, Showhigh, (u8*)"从机连接失败", GB2312_24X24);
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
						Timer1_Open();//打开定时器计时
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
							ILI9486_showstring_Ch(20, Showhigh, (u8*)"已连接从机：", GB2312_24X24);
							s2 = slaver_flag + 0x30;
							ILI9486_showstring_En(160, Showhigh, (u8*)&s2, ASCII_12X24); 
							Showhigh += 40;
							//时基同步处理（未开发）
						}
						bxflag = 0;
					}
				}
			}
		  USART_Cmd(USART2, ENABLE);
		}
		else if(Connection_count > timer_slave_max){
				ILI9486_showstring_Ch(20, Showhigh, (u8*)"从机连接数量已满", GB2312_24X24);
		}
	}
}


///**********************************************************************
//  * @ Taskname ： Buchang_time_Handle
//  * @ brief：     同步从机时基
//  * @ param    ： 无 
//  * @ retval   ： 无
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
//	//接受字符串
//	//接受字符串与buchang_time进行大小比较，得出来回两端传输距离所耗时间
//	//将其补偿时间发送给从机，让从机更正时基
//}
/**********************************************************************
  * @ Taskname ： Face_SwitchTask
  * @ brief：     处理显示界面
  * @ param    ： 无 
  * @ retval   ： 无
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
			MainMenu();//主界面
			xEventGroupClearBits(EventGroupHandler,EVENTBIT_0);
			BEEP(0);
			Showhigh = 150;
			first_Print_flag = 0;
		}
		else if(1 == InterfaceFlag)
		{
			printf("单圈计时界面\n\r");
			Lap_Test_Menu();//单圈计时界面
			xEventGroupSetBits(EventGroupHandler,EVENTBIT_0);
			Connection_Count_Show();
		}
		else if(2 == InterfaceFlag)
		{
			printf("从机数量检测界面\n\r");
			USART_Cmd(USART2, ENABLE);//打开串口
			for(uint8_t t = 0;t<10;t++){
				USART_SendData(USART2 ,slaver_reset);
				vTaskDelay(5);
			}
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			Slaver_Check_Interface();//从机数量检测界面
			xEventGroupClearBits(EventGroupHandler,EVENTBIT_0);
		}
		else if(3 == InterfaceFlag)
		{
			printf("多从机加速测试界面\n\r");
			USART_Cmd(USART2, DISABLE);//关闭串口
			Multi_slave_function_interface();//多从机加速测试界面
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
  * @ APIname ： TestData_Save
  * @ brief：     保存数据
  * @ param    ： Timevalue 
  * @ retval   ： 无
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
  * @ APIname ： Usart_Handle
  * @ brief：     处理通讯数据
  * @ param    ： rd
  * @ retval   ： 无
  ********************************************************************/


inline static void Usart_Handle(uint8_t rd)
{
	uint8_t j = 0,n = 5;
	bool m = 0;
	uint8_t slaver_display;
	if((rd&0x0f) >0&&(rd&0x0f)<=timer_slave_max)
	{
//		printf("接受从机数据\r\n");
		for(j = 0; j<slaver_count; j++){
			if((rd&0x0f) == timer_slaver_num[j]){
//				printf("从机重复触发\r\n");
				m = 1;
				break;
			}
		}
		if(m == 0)
		{
			if((Time3value-(rd&0xf0)*5) <= 60000){//溢出处理
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
			Show_Data(Cur_Data , 190+40*slaver_display);									//数据显示
			xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
			if(Data2_Count <= 1000){
				TestData_Save(Cur_Data);
			}
			timer_slaver_num[slaver_count] = (rd&0x0f);
//			printf("%d\r\n",timer_slaver_num[slaver_count]);
			while(n--){
				USART_SendData(USART2,(rd&0x0f)+0xf0);               /*给从机发送停止信号*/		
				vTaskDelay(1);
			}
			slaver_count++;
		}
//		printf("slaver_count = %d\r\n",slaver_count);
		USART_Cmd(USART2, ENABLE);
	}
	else
	{
		Cur_Data = 1110;                                    //1.11s表示数据错误
		Show_Data(Cur_Data , 190+40*slaver_count);                 //数据显示
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
		ILI9486_showstring_Ch(141, 140, (u8*)"计时结束", GB2312_24X24);
		vTaskDelay(1000);
		ILI9486_clear_screen(130, 137, 150, 30);
		ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
		ILI9486_showstring_Ch(141, 140, (u8*)"准备计时", GB2312_24X24);
		for(j = 0;j < slaver_count;j++){
			timer_slaver_num[j] = 0;
		}
		slaver_count = 0; 
	}
}



/**********************************************************************
  * @ Taskname ： BEEP_CheckoutTask
  * @ brief：     检测激光电平
  * @ param    ： 无 
  * @ retval   ： 无
  ********************************************************************/
static void BEEP_CheckoutTask(void* parameter)
{
	static uint8_t beep_status = 0;
	EventBits_t r_event;
	for(;;)
	{
		r_event = xEventGroupWaitBits(EventGroupHandler,EVENTBIT_0,pdFALSE,pdTRUE,portMAX_DELAY); //pdFALSE：不清除该事件标志位
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
  * @ Taskname ： IWDG_Task
  * @ brief：     防止程序跑飞
  * @ param    ： 无 
  * @ retval   ： 无
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
  * @ Taskname ： EM5820_Print_Task
  * @ brief：     嵌入式热敏打印
  * @ param    ： 无 
  * @ retval   ： 无
  ********************************************************************/
static void EM5820_Print_Task(void* parameter)
{
	uint8_t buf[100]={0};
	static uint16_t print_count = 1;
	EventBits_t r_event;
	for(;;)
	{
		r_event = xEventGroupWaitBits(EventGroupHandler,EVENTBIT_1,pdTRUE,pdTRUE,portMAX_DELAY); //pdFALSE：不清除该事件标志位
		if((r_event&EVENTBIT_1) == EVENTBIT_1){
			if(EM5820_flag == 0)              //允许热敏打印
			{
				print_count++;
				if(first_Print_flag == 0){
					first_Print_flag = 1;
					print_count = 0;
					if(InterfaceFlag == 1){
						InitializePrint();
						Print_ASCII(" 单圈测试数据:");
						select_lines(1);   /* 换行1次 */
					}
					else if(InterfaceFlag == 3){
						InitializePrint();
						Print_ASCII(" 区间测试数据:");
						select_lines(1);
					}
				}
				else if(InterfaceFlag == 1){
					InitializePrint();
					sprintf((char *)buf,"单圈-Data%d  :  %.2fs",print_count,(float)Exti_Data[0]/100);
					Print_ASCII(buf);
					select_lines(1);
				}
				else if(InterfaceFlag == 3){
					InitializePrint();
					sprintf((char *)buf,"区间-Data%d  :  %.2fs",print_count,(float)Cur_Data/100);
					Print_ASCII(buf);
					select_lines(1);
				}
			}
		}
	}
}


