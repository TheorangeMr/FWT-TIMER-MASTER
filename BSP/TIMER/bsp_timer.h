#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void TIMER3_Init(u16 arr,u16 psc);
void Timer3_Open(void);
void Timer3_Off(void);
void TIMER2_Init(u16 arr,u16 psc);
void Timer2_Open(void);
void Timer2_Off(void);
void TIMER1_Init(u16 arr,u16 psc);
void Timer1_Open(void);
void Timer1_Off(void);

#endif

