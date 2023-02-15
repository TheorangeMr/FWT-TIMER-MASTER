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
	Empty,
	Backtrack1
}ButtonValue;

#define BUTTON_NUM 9


void Button_Init(void);
void Button_Dealing(u8 KeyValue);
static void Button_Draw(void* button);
static void Determine_Fun(void);
static void Backtrack_Fun(void);
void View_Fun(void);
static void Up_Fun(void);
static void Down_Fun(void);
void Ready_Fun(void);
void Empty_Fun(void);
static void Backtrack1_Fun(void);

extern BUTTON button[BUTTON_NUM];      //°´¼üÊý×é

#endif
