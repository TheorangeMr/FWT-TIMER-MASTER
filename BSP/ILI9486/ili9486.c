/*******************************************
	*文件名 ：  Exti.c
	*作   者：  王峰
	*修改时间： 2020.10.29
	*版   本：  v1.0
*******************************************/


#include "ili9486.h"
#include "bsp_delay.h"
#include "bsp_spi.h"



u16 screen_x_length = ILI9486_SCREEN_LESSWIDTH;          //默认设置x方向长度更短
u16 screen_y_length = ILI9486_SCREEN_MOREWIDTH;          //默认设置Y方向长度更长
static u32 current_txt_color = BLACK;                    //默认设置文本颜色为黑色（前景色）
static u32 current_back_color = WHITE;                   //默认设置背景颜色为白色（背景色）

//ILI9486引脚配置函数
//2020.7.14
//作者：王峰
void ILI9486_tftlcd_gpio_config()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//使能ILI9486 FSMC相关引脚的时钟
	RCC_APB2PeriphClockCmd(ILI9486_CS_CLK | ILI9486_DC_CLK | ILI9486_BL_CLK | ILI9486_RES_CLK, ENABLE);
	
	//配置背光控制引脚工作模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ILI9486_BL_PIN;
	GPIO_Init(ILI9486_BL_PORT , &GPIO_InitStructure);
	
	//片选引脚
	GPIO_InitStructure.GPIO_Pin = ILI9486_CS_PIN;
	GPIO_Init(ILI9486_CS_PORT , &GPIO_InitStructure);
	
	//数据/命令控制引脚
	GPIO_InitStructure.GPIO_Pin = ILI9486_DC_PIN;
	GPIO_Init(ILI9486_DC_PORT , &GPIO_InitStructure);
	
	//复位引脚
	GPIO_InitStructure.GPIO_Pin = ILI9486_RES_PIN;
	GPIO_Init(ILI9486_RES_PORT , &GPIO_InitStructure);
	
	GPIO_SetBits(ILI9486_BL_PORT, ILI9486_BL_PIN);
	
	RES_SET();
	delay_xms(50);
	RES_CLR();
	delay_xms(50);
	RES_SET();
}


//ILI9486写命令函数
//2020.10.21
//作者：王峰
void ILI9486_write_cmd(u8 cmd)
{
	ILI9486_CS_L;
	ILI9486_DC_L;
	
	SPI_SendByte(cmd);
	
	ILI9486_CS_H;
}


//ILI9486写参数函数
//2020.10.21
//作者：王峰
void ILI9486_write_cmd_data(u8 zdata)
{
	ILI9486_CS_L;
	ILI9486_DC_H;
	SPI_SendByte(zdata);
	
	ILI9486_CS_H;
}

//ILI9486写数据函数
//2020.10.21
//作者：王峰
void ILI9486_write_data(u32 zdata)
{
	ILI9486_CS_L;
	ILI9486_DC_H;
	
	SPI_SendByte(zdata>>16);
	SPI_SendByte(zdata>>8);
	SPI_SendByte(zdata);
	
	ILI9486_CS_H;
}



//读取ILI9486数据
//2020.10.21
//作者：王峰
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


////液晶初始化函数
////2022.5.2
////作者：罗成
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
	ILI9486_write_cmd_data( 0x08 );    /*竖屏  右下角到 (起点)到左上角 (终点)扫描方式*/
	
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


//ILI9486 像素点格式读取函数
//2020.7.14
//作者：王峰
u16 ILI9486_pixel_format_read(void)
{
	ILI9486_write_cmd(0x0c);                      //写入Read Display Pixel Format命令
	//ILI9486_read_data();                          //读回的第一个数据为无效的
	return ILI9486_read_data();
}


