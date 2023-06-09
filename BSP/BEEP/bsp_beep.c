/*******************************************
	*	@file ：  bsp_beep.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v1.0
*******************************************/
#include "bsp_beep.h"   

 /**
  * @brief  初始化控制蜂鸣器的IO
  * @param  无
  * @retval 无
  */
void BEEP_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启控制蜂鸣器的GPIO的端口时钟*/
		RCC_APB2PeriphClockCmd( BEEP_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE); 
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
		/*选择要控制蜂鸣器的GPIO*/															   
		GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;	

		/*设置GPIO模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置GPIO速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化控制蜂鸣器的GPIO*/
		GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);			  
}

void BEEP_Init(void)
{
	BEEP_GPIO_Config();
	/* 关闭蜂鸣器*/
  GPIO_WriteBit(BEEP_GPIO_PORT, BEEP_GPIO_PIN, Bit_RESET); 
//	  GPIO_WriteBit(BEEP_GPIO_PORT, BEEP_GPIO_PIN, Bit_SET); 
}


/*********************************************END OF FILE**********************/
