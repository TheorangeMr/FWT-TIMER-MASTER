/*******************************************
	*	@file ��  bsp_timer.c
	* @author��  �޳�
	* @data�� 2023.01.15
	* @version��  v1.0
*******************************************/
#include "bsp_timer.h"

extern vu16 Time1value;

void TIMER3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);            //ʹ�ܶ�ʱ��ʱ��
	
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;  //����Ϊ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Period=arr;                      //�Զ�װ�س�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;                   //Ԥ��Ƶϵ��
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);             //��ʼ����ʱ��
    
  TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);                   //ʹ�ܸ����ж�ǰ��������жϱ�־��
                                                                 //��ֹһ��ʼʹ�ܾͽ�������ж�
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                       //������ʱ��3�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void Timer3_Open(void)
{
	TIM_SetCounter(TIM3,0);                                                     //��ʱ����ֵ����
	TIM_Cmd(TIM3,ENABLE);                                                       //ʹ�ܶ�ʱ��3
}


void Timer3_Off(void)
{
  TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
	TIM_Cmd(TIM3,DISABLE);                                                      //��ʹ�ܶ�ʱ��3
}



void TIMER2_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);            //ʹ�ܶ�ʱ��ʱ��
	
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;  //����Ϊ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Period=arr;                      //�Զ�װ�س�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;                   //Ԥ��Ƶϵ��
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);             //��ʼ����ʱ��
    
  TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);                   //ʹ�ܸ����ж�ǰ��������жϱ�־��
                                                                   //��ֹһ��ʼʹ�ܾͽ�������ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                       //������ʱ��2�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void Timer2_Open(void)
{
  TIM_SetCounter(TIM2, 0);                                                    //��ʱ����ֵ����
	TIM_Cmd(TIM2,ENABLE);                                                       //ʹ�ܶ�ʱ��2       
}

void Timer2_Off(void)
{
  TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
	TIM_Cmd(TIM2, DISABLE);        
}


void TIMER1_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);            //ʹ�ܶ�ʱ��ʱ��
	
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;  //����Ϊ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Period=arr;                      //�Զ�װ�س�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;                   //Ԥ��Ƶϵ��
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);             //��ʼ����ʱ��
    
  TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);                   //ʹ�ܸ����ж�ǰ��������жϱ�־��
                                                                   //��ֹһ��ʼʹ�ܾͽ�������ж�
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);                       //������ʱ��2�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Timer1_Open(void)
{
	Time1value = 0;
  TIM_SetCounter(TIM1, 0);                                                    //��ʱ����ֵ����
	TIM_Cmd(TIM1,ENABLE);                                                       //ʹ�ܶ�ʱ��2       
}

void Timer1_Off(void)
{
  TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
	TIM_Cmd(TIM1, DISABLE);      
}
