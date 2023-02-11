#ifndef __ILI9486_TFTLCD_H
#define __ILI9486_TFTLCD_H


#include "GT30L32S4W.h"

//ILI9486�����ź���

//Ƭѡ��
#define      ILI9486_CS_CLK           RCC_APB2Periph_GPIOA
#define      ILI9486_CS_PORT          GPIOA
#define      ILI9486_CS_PIN           GPIO_Pin_1
//����/����ѡ������
#define      ILI9486_DC_CLK           RCC_APB2Periph_GPIOA
#define      ILI9486_DC_PORT          GPIOA
#define      ILI9486_DC_PIN           GPIO_Pin_4
//�����������
#define      ILI9486_BL_CLK           RCC_APB2Periph_GPIOA
#define      ILI9486_BL_PORT          GPIOA
#define      ILI9486_BL_PIN           GPIO_Pin_3
//��λ����
#define      ILI9486_RES_CLK          RCC_APB2Periph_GPIOA
#define      ILI9486_RES_PORT         GPIOA
#define      ILI9486_RES_PIN          GPIO_Pin_0

//Ƭѡ����
#define      ILI9486_CS_H             GPIO_SetBits(ILI9486_CS_PORT, ILI9486_CS_PIN)
//Ƭѡ����
#define      ILI9486_CS_L             GPIO_ResetBits(ILI9486_CS_PORT, ILI9486_CS_PIN)
//����/������������
#define      ILI9486_DC_H             GPIO_SetBits(ILI9486_DC_PORT, ILI9486_DC_PIN)
//����/������������
#define      ILI9486_DC_L             GPIO_ResetBits(ILI9486_DC_PORT, ILI9486_DC_PIN)
//����ʹ��
#define      BLK_ENABLE               GPIO_SetBits(ILI9486_BL_PORT, ILI9486_BL_PIN)
//���ⲻʹ��
#define      BLK_DISABLE              GPIO_ResetBits(ILI9486_BL_PORT, ILI9486_BL_PIN)

#define      RES_CLR()                GPIO_ResetBits(ILI9486_RES_PORT, ILI9486_RES_PIN)//LCD RES
#define      RES_SET()                GPIO_SetBits(ILI9486_RES_PORT, ILI9486_RES_PIN)


/******************************************ILI9486��������*****************************************/
//���������꣨colnum address set��
#define     SET_COLNUM_ADDR_CMD           0x2a
//����ҳ���꣨page address set��
#define     SET_PAGE_ADDR_CMD             0x2b
//������� ��memory write��
#define     MEMORY_WRITE_CMD              0x2c 
//����ɨ��ģʽ��memory access  control��
#define     SET_SCAN_MODE                 0x36
//�����ص����ݣ�memory read��
#define     READ_MEMORY                   0x2e

/*************************************************RGB565��غ�***********************************************/

//RGB888תRGB565
#define      RGB888_TO_RGB565(r,g,b)         (u16)(((r & 0x1f)<<11) | ((g & 0x3f)<<5) | ((b & 0x1f)))

//������ɫ
#define      WHITE		 		                 0xFFFFFF	                 //��ɫ
#define      BLACK                         0x000000	                 //��ɫ 
#define      GREY                          0xC0C0C0	                 //��ɫ 
#define      BLUE                          0x0000FF	                 //��ɫ 
#define      BLUE2                         0xF0FFFF	                 //ǳ��ɫ 
#define      RED                           0xFF0000	                 //��ɫ 
#define      MAGENTA                       0xFF00FF	                 //����ɫ�����ɫ 
#define      GREEN                         0x008000	                 //��ɫ 
#define      CYAN                          0x7FFFD4	                 //����ɫ����ɫ 
#define      YELLOW                        0xFFFF00	                 //��ɫ 


/********************************************Һ����������غ�**************************************/
#define      ILI9486_SCREEN_START_X           0                      //��Ļ��ʼ�������
#define      ILI9486_SCREEN_START_Y           0                      //��Ļ��ʼ��������
#define      ILI9486_SCREEN_LESSWIDTH        320                     //��Ļ��խ����Ŀ��
#define      ILI9486_SCREEN_MOREWIDTH        480                     //��Ļ�Ͽ���Ŀ��


/***************************************ILI9486�����������*************************************/


void ILI9486_tftlcd_gpio_config(void);
void ILI9486_tftlcd_init(void);
u16  ILI9486_pixel_format_read(void);
void ILI9486_write_cmd(u8 cmd);
void ILI9486_write_cmd_data(u8 zdata);
void ILI9486_write_data(u32 zdata);
u16  ILI9486_read_data(void);
void ILI9486_open_window(u16 start_x , u16 start_y , u16 width , u16 height);
void ILI9486_draw_rectangle(u16 satrt_x , u16 start_y , u16 width , u16 height, u32 color);
void ILI9486_draw_circle(u16 center_x , u16 center_y , u16 radius);
void ILI9486_draw_line(u16 start_x , u16 start_y , u16 end_x , u16 end_y);
void ILI9486_display_on(u8 enable_state);
void ILI9486_putpixel(u16 posx , u16 posy);
void ILI9486_showchar_En(u16 pos_x , u16 pos_y , u8 zdata, FONT font);
void ILI9486_showstring_En(u16 pos_x , u16 pos_y , u8 *zdata, FONT font);
void ILI9486_showchar_Ch(u16 pos_x , u16 pos_y , u8 *zdata, FONT font);
void ILI9486_showstring_Ch(u16 pos_x , u16 pos_y , u8 *zdata, FONT font);
void ILI9486_clear_screen(u16 start_x , u16 start_y , u16 width , u16 height);
void ILI9486_set_txtcolor(u32 color);
void ILI9486_set_backcolor(u32 color);
void ILI9486_gram_scan_mode(u8 mode);
u16  ILI9486_read_pixel_data(u16 posx , u16 posy);
void ILI9486_draw_cross(u16 posx , u16 posy);

extern u16 screen_x_length;     
extern u16 screen_y_length;    


#endif