//ILI9486开窗函数
//2020.7.14
//作者： 王峰
//参数： 列起始坐标start_x ， 行起始坐标start_y , 高度height ， 宽度width
 void ILI9486_open_window(u16 start_x , u16 start_y , u16 width , u16 height)
{
	u16 end_x = 0 , end_y = 0 ;
	end_x = start_x + width;
	end_y = start_y + height;
	
	//设置列起始,终止坐标
	ILI9486_write_cmd(SET_COLNUM_ADDR_CMD);
	ILI9486_write_cmd_data((start_x & 0xff00) >> 8);
	ILI9486_write_cmd_data(start_x & 0x00ff);
	ILI9486_write_cmd_data((end_x & 0xff00) >> 8);
	ILI9486_write_cmd_data(end_x & 0x00ff);
	
	//设置行起始，终止坐标
	ILI9486_write_cmd(SET_PAGE_ADDR_CMD);
	ILI9486_write_cmd_data((start_y & 0xff00) >> 8);
	ILI9486_write_cmd_data(start_y &0x00ff);
	ILI9486_write_cmd_data((end_y & 0xff00) >> 8);
	ILI9486_write_cmd_data(end_y & 0x00ff);
	
}


//ILI9486画矩形函数
//2020.7.14
//作者：王峰
//参数：起始点坐标start_x,start_y,宽度width,高度height
void ILI9486_draw_rectangle(u16 start_x , u16 start_y , u16 width , u16 height, u32 color)
{
    /*
	//画实心矩形
    u16 i = 0;
	ILI9486_open_window(start_x , start_y , width , height);
	
	//设置像素点填充颜色
	ILI9486_write_cmd(MEMORY_WRITE_CMD);
	for(i = 0 ; i < (width * height) ; i++){
		ILI9486_write_data(color);
	}
    */
	
	//画空心矩形
	ILI9486_draw_line(start_x , start_y , start_x + width , start_y);
	ILI9486_draw_line(start_x + width , start_y , start_x + width , start_y + height);
	ILI9486_draw_line(start_x + width , start_y + height , start_x , start_y + height);
	ILI9486_draw_line(start_x , start_y + height , start_x , start_y);
	
}


