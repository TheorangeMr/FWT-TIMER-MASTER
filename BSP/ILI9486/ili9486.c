/*******************************************
	*�ļ��� ��  Exti.c
	*��   �ߣ�  ����
	*�޸�ʱ�䣺 2020.10.29
	*��   ����  v1.0
*******************************************/


#include "ili9486.h"
#include "bsp_delay.h"
#include "bsp_spi.h"



u16 screen_x_length = ILI9486_SCREEN_LESSWIDTH;          //Ĭ������x���򳤶ȸ���
u16 screen_y_length = ILI9486_SCREEN_MOREWIDTH;          //Ĭ������Y���򳤶ȸ���
static u32 current_txt_color = BLACK;                    //Ĭ�������ı���ɫΪ��ɫ��ǰ��ɫ��
static u32 current_back_color = WHITE;                   //Ĭ�����ñ�����ɫΪ��ɫ������ɫ��

//ILI9486�������ú���
//2020.7.14
//���ߣ�����
void ILI9486_tftlcd_gpio_config()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//ʹ��ILI9486 FSMC������ŵ�ʱ��
	RCC_APB2PeriphClockCmd(ILI9486_CS_CLK | ILI9486_DC_CLK | ILI9486_BL_CLK | ILI9486_RES_CLK, ENABLE);
	
	//���ñ���������Ź���ģʽ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ILI9486_BL_PIN;
	GPIO_Init(ILI9486_BL_PORT , &GPIO_InitStructure);
	
	//Ƭѡ����
	GPIO_InitStructure.GPIO_Pin = ILI9486_CS_PIN;
	GPIO_Init(ILI9486_CS_PORT , &GPIO_InitStructure);
	
	//����/�����������
	GPIO_InitStructure.GPIO_Pin = ILI9486_DC_PIN;
	GPIO_Init(ILI9486_DC_PORT , &GPIO_InitStructure);
	
	//��λ����
	GPIO_InitStructure.GPIO_Pin = ILI9486_RES_PIN;
	GPIO_Init(ILI9486_RES_PORT , &GPIO_InitStructure);
	
	GPIO_SetBits(ILI9486_BL_PORT, ILI9486_BL_PIN);
	
	RES_SET();
	delay_xms(50);
	RES_CLR();
	delay_xms(50);
	RES_SET();
}


//ILI9486д�����
//2020.10.21
//���ߣ�����
void ILI9486_write_cmd(u8 cmd)
{
	ILI9486_CS_L;
	ILI9486_DC_L;
	
	SPI_SendByte(cmd);
	
	ILI9486_CS_H;
}


//ILI9486д��������
//2020.10.21
//���ߣ�����
void ILI9486_write_cmd_data(u8 zdata)
{
	ILI9486_CS_L;
	ILI9486_DC_H;
	SPI_SendByte(zdata);
	
	ILI9486_CS_H;
}

//ILI9486д���ݺ���
//2020.10.21
//���ߣ�����
void ILI9486_write_data(u32 zdata)
{
	ILI9486_CS_L;
	ILI9486_DC_H;
	
	SPI_SendByte(zdata>>16);
	SPI_SendByte(zdata>>8);
	SPI_SendByte(zdata);
	
	ILI9486_CS_H;
}



//��ȡILI9486����
//2020.10.21
//���ߣ�����
u16  ILI9486_read_data(void)
{
	u16 temp;
	
	ILI9486_CS_L;
	
	temp += SPI_SendByte(Dummy);
	temp <<= 8;
	temp += SPI_SendByte(Dummy);
	
	ILI9486_CS_H;
	
	return temp;
}


