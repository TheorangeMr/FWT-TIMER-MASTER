/*******************************************
	*	@file ��  bsp_exti.c
	* @author��  �޳�
	* @data�� 2023.01.15
	* @version��  v1.0
*******************************************/

#include "bsp_exti.h"
#include "bsp_timer.h"

void Exti_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(EXTI_CLK|RCC_APB2Periph_AFIO,ENABLE);  //ʹ���ⲿ�ж�����ʱ��,ʹ��AFIOʱ��

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                 //�����ж�����Ϊ��������
	GPIO_InitStructure.GPIO_Pin = EXTI_PIN;
	GPIO_Init(EXTI_PORT,&GPIO_InitStructure);
	
	//����GPIOA���ж��ߵ�ӳ���ϵ
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
    
  EXTI_ClearITPendingBit(EXTI_Line8);
	
	//��ʼ����8�жϣ����ô�������
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_WAY;                     //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                       //�ж���ʹ��
	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
	EXTI_Init(&EXTI_InitStructure);
	Exti_Close();	
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTIX_IRQn;             //�ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;      //�����������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;             //������Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);

}



//�����жϺ���
void Exti_Open(void)
{
	TIM_ITConfig(ADVANCE_TIMEX, TIM_IT_CC1, ENABLE);		                    //��ֹ����/�Ƚ�1�ж�
//  EXTI_ClearITPendingBit(EXTI_Line8);
//	EXTI->IMR |= (EXTI_Line8);
}

//�ر��ⲿ�жϺ���
void Exti_Close(void)
{
	TIM_ITConfig(ADVANCE_TIMEX, TIM_IT_CC1, ENABLE);		                    //��ֹ����/�Ƚ�1�ж�
//  EXTI_ClearITPendingBit(EXTI_Line8);
//	EXTI->IMR &= ~(EXTI_Line8);
}
