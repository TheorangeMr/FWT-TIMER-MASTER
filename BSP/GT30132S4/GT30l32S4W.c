/*******************************************
	*�ļ��� ��  GT30L32S4W.c
	*��   �ߣ�  ����
	*�޸�ʱ�䣺 2020.10.20
	*��   ����  v1.0
*******************************************/


#include "GT30L32S4W.h"
#include "bsp_spi.h"


/*
	*��������Gt30L32S4W_Init()
	*��  �ܣ�GT30L32S4WоƬ��ʼ������
	*��  ���� ��
	*����ֵ�� ��
	*��  �ߣ� ����
	*��  �ڣ� 2020.10.20
*/
void GT30L32S4W_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GT30L32S4W_CS_PIN;
	GPIO_Init(GT30L32S4W_CS_PORT, &GPIO_InitStructure);
}


/*
	*��������GT30L32S4W_Read()
	*��  �ܣ���ȡGT30L32S4WоƬһ���ַ��ĵ�������
	*��  ���� addr����ַ��databuffer�����ݻ������飬 font������
	*����ֵ�� ��
	*��  �ߣ� ����
	*��  �ڣ� 2020.10.20
*/
void GT30L32S4W_Read(u32 addr, u8* databuffer, FONT font)
{
	u8 ByteNum = 0, i = 0;

	switch (font)
	{
		case GB2312_12X12: ByteNum = 24; break;

		case GB2312_15X16: ByteNum = 32; break;

		case GB2312_24X24: ByteNum = 72; break;

		case GB2312_32X32: ByteNum = 128; break;

		case GB2312_8X16: ByteNum = 16; break;

		case GB2312_6X12: ByteNum = 12; break;

		case ASCII_6X12: ByteNum = 12; break;

		case ASCII_8X16: ByteNum = 16; break;

		case ASCII_12X24: ByteNum = 48; break;

		case ASCII_16X32: ByteNum = 64; break;

		default: break;
	}

	GT30L32S4W_CS_L;                           // ����Ƭѡ

	SPI_SendByte(GT30L32S4W_READ);      //���Ͷ�ָ��

	//����24λ��ַ
	SPI_SendByte((addr & 0xffffff) >> 16);
	SPI_SendByte((addr & 0xffff) >> 8);
	SPI_SendByte(addr & 0xff);

	while (i < ByteNum)
	{
		databuffer[i++] = SPI_SendByte(Dummy);
	}

	GT30L32S4W_CS_H;                           //����Ƭѡ
}


/*
	*��������GT30L32S4W_Fast_Read()
	*��  �ܣ����ٶ�ȡGT30L32S4WоƬ������
	*��  ���� addr����ַ��databuffer�����ݻ������飬 font:����
	*����ֵ�� ��
	*��  �ߣ� ����
	*��  �ڣ� 2020.10.20
*/
void GT30L32S4W_Fast_Read(u32 addr, u8* databuffer, FONT font)
{
	u8 ByteNum = 0, i = 0;

	switch (font)
	{
		case GB2312_12X12: ByteNum = 24; break;

		case GB2312_15X16: ByteNum = 32; break;

		case GB2312_24X24: ByteNum = 72; break;

		case GB2312_32X32: ByteNum = 128; break;

		case GB2312_8X16: ByteNum = 16; break;

		case GB2312_6X12: ByteNum = 12; break;

		case ASCII_6X12: ByteNum = 12; break;

		case ASCII_8X16: ByteNum = 16; break;
			
		case ASCII_12X24: ByteNum = 48; break;

		case ASCII_16X32: ByteNum = 64; break;

		default: break;
	}

	GT30L32S4W_CS_L;                              // ����Ƭѡ

	SPI_SendByte(GT30L32S4W_FAST_READ);         //���Ͷ�ָ��

	//����24λ��ַ
	SPI_SendByte((addr & 0xffffff) >> 16);
	SPI_SendByte((addr & 0xffff) >> 8);
	SPI_SendByte(addr & 0xff);

	SPI_SendByte(0xff);                         //д��һ��Dummy Byte

	while (i < ByteNum)
	{
		databuffer[i++] = SPI_SendByte(Dummy);
	}

	GT30L32S4W_CS_H;                               //����Ƭѡ
}


/*
	*��������GB2312_Addr_Tans()
	*��  �ܣ����ַ�������ת��ΪGT30L32S4W�ڲ���ַ
	*��  ���� code���ַ�������,font������
	*����ֵ�� GT30L32S4W�ڲ���ַ,����0xffffffff��˵���������
	*��  �ߣ� ����
	*��  �ڣ� 2020.10.20
*/
u32  GB2312_Addr_Tans(u8* code, FONT font)
{
	u32 addr = 0xffffffff;
	
	switch (font)
	{
		case GB2312_12X12:
			addr = GB2312_12X12_ADDR(code);
			break;
		case GB2312_15X16:
			addr = GB2312_15X16_ADDR(code);
			break;
		case GB2312_24X24:
			addr = GB2312_24X24_ADDR(code);
			break;
		case GB2312_32X32:
			addr = GB2312_32X32_ADDR(code);
			break;
		case GB2312_8X16:
			addr = GB2312_8X16_ADDR(code);
			break;
		case GB2312_6X12:
			addr = GB2312_6X12_ADDR(code);
			break;
		default:
			addr = 0xffffffff;
			break;
	}
	
	return addr;
}