////Һ����ʼ������
////2022.5.2
////���ߣ��޳�
void ILI9486_tftlcd_init(void)
{
	ILI9486_tftlcd_gpio_config();
	
	/* Sleep Out (11h)  */
	ILI9486_write_cmd(0x11); //Exit Sleep
	delay_xms(60);
	
	/* Enable 3G (F2h) */
	ILI9486_write_cmd(0XF2);
	ILI9486_write_cmd_data(0x18);
	ILI9486_write_cmd_data(0xA3);
	ILI9486_write_cmd_data(0x12);
	ILI9486_write_cmd_data(0x02);
	ILI9486_write_cmd_data(0XB2);
	ILI9486_write_cmd_data(0x12);
	ILI9486_write_cmd_data(0xFF);
	ILI9486_write_cmd_data(0x10);
	ILI9486_write_cmd_data(0x00);
	ILI9486_write_cmd_data(0XF8);
	ILI9486_write_cmd_data(0x21);
	ILI9486_write_cmd_data(0x04);
	
	/* Normal Display Mode ON  */
	ILI9486_write_cmd(0X13);
	
	/* memory access control set */
	ILI9486_write_cmd( 0x36 ); 	
	ILI9486_write_cmd_data( 0x08 );    /*����  ���½ǵ� (���)�����Ͻ� (�յ�)ɨ�跽ʽ*/
	
	/* Display Inversion Control (B4h) */
	ILI9486_write_cmd(0xB4);
	ILI9486_write_cmd_data(0x02);
	
	/* Display Function Control (B6h) */
	ILI9486_write_cmd(0xB6);
	ILI9486_write_cmd_data(0x02);
	ILI9486_write_cmd_data(0x22);
	
	/* Power Control 2 (C1h) */
	ILI9486_write_cmd(0xC1);
	ILI9486_write_cmd_data(0x41);
	
	/* VCOM Control (C5h) */
	ILI9486_write_cmd(0xC5);
	ILI9486_write_cmd_data(0x00);
	ILI9486_write_cmd_data(0x18);


	/*  Pixel Format Set (3Ah)  */
	ILI9486_write_cmd(0x3a);
	ILI9486_write_cmd_data(0x66);
	delay_xms(50);


	/* PGAMCTRL(Positive Gamma Control) (E0h) */
	ILI9486_write_cmd(0xE0);
	ILI9486_write_cmd_data(0x0F);
	ILI9486_write_cmd_data(0x1F);
	ILI9486_write_cmd_data(0x1C);
	ILI9486_write_cmd_data(0x0C);
	ILI9486_write_cmd_data(0x0F);
	ILI9486_write_cmd_data(0x08);
	ILI9486_write_cmd_data(0x48);
	ILI9486_write_cmd_data(0x98);
	ILI9486_write_cmd_data(0x37);
	ILI9486_write_cmd_data(0x0A);
	ILI9486_write_cmd_data(0x13);
	ILI9486_write_cmd_data(0x04);
	ILI9486_write_cmd_data(0x11);
	ILI9486_write_cmd_data(0x0D);
	ILI9486_write_cmd_data(0x00);
	
	/* NGAMCTRL (Negative Gamma Correction) (E1h) */
	ILI9486_write_cmd(0xE1);
	ILI9486_write_cmd_data(0x0F);
	ILI9486_write_cmd_data(0x32);
	ILI9486_write_cmd_data(0x2E);
	ILI9486_write_cmd_data(0x0B);
	ILI9486_write_cmd_data(0x0D);
	ILI9486_write_cmd_data(0x05);
	ILI9486_write_cmd_data(0x47);
	ILI9486_write_cmd_data(0x75);
	ILI9486_write_cmd_data(0x37);
	ILI9486_write_cmd_data(0x06);
	ILI9486_write_cmd_data(0x10);
	ILI9486_write_cmd_data(0x03);
	ILI9486_write_cmd_data(0x24);
	ILI9486_write_cmd_data(0x20);
	ILI9486_write_cmd_data(0x00);
	
	ILI9486_write_cmd(0x11);
	delay_xms(120);


	/* Display ON (29h) */
	ILI9486_write_cmd(0x29);
	/* Memory Write (2Ch) */
	ILI9486_write_cmd(0x2C);
	
	//ILI9486_display_on(ENABLE);
	ILI9486_clear_screen(0,0,screen_x_length,screen_y_length);
}


