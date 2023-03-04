/*******************************************
	*	@file ��  bsp_timer.c
	* @author��  �޳�
	* @data�� 2023.01.15
	* @version��  v1.0
*******************************************/
#include "bsp_timer.h"

extern vu16 Time1value;

void TIMER4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);            //ʹ�ܶ�ʱ��ʱ��
	
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;  //����Ϊ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_Period=arr;                      //�Զ�װ�س�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;                   //Ԥ��Ƶϵ��
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);             //��ʼ����ʱ��
    
  TIM_ClearITPendingBit(TIM4,TIM_FLAG_Update);                   //ʹ�ܸ����ж�ǰ��������жϱ�־��
                                                                 //��ֹһ��ʼʹ�ܾͽ�������ж�
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                       //������ʱ��3�����ж�
	TIM_Cmd(TIM4,DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Timer4_Open(void)
{
	TIM_SetCounter(TIM4,0);                                                     //��ʱ����ֵ����
	TIM_Cmd(TIM4,ENABLE);                                                       //ʹ�ܶ�ʱ��3
}


void Timer4_Off(void)
{
  TIM_ClearITPendingBit(TIM4,TIM_FLAG_Update);
	TIM_Cmd(TIM4,DISABLE);                                                      //��ʹ�ܶ�ʱ��3
}

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
	TIM_Cmd(TIM3,DISABLE);
	
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
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);    
	
  TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);                   //ʹ�ܸ����ж�ǰ��������жϱ�־��
                                                                   //��ֹһ��ʼʹ�ܾͽ�������ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                       //������ʱ��2�����ж�
	TIM_Cmd(TIM2,DISABLE);
	

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
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(ADVANCE_TIM1_CLK, ENABLE);

	/* TIM1 channel 1 pin (PA.08) configuration */
	GPIO_InitStructure.GPIO_Pin = ADVANCE_TIM1_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                         //�½��ش�������Ϊ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADVANCE_TIM1_CH1_GPIO_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��1 */
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;               //����Ϊ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			              //����Ƶ
	TIM_TimeBaseInitStructure.TIM_Period = arr;											              //�Զ�װ�س�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;									              //Ԥ��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = ADVANCE_TIM1_RepetitionCount;//�߼���ʱ����������
	TIM_TimeBaseInit(ADVANCE_TIMEX, &TIM_TimeBaseInitStructure);				          //��ʼ����ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel= ADVANCE_TIM1_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(ADVANCE_TIMEX,TIM_IT_Update,ENABLE);                       //������ʱ��2�����ж�
	TIM_Cmd(ADVANCE_TIMEX,DISABLE);	
																																								
}

void Timer1_Open(void)
{
	Time1value = 0;
  TIM_SetCounter(ADVANCE_TIMEX, 0);                                                    //��ʱ����ֵ����
	TIM_Cmd(ADVANCE_TIMEX,ENABLE);                                                       //ʹ�ܶ�ʱ��2       
}

void Timer1_Off(void)
{
  TIM_ClearITPendingBit(ADVANCE_TIMEX, TIM_FLAG_Update);
	TIM_Cmd(ADVANCE_TIMEX, DISABLE);      
}
