/*******************************************
	*	@file ：  bsp_interface.c
	* @author：  罗成
	* @data： 2023.01.15
	* @version：  v1.0
*******************************************/

#include "bsp_interface.h"
#include "bsp_button.h"
#include "ili9486.h"
#include "bsp_exti.h"
#include "bsp_usart.h"
#include "bsp_timer.h"

u16 Page_Start = 0;                    //页首
u16 Data1_Seek = 0;                    //加速数据定位值
u16 Data2_Seek = 0;                    //单圈数据定位值
u8 SelectFlag = 1;                     //选择标志
u8 InterfaceFlag = 0;                 //界面标志

extern uint8_t Connection_count;


/*
	*APIname：MainMenu()
	*brief：  主界面显示函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/
void MainMenu(void)                     
{
	ILI9486_clear_screen(0, 80, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH-40);//清屏

	vTaskDelay(100);
	
	ILI9486_showstring_Ch(88, 80, (u8*)"计时器", GB2312_32X32);
	ILI9486_showstring_En(184, 80, (u8*)"6.0", ASCII_16X32);
	ILI9486_showstring_Ch(100, 140, (u8*)"Ⅰ、单圈测试", GB2312_24X24);
	ILI9486_showstring_Ch(100, 180, (u8*)"Ⅱ、多从机测试", GB2312_24X24);

	if (SelectFlag == 1)
	{
		ILI9486_showstring_Ch(70, 140, (u8*)"◆", GB2312_24X24);
	}
	else if(SelectFlag == 2)
	{
		ILI9486_showstring_Ch(70, 180, (u8*)"◆", GB2312_24X24);
	}
	button[Up].ButtonDraw(&button[Up]);                                        
	button[Down].ButtonDraw(&button[Down]);
	button[Determine].ButtonDraw(&button[Determine]);
}


/*
	*APIname：Lap_Test_Menu()
	*brief：  单圈测试界面显示函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/
void Lap_Test_Menu(void)
{
	ILI9486_clear_screen(0, 80, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH);//清屏
	ILI9486_showstring_Ch(100, 80, (u8*)"单圈测试", GB2312_32X32);
	ILI9486_showstring_Ch(20, 140, (u8*)"状态框：", GB2312_24X24);
	ILI9486_showstring_Ch(141, 140, (u8*)"空闲", GB2312_24X24);   	
	ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);

	ILI9486_draw_rectangle(20, 180, 280, 100, BLUE);
	ILI9486_showstring_Ch(30, 200, (u8*)"本次测试数据：", GB2312_24X24);
	ILI9486_showstring_Ch(30, 240, (u8*)"上次测试数据：", GB2312_24X24);

	button[View].ButtonDraw(&button[View]);
	button[Ready].ButtonDraw(&button[Ready]);
	button[Backtrack].ButtonDraw(&button[Backtrack]);
}

/*
	*函数名：Show_Data()
	*功  能：显示当前测试数据
	*参  数： zdata：测试数据
	*返回值： 无
	*作  者： 王峰
	*日  期： 2020.11.07
*/
void Show_Data(u16 zdata, u16 pos_y)
{
	u16 temp = zdata;
	u8 TestData[20] = { 0 };
	u8 num = 0;

	while (temp)
	{
		TestData[num++] = temp % 10 + 0x30;
		temp /= 10;
	}

	u16 pos_x = 210;
	
	if(zdata < 100)
	{
		ILI9486_showchar_En(pos_x, pos_y, '0', ASCII_12X24);
		pos_x += 12;
	}

	while (num)
	{
		num--;
		if (num == 1)
		{
			ILI9486_showchar_En(pos_x, pos_y, '.', ASCII_12X24);
			pos_x += 12;
		}
		ILI9486_showchar_En(pos_x, pos_y, TestData[num], ASCII_12X24);
		pos_x += 12;
	}
}