//ILI9486 ���ص��ʽ��ȡ����
//2020.7.14
//���ߣ�����
u16 ILI9486_pixel_format_read(void)
{
	ILI9486_write_cmd(0x0c);                      //д��Read Display Pixel Format����
	//ILI9486_read_data();                          //���صĵ�һ������Ϊ��Ч��
	return ILI9486_read_data();
}


//ILI9486��������
//2020.7.14
//���ߣ� ����
//������ ����ʼ����start_x �� ����ʼ����start_y , �߶�height �� ���width
 void ILI9486_open_window(u16 start_x , u16 start_y , u16 width , u16 height)
{
	u16 end_x = 0 , end_y = 0 ;
	end_x = start_x + width;
	end_y = start_y + height;
	
	//��������ʼ,��ֹ����
	ILI9486_write_cmd(SET_COLNUM_ADDR_CMD);
	ILI9486_write_cmd_data((start_x & 0xff00) >> 8);
	ILI9486_write_cmd_data(start_x & 0x00ff);
	ILI9486_write_cmd_data((end_x & 0xff00) >> 8);
	ILI9486_write_cmd_data(end_x & 0x00ff);
	
	//��������ʼ����ֹ����
	ILI9486_write_cmd(SET_PAGE_ADDR_CMD);
	ILI9486_write_cmd_data((start_y & 0xff00) >> 8);
	ILI9486_write_cmd_data(start_y &0x00ff);
	ILI9486_write_cmd_data((end_y & 0xff00) >> 8);
	ILI9486_write_cmd_data(end_y & 0x00ff);
	
}


//ILI9486�����κ���
//2020.7.14
//���ߣ�����
//��������ʼ������start_x,start_y,���width,�߶�height
void ILI9486_draw_rectangle(u16 start_x , u16 start_y , u16 width , u16 height, u32 color)
{
    /*
	//��ʵ�ľ���
    u16 i = 0;
	ILI9486_open_window(start_x , start_y , width , height);
	
	//�������ص������ɫ
	ILI9486_write_cmd(MEMORY_WRITE_CMD);
	for(i = 0 ; i < (width * height) ; i++){
		ILI9486_write_data(color);
	}
    */
	
	//�����ľ���
	ILI9486_draw_line(start_x , start_y , start_x + width , start_y);
	ILI9486_draw_line(start_x + width , start_y , start_x + width , start_y + height);
	ILI9486_draw_line(start_x + width , start_y + height , start_x , start_y + height);
	ILI9486_draw_line(start_x , start_y + height , start_x , start_y);
	
}