//ILI9486画圆函数
//2020.7.15
//作者：王峰
//参数：圆心坐标，半径
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
	
	/*此方法理论上可以，但可能是因为运算过于复杂，时间过长导致扫描坐标和循环体的坐标对应不上
	  图形变形
	//开窗
	ILI9486_open_window(start_x , start_y , height , width);
	
	//画圆
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
	
	//填充画圆法
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
	//中点画圆法
	while(x < y){
		//根据圆的八分对称性
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


//ILI9486画线函数
//2020.7.15
//作者：王峰
void ILI9486_draw_line(u16 start_x , u16 start_y , u16 end_x , u16 end_y)
{
	int16_t k = 0 , dx = 0 , dy = 0;
	u16 start_x0 = start_x , start_y0 = start_y;
	
	//确定增量方向，并且增量归一化
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
	
	//竖直直线
	if(start_x == end_x){
		while(start_y != end_y){
			ILI9486_putpixel(start_x , start_y);
			start_y += dy;
		}
	}
	//水平直线
	else if(start_y == end_y){
		while(start_x != end_x){
			ILI9486_putpixel(start_x , start_y);
			start_x += dx;
		}
	}
	//非特殊直线
	else{
		k = (end_y - start_y)/(end_x - start_x);
		while(start_x != end_x){
			ILI9486_putpixel(start_x , start_y);
			start_x += dx;
			start_y = start_y0 + k*(start_x - start_x0);
		}
	}
}

//背光设置函数
//2020.7.14
//作者：王峰
//参数 ：使能状态 
void ILI9486_display_on(u8 enable_state)
{
	if(enable_state){
		GPIO_SetBits(ILI9486_BL_PORT , ILI9486_BL_PIN);
	}
	else{
		GPIO_ResetBits(ILI9486_BL_PORT , ILI9486_BL_PIN);
	}
}


//像素点填充（即画点）函数
//2020.7.15
//作者：王峰
//参数：像素点坐标
void ILI9486_putpixel(u16 posx , u16 posy)
{
	//设置列起始,终止坐标
	ILI9486_write_cmd(SET_COLNUM_ADDR_CMD);
	ILI9486_write_cmd_data((posx & 0xff00) >> 8);
	ILI9486_write_cmd_data(posx & 0x00ff);
	ILI9486_write_cmd_data((posx & 0xff00) >> 8);
	ILI9486_write_cmd_data(posx & 0x00ff);
	
	//设置行起始，终止坐标
	ILI9486_write_cmd(SET_PAGE_ADDR_CMD);
	ILI9486_write_cmd_data((posy & 0xff00) >> 8);
	ILI9486_write_cmd_data(posy &0x00ff);
	ILI9486_write_cmd_data((posy & 0xff00) >> 8);
	ILI9486_write_cmd_data(posy & 0x00ff);
	
	//设置像素点填充颜色
	ILI9486_write_cmd(MEMORY_WRITE_CMD);
	ILI9486_write_data(current_txt_color);
}


//液晶显示英文字符函数
//2020.7.18
//作者：王峰
//参数：显示位置坐标 ， 显示字符
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


//液晶显示汉字字符函数
//2020.7.18
//作者： 王峰
//参数：显示位置坐标 ， 显示数据机内码
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



//汉字字符串显示函数
//2020.7.18
//作者：王峰
//参数 起始坐标pos_x,pos_y ， 字符串指针 *zdata
void ILI9486_showstring_Ch(u16 pos_x , u16 pos_y , u8 *zdata, FONT font)
{
	u16 i = 0;
	u8 column = 0, rownum = 0;
	u8 chinese_char[2] = {0};
	u16 current_x = pos_x ,current_y = pos_y;                              //位置指针
	
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
		
		chinese_char[0] = zdata[i++];                                      //将汉字信息放到构建的汉字模拟变量（数组）
		chinese_char[1] = zdata[i++];
		
		ILI9486_showchar_Ch(current_x , current_y , chinese_char, font);   //显示当前汉字字符
		current_x += column;
		
		if(current_x >= screen_x_length){                                  //如果到达x方向末尾，就要换行
			current_x = 0;
			current_y += rownum;
		}
		
		if(current_y >= screen_y_length){                                  //如果到达y方向末尾，则结束，无法显示
			break;
		}
	}
}



//显示英文字符串函数
//2020.7.18
//作者：王峰
//参数 起始坐标pos_x,pos_y ， 字符串指针 *zdata
void ILI9486_showstring_En(u16 pos_x , u16 pos_y , u8 *zdata, FONT font)
{
	u16 i = 0;
	u8 column = 0, rownum = 0;
	u16 current_x = pos_x ,current_y = pos_y;                             // 位置指针

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
		
		ILI9486_showchar_En(current_x , current_y ,zdata[i++], font);     //显示当前字符
		current_x   +=  column;
		
		if(current_x >= screen_x_length){                                 //如果到达x方向末尾，就要换行
			current_x = 0;
			current_y += rownum;
		}
		
		if(current_y >= screen_y_length){                                 //如果到达y方向末尾，则结束，无法显示
			break;
		}
	}
}


//液晶清屏函数
//2020.7.18
//作者： 王峰
void ILI9486_clear_screen(u16 start_x , u16 start_y , u16 width , u16 height)
{
	u32 i = 0 ; 
	
	//全屏开窗
	ILI9486_open_window(start_x , start_y , width , height);
	
	//设置像素点填充颜色为背景色
	ILI9486_write_cmd(MEMORY_WRITE_CMD);
	for(i = 0 ; i < (width * height) ; i++){
		ILI9486_write_data(current_back_color);
	}
	
}


//设置文本颜色函数
//2020.7.18
//作者：王峰
//参数  color：设置的颜色
void ILI9486_set_txtcolor(u32 color)
{
	current_txt_color = color;
}


//设置背景颜色函数
//2020.7.18
//作者：王峰
//参数  color：设置的颜色
void ILI9486_set_backcolor(u32 color)
{
	current_back_color = color;
	ILI9486_clear_screen(ILI9486_SCREEN_START_X , ILI9486_SCREEN_START_Y , screen_x_length , screen_y_length);
}



/**
 * @brief  设置ILI9486的GRAM的扫描方向 
 * @param  mode ：选择GRAM的扫描方向 
 *     @arg 0-7 :参数可选值为0-7这八个方向
 *
 *	！！！其中0、3、5、6 模式适合从左至右显示文字，
 *				不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 *		
 *	其中0、2、4、6 模式的X方向像素为240，Y方向像素为320
 *	其中1、3、5、7 模式下X方向像素为320，Y方向像素为240
 *
 *	其中 6 模式为大部分液晶例程的默认显示方向
 *	其中 3 模式为摄像头例程使用的方向
 *	其中 0 模式为BMP图片显示例程使用的方向
 *
 * @retval 无
 * @note  坐标图例：A表示向上，V表示向下，<表示向左，>表示向右
					X表示X轴，Y表示Y轴

------------------------------------------------------------
模式0：				.		模式1：		.	模式2：			.	模式3：					
					A		.	A		.		A					.		A									
					|		.	|		.		|					.		|							
					Y		.	X		.		Y					.		X					
					0		.	1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
模式4：				.	模式5：			.	模式6：			.	模式7：					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.	5				6		.		7					.		
					Y		.	X				Y		.		X					.						
					|		.	|				|		.		|					.								
					V		.	v			    V		.		V					.			
---------------------------------------------------------				
											 LCD屏示例
								|-----------------|
								|			野火Logo		            |
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								屏幕正面（宽240，高320）

 *******************************************************/
