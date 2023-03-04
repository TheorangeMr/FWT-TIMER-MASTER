/*******************************************
	*	@file ：  bsp_button.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v1.0
*******************************************/

#include "bsp_button.h"
#include "ili9486.h"
#include "bsp_timer.h"
#include "bsp_exti.h"
#include "bsp_interface.h"

extern uint8_t Lap_stflag;
extern uint8_t Connection_count;
extern SemaphoreHandle_t  BinarySem3_Handle;
extern uint16_t Data1_Count;
extern uint16_t Data2_Count; 
extern uint16_t DataBuffer1[1024];
extern uint16_t DataBuffer2[1024];
extern uint16_t Showhigh;

BUTTON button[BUTTON_NUM];


/*
	*函数名：Button_Init()
	*功  能：按键初始化函数
	*参  数： 无
	*返回值： 无
	*作  者： 王峰，罗成
	*日  期： 2020.10.24，2023,02.15
*/
void Button_Init(void)
{
	//“确定”键
	button[0].start_x = screen_x_length - 50;
	button[0].start_y = screen_y_length - 30;
	button[0].end_x = screen_x_length;
	button[0].end_y = screen_y_length;
	button[0].button_value = 0;
	button[0].ButtonDraw = Button_Draw;
	button[0].ButtonCommand = Determine_Fun;
	//“返回”键
	button[1].start_x = screen_x_length - 50;
	button[1].start_y = screen_y_length - 30;
	button[1].end_x = screen_x_length;
	button[1].end_y = screen_y_length;
	button[1].button_value = 1;
	button[1].ButtonDraw = Button_Draw;
	button[1].ButtonCommand = Backtrack_Fun;
	//“查看”键
	button[2].start_x = screen_x_length - 230;
	button[2].start_y = screen_y_length - 30;
	button[2].end_x = screen_x_length - 180;
	button[2].end_y = screen_y_length;
	button[2].button_value = 2;
	button[2].ButtonDraw = Button_Draw;
	button[2].ButtonCommand = View_Fun;
	//“上翻”键
	button[3].start_x = screen_x_length - 140;
	button[3].start_y = screen_y_length - 30;
	button[3].end_x = screen_x_length - 90;
	button[3].end_y = screen_y_length;
	button[3].button_value = 3;
	button[3].ButtonDraw = Button_Draw;
	button[3].ButtonCommand = Up_Fun;
	//“下翻”键
	button[4].start_x = screen_x_length - 230;
	button[4].start_y = screen_y_length - 30;
	button[4].end_x = screen_x_length - 180;
	button[4].end_y = screen_y_length;
	button[4].button_value = 4;
	button[4].ButtonDraw = Button_Draw;
	button[4].ButtonCommand = Down_Fun;
	//“准备”键
	button[5].start_x = screen_x_length - 140;
	button[5].start_y = screen_y_length - 30;
	button[5].end_x = screen_x_length - 90;
	button[5].end_y = screen_y_length;
	button[5].button_value = 5;
	button[5].ButtonDraw = Button_Draw;
	button[5].ButtonCommand = Ready_Fun;
	//“清空”键
	button[6].start_x = screen_x_length - 320;
	button[6].start_y = screen_y_length - 30;
	button[6].end_x = screen_x_length - 270;
	button[6].end_y = screen_y_length;
	button[6].button_value = 6;
	button[6].ButtonDraw = Button_Draw;
	button[6].ButtonCommand = Empty_Fun;
		//“返回”键(此返回用于检测从机数量)
	button[7].start_x = screen_x_length - 320;
	button[7].start_y = screen_y_length - 30;
	button[7].end_x = screen_x_length - 270;
	button[7].end_y = screen_y_length;
	button[7].button_value = 7;
	button[7].ButtonDraw = Button_Draw;
	button[7].ButtonCommand = Backtrack1_Fun;
}

