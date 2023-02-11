#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stdio.h"
#include "stm32f10x.h"


#define PASSWORD1             (u8)0x88   //接收到从机信号密钥
#define PASSWORD2             (u8)0x66   //串口通讯连接密钥(从机1)
#define PASSWORD3             (u8)0x55   //串口通讯连接密钥(从机2)

void uart1_init(u32 bound);
void uart2_init(u32 bound);

#endif