void ILI9486_gram_scan_mode(u8 mode)
{
	/*根据模式更新XY方向的像素宽度*/
	
	//模式 0,2,4,6
	if(mode%2 == 0){
		screen_x_length = ILI9486_SCREEN_LESSWIDTH;
		screen_y_length = ILI9486_SCREEN_MOREWIDTH;
	}
	//模式1,3,5,7
	else{
		screen_x_length = ILI9486_SCREEN_MOREWIDTH;
		screen_y_length = ILI9486_SCREEN_LESSWIDTH;
	}
	
	//写入36H命令，设置扫描模式
	ILI9486_write_cmd(SET_SCAN_MODE);
	ILI9486_write_data(0x08 | (mode << 5));
}


//读取像素点数据函数
//2020.7.20
//作者：王峰
//参数：像素点坐标 posx , posy
/**********说明*********
RG_data : R1[4:0]xxx G1[5:0]xx
BR_data : B1[4:0]xxx R2[4:0]xxx
**********************/
u16 ILI9486_read_pixel_data(u16 posx , u16 posy)
{
	u16 RG_data  = 0 , BR_data = 0 ;
	
	//设置X起始,终止坐标
	ILI9486_write_cmd(SET_COLNUM_ADDR_CMD);
	ILI9486_write_data((posx & 0xff00) >> 8);
	ILI9486_write_data(posx & 0x00ff);
	ILI9486_write_data((posx & 0xff00) >> 8);
	ILI9486_write_data(posx & 0x00ff);
	
	//设置Y起始，终止坐标
	ILI9486_write_cmd(SET_PAGE_ADDR_CMD);
	ILI9486_write_data((posy & 0xff00) >> 8);
	ILI9486_write_data(posy &0x00ff);
	ILI9486_write_data((posy & 0xff00) >> 8);
	ILI9486_write_data(posy & 0x00ff);
	
	//写入读像素点数据命令
	ILI9486_write_cmd(READ_MEMORY);
	RG_data = ILI9486_read_data();                        //第一个数据为无效 dummy
	RG_data = ILI9486_read_data();                        //第二个数据为红绿分量
	BR_data = ILI9486_read_data();                        //第三个数据为蓝红分量
	
	return (u16) (RG_data >> 11) << 11 | ((RG_data & 0x00ff) >> 2) <<5 | (BR_data & 0xff00) >> 11 ;
}


//画十字架函数
//2020.7.20
//作者：王峰
//参数：十字架中心坐标 posx ,posy
void ILI9486_draw_cross(u16 posx , u16 posy)
{
	u16 level_sx = 0 , level_sy = 0 , level_ex = 0 , level_ey = 0;
	u16 vertical_sx = 0 , vertical_sy  = 0 , vertical_ex = 0 ,vertical_ey = 0;
	
	//计算十字架水平线起点，终点坐标
	level_sx = posx - 10;
	level_sy = posy;
	level_ex = posx + 10;
	level_ey = posy;
	
	//计算十字架竖直线起点，终点坐标
	vertical_sx = posx;
	vertical_sy = posy -10;
	vertical_ex = posx;
	vertical_ey = posy +10;
	
	//画出水平线和竖直线
	ILI9486_draw_line(level_sx , level_sy , level_ex , level_ey);
	ILI9486_draw_line(vertical_sx , vertical_sy , vertical_ex , vertical_ey);
}