//ILI9486��Բ����
//2020.7.15
//���ߣ�����
//������Բ�����꣬�뾶
void ILI9486_draw_circle(u16 center_x , u16 center_y , u16 radius)
{
	/*
	u16 i = 0 , j = 0 ;
	u16 tempx = 0 , tempy = 0;
	u16 start_x = 0 , start_y = 0 , height  = 0 , width = 0 ;
    start_x = center_x - radius ;
	start_y = center_y - radius ;
	height = 2 * radius;
	width = 2 * radius;
	*/
	
	/*�˷��������Ͽ��ԣ�����������Ϊ������ڸ��ӣ�ʱ���������ɨ�������ѭ����������Ӧ����
	  ͼ�α���
	//����
	ILI9486_open_window(start_x , start_y , height , width);
	
	//��Բ
	ILI9486_write_cmd(MEMORY_WRITE_CMD);
	for(i = start_y ; i < (start_y + height) ; i++){
		if(i <= center_y){
			tempy = center_y - i;
		}
		else{
			tempy = i - center_y; 
		}
		for(j = start_x ; j < (start_x + width) ; j++){
			if(j <= center_x){
				tempx = center_x - j;
			}
			else{
				tempx = j - center_x; 
			}
			if((tempx*tempx + tempy*tempy) <= radius*radius){
				ILI9486_write_data(RGB888_TO_RGB565(255 , 0 , 0));
			}
			else{
				ILI9486_write_data(RGB888_TO_RGB565(0 , 0 , 0));
			}
		}
	} 
	*/
	
	//��仭Բ��
	/*for(i = start_y ; i < (start_y + height) ; i++){
		if(i <= center_y){
			tempy = center_y - i;
		}
		else{
			tempy = i - center_y; 
		}
		for(j = start_x ; j < (start_x + width) ; j++){
			if(j <= center_x){
				tempx = center_x - j;
			}
			else{
				tempx = j - center_x; 
			}
			if((tempx*tempx + tempy*tempy) <= radius*radius){
				ILI9486_putpixel(i,j);
			} 
		}
	} */
	
	u16 offset_x = center_x , offset_y = center_y , x = 0 , y = radius ;
	double d = 1.25 - radius ;
	//�е㻭Բ��
	while(x < y){
		//����Բ�İ˷ֶԳ���
		ILI9486_putpixel(x + offset_x , y + offset_y);
		ILI9486_putpixel(x + offset_x , -y + offset_y);
		ILI9486_putpixel(y + offset_x , x + offset_y);
		ILI9486_putpixel(y + offset_x , -x + offset_y);
		ILI9486_putpixel(-y + offset_x , x + offset_y);
		ILI9486_putpixel(-y + offset_x , -x + offset_y);
		ILI9486_putpixel(-x + offset_x , y + offset_y);
		ILI9486_putpixel(-x + offset_x , -y + offset_y);
		
		if(d < 0){
			d = d + 2*x +3;
		}
		else{
			d = d +2*(x - y) +5;
            y--;			
		}
		x++;
	}
	
}


//ILI9486���ߺ���
//2020.7.15
//���ߣ�����
void ILI9486_draw_line(u16 start_x , u16 start_y , u16 end_x , u16 end_y)
{
	int16_t k = 0 , dx = 0 , dy = 0;
	u16 start_x0 = start_x , start_y0 = start_y;
	
	//ȷ���������򣬲���������һ��
	if((end_x - start_x) & 0x8000){
		dx = -1;
	}
	else{
		dx = 1;
	}
	if((end_y - start_y) & 0x8000){
		dy = -1;
	}
	else{
		dy = 1;
	}
	
	//��ֱֱ��
	if(start_x == end_x){
		while(start_y != end_y){
			ILI9486_putpixel(start_x , start_y);
			start_y += dy;
		}
	}
	//ˮƽֱ��
	else if(start_y == end_y){
		while(start_x != end_x){
			ILI9486_putpixel(start_x , start_y);
			start_x += dx;
		}
	}
	//������ֱ��
	else{
		k = (end_y - start_y)/(end_x - start_x);
		while(start_x != end_x){
			ILI9486_putpixel(start_x , start_y);
			start_x += dx;
			start_y = start_y0 + k*(start_x - start_x0);
		}
	}
}

//�������ú���
//2020.7.14
//���ߣ�����
//���� ��ʹ��״̬ 
void ILI9486_display_on(u8 enable_state)
{
	if(enable_state){
		GPIO_SetBits(ILI9486_BL_PORT , ILI9486_BL_PIN);
	}
	else{
		GPIO_ResetBits(ILI9486_BL_PORT , ILI9486_BL_PIN);
	}
}


//���ص���䣨�����㣩����
//2020.7.15
//���ߣ�����
//���������ص�����
void ILI9486_putpixel(u16 posx , u16 posy)
{
	//��������ʼ,��ֹ����
	ILI9486_write_cmd(SET_COLNUM_ADDR_CMD);
	ILI9486_write_cmd_data((posx & 0xff00) >> 8);
	ILI9486_write_cmd_data(posx & 0x00ff);
	ILI9486_write_cmd_data((posx & 0xff00) >> 8);
	ILI9486_write_cmd_data(posx & 0x00ff);
	
	//��������ʼ����ֹ����
	ILI9486_write_cmd(SET_PAGE_ADDR_CMD);
	ILI9486_write_cmd_data((posy & 0xff00) >> 8);
	ILI9486_write_cmd_data(posy &0x00ff);
	ILI9486_write_cmd_data((posy & 0xff00) >> 8);
	ILI9486_write_cmd_data(posy & 0x00ff);
	
	//�������ص������ɫ
	ILI9486_write_cmd(MEMORY_WRITE_CMD);
	ILI9486_write_data(current_txt_color);
}


