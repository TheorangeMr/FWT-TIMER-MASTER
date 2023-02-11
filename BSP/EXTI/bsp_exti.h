#ifndef __BSP_EXTI_H
#define __BSP_EXTI_H

#include "stm32f10x.h"

/************************外部中断引脚相关宏定义***************************/

#define      EXTI_PORT         GPIOA
#define      EXTI_PIN          GPIO_Pin_8
#define      EXTI_CLK          RCC_APB2Periph_GPIOA
#define      EXTI_READ()       GPIO_ReadInputDataBit(EXTI_PORT, EXTI_PIN)
#define      EXTIX_IRQn        EXTI9_5_IRQn
#define      EXTIX_IRQHandler  EXTI9_5_IRQHandler
#define      EXTI_WAY          EXTI_Trigger_Falling

#define      EXTI_LineX        EXTI_Line8


/************************外部中断函数声明***************************/

void Exti_Init(void);
void Exti_Close(void);
void Exti_Open(void);

#endif

