/*******************************************
	*	@file ：  bsp_spi.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v1.0
*******************************************/
#include "bsp_spi.h"
#include "bsp_usart.h"

static vu32 SPITimeout = SPI_LONG_TIMEOUT;
static u16 SPI_TIMEOUT_UsertCallback(uint8_t errorCode);


void SPI_GPIO_Config(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	/*GPIO时钟使能*/
	RCC_APB2PeriphClockCmd(FLASH_SPI_GPIO_CLK , ENABLE);
	RCC_APB2PeriphClockCmd( FLASH_SPI_CLK, ENABLE );
	/*SCK配置*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStruct);
	/*MOSI配置*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStruct);
	/*MISO配置*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStruct);
		/*NSS配置*/
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;       //   使用硬件NSS
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_NSS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	       
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_NSS_PORT, &GPIO_InitStruct);
}


void SPI_Config(void)
{
	SPI_InitTypeDef   SPI_InitStruct;
	SPI_InitStruct.SPI_BaudRatePrescaler = FLASH_SPI_BaudRate;               /*SPI波特率配置*/
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;                                /*时钟相位*/
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;                                 /*时钟极性*/
	SPI_InitStruct.SPI_CRCPolynomial = 0;                                    /*CRC校验表达式*/ //不使用校验功能  //      
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                           /*设置SPI的数据帧长度，可选8/16位*/
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	         /*设置单双向模式*/
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                          /*设置MSB/LSB先行*/
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                               /*设置主/从机模式*/
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;	                                 /*片选设置*/
	SPI_Init(FLASH_SPIx, &SPI_InitStruct);
	SPI_Cmd(FLASH_SPIx, ENABLE);
}

//发送并接收一个字节
uint8_t SPI_SendByte(uint8_t data)
{
	SPITimeout = SPI_FLAG_TIMEOUT;
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0)
		{
			return SPI_TIMEOUT_UsertCallback(0);
		}
	}
	SPI_I2S_SendData(FLASH_SPIx,data);
	SPITimeout = SPI_FLAG_TIMEOUT;

	while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0)
		{
			return SPI_TIMEOUT_UsertCallback(1);
		}		
	}
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}



void SPI_ILI9486_Init(void)
{
	SPI_GPIO_Config();
	SPI_Config();
}

static uint16_t SPI_TIMEOUT_UsertCallback(uint8_t errorCode)
{
	FLASH_ERROR("SPI 等待超时! errorCode = %d",errorCode);
	return 0;
}
