/*******************************************
	*文件名 ：  GT30L32S4W.h
	*作   者：  王峰
	*修改时间： 2020.10.20
	*版   本：  v1.0
*******************************************/

#ifndef _GT30L32S4W_H
#define _GT30L32S4W_H

#include "stm32f10x.h"


/*************************** GT30L32S4W相关指令*************************/

//读指令
#define      GT30L32S4W_READ           0x03
//快读指令
#define      GT30L32S4W_FAST_READ      0x0B

/***************************GT30L32S4W相关引脚*************************/

//片选引脚
#define      GT30L32S4W_CS_CLK           RCC_APB2Periph_GPIOB
#define      GT30L32S4W_CS_PORT          GPIOB
#define      GT30L32S4W_CS_PIN           GPIO_Pin_6
#define      GT30L32S4W_CS_L             GPIO_ResetBits(GT30L32S4W_CS_PORT, GT30L32S4W_CS_PIN)
#define      GT30L32S4W_CS_H             GPIO_SetBits(GT30L32S4W_CS_PORT, GT30L32S4W_CS_PIN)

/*************************** GT30L32S4W字库地址*************************/

#define     GB2312_12X12_BASEADD       0x000000
#define     GB2312_15X16_BASEADD       0x02C9D0
#define     GB2312_24X24_BASEADD       0x068190
#define     GB2312_32X32_BASEADD       0x0EDF00
#define     GB2312_8X16_BASEADD        0x1DD790
#define     GB2312_6X12_BASEADD        0x1DBE0C
#define     ASCII_6X12_BASEADD         0x1DBE00
#define     ASCII_8X16_BASEADD         0x1DD780
#define     ASCII_12X24_BASEADD        0x1DFF00
#define     ASCII_16X32_BASEADD        0x1E5A50

/**************************GT30L32S4W字体枚举***********************/

typedef enum
{
	GB2312_12X12 = 0,
	GB2312_15X16,
	GB2312_24X24,
	GB2312_32X32,
	GB2312_8X16,
	GB2312_6X12,
	ASCII_6X12,
	ASCII_8X16,
	ASCII_12X24,
	ASCII_16X32,
}FONT;


/*************************** GT30L32S4W相关函数*************************/

void GT30L32S4W_Init(void);
void GT30L32S4W_Read(u32 addr, u8* databuffer, FONT font);
void GT30L32S4W_Fast_Read(u32 addr, u8* databuffer, FONT font);
u32  GB2312_Addr_Tans(u8* code, FONT font);
u32  ASCII_Addr_Tans(char code, FONT font);
u32  GB2312_12X12_ADDR(u8* code);
u32  GB2312_15X16_ADDR(u8* code);
u32  GB2312_24X24_ADDR(u8* code);
u32  GB2312_32X32_ADDR(u8* code);
u32  GB2312_8X16_ADDR(u8* code);
u32  GB2312_6X12_ADDR(u8* code);
u32  ASCII_6X12_ADDR(char code);
u32  ASCII_8X16_ADDR(char code);
u32  ASCII_12X24_ADDR(char code);
u32  ASCII_16X32_ADDR(char code);


#endif // !_GT30L32S4W_H