/*
	*APIname：Connection_Count_Show()
	*brief：  从机连接数量显示函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/
void Connection_Count_Show(void)                                   
{
	ILI9486_clear_screen(150, 10, 100, 44);

	ILI9486_showstring_Ch(20, 20, (u8*)"串口通讯：", GB2312_24X24);
	if(Connection_count != 0)
	{
		ILI9486_draw_rectangle(150, 10, 50, 44, BLUE);
		ILI9486_showstring_En(160, 20, (u8*)Connection_count, ASCII_12X24);
	}
	else
	{
		ILI9486_draw_rectangle(150, 10, 140, 44, BLUE);
		ILI9486_showstring_Ch(160, 20, (u8*)"无从机连接", GB2312_24X24);
	}
}

/*
	*APIname：Connection_Status_Show()
	*brief：  从机检测界面显示函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/

//
void Slaver_Check_Interface(void)
{
	Connection_count = 0;
	ILI9486_clear_screen(0, 0, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH);
	ILI9486_showstring_Ch(20, 220, (u8*)"串口通讯检测中", GB2312_24X24);
	ILI9486_showstring_En(190, 220, (u8*)".......", ASCII_12X24);
	button[Determine].ButtonDraw(&button[Determine]);
	button[Backtrack1].ButtonDraw(&button[Backtrack1]);
}


/*
	*APIname：Multi_slave_function_interface()
	*brief：  多从机功能界面显示函数
	*param：  无
	*retval： 无
	*author： 罗成
	*data：   2023.1.15
*/

void Multi_slave_function_interface(void)
{
	uint8_t i = 0;
	char str[20] = {0};
	Exti_Close();
	Timer3_Off();
	ILI9486_clear_screen(0, 80, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH);    //清屏	
	
	ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
	ILI9486_showstring_Ch(100, 80, (u8*)"多从机测试", GB2312_32X32);
	ILI9486_showstring_Ch(20, 140, (u8*)"状态框：", GB2312_24X24);
	ILI9486_showstring_Ch(141, 140, (u8*)"空闲", GB2312_24X24);                         //需要测试观察现象
	while(Connection_count--)
	{
		ILI9486_draw_rectangle(20, 180, 280, Connection_count*50, BLUE);
		sprintf(str,"%s：%d","本次测试数据",i+1);
		ILI9486_showstring_Ch(30, (200+i*40), (u8*)str, GB2312_24X24);
		i++;
	}
	button[View].ButtonDraw(&button[View]);
	button[Ready].ButtonDraw(&button[Ready]);
	button[Backtrack].ButtonDraw(&button[Backtrack]);
}

/*
	*函数名：Show_LapRecords()
	*功  能：显示单圈测试记录
	*参  数： zdata: 加速测试数据缓存区
	*返回值： 无
	*作  者： 王峰,罗成
	*日  期： 2021.09.02 修改：2023.1.15
*/
void Show_LapRecords(u16* zdata, u16 DataCount)
{
	u16 temp;
	u8 num = 0;
	u8 bit = 0;
	u16 pos_y = 120; 
	u16 pos_x = 200;
	u8 TestData[6] = { 0 };
	char StrBuf[5] = { 0 };
	
	ILI9486_clear_screen(0, 80, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH);//清屏	
	
	/* 测试数据定位归0 */
	Data2_Seek = 0;
	
	/* 页首归0 */
	Page_Start = 0;
	
	ILI9486_showstring_Ch(110, 80, (u8*)"单圈测试数据", GB2312_24X24);
	
	/* 判断测试数据是否为空 */
	if(DataCount == 0)
	{
		ILI9486_draw_rectangle(60,200,240,50,BLUE);
		ILI9486_showstring_Ch(76, 210, (u8*)"单圈测试数据为空！", GB2312_24X24);
	}
	else
	{
			/* 显示当前8条数据 */
		while(num < 8 && Data2_Seek < DataCount)
		{
			ILI9486_showchar_Ch(20, pos_y, (u8*)"第", GB2312_24X24);
			sprintf(StrBuf, "%-3d", Data2_Seek+1);
			ILI9486_showstring_En(44, pos_y, (u8*)StrBuf, ASCII_12X24);
			ILI9486_showstring_Ch(80, pos_y, (u8*)"组测试数据：", GB2312_24X24);

			temp = zdata[Data2_Seek];
			
			bit = 0;
			while (temp)
			{
				TestData[bit++] = temp % 10 + 0x30;
				temp /= 10;
			}

			pos_x = 210;
			
			if(zdata[Data2_Seek] < 100)
			{
				ILI9486_showchar_En(pos_x, pos_y, '0', ASCII_12X24);
				pos_x += 12;
			}

			/* 显示数据 */
			while (bit)
			{
				bit--;
				if (bit == 1)
				{
					ILI9486_showchar_En(pos_x, pos_y, '.', ASCII_12X24);
					pos_x += 12;
				}
				ILI9486_showchar_En(pos_x, pos_y, TestData[bit], ASCII_12X24);
				pos_x += 12;
			}
			Data2_Seek++;
			num++;
			pos_y += 30;
		}
	}
	button[Up].ButtonDraw(&button[Up]);                  
	button[Down].ButtonDraw(&button[Down]);
	button[Empty].ButtonDraw(&button[Empty]);
	button[Backtrack].ButtonDraw(&button[Backtrack]);
}

