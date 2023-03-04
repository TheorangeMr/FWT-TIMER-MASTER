#ifndef __AS32_100_C_H
#define __AS32_100_C_H

#include "stm32f10x.h"

#define M0_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define M0_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define M0_GPIO_PIN		    GPIO_Pin_3		


#define M1_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define M1_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define M1_GPIO_PIN		    GPIO_Pin_4	

#define AUX_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define AUX_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define AUX_GPIO_PIN		  GPIO_Pin_5	


void AS32_GPIO_Config(void);
void AS32_Init(void);



#endif