//Һ����ʾӢ���ַ�����
//2020.7.18
//���ߣ�����
//��������ʾλ������ �� ��ʾ�ַ�
void ILI9486_showchar_En(u16 pos_x , u16 pos_y , u8 zdata, FONT font)
{
	u32 addr = 0;
	u8 i = 0 , j = 0, k = 0;
	u8 column = 0, rownum = 0;
	u8 databuffer[64] = {0}; 
	
	addr = ASCII_Addr_Tans(zdata, font);
	
	GT30L32S4W_Read(addr, databuffer, font);

	switch (font)
	{
	case ASCII_6X12: rownum = 12; column = 1; break;

	case ASCII_8X16: rownum = 16; column = 1; break;

	case ASCII_12X24: rownum = 24; column = 2; break;

	case ASCII_16X32: rownum = 32; column = 2; break;

	default:
		break;
	}
	
	for (i = 0; i < rownum; i++) {
		for (j = 0; j < column; j++) {
			for (k = 0; k < 8; k++) {
				if ((databuffer[i * column + j] << k) & 0x80) {
					ILI9486_putpixel(pos_x + j * 8 + k, pos_y + i);
				}
			}
		}
	}
}


//Һ����ʾ�����ַ�����
//2020.7.18
//���ߣ� ����
//��������ʾλ������ �� ��ʾ���ݻ�����
void ILI9486_showchar_Ch(u16 pos_x , u16 pos_y , u8* zdata, FONT font)
{
	u32 addr = 0 ;
	u8 i = 0 , j = 0 , k = 0;
	u8 column = 0, rownum = 0;
	u8 databuffer[128] = {0};
	
	addr = GB2312_Addr_Tans(zdata, font);
	
	GT30L32S4W_Read(addr, databuffer, font);
	
	switch((u8)font)
	{
		case GB2312_12X12: rownum = 12; column = 2; break;
		
		case GB2312_15X16: rownum = 16; column = 2; break;
		
		case GB2312_24X24: rownum = 24; column = 3; break;
		
		case GB2312_32X32: rownum = 32; column = 4; break;
		
		case GB2312_8X16: rownum = 16; column = 1; break;
		
		case GB2312_6X12: rownum = 12; column = 1; break;
	}
	
	for(i = 0 ; i < rownum ; i++){
		for(j = 0 ; j < column ; j++){
			for(k = 0 ; k < 8 ; k++){
				if((databuffer[i*column + j] << k) & 0x80){
					ILI9486_putpixel(pos_x + j*8 + k ,pos_y + i);
				}
			}
		}
	}
}



//�����ַ�����ʾ����
//2020.7.18
//���ߣ�����
//���� ��ʼ����pos_x,pos_y �� �ַ���ָ�� *zdata
void ILI9486_showstring_Ch(u16 pos_x , u16 pos_y , u8 *zdata, FONT font)
{
	u16 i = 0;
	u8 column = 0, rownum = 0;
	u8 chinese_char[2] = {0};
	u16 current_x = pos_x ,current_y = pos_y;                              //λ��ָ��
	
	switch((u8)font)
	{
		case GB2312_12X12: column = 12; rownum = 12; break;
		
		case GB2312_15X16: column = 16; rownum = 16; break;
		
		case GB2312_24X24: column = 24; rownum = 24; break;
		
		case GB2312_32X32: column = 32; rownum = 32; break;
		
		case GB2312_8X16: column = 8; rownum = 16; break;
		
		case GB2312_6X12: column = 6; rownum = 12; break;
	}
	
	while(zdata[i] != '\0'){
		
		chinese_char[0] = zdata[i++];                                      //��������Ϣ�ŵ������ĺ���ģ����������飩
		chinese_char[1] = zdata[i++];
		
		ILI9486_showchar_Ch(current_x , current_y , chinese_char, font);   //��ʾ��ǰ�����ַ�
		current_x += column;
		
		if(current_x >= screen_x_length){                                  //�������x����ĩβ����Ҫ����
			current_x = 0;
			current_y += rownum;
		}
		
		if(current_y >= screen_y_length){                                  //�������y����ĩβ����������޷���ʾ
			break;
		}
	}
}



