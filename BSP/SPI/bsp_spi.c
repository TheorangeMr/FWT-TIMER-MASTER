/*******************************************
	*	@file ��  bsp_spi.c
	* @author��  �޳�
	* @data�� 2023.01.15
	* @version��  v1.0
*******************************************/
#include "bsp_spi.h"
#include "bsp_usart.h"

static vu32 SPITimeout = SPI_LONG_TIMEOUT;
static u16 SPI_TIMEOUT_UsertCallback(uint8_t errorCode);


void SPI_GPIO_Config(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	/*GPIOʱ��ʹ��*/
	RCC_APB2PeriphClockCmd(FLASH_SPI_GPIO_CLK , ENABLE);
	RCC_APB2PeriphClockCmd( FLASH_SPI_CLK, ENABLE );
	/*SCK����*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStruct);
	/*MOSI����*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStruct);
	/*MISO����*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStruct);
		/*NSS����*/
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;       //   ʹ��Ӳ��NSS
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_NSS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	       
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_NSS_PORT, &GPIO_InitStruct);
}


void SPI_Config(void)
{
	SPI_InitTypeDef   SPI_InitStruct;
	SPI_InitStruct.SPI_BaudRatePrescaler = FLASH_SPI_BaudRate;               /*SPI����������*/
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;                                /*ʱ����λ*/
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;                                 /*ʱ�Ӽ���*/
	SPI_InitStruct.SPI_CRCPolynomial = 0;                                    /*CRCУ����ʽ*/ //��ʹ��У�鹦��  //      
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                           /*����SPI������֡���ȣ���ѡ8/16λ*/
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	         /*���õ�˫��ģʽ*/
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                          /*����MSB/LSB����*/
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                               /*������/�ӻ�ģʽ*/
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;	                                 /*Ƭѡ����*/
	SPI_Init(FLASH_SPIx, &SPI_InitStruct);
	SPI_Cmd(FLASH_SPIx, ENABLE);
}

//���Ͳ�����һ���ֽ�
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
	FLASH_ERROR("SPI �ȴ���ʱ! errorCode = %d",errorCode);
	return 0;
}