/*
	*函数名：Show_AcceleratedRecords()
	*功  能：显示加速测试记录
	*参  数： zdata: 加速测试数据缓存区
	*返回值： 无
	*作  者： 王峰,罗成
	*日  期： 2021.09.02 修改：2023.1.15
*/
void Show_AcceleratedRecords(u16* zdata, u16 DataCount)
{
	ILI9486_clear_screen(0, 80, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH);//清屏

	u16 temp;
	u8 bit = 0;
	u16 pos_y = 120; 
	u16 pos_x = 200;
	u8 TestData[6] = { 0 };
	char StrBuf[5] = { 0 };
	
	/* 测试数据定位归0 */
	Data1_Seek = 0;
	
	/* 页首归0 */
	Page_Start = 0;
	
	ILI9486_showstring_Ch(110, 80, (u8*)"加速测试数据", GB2312_24X24);
	
	/* 判断测试数据是否为空 */
	if(DataCount == 0)
	{
		ILI9486_draw_rectangle(60,200,240,50,BLUE);
		ILI9486_showstring_Ch(76, 210, (u8*)"加速测试数据为空！", GB2312_24X24);
	}
	else
	{
			/* 显示当前8条数据 */
		while(Data1_Seek < 8 && Data1_Seek < DataCount)
		{
			/* 显示提示信息 */
			ILI9486_showchar_Ch(20, pos_y, (u8*)"第", GB2312_24X24);
			sprintf(StrBuf, "%-3d", Data1_Seek+1);
			ILI9486_showstring_En(44, pos_y, (u8*)StrBuf, ASCII_12X24);
			ILI9486_showstring_Ch(80, pos_y, (u8*)"组测试数据：", GB2312_24X24);

			temp = zdata[Data1_Seek];
			
			bit = 0;
			while (temp)
			{
				TestData[bit++] = temp % 10 + 0x30;
				temp /= 10;
			}

			pos_x = 210;
			
			if(zdata[Data1_Seek] < 100)
			{
				ILI9486_showchar_En(pos_x, pos_y, '0', ASCII_12X24);
				pos_x += 12;
			}
		
			/* 显示数据 */
			while (bit)
			{
				bit--;
				if (bit == 1)
				{
					ILI9486_showchar_En(pos_x, pos_y, '.', ASCII_12X24);
					pos_x += 12;
				}
				ILI9486_showchar_En(pos_x, pos_y, TestData[bit], ASCII_12X24);
				pos_x += 12;
			}
			Data1_Seek++;
			pos_y += 30;
		}
	}
	button[Up].ButtonDraw(&button[Up]);
	button[Down].ButtonDraw(&button[Down]);
	button[Empty].ButtonDraw(&button[Empty]);
	button[Backtrack].ButtonDraw(&button[Backtrack]);
}