/*
	*APIname：Button_Dealing()
	*brief：  按键功能处理函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/

void Button_Dealing(u8 KeyValue)
{
	if(InterfaceFlag == 0)//主界面
	{
		switch(KeyValue)
		{
			case 1: button[Down].ButtonCommand(); break;
			case 2: button[Up].ButtonCommand(); break;
			case 3: button[Determine].ButtonCommand(); break;
			default: break;
		}
	}
	else if(1 == InterfaceFlag)//单圈计时界面
	{
		switch(KeyValue)
		{
			case 1: button[View].ButtonCommand(); break;
			case 2: button[Ready].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(2 == InterfaceFlag)//检测从机功能界面
	{
		switch(KeyValue)
		{
			case 0: button[Backtrack1].ButtonCommand(); break;
			case 3: button[Determine].ButtonCommand(); break;
			default: break;
		}
	}
	else if(3 == InterfaceFlag)//多从机测试显示界面
	{
		switch(KeyValue)
		{
			case 1: button[View].ButtonCommand(); break;
			case 2: button[Ready].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(4 == InterfaceFlag)//单圈测试数据显示界面
	{
		switch(KeyValue)
		{
			case 0: button[Empty].ButtonCommand();break;
			case 1: button[Down].ButtonCommand();break;
			case 2: button[Up].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(5 == InterfaceFlag)//多机测试数据显示界面
	{
		switch(KeyValue)
		{
			case 0: button[Empty].ButtonCommand();break;
			case 1: button[Down].ButtonCommand();break;
			case 2: button[Up].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(6 == InterfaceFlag)
	{
		switch(KeyValue)
		{
			case 2: button[Determine].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(7 == InterfaceFlag)
	{
		switch(KeyValue)
		{
			case 2: button[Determine].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
}


/*
	*函数名：Button_Draw()
	*功  能：按键描绘函数
	*参  数： button：BUTTON结构体
	*返回值： 无
	*作  者： 王峰
	*日  期： 2020.10.24
*/
static void Button_Draw(void* button)
{
	BUTTON* ptr = (BUTTON*)button;

	char str[6] = { 0 };

	switch ((u8)ptr->button_value)
	{
		case Determine: sprintf(str, "%s", "确定"); break;

		case Backtrack: sprintf(str, "%s", "返回"); break;

		case View: sprintf(str, "%s", "查看"); break;

		case Up: sprintf(str, "%s", "↑"); break;

		case Down: sprintf(str, "%s", "↓"); break;

		case Ready: sprintf(str, "%s", "准备"); break;

		case Empty: sprintf(str, "%s", "清空"); break;
		
		case Backtrack1: sprintf(str, "%s", "返回"); break;

		default: break;
	}
	ILI9486_draw_rectangle(ptr->start_x, ptr->start_y, 50, 30, BLUE);
	ILI9486_showstring_Ch(ptr->start_x + 1, ptr->start_y + 3, (u8*)str, GB2312_24X24);
}

/*
	*APIname：Determine_Fun()
	*brief：  确认功能函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/

static void Determine_Fun(void)
{
	if (0 == InterfaceFlag)
	{
		if (SelectFlag == 1)
		{
			InterfaceFlag = 1;
			xSemaphoreGive(BinarySem3_Handle);
		}
		else if(SelectFlag == 2)
		{
			InterfaceFlag = 2;
			xSemaphoreGive(BinarySem3_Handle);
		}
		else if(SelectFlag == 3)
		{
//			ILI9486_clear_screen(200, 220, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH);
//			ILI9486_showstring_Ch(100, 220, (u8*)"开", GB2312_24X24);
//			InterfaceFlag = 2;
//			xSemaphoreGive(BinarySem3_Handle);
		}
	}
	else if(2 == InterfaceFlag)
	{
		if(Connection_count >= 1){
			InterfaceFlag = 3;
			xSemaphoreGive(BinarySem3_Handle);
		}
		else{
			InterfaceFlag = 0;
			xSemaphoreGive(BinarySem3_Handle);
		}
	}
	else if (6 == InterfaceFlag)                                                       
	{
		if(Data1_Count > 0)
		{
			while(Data1_Count--){
				DataBuffer1[Data1_Count]  = 0;
			}
		}
		InterfaceFlag = 4;
		Data1_Count = 0;
		xSemaphoreGive(BinarySem3_Handle);		
	}
	else if (7 == InterfaceFlag)
	{
		if(Data2_Count > 0)
		{
			while(Data2_Count--){
				DataBuffer2[Data2_Count] = 0;
			}
		}
		InterfaceFlag = 5;
		Data2_Count = 0;
		xSemaphoreGive(BinarySem3_Handle);
	}
}


/*
	*APIname：Backtrack_Fun()
	*brief：  返回功能函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/
static void Backtrack_Fun(void)
{
	Timer3_Off();
	if(InterfaceFlag <= 2)
	{
		InterfaceFlag = 0;
		Exti_Close();
		xSemaphoreGive(BinarySem3_Handle);
	}
	else if(InterfaceFlag == 3)                                                      
	{
		InterfaceFlag = 2;
		Showhigh = 150;
		xSemaphoreGive(BinarySem3_Handle);
	}
	else if(InterfaceFlag == 4)
	{
		InterfaceFlag = 1;
		xSemaphoreGive(BinarySem3_Handle);
	}
	else if(InterfaceFlag == 5)
	{
		InterfaceFlag = 3;
		xSemaphoreGive(BinarySem3_Handle);
	}
	else if(InterfaceFlag == 6)
	{
		InterfaceFlag = 4;
		xSemaphoreGive(BinarySem3_Handle);
	}
	else if(InterfaceFlag == 7)
	{
		InterfaceFlag = 5;
		xSemaphoreGive(BinarySem3_Handle);
	}
}

/*
	*APIname：Backtrack1_Fun()
	*brief：  返回功能函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/
static void Backtrack1_Fun(void)
{
	InterfaceFlag = 0;
	xSemaphoreGive(BinarySem3_Handle);
}


/*
	*函数名：View_Fun()
	*功  能：从功能界面跳转到数据显示界面
	*参  数：无
	*返回值： 无
	*作  者： 王峰，罗成
	*日  期： 2020.10.24 修改：2023.1.15
*/
void View_Fun(void)
{
	if (InterfaceFlag == 1)
	{
		InterfaceFlag = 4;
		Exti_Close();                                      /* 关闭外部中断，防止查看数据被中断干扰 */
		xSemaphoreGive(BinarySem3_Handle);
	}
	if (InterfaceFlag == 3)
	{
		InterfaceFlag = 5;                              
		Exti_Close();
		xSemaphoreGive(BinarySem3_Handle);
	}
}


