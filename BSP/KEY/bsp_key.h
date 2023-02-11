#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"


/***************************按键引脚相关宏定义*************************/

//KEY0
#define       KEY0_CLK       RCC_APB2Periph_GPIOB
#define       KEY0_PORT      GPIOB
#define       KEY0_PIN       GPIO_Pin_10
#define       KEY0_READ      GPIO_ReadInputDataBit(KEY0_PORT,KEY0_PIN)
//KEY1
#define       KEY1_CLK       RCC_APB2Periph_GPIOB
#define       KEY1_PORT      GPIOB
#define       KEY1_PIN       GPIO_Pin_11
#define       KEY1_READ      GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)
//KEY2
#define       KEY2_CLK       RCC_APB2Periph_GPIOB
#define       KEY2_PORT      GPIOB
#define       KEY2_PIN       GPIO_Pin_12
#define       KEY2_READ      GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)
//KEY3
#define       KEY3_CLK       RCC_APB2Periph_GPIOB
#define       KEY3_PORT      GPIOB
#define       KEY3_PIN       GPIO_Pin_13
#define       KEY3_READ      GPIO_ReadInputDataBit(KEY3_PORT,KEY3_PIN)


//按键键值（枚举）
typedef enum{
	KEY0_VALUE = 0,
	KEY1_VALUE,
	KEY2_VALUE,
	KEY3_VALUE
}key;

u8 Key_Scan(u8 mode);
void Key_Init(void);

#endif