/*
	*函数名：Page_Down()
	*功  能：下一页
	*参  数： 
	*返回值： 无
	*作  者： 王峰,罗成
	*日  期： 2021.09.01  修改：2023.1.15
*/
void Page_Down(u16* zdata, u16 DataCount)
{
	/* 如果是最后一页 */
	if(Data1_Seek >= DataCount || Data2_Seek >= DataCount)
	{
		return;
	}
	
	ILI9486_clear_screen(0, 120, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH - 160);//清屏
	
	u16 temp;
	u8 num = 0;
	u8 bit = 0;
	u16 pos_y = 120; 
	u16 pos_x = 200;
	u8 TestData[6] = { 0 };
	char StrBuf[5] = { 0 };
	
	/* 如果是加速数据显示界面 */
	if(5 == InterfaceFlag)
	{
		Page_Start = Data1_Seek;
		
		/* 显示当前8条数据 */
		while(num < 8 && Data1_Seek < DataCount)
		{
			ILI9486_showchar_Ch(20, pos_y, (u8*)"第", GB2312_24X24);
			sprintf(StrBuf, "%-3d", Data1_Seek+1);
			ILI9486_showstring_En(44, pos_y, (u8*)StrBuf, ASCII_12X24);
			ILI9486_showstring_Ch(80, pos_y, (u8*)"组测试数据：", GB2312_24X24);

			temp = zdata[Data1_Seek];
			
			bit = 0;
			while (temp)
			{
				TestData[bit++] = temp % 10 + 0x30;
				temp /= 10;
			}

			pos_x = 210;
			
			if(zdata[Data1_Seek] < 100)
			{
				ILI9486_showchar_En(pos_x, pos_y, '0', ASCII_12X24);
				pos_x += 12;
			}

			while (bit)
			{
				bit--;
				if (bit == 1)
				{
					ILI9486_showchar_En(pos_x, pos_y, '.', ASCII_12X24);
					pos_x += 12;
				}
				ILI9486_showchar_En(pos_x, pos_y, TestData[bit], ASCII_12X24);
				pos_x += 12;
			}
			Data1_Seek++;
			num++;
			pos_y += 30;
		}
	}
	/* 如果是单圈数据显示界面 */
	else if(4 == InterfaceFlag)
	{
		Page_Start = Data2_Seek;
		
		/* 显示当前8条数据 */
		while(num < 8 && Data2_Seek < DataCount)
		{
			ILI9486_showchar_Ch(20, pos_y, (u8*)"第", GB2312_24X24);
			sprintf(StrBuf, "%-3d", Data2_Seek+1);
			ILI9486_showstring_En(44, pos_y, (u8*)StrBuf, ASCII_12X24);
			ILI9486_showstring_Ch(80, pos_y, (u8*)"组测试数据：", GB2312_24X24);

			temp = zdata[Data2_Seek];
			
			bit = 0;
			while (temp)
			{
				TestData[bit++] = temp % 10 + 0x30;
				temp /= 10;
			}

			pos_x = 210;
			
			if(zdata[Data2_Seek] < 100)
			{
				ILI9486_showchar_En(pos_x, pos_y, '0', ASCII_12X24);
				pos_x += 12;
			}

			while (bit)
			{
				bit--;
				if (bit == 1)
				{
					ILI9486_showchar_En(pos_x, pos_y, '.', ASCII_12X24);
					pos_x += 12;
				}
				ILI9486_showchar_En(pos_x, pos_y, TestData[bit], ASCII_12X24);
				pos_x += 12;
			}
			Data2_Seek++;
			num++;
			pos_y += 30;
		}
	}
}