//��ʾӢ���ַ�������
//2020.7.18
//���ߣ�����
//���� ��ʼ����pos_x,pos_y �� �ַ���ָ�� *zdata
void ILI9486_showstring_En(u16 pos_x , u16 pos_y , u8 *zdata, FONT font)
{
	u16 i = 0;
	u8 column = 0, rownum = 0;
	u16 current_x = pos_x ,current_y = pos_y;                             // λ��ָ��

	switch (font)
	{
	case ASCII_6X12: rownum = 12; column = 6; break;

	case ASCII_8X16: rownum = 16; column = 8; break;

	case ASCII_12X24: rownum = 24; column = 12; break;

	case ASCII_16X32: rownum = 32; column = 16; break;

	default:
		break;
	}
	
	while(zdata[i] != '\0' ){
		
		ILI9486_showchar_En(current_x , current_y ,zdata[i++], font);     //��ʾ��ǰ�ַ�
		current_x   +=  column;
		
		if(current_x >= screen_x_length){                                 //�������x����ĩβ����Ҫ����
			current_x = 0;
			current_y += rownum;
		}
		
		if(current_y >= screen_y_length){                                 //�������y����ĩβ����������޷���ʾ
			break;
		}
	}
}


//Һ����������
//2020.7.18
//���ߣ� ����
void ILI9486_clear_screen(u16 start_x , u16 start_y , u16 width , u16 height)
{
	u32 i = 0 ; 
	
	//ȫ������
	ILI9486_open_window(start_x , start_y , width , height);
	
	//�������ص������ɫΪ����ɫ
	ILI9486_write_cmd(MEMORY_WRITE_CMD);
	for(i = 0 ; i < (width * height) ; i++){
		ILI9486_write_data(current_back_color);
	}
	
}


//�����ı���ɫ����
//2020.7.18
//���ߣ�����
//����  color�����õ���ɫ
void ILI9486_set_txtcolor(u32 color)
{
	current_txt_color = color;
}


//���ñ�����ɫ����
//2020.7.18
//���ߣ�����
//����  color�����õ���ɫ
void ILI9486_set_backcolor(u32 color)
{
	current_back_color = color;
	ILI9486_clear_screen(ILI9486_SCREEN_START_X , ILI9486_SCREEN_START_Y , screen_x_length , screen_y_length);
}



/**
 * @brief  ����ILI9486��GRAM��ɨ�跽�� 
 * @param  mode ��ѡ��GRAM��ɨ�跽�� 
 *     @arg 0-7 :������ѡֵΪ0-7��˸�����
 *
 *	����������0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 *				���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 *		
 *	����0��2��4��6 ģʽ��X��������Ϊ240��Y��������Ϊ320
 *	����1��3��5��7 ģʽ��X��������Ϊ320��Y��������Ϊ240
 *
 *	���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����
 *	���� 3 ģʽΪ����ͷ����ʹ�õķ���
 *	���� 0 ģʽΪBMPͼƬ��ʾ����ʹ�õķ���
 *
 * @retval ��
 * @note  ����ͼ����A��ʾ���ϣ�V��ʾ���£�<��ʾ����>��ʾ����
					X��ʾX�ᣬY��ʾY��

------------------------------------------------------------
ģʽ0��				.		ģʽ1��		.	ģʽ2��			.	ģʽ3��					
					A		.	A		.		A					.		A									
					|		.	|		.		|					.		|							
					Y		.	X		.		Y					.		X					
					0		.	1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
ģʽ4��				.	ģʽ5��			.	ģʽ6��			.	ģʽ7��					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.	5				6		.		7					.		
					Y		.	X				Y		.		X					.						
					|		.	|				|		.		|					.								
					V		.	v			    V		.		V					.			
---------------------------------------------------------				
											 LCD��ʾ��
								|-----------------|
								|			Ұ��Logo		            |
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								��Ļ���棨��240����320��

 *******************************************************/