static void Up_Fun(void)
{
	if(InterfaceFlag == 0)                                          /* 主界面 */
	{
		SelectFlag--;
		if (SelectFlag < 1) SelectFlag = 3;                                       
		if (SelectFlag == 1)
		{
			ILI9486_clear_screen(70, 180, 30, 30);
			ILI9486_showstring_Ch(70, 140, (u8*)"◆", GB2312_24X24);
		}
		else if(SelectFlag == 2)
		{
			ILI9486_clear_screen(70, 220, 30, 30);
			ILI9486_showstring_Ch(70, 180, (u8*)"◆", GB2312_24X24);
		}
		else                                                                       
		{
			ILI9486_clear_screen(70, 140, 30, 30);
			ILI9486_showstring_Ch(70, 220, (u8*)"◆", GB2312_24X24);			
		}
	}
	else if(4 == InterfaceFlag)                                    /* 单圈测试数据界面 */
	{
		Page_Up(DataBuffer1, Data1_Count);
	}
	else if(5 == InterfaceFlag)                                    /* 加速测试数据界面 */
	{
		Page_Up(DataBuffer2, Data2_Count);
	}
}


static void Down_Fun(void)
{
	if(InterfaceFlag == 0)
	{
		SelectFlag++;
		if (SelectFlag > 3) SelectFlag = 1;
		
		if (SelectFlag == 1)
		{
			ILI9486_clear_screen(70, 220, 30, 30);
			ILI9486_showstring_Ch(70, 140, (u8*)"◆", GB2312_24X24);
		}
		else if(SelectFlag == 2)
		{
			ILI9486_clear_screen(70, 140, 30, 30);
			ILI9486_showstring_Ch(70, 180, (u8*)"◆", GB2312_24X24);
		}
		else
		{
			ILI9486_clear_screen(70, 180, 30, 30);
			ILI9486_showstring_Ch(70, 220, (u8*)"◆", GB2312_24X24);
		}		
	}
	else if(4 == InterfaceFlag)                                                       
	{
		Page_Down(DataBuffer1, Data1_Count);
	}
	else if(5 == InterfaceFlag)                                                   
	{
		Page_Down(DataBuffer2, Data2_Count);
	}
}

/*
	*APIname：Ready_Fun()
	*brief：  准备计时函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/
//在计时中手动按下准备按钮可以结束计时（数据不保存），进入下次测试准备
void Ready_Fun(void)
{
	char str[20] = { 0 };
	Timer3_Off();
	Lap_stflag = 0;
	sprintf(str, "%s", "准备计时");
	ILI9486_clear_screen(130, 137, 150, 30);
	ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
	ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
	Exti_Open();
}

void Empty_Fun(void)
{
	InterfaceFlag += 2;
	Empty_Records();
}