/*
	*函数名：Page_Up()
	*功  能：上一页
	*参  数： 
	*返回值： 无
	*作  者： 王峰，罗成
	*日  期： 2021.09.01 ，修改：2023.1.15
*/
void Page_Up(u16* zdata, u16 DataCount)
{
	u16 temp;
	u8 num = 0;
	u8 bit = 0;
	u16 pos_y = 120;
	u16 pos_x = 200;
	u8 TestData[6] = { 0 };
	char StrBuf[5] = { 0 };	
	/* 如果是第一页直接返回 */
	if(Page_Start == 0)
	{
		return;
	}
	ILI9486_clear_screen(0, 120, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH - 160);//清屏
	/* 如果是加速数据显示界面 */
	if(5 == InterfaceFlag)
	{
		if(Page_Start > 8)
		{
			Page_Start -= 9;
		}
		else 
		{
			Page_Start = 0;
		}
		
		Data1_Seek = Page_Start;
		
		/* 显示当前8条数据 */
		while(num < 8)
		{
			ILI9486_showchar_Ch(20, pos_y, (u8*)"第", GB2312_24X24);
			sprintf(StrBuf, "%-3d", Data1_Seek+1);
			ILI9486_showstring_En(44, pos_y, (u8*)StrBuf, ASCII_12X24);
			ILI9486_showstring_Ch(80, pos_y, (u8*)"组测试数据：", GB2312_24X24);

			temp = zdata[Data1_Seek];
			
			bit = 0;
			while (temp)
			{
				TestData[bit++] = temp % 10 + 0x30;
				temp /= 10;
			}

			pos_x = 210;
			
			if(zdata[Data1_Seek] < 100)
			{
				ILI9486_showchar_En(pos_x, pos_y, '0', ASCII_12X24);
				pos_x += 12;
			}

			while (bit)
			{
				bit--;
				if (bit == 1)
				{
					ILI9486_showchar_En(pos_x, pos_y, '.', ASCII_12X24);
					pos_x += 12;
				}
				ILI9486_showchar_En(pos_x, pos_y, TestData[bit], ASCII_12X24);
				pos_x += 12;
			}
			Data1_Seek++;
			num++;
			pos_y += 30;
		}
	}
	/* 如果是在单圈数据显示界面 */
	else if(4 == InterfaceFlag)
	{
		if(Page_Start > 8)
		{
			Page_Start -= 9;
		}
		else 
		{
			Page_Start = 0;
		}
		
		Data2_Seek = Page_Start;
		
		/* 显示当前8条数据 */
		while(num < 8)
		{
			ILI9486_showchar_Ch(20, pos_y, (u8*)"第", GB2312_24X24);
			sprintf(StrBuf, "%-3d", Data2_Seek+1);
			ILI9486_showstring_En(44, pos_y, (u8*)StrBuf, ASCII_12X24);
			ILI9486_showstring_Ch(80, pos_y, (u8*)"组测试数据：", GB2312_24X24);

			temp = zdata[Data2_Seek];
			
			bit = 0;
			while (temp)
			{
				TestData[bit++] = temp % 10 + 0x30;
				temp /= 10;
			}

			pos_x = 210;
			
			if(zdata[Data2_Seek] < 100)
			{
				ILI9486_showchar_En(pos_x, pos_y, '0', ASCII_12X24);
				pos_x += 12;
			}

			while (bit)
			{
				bit--;
				if (bit == 1)
				{
					ILI9486_showchar_En(pos_x, pos_y, '.', ASCII_12X24);
					pos_x += 12;
				}
				ILI9486_showchar_En(pos_x, pos_y, TestData[bit], ASCII_12X24);
				pos_x += 12;
			}
			Data2_Seek++;
			num++;
			pos_y += 30;
		}
	}
}

/*
	*函数名：Empty_Records()
	*功  能：清空测试记录
	*参  数： 无
	*返回值： 无
	*作  者： 王峰
	*日  期： 2020.10.24
*/
void Empty_Records()
{
	ILI9486_clear_screen(0, 80, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH - 40);//清屏
	ILI9486_draw_rectangle(60, 200, 200, 80, BLUE);
	if(InterfaceFlag == 6)
	{
		ILI9486_showstring_Ch(88, 210, (u8*)"确定清空单圈", GB2312_24X24);
		ILI9486_showstring_Ch(98, 250, (u8*)"测试数据？", GB2312_24X24);
	}
	else if(InterfaceFlag == 7)
	{
		ILI9486_showstring_Ch(88, 210, (u8*)"确定清空加速", GB2312_24X24);
		ILI9486_showstring_Ch(98, 250, (u8*)"测试数据？", GB2312_24X24);
	}
	
	button[Determine].start_x = button[Up].start_x;
	button[Determine].start_y = button[Up].start_y;
	button[Determine].end_x = button[Up].end_x;
	button[Determine].end_y = button[Up].end_y;
	
	button[Determine].ButtonDraw(&button[Determine]);
	button[Backtrack].ButtonDraw(&button[Backtrack]);
	
	button[Determine].start_x = button[Backtrack].start_x;
	button[Determine].start_y = button[Backtrack].start_y;
	button[Determine].end_x = button[Backtrack].end_x;
	button[Determine].end_y = button[Backtrack].end_y;
}
