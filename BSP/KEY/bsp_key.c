/*******************************************
	*	@file ：  bsp_key.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v1.0
*******************************************/

#include "bsp_key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(KEY0_CLK | KEY1_CLK | KEY2_CLK | KEY3_CLK,ENABLE); //使能按键IO口时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin=KEY0_PIN;
	GPIO_Init(KEY0_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY1_PIN;
	GPIO_Init(KEY1_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY2_PIN;
	GPIO_Init(KEY2_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY3_PIN;
	GPIO_Init(KEY3_PORT,&GPIO_InitStructure);
}

//按键扫描函数
u8 Key_Scan(u8 mode)
{
	static u8 key_up=1;  //1表示上一时刻按键未被按下，0表示上一时刻按键已被按下
	if(mode==1) key_up=1;//支持连续按，状态条件始终满足
	
	if(key_up == 1)
	{
		if(KEY0_READ==0 || KEY1_READ==0 || KEY2_READ==0 || KEY3_READ==0)
		{
			vTaskDelay(10);//延时消抖
			key_up=0;//标记这次按键已被按下
			if(KEY0_READ == 0) return KEY0_VALUE;
			if(KEY1_READ == 0) return KEY1_VALUE;
			if(KEY2_READ == 0) return KEY2_VALUE;
			if(KEY3_READ == 0) return KEY3_VALUE;
		}
	}
	else if(KEY0_READ==1 && KEY1_READ==1 && KEY2_READ==1 && KEY3_READ==1) key_up=1;
	return 0xff;//返回0xff表示没有KEY被按下
}

