/*******************************************
	*	@file ��  bsp_key.c
	* @author��  �޳�
	* @data�� 2023.01.15
	* @version��  v1.0
*******************************************/

#include "bsp_key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(KEY0_CLK | KEY1_CLK | KEY2_CLK | KEY3_CLK,ENABLE); //ʹ�ܰ���IO��ʱ��
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Pin=KEY0_PIN;
	GPIO_Init(KEY0_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY1_PIN;
	GPIO_Init(KEY1_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY2_PIN;
	GPIO_Init(KEY2_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY3_PIN;
	GPIO_Init(KEY3_PORT,&GPIO_InitStructure);
}

//����ɨ�躯��
u8 Key_Scan(u8 mode)
{
	static u8 key_up=1;  //1��ʾ��һʱ�̰���δ�����£�0��ʾ��һʱ�̰����ѱ�����
	if(mode==1) key_up=1;//֧����������״̬����ʼ������
	
	if(key_up == 1)
	{
		if(KEY0_READ==0 || KEY1_READ==0 || KEY2_READ==0 || KEY3_READ==0)
		{
			vTaskDelay(10);//��ʱ����
			key_up=0;//�����ΰ����ѱ�����
			if(KEY0_READ == 0) return KEY0_VALUE;
			if(KEY1_READ == 0) return KEY1_VALUE;
			if(KEY2_READ == 0) return KEY2_VALUE;
			if(KEY3_READ == 0) return KEY3_VALUE;
		}
	}
	else if(KEY0_READ==1 && KEY1_READ==1 && KEY2_READ==1 && KEY3_READ==1) key_up=1;
	return 0xff;//����0xff��ʾû��KEY������
}

