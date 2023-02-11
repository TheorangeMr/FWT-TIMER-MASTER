/*******************************************
	*	@file ��  bsp_button.c
	* @author��  �޳�
	* @data�� 2023.01.15
	* @version��  v1.0
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
BUTTON button[BUTTON_NUM];


/*
	*��������Button_Init()
	*��  �ܣ�������ʼ������
	*��  ���� ��
	*����ֵ�� ��
	*��  �ߣ� ����
	*��  �ڣ� 2020.10.24
*/
void Button_Init(void)
{
	//��ȷ������
	button[0].start_x = screen_x_length - 50;
	button[0].start_y = screen_y_length - 30;
	button[0].end_x = screen_x_length;
	button[0].end_y = screen_y_length;
	button[0].button_value = 0;
	button[0].ButtonDraw = Button_Draw;
	button[0].ButtonCommand = Determine_Fun;
	//�����ء���
	button[1].start_x = screen_x_length - 50;
	button[1].start_y = screen_y_length - 30;
	button[1].end_x = screen_x_length;
	button[1].end_y = screen_y_length;
	button[1].button_value = 1;
	button[1].ButtonDraw = Button_Draw;
	button[1].ButtonCommand = Backtrack_Fun;
	//���鿴����
	button[2].start_x = screen_x_length - 230;
	button[2].start_y = screen_y_length - 30;
	button[2].end_x = screen_x_length - 180;
	button[2].end_y = screen_y_length;
	button[2].button_value = 2;
	button[2].ButtonDraw = Button_Draw;
	button[2].ButtonCommand = View_Fun;
	//���Ϸ�����
	button[3].start_x = screen_x_length - 140;
	button[3].start_y = screen_y_length - 30;
	button[3].end_x = screen_x_length - 90;
	button[3].end_y = screen_y_length;
	button[3].button_value = 3;
	button[3].ButtonDraw = Button_Draw;
	button[3].ButtonCommand = Up_Fun;
	//���·�����
	button[4].start_x = screen_x_length - 230;
	button[4].start_y = screen_y_length - 30;
	button[4].end_x = screen_x_length - 180;
	button[4].end_y = screen_y_length;
	button[4].button_value = 4;
	button[4].ButtonDraw = Button_Draw;
	button[4].ButtonCommand = Down_Fun;
	//��׼������
	button[5].start_x = screen_x_length - 140;
	button[5].start_y = screen_y_length - 30;
	button[5].end_x = screen_x_length - 90;
	button[5].end_y = screen_y_length;
	button[5].button_value = 5;
	button[5].ButtonDraw = Button_Draw;
	button[5].ButtonCommand = Ready_Fun;
	//����ա���
	button[6].start_x = screen_x_length - 320;
	button[6].start_y = screen_y_length - 30;
	button[6].end_x = screen_x_length - 270;
	button[6].end_y = screen_y_length;
	button[6].button_value = 6;
	button[6].ButtonDraw = Button_Draw;
	button[6].ButtonCommand = Empty_Fun;
}

/*
	*APIname��Button_Dealing()
	*brief��  �������ܴ�����
	*param��  ��
	*retval�� ��
	*author�� �޳�
	*data��   2023.1.15
*/

