#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "semphr.h"

typedef struct
{
	u16 start_x;
	u16 start_y;
	u16 end_x;
	u16 end_y;

	u8 button_value;

	void (*ButtonDraw)(void* button);
	void (*ButtonCommand)(void);

}BUTTON;

typedef enum
{
	Determine = 0,
	Backtrack,
	View,
	Up,
	Down,
	Ready,
	Empty
}ButtonValue;

#define BUTTON_NUM 7


void Button_Init(void);
void Button_Dealing(u8 KeyValue);
void Button_Draw(void* button);
void Determine_Fun(void);
void Backtrack_Fun(void);
void View_Fun(void);
void Up_Fun(void);
void Down_Fun(void);
void Ready_Fun(void);
void Empty_Fun(void);

extern BUTTON button[BUTTON_NUM];      //°´¼üÊý×é

#endif
