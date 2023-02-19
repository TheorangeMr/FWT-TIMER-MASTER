#ifndef __BSP_INTERFACE_H
#define __BSP_INTERFACE_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"

#define MAXSIZE 20

#if 0
typedef struct
{
	u16 front;
	u16 rear;
	u16 Data[MAXSIZE];
}Queue;
#endif

void MainMenu(void);
void Lap_Test_Menu(void);
void Connection_Count_Show(void);
void Slaver_Check_Interface(void);
void Multi_slave_function_interface(void);
void Show_AcceleratedRecords(u16* zdata, u16 DataCount);
void Show_LapRecords(u16* zdata, u16 DataCount);
void Page_Down(u16* zdata, u16 DataCount);
void Page_Up(u16* zdata, u16 DataCount);
void Show_Data(u16 zdata, u16 pos_y);
void Empty_Records(void);


extern vu8 InterfaceFlag;                  //界面标志
extern u8 SelectFlag;                     //选择标志
#endif