void ILI9486_gram_scan_mode(u8 mode)
{
	/*����ģʽ����XY��������ؿ��*/
	
	//ģʽ 0,2,4,6
	if(mode%2 == 0){
		screen_x_length = ILI9486_SCREEN_LESSWIDTH;
		screen_y_length = ILI9486_SCREEN_MOREWIDTH;
	}
	//ģʽ1,3,5,7
	else{
		screen_x_length = ILI9486_SCREEN_MOREWIDTH;
		screen_y_length = ILI9486_SCREEN_LESSWIDTH;
	}
	
	//д��36H�������ɨ��ģʽ
	ILI9486_write_cmd(SET_SCAN_MODE);
	ILI9486_write_data(0x08 | (mode << 5));
}


//��ȡ���ص����ݺ���
//2020.7.20
//���ߣ�����
//���������ص����� posx , posy
/**********˵��*********
RG_data : R1[4:0]xxx G1[5:0]xx
BR_data : B1[4:0]xxx R2[4:0]xxx
**********************/
u16 ILI9486_read_pixel_data(u16 posx , u16 posy)
{
	u16 RG_data  = 0 , BR_data = 0 ;
	
	//����X��ʼ,��ֹ����
	ILI9486_write_cmd(SET_COLNUM_ADDR_CMD);
	ILI9486_write_data((posx & 0xff00) >> 8);
	ILI9486_write_data(posx & 0x00ff);
	ILI9486_write_data((posx & 0xff00) >> 8);
	ILI9486_write_data(posx & 0x00ff);
	
	//����Y��ʼ����ֹ����
	ILI9486_write_cmd(SET_PAGE_ADDR_CMD);
	ILI9486_write_data((posy & 0xff00) >> 8);
	ILI9486_write_data(posy &0x00ff);
	ILI9486_write_data((posy & 0xff00) >> 8);
	ILI9486_write_data(posy & 0x00ff);
	
	//д������ص���������
	ILI9486_write_cmd(READ_MEMORY);
	RG_data = ILI9486_read_data();                        //��һ������Ϊ��Ч dummy
	RG_data = ILI9486_read_data();                        //�ڶ�������Ϊ���̷���
	BR_data = ILI9486_read_data();                        //����������Ϊ�������
	
	return (u16) (RG_data >> 11) << 11 | ((RG_data & 0x00ff) >> 2) <<5 | (BR_data & 0xff00) >> 11 ;
}


//��ʮ�ּܺ���
//2020.7.20
//���ߣ�����
//������ʮ�ּ��������� posx ,posy
void ILI9486_draw_cross(u16 posx , u16 posy)
{
	u16 level_sx = 0 , level_sy = 0 , level_ex = 0 , level_ey = 0;
	u16 vertical_sx = 0 , vertical_sy  = 0 , vertical_ex = 0 ,vertical_ey = 0;
	
	//����ʮ�ּ�ˮƽ����㣬�յ�����
	level_sx = posx - 10;
	level_sy = posy;
	level_ex = posx + 10;
	level_ey = posy;
	
	//����ʮ�ּ���ֱ����㣬�յ�����
	vertical_sx = posx;
	vertical_sy = posy -10;
	vertical_ex = posx;
	vertical_ey = posy +10;
	
	//����ˮƽ�ߺ���ֱ��
	ILI9486_draw_line(level_sx , level_sy , level_ex , level_ey);
	ILI9486_draw_line(vertical_sx , vertical_sy , vertical_ex , vertical_ey);
}

