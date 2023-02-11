#ifndef  _BSP_SPI_FLASH_H_
#define  _BSP_SPI_FLASH_H_

#include<stm32f10x.h>

#define FLASH_SPI_GPIO_CLK     (RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC)
#define FLASH_SPI_CLK          RCC_APB2Periph_SPI1

#define FLASH_SPIx             SPI1
#define FLASH_SPI_BaudRate     SPI_BaudRatePrescaler_2

#define FLASH_SPI_SCK_PORT          GPIOA
#define FLASH_SPI_SCK_PIN      GPIO_Pin_5
#define FLASH_SPI_MISO_PORT         GPIOA
#define FLASH_SPI_MISO_PIN     GPIO_Pin_6
#define FLASH_SPI_MOSI_PORT         GPIOA
#define FLASH_SPI_MOSI_PIN     GPIO_Pin_7
    /*片选CS*/
#define FLASH_SPI_NSS_PORT          GPIOA
#define FLASH_SPI_NSS_PIN      GPIO_Pin_4


#define  SPI_FLASH_CS_HIGH()              GPIO_SetBits(FLASH_SPI_NSS_PORT, FLASH_SPI_NSS_PIN);
#define  SPI_FLASH_CS_LOW()               GPIO_ResetBits(FLASH_SPI_NSS_PORT, FLASH_SPI_NSS_PIN);

/*等待超时时间*/
#define SPI_FLAG_TIMEOUT                 ((uint32_t)0x1000)
#define SPI_LONG_TIMEOUT                 ((uint32_t)(10 * SPI_FLAG_TIMEOUT))

#define FLASH_ERROR(fmt,arg...)           printf("<<-FLASH-ERROR->> "fmt"\n",##arg)

#define Dummy                             0x00


void SPI_GPIO_Config(void);
void SPI_Config(void);
void SPI_ILI9486_Init(void);
uint8_t SPI_SendByte(uint8_t data);


#endif  /*_BSP_SPI_FLASH_H_*/