/*
	*��������ASCII_Addr_Tans()
	*��  �ܣ����ַ�ASCII��ת��ΪGT30L32S4W�ڲ���ַ
	*��  ���� code���ַ�ASCII��,font������
	*����ֵ�� GT30L32S4W�ڲ���ַ,����0xffffffff��˵���������
	*��  �ߣ� ����
	*��  �ڣ� 2020.10.20
*/
u32  ASCII_Addr_Tans(char code, FONT font)
{
	u32 addr = 0xffffffff;
	
	switch (font)
	{
		case ASCII_6X12:
			addr = ASCII_6X12_ADDR(code);
			break;
		case ASCII_8X16:
			addr = ASCII_8X16_ADDR(code);
			break;
		case ASCII_12X24:
			addr = ASCII_12X24_ADDR(code);
			break;
		case ASCII_16X32:
			addr = ASCII_16X32_ADDR(code);
			break;
		default:
			addr = 0xffffffff;
			break;
	}
	
	return addr;
}


//GB2312_12X12��ַ���㺯��
u32  GB2312_12X12_ADDR(u8* code)
{
	if (code[0] >= 0xA1 && code[0] <= 0xA9 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xA1) * 94 + (code[1] - 0xA1)) * 24 + GB2312_12X12_BASEADD;
	}
	else if (code[0] >= 0xB0 && code[0] <= 0xF7 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xB0) * 94 + (code[1] - 0xA1) + 846) * 24 + GB2312_12X12_BASEADD;
	}
	return 0xffffffff;
}

//GB2312_15X16��ַ���㺯��
u32  GB2312_15X16_ADDR(u8* code)
{
	if (code[0] >= 0xA1 && code[0] <= 0xA9 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xA1) * 94 + (code[1] - 0xA1)) * 32 + GB2312_15X16_BASEADD;
	}
	else if (code[0] >= 0xB0 && code[0] <= 0xF7 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xB0) * 94 + (code[1] - 0xA1) + 846) * 32 + GB2312_15X16_BASEADD;
	}
	return 0xffffffff;
}

//GB2312_24X24��ַ���㺯��
u32  GB2312_24X24_ADDR(u8* code)
{
	if (code[0] >= 0xA1 && code[0] <= 0xA9 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xA1) * 94 + (code[1] - 0xA1)) * 72 + GB2312_24X24_BASEADD;
	}
	else if (code[0] >= 0xB0 && code[0] <= 0xF7 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xB0) * 94 + (code[1] - 0xA1) + 846) * 72 + GB2312_24X24_BASEADD;
	}
	return 0xffffffff;
}

//GB2312_32X32��ַ���㺯��
u32  GB2312_32X32_ADDR(u8* code)
{
	if (code[0] >= 0xA1 && code[0] <= 0xA9 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xA1) * 94 + (code[1] - 0xA1)) * 128 + GB2312_32X32_BASEADD;
	}
	else if (code[0] >= 0xB0 && code[0] <= 0xF7 && code[1] >= 0xA1)
	{
		return ((code[0] - 0xB0) * 94 + (code[1] - 0xA1) + 846) * 128 + GB2312_32X32_BASEADD;
	}
	return 0xffffffff;
}

//GB2312_8X16��ַ���㺯��
u32  GB2312_8X16_ADDR(u8* code)
{
	u16 fontcode = (code[0] << 8) | code[1];
	if (fontcode >= 0xAAA1 && fontcode <= 0xAAFE)
	{
		return (fontcode - 0xAAA1) * 16 + GB2312_8X16_BASEADD;
	}
	else if (fontcode >= 0xABA1 && fontcode <= 0xABC0)
	{
		return (fontcode - 0xABA1 + 95) * 16 + GB2312_8X16_BASEADD;
	}
	return 0xffffffff;
}

//GB2312_6X12��ַ���㺯��
u32  GB2312_6X12_ADDR(u8* code)
{
	u16 fontcode = (code[0] << 8) | code[1];
	if (fontcode >= 0xAAA1 && fontcode <= 0xAAFE)
	{
		return (fontcode - 0xAAA1) * 12 + GB2312_8X16_BASEADD;
	}
	else if (fontcode >= 0xABA1 && fontcode <= 0xABC0)
	{
		return (fontcode - 0xABA1 + 95) * 12 + GB2312_8X16_BASEADD;
	}
	return 0xffffffff;
}

//ASCII_6X12��ַ���㺯��
u32  ASCII_6X12_ADDR(char code)
{
	if (code >= 0x20 && code <= 0x7E)
	{
		return (code - 0x20) * 12 + ASCII_6X12_BASEADD;
	}
	return 0xffffffff;
}

//ASCII_8X16��ַ���㺯��
u32  ASCII_8X16_ADDR(char code)
{
	if (code >= 0x20 && code <= 0x7E)
	{
		return (code - 0x20) * 16 + ASCII_8X16_BASEADD;
	}
	return 0xffffffff;
}

//ASCII_12X24��ַ���㺯��
u32  ASCII_12X24_ADDR(char code)
{
	if (code >= 0x20 && code <= 0x7E)
	{
		return (code - 0x20) * 48 + ASCII_12X24_BASEADD;
	}
	return 0xffffffff;
}

//ASCII_16X32��ַ���㺯��
u32  ASCII_16X32_ADDR(char code)
{
	if (code >= 0x20 && code <= 0x7E)
	{
		return (code - 0x20) * 64 + ASCII_16X32_BASEADD;
	}
	return 0xffffffff;
}


