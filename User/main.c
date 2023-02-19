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

//调试变量
extern vu8 ek;

/*
*************************************************************************
*                            内核对象句柄 
*************************************************************************
*/

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
static TaskHandle_t EXTIX_DealTask_Handler= NULL;
static TaskHandle_t USART2_DealTask_Handler= NULL;
static TaskHandle_t Face_SwitchTask_Handler= NULL;

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


/*
*************************************************************************
*                             全局变量
*************************************************************************
*/

uint8_t Lap_stflag = 0;                     //单圈计时起始计数标志
uint8_t Connection_count = 0;               //连接从机通讯计数
uint8_t bxflag = 0;
uint16_t Showhigh = 260;                    //从机检测显示位置
uint16_t DataBuffer1[1024] = {0};           //加速测试数据缓存区
uint16_t DataBuffer2[1024] = {0};           //单圈测试数据缓存区
uint16_t Data1_Count = 0;                  //加速测试数据计数器
uint16_t Data2_Count = 0;                  //单圈测试数据计数器

extern vu32 Time3value;
extern vu8 Timer1flag;
extern vu8 EXTIX_Flag;



/*
*************************************************************************
*                             任务优先级
*************************************************************************
*/

#define   KEY_Priority              8
#define   EXIT_Priority             9
#define   USART_Priority            10
#define   Interface_Priority        11

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
	uart1_init(115200);
	uart2_init(115200);
	BEEP_GPIO_Config();
	Key_Init();
	Button_Init();
	
//	Exti_Init();
	TIMER1_Init(9999, 7199);                                                      //使用定时器1的捕获功能
	
	TIMER4_Init(9999, 7199);																											//(7199+1)分频，计数（4999+1）次     1s
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
	ILI9486_showstring_Ch(50,200,(u8*)"飞翼车队电气组",GB2312_32X32);
	delay_xms(500);
//	Timer2_Open();                                                                //打开定时器2定时喂狗
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
	
	BinarySem1_Handle = xSemaphoreCreateBinary();
	BinarySem2_Handle = xSemaphoreCreateBinary();
	BinarySem3_Handle = xSemaphoreCreateBinary();
	if(NULL != BinarySem1_Handle){
		printf("BinarySem1_Handle 二值信号量创建成功！");
	}
	if(NULL != BinarySem2_Handle){
		printf("BinarySem2_Handle 二值信号量创建成功！");
	}
	if(NULL != BinarySem3_Handle){
		printf("BinarySem3_Handle 二值信号量创建成功！");
	}	
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
															(const char* 	)"Face_SwitchTask",		         //任务名称
															(uint16_t 		)512,					                           //任务堆栈大小
															(void* 		  	)NULL,  				                         //传递给任务函数的参数
															(UBaseType_t 	)Interface_Priority, 				                  //任务优先级
															(TaskHandle_t*  )&Face_SwitchTask_Handler);	           //任务控制块指针   
	
	if(pdPASS == xReturn)/* 创建成功 */
		printf("Face_SwitchTask任务创建成功!\r\n");
	else
		printf("Face_SwitchTask任务创建失败!\r\n");


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
  * @ Taskname ： EXTIX_DealTask
  * @ brief：     处理外部中断
  * @ param    ： 无 
  * @ retval   ： 无
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
		while(i < 10)                                                                //软件滤波，需要耗费10ms
		{
			if(EXTI_READ() == 0) cnt++;
			vTaskDelay(1);
			i++;
		}
		BEEP(0);
		if(cnt > 5)                                                                 //如果有效采样次数大于设定值，则是中断信号
		{
			//单圈计时模式
			if(1 == InterfaceFlag)
			{
				if(0 == Lap_stflag)
				{
					n = 0;
					Timer3_Open();
					Lap_stflag = 1;
					Time3value = 0;
					sprintf(str, "%s", "计时中");
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
					sprintf(str, "%s", "计时结束");
					ILI9486_clear_screen(130, 137, 150, 30);//清除当前测试数据
					ILI9486_clear_screen(200, 200, 90, 40); 
					ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
					ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);           
					Show_Data(Exti_Data[0] , 200);
					if(Data1_Count <= 1000)
					{
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
							Show_Data(Exti_Data[1] , 240);}
						if(0 == n){n++;}
					}
				}
			}
			//其他模式
			else if(3 == InterfaceFlag)
			{
				Exti_Close();
				USART_Cmd(USART2, ENABLE);		//开串口
				sprintf(str, "%s", "计时中");
				ILI9486_clear_screen(130, 137, 150, 30);
				ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
				ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);            
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
	uint8_t rdata,slaver_flag= 0;
	static uint8_t i = 0;
	uint32_t Cur_Data = 0;
	char str[30] = {0};
	while(1)
	{
		xSemaphoreTake(BinarySem2_Handle,portMAX_DELAY);
		rdata = USART_ReceiveData(USART2);
		if(3 == InterfaceFlag)                              //多从机测试
		{
			Connection_count--;
			if((rdata&0x0f) == (PASSWORD1&0x0f)) 
			{
				Cur_Data = Time3value+(rdata&0xf0)*5;          
				Show_Data(Cur_Data , 200+40*i);									//数据显示
				if(Data2_Count <= 1000)
				{
					TestData_Save(Cur_Data);
				}
				USART_SendData(USART2,PASSWORD1);               /*给从机发送停止信号*/
				i++;
			}
			else
			{
				Cur_Data = 1110;                                    //1.11s表示数据错误
				Show_Data(Cur_Data , 200+40*i);                 //数据显示
				USART_SendData(USART2,PASSWORD1);               /*给从机发送停止信号*/
				i++;
			}
			if(Connection_count == i+1)
			{
				Exti_Open();
				Timer3_Off();
				sprintf(str, "%s", "计时结束");
				ILI9486_clear_screen(130, 137, 150, 30);
				ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
				ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
				vTaskDelay(2000);
				sprintf(str, "%s", "准备计时");
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
				sprintf(str, "%s%d", "从机连接失败: ",slaver_flag);
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
						Timer1_Open();//打开定时器计时
					}				
				}
				else if(1 == bxflag)
				{
					if(rdata == (0xc0+slaver_flag))
					{
						Timer1_Off();
						Connection_count++;
						sprintf(str, "%s%d", "已连接从机: ",slaver_flag);
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
  * @ Taskname ： Face_SwitchTask
  * @ brief：     处理显示界面
  * @ param    ： 无 
  * @ retval   ： 无
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
			MainMenu();//主界面
		}
		else if(1 == InterfaceFlag)
		{
			printf("单圈计时界面\n\r");
			Lap_Test_Menu();//单圈计时界面
			Connection_Count_Show();
		}
		else if(2 == InterfaceFlag)
		{
			Slaver_Check_Interface();//从机数量检测界面
			USART_Cmd(USART2, ENABLE);//打开串口
		}
		else if(3 == InterfaceFlag)
		{
			USART_Cmd(USART2, DISABLE);//关闭串口
			Multi_slave_function_interface();//多从机加速测试界面
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
  * @ APIname ： TestData_Save
  * @ brief：     保存数据
  * @ param    ： Timevalue 
  * @ retval   ： 无
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
