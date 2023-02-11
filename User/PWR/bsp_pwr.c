/*******************************************
	*�ļ��� ��  bsp_pwr.c
	*��   �ߣ�  �޳�
	*�޸�ʱ�䣺 2022.06.10
	*��   ����  v1.7
*******************************************/

#include "bsp_pwr.h"
#include "interface.h"
#include "timer_interrupt.h"
#include "Exti.h"
#include "ili9486.h"

extern vu8 Connection_Flag1;            //����ͨѶ���ӱ�־
extern vu8 Connection_Flag2;
extern vu8 Show_flag;
extern vu16 TestTimes;                //����ʱ��
extern u8 MenuFlag;                   //�����־
extern uint8_t pwr_flag;
extern uint16_t time2;

/*˯��ģʽ*/
void Sleep_mode(void)
{
	Timer3_Off();
	Flag_DeInit();
	__WFI();
}
/*ֹͣģʽ*/

void Stop_mode(void)
{
	Show_flag = 1;
	Exti_Open();
	Timer3_Off();
	Flag_DeInit();
//	DBGMCU_Config(DBGMCU_IWDG_STOP,ENABLE);      /*��Ч*/
	
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

void SYSCLKConfig_STOP(void)
{
	  SystemInit();
}

void Stop_interface(void)
{
	TestTimes = 0;
	pwr_flag = 0;
	ILI9486_clear_screen(0, 80, ILI9486_SCREEN_LESSWIDTH, ILI9486_SCREEN_MOREWIDTH);//����
	ILI9486_showstring_Ch(20, 220, (u8*)"˯�ߣ��жϻ��ѣ�", GB2312_24X24);
	Stop_mode();
	SYSCLKConfig_STOP();
	MainMenu();
	MenuFlag = 0;
	Show_flag = 0;
	Connection_Flag1 = 1;            //����ͨѶ���ӱ�־
	Connection_Flag2 = 1;
}

