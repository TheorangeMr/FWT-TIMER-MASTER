/*******************************************
	*	@file ：  bsp_beep.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v1.0
*******************************************/

#include "as32-100-c.h"


 /**
  * @brief  初始化通讯模块的IO
  * @param  无
  * @retval 无
  */
void AS32_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd( M0_GPIO_CLK|M1_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE); 	
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
		GPIO_InitStructure.GPIO_Pin = M0_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(M0_GPIO_PORT, &GPIO_InitStructure);	
		GPIO_InitStructure.GPIO_Pin = M1_GPIO_PIN;
		GPIO_Init(M1_GPIO_PORT, &GPIO_InitStructure);
}

void AS32_Init(void)
{
	AS32_GPIO_Config();
	GPIO_ResetBits(M0_GPIO_PORT, M0_GPIO_PIN);	
	GPIO_ResetBits(M1_GPIO_PORT, M1_GPIO_PIN);
}
