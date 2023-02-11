#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stdio.h"
#include "stm32f10x.h"


#define PASSWORD1             (u8)0x88   //���յ��ӻ��ź���Կ
#define PASSWORD2             (u8)0x66   //����ͨѶ������Կ(�ӻ�1)
#define PASSWORD3             (u8)0x55   //����ͨѶ������Կ(�ӻ�2)

void uart1_init(u32 bound);
void uart2_init(u32 bound);

#endif


