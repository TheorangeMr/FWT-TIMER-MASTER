/*******************************************
	*	@file ：  bsp_exti.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v1.0
*******************************************/

#include "bsp_exti.h"
#include "bsp_timer.h"

void Exti_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(EXTI_CLK|RCC_APB2Periph_AFIO,ENABLE);  //使能外部中断引脚时钟,使能AFIO时钟

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                 //设置中断引脚为上拉输入
	GPIO_InitStructure.GPIO_Pin = EXTI_PIN;
	GPIO_Init(EXTI_PORT,&GPIO_InitStructure);
	
	//设置GPIOA与中断线的映射关系
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
    
  EXTI_ClearITPendingBit(EXTI_Line8);
	
	//初始化线8中断，设置触发条件
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_WAY;                     //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                       //中断线使能
	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
	EXTI_Init(&EXTI_InitStructure);
	Exti_Close();	
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTIX_IRQn;             //外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;      //设置抢断优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;             //设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                //使能中断通道
	NVIC_Init(&NVIC_InitStructure);

}



//开启中断函数
void Exti_Open(void)
{
	TIM_ITConfig(ADVANCE_TIMEX, TIM_IT_CC1, ENABLE);		                    //禁止捕获/比较1中断
//  EXTI_ClearITPendingBit(EXTI_Line8);
//	EXTI->IMR |= (EXTI_Line8);
}

//关闭外部中断函数
void Exti_Close(void)
{
	TIM_ITConfig(ADVANCE_TIMEX, TIM_IT_CC1, ENABLE);		                    //禁止捕获/比较1中断
//  EXTI_ClearITPendingBit(EXTI_Line8);
//	EXTI->IMR &= ~(EXTI_Line8);
}