void Button_Dealing(u8 KeyValue)
{
	if(InterfaceFlag == 0)//������
	{
		switch(KeyValue)
		{
			case 1: button[Down].ButtonCommand(); break;
			case 2: button[Up].ButtonCommand(); break;
			case 3: button[Determine].ButtonCommand(); break;
			default: break;
		}
	}
	else if(1 == InterfaceFlag)//��Ȧ��ʱ����
	{
		switch(KeyValue)
		{
			case 1: button[View].ButtonCommand(); break;
			case 2: button[Ready].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(2 == InterfaceFlag)//���ӻ����ܽ���
	{
		switch(KeyValue)
		{
			case 1: button[Determine].ButtonCommand(); break;
			case 2: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(3 == InterfaceFlag)//��ӻ�������ʾ����
	{
		switch(KeyValue)
		{
			case 1: button[View].ButtonCommand(); break;
			case 2: button[Ready].ButtonCommand(); break;
			case 3: button[Backtrack].ButtonCommand(); break;
			default: break;
		}
	}
	else if(4 == InterfaceFlag)//��Ȧ����������ʾ����
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
	else if(5 == InterfaceFlag)//�������������ʾ����
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
}


/*
	*��������Button_Draw()
	*��  �ܣ�������溯��
	*��  ���� button��BUTTON�ṹ��
	*����ֵ�� ��
	*��  �ߣ� ����
	*��  �ڣ� 2020.10.24
*/
void Button_Draw(void* button)
{
	BUTTON* ptr = (BUTTON*)button;

	char str[6] = { 0 };

	switch ((u8)ptr->button_value)
	{
		case Determine: sprintf(str, "%s", "ȷ��"); break;

		case Backtrack: sprintf(str, "%s", "����"); break;

		case View: sprintf(str, "%s", "�鿴"); break;

		case Up: sprintf(str, "%s", "��"); break;

		case Down: sprintf(str, "%s", "��"); break;

		case Ready: sprintf(str, "%s", "׼��"); break;

		case Empty: sprintf(str, "%s", "���"); break;

		default: break;
	}
	ILI9486_draw_rectangle(ptr->start_x, ptr->start_y, 50, 30, BLUE);
	ILI9486_showstring_Ch(ptr->start_x + 1, ptr->start_y + 3, (u8*)str, GB2312_24X24);
}

/*
	*APIname��Determine_Fun()
	*brief��  ȷ�Ϲ��ܺ���
	*param��  ��
	*retval�� ��
	*author�� �޳�
	*data��   2023.1.15
*/

void Determine_Fun(void)
{
	if (InterfaceFlag == 0)
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
		Empty_Insure();
		Data1_Count = 0;
	}
	else if (7 == InterfaceFlag)
	{
		Empty_Insure();
		Data2_Count = 0;
	}
}

/*
	*APIname��Backtrack_Fun()
	*brief��  ���ع��ܺ���
	*param��  ��
	*retval�� ��
	*author�� �޳�
	*data��   2023.1.15
*/
void Backtrack_Fun(void)
{
	Timer3_Off();
	if(InterfaceFlag <= 2)
	{
		InterfaceFlag = 0;
		xSemaphoreGive(BinarySem3_Handle);
	}
	else if(InterfaceFlag == 3)                                                      
	{
		InterfaceFlag = 2;
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
}

/*
	*��������View_Fun()
	*��  �ܣ��ӹ��ܽ�����ת��������ʾ����
	*��  ������
	*����ֵ�� ��
	*��  �ߣ� ���壬�޳�
	*��  �ڣ� 2020.10.24 �޸ģ�2023.1.15
*/
void View_Fun(void)
{
	if (InterfaceFlag == 1)
	{
		InterfaceFlag = 4;
		Exti_Close();                                      /* �ر��ⲿ�жϣ���ֹ�鿴���ݱ��жϸ��� */
		xSemaphoreGive(BinarySem3_Handle);
	}
	if (InterfaceFlag == 2)
	{
		InterfaceFlag = 5;                              
		Exti_Close();
		xSemaphoreGive(BinarySem3_Handle);
	}
}


void Up_Fun(void)
{
	if(InterfaceFlag == 0)                                          /* ������ */
	{
		SelectFlag--;
		if (SelectFlag < 1) SelectFlag = 2;                                       
		if (SelectFlag == 1)
		{
			ILI9486_clear_screen(70, 180, 30, 30);
			ILI9486_showstring_Ch(70, 140, (u8*)"��", GB2312_24X24);
		}
		else
		{
			ILI9486_clear_screen(70, 220, 30, 30);
			ILI9486_showstring_Ch(70, 180, (u8*)"��", GB2312_24X24);
		}
	}
	else if(4 == InterfaceFlag)                                    /* ��Ȧ�������ݽ��� */
	{
		Page_Up(DataBuffer1, Data1_Count);
	}
	else if(5 == InterfaceFlag)                                    /* ���ٲ������ݽ��� */
	{
		Page_Up(DataBuffer2, Data2_Count);
	}
}


void Down_Fun(void)
{
	if(InterfaceFlag == 0)
	{
		SelectFlag++;
		if (SelectFlag > 2) SelectFlag = 1;
		if (SelectFlag == 1)
		{
			ILI9486_clear_screen(70, 220, 30, 30);
			ILI9486_showstring_Ch(70, 140, (u8*)"��", GB2312_24X24);
		}
		else
		{
			ILI9486_clear_screen(70, 140, 30, 30);
			ILI9486_showstring_Ch(70, 180, (u8*)"��", GB2312_24X24);
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
	*APIname��Ready_Fun()
	*brief��  ׼����ʱ����
	*param��  ��
	*retval�� ��
	*author�� �޳�
	*data��   2023.1.15
*/
//�ڼ�ʱ���ֶ�����׼����ť���Խ�����ʱ�����ݲ����棩�������´β���׼��
void Ready_Fun(void)
{
	char str[20] = { 0 };
	Timer3_Off();
	Lap_stflag = 0;
	sprintf(str, "%s", "׼����ʱ");
	ILI9486_clear_screen(130, 137, 150, 30);
	ILI9486_draw_rectangle(130, 137, 150, 30, BLUE);
	ILI9486_showstring_Ch(141, 140, (u8*)str, GB2312_24X24);
	Exti_Open();
}

void Empty_Fun(void)
{
	Empty_Records();
}

