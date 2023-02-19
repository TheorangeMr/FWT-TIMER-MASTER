#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

#define  ADVANCE_TIMEX                    TIM1
#define  ADVANCE_TIM1_CLK                 RCC_APB2Periph_TIM1

//使用的定时器1捕获通道
#define  ADVANCE_TIM1_IT_CCx              TIM_IT_CC1
//定时器1的捕获中断，更新中断
#define  ADVANCE_TIM1_IRQ                  TIM1_UP_IRQn
#define  ADVANCE_TIM1_CCx_IRQ              TIM1_CC_IRQn   
#define  ADVANCE_TIM1_IRQHandler           TIM1_UP_IRQHandler
#define  ADVANCE_TIM1_CCx_IRQHandler       TIM1_CC_IRQHandler
//定时器1外部引脚
#define  ADVANCE_TIM1_CH1_GPIO_CLK         RCC_APB2Periph_GPIOA
#define  ADVANCE_TIM1_CH1_GPIO_PORT        GPIOA
#define  ADVANCE_TIM1_CH1_PIN              GPIO_Pin_8
#define  ADVANCE_TIM1_CHANNEL_x            TIM_Channel_1
#define  ADVANCE_TIM1_RepetitionCount      0

void TIMER4_Init(u16 arr,u16 psc);
void Timer4_Open(void);
void Timer4_Off(void);
void TIMER3_Init(u16 arr,u16 psc);
void Timer3_Open(void);
void Timer3_Off(void);
void TIMER2_Init(u16 arr,u16 psc);
void Timer2_Open(void);
void Timer2_Off(void);
void TIMER1_Init(u16 arr,u16 psc);
void Timer1_Open(void);
void Timer1_Off(void);
static void NVIC_Time1_Config(void);


#endif

