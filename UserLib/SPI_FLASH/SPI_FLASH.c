/*
 * SPI_FLASH.c
 *
 *  Created on: 2020年5月22日
 *      Author: WangXiang
 */
#include "SPI_FLASH.h"
#include "UserConfig.h"
#include <stdarg.h>
__attribute__((alias("SPI_iprintf"))) int SPI_printf(const char *fmt, ...);

uint32_t HeadPage, HeadSector, ReadPage;

/*
 *	SPI协议初始化
 *	8分频,频率8MHz
 */
void SPI_INIT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					   //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				   //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						   //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;					   //数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //定义波特率预分频的值:波特率预分频值为8-9M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;			//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);							//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	//	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Reset);
	pinModeA(GPIO_Pin_4, OUTPUT);
	pinModeC(GPIO_Pin_4, OUTPUT);
	digitalWriteC(GPIO_Pin_4, HIGH);
//	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	CS_HIGH
	;

	SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}

/*
 * SPI协议输出1字节
 * 超时自动退出(5s)
 */
void SPI_write(u8 TxData)
{
	u16 count = 0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		delay_us(100);
		count++;
		if (count > 50000)
			return;
	}
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
}

/*
 * 向W25Q128芯片指定页写入字符串
 * 请配合片选/写使能使用
 */
int SPI_writeStr(uint32_t page, char *str)
{
	SPI_write((((page - 1) * 0x100) >> 16) & (0xFF));
	SPI_write((((page - 1) * 0x100) >> 8) & (0xFF));
	SPI_write((((page - 1) * 0x100)) & (0xFF));
	uint32_t add = page;
	uint16_t j = 0;
	while (str[j] != '\0') //遍历字符串，直到字符串为空
	{
		if (j < 256)
		{
			SPI_write(str[j]);
			j++;
		} else
			break;
	}
	if (j >= 256)
	{
		SPI_write(((page) >> 16) & (0xFF));
		SPI_write(((page) >> 8) & (0xFF));
		SPI_write(((page)) & (0xFF));
		add = (page) + 1;
		while (str[j] != '\0') //遍历字符串，直到字符串为空
		{
			SPI_write(str[j]);
			j++;
		}
	}
	return add;
}

/*
 * 打开W25Q128芯片的写使能
 */
void WriteEN()
{
	CS_LOW;
	SPI_write(FLASH_WRITE_ENABLE_CMD);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		;
	CS_HIGH
	;
}

/*
 * SPI协议W25Q128 Flash储存芯片复位
 */
void SPI_FlashReset()
{
	CheckBusy();
	CS_LOW;
	SPI_write(0x66);
	SPI_write(0x99);
	CS_HIGH
	;
	delay_us(100);
}

/*
 * SPI协议W25Q128 Flash储存芯片忙碌检查
 */
void CheckBusy()
{
	uint8_t flag = 0;
	int count = 0;
	while (1)
	{
		CS_LOW;
		SPI_write(FLASH_READ_SR_CMD);
		SPI_write(FLASH_READ_SR_CMD);
		flag = SPI_MasterSendReceiveByte(0x00);
		if (!(flag & 0x01))
		{
			CS_HIGH
			;
			return;
		}
		CS_HIGH
		;
		delay_us(100);
		count++;
		if (count > 100000)
			return;
	}
}

/*
 * SPI协议W25Q128 Flash储存芯片顺序写入
 */
int SPI_iprintf(const char *fmt, ...)
{
	int length = 0;
	va_list va;
	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	int add = HeadPage;
	va_end(va);
	{
		char buf[length];
		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);
		if (HeadPage == 65530)
		{
			CheckBusy();
			SPI_EraseSector(2);
			HeadPage = 17;
			HeadSector = 2;
		}
		if (HeadSector != ((HeadPage - 1) / 16) + 1)
		{
			CheckBusy();
			SPI_EraseSector(((HeadPage - 1) / 16) + 1);
			HeadSector = ((HeadPage - 1) / 16) + 1;
		}
		CheckBusy();
		WriteEN();
		CS_LOW;
		SPI_write(FLASH_WRITE_PAGE);
		add = SPI_writeStr(HeadPage, buf);
		HeadPage++;
		CS_HIGH
		;
		va_end(va);
	}
	return add;
}

/*
 * SPI协议发送并读取数据
 */
uint8_t SPI_MasterSendReceiveByte(uint8_t spi_byte)
{
	uint8_t count = 0;
	SPI_I2S_ReceiveData(SPI1);
	SPI_write(spi_byte);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		delay_us(100);
		count++;
		if (count > 5000)
			return 0;
	}
	return SPI_I2S_ReceiveData(SPI1);
}

/*
 * SPI协议W25Q128 Flash储存芯片全片擦除
 */
void SPI_EraseChip()
{
	CheckBusy();
	WriteEN();
	CS_LOW;
	SPI_write(0x60);
	CS_HIGH
	;
}

/*
 * SPI协议W25Q128 Flash储存芯片擦除指定块
 */
void SPI_EraseSector(uint32_t sector)
{
	CheckBusy();
	WriteEN();
	CS_LOW;
	SPI_write(0x20);
	SPI_write((((sector - 1) * 0x1000) >> 16) & (0xFF));
	SPI_write((((sector - 1) * 0x1000) >> 8) & (0xFF));
	SPI_write(((sector - 1) * 0x1000) & (0xFF));
	CS_HIGH
	;
}

/*
 * SPI协议W25Q128 Flash储存芯片读取指定页面
 * 读取256次,数组需要足够大
 */
void SPI_FlashReadPage(uint32_t page, char str[])
{
	int i = 0;
	CheckBusy();
	CS_LOW;
	SPI_write(FLASH_FASTREAD_DATA);
	SPI_write((((page - 1) * 0x100) >> 16) & (0xFF));
	SPI_write((((page - 1) * 0x100) >> 8) & (0xFF));
	SPI_write((((page - 1) * 0x100)) & (0xFF));
	SPI_MasterSendReceiveByte(0xFF);
	for (i = 0; i < 256; i++)
	{
		str[i] = SPI_MasterSendReceiveByte(0xFF);
	}
	CS_HIGH
	;
}

/*
 * SPI协议W25Q128 Flash储存芯片读取指定页面的多个字节
 */
void SPI_FlashReadPageByte(uint32_t page, uint8_t number, uint8_t str[])
{
	uint8_t data = 0;
	CheckBusy();
	CS_LOW;
	SPI_write(FLASH_FASTREAD_DATA);
	SPI_write((((page - 1) * 0x100) >> 16) & (0xFF));
	SPI_write((((page - 1) * 0x100) >> 8) & (0xFF));
	SPI_write((((page - 1) * 0x100)) & (0xFF));
	SPI_MasterSendReceiveByte(0xFF);
	for (data = 0; data < number; data++)
		str[data] = SPI_MasterSendReceiveByte(0xFF);
	CS_HIGH
	;
}

/*
 * SPI协议W25Q128 Flash储存芯片写入指定页面的指定偏移量位置写入不定长度的字符串
 */
void SPI_WriteFlashPageByte(uint32_t page, uint8_t offset, char str[])
{
	int i = 0;
	CheckBusy();
	WriteEN();
	CS_LOW;
	SPI_write(FLASH_WRITE_PAGE);
	SPI_write((((page - 1) * 0x100) >> 16) & (0xFF));
	SPI_write((((page - 1) * 0x100) >> 8) & (0xFF));
	SPI_write((((page - 1) * 0x100)) & (0xFF));
	for (; offset > 0; offset--)
	{
		SPI_write(0XFF);
	}
	for (i = 0; str[i] != '\0'; i++)
		SPI_write(str[i]);
	CS_HIGH
	;
}

/*
 * SPI协议W25Q128 Flash储存芯片读取上次保存的写入位置
 */
void SPI_FlashFindHeadPage()
{
	uint32_t Page1Data = 0, Page2Data = 0, Page3Data = 0;
	uint8_t tempData[5] = { '\0' };
	CheckBusy();

	SPI_FlashReadPageByte(1, 2, tempData); //读取当前写入的page
	Page1Data = (tempData[0] << 8) | tempData[1] | 0x000000;

	SPI_FlashReadPageByte(2, 2, tempData); //读取当前写入的sector
	Page2Data = (tempData[0] << 8) + tempData[1];

	SPI_FlashReadPageByte(3, 2, tempData); //读取当前写入的读取page
	Page3Data = (tempData[0] << 8) + tempData[1];

	if (Page1Data == 0xFFFF)
	{
		SPI_EraseSector(2);
		HeadPage = 17;
		HeadSector = 2;
	} else
		HeadPage = Page1Data;
	if (Page2Data == ((HeadPage - 1) / 16) + 1)
		HeadSector = Page2Data;
	else
	{
		HeadSector = ((HeadPage - 1) / 16) + 1;
	}
	if (Page3Data == 0xFFFF)
	{
		ReadPage = 17;
	}
}

/*
 * SPI协议W25Q128 Flash储存芯片掉电前保存相关数据
 */
void SPI_FlashLostPower()
{
	uint8_t data[5] = { '\0' };
	data[0] = (HeadPage >> 8) & 0xFF;
	data[1] = (HeadPage) & 0xFF;

	CheckBusy();
	SPI_EraseSector(1);
	CheckBusy();

	WriteEN();
	CS_LOW;
	SPI_write(FLASH_WRITE_PAGE);
	SPI_writeStr(1, (char *) data);
	CS_HIGH
	;

	data[0] = (HeadSector >> 8) & 0xFF;
	data[1] = (HeadSector) & 0xFF;

	CheckBusy();
	WriteEN();
	CS_LOW;
	SPI_write(FLASH_WRITE_PAGE);
	SPI_writeStr(2, (char *) data);
	CS_HIGH
	;

	data[0] = (ReadPage >> 8) & 0xFF;
	data[1] = (ReadPage) & 0xFF;

	CheckBusy();
	WriteEN();
	CS_LOW;
	SPI_write(FLASH_WRITE_PAGE);
	SPI_writeStr(3, (char *) data);
	CS_HIGH
	;
}

extern void clearStr(char *str, uint8_t i);

/*
 * SPI协议W25Q128 Flash储存芯片自动指定长度数组的数据
 * 最大读取长度255,读取之后页面自加
 * 		如果读取指针在写入的之前(大于),将指针置于下一个块的首地址
 * 		如果读取指针在写入之后(小于),等待,返回Wait或者W(视数组大小而定)
 */
void SPI_FlashAutoRead(char data[])
{
	uint8_t length = sizeof(data);//获取数组的长度
	clearStr(data, length);
	if (ReadPage >= 65530) //读取了一个轮回
	{
		ReadPage = 17; //从开头开始读取
	}
	if (HeadSector == ((ReadPage - 1) / 16) + 1) //如果当前读取页面与写入同属于同一个块
	{
		if (ReadPage > HeadPage) //读取指针在写入之前,将指针置于下一个块的首地址
			ReadPage = (HeadSector) * 16 + 1;
		if (ReadPage <= HeadPage) //读取指针在写入之后,等待
		{
			if (length >= 4)
			{
				data[0] = 'W';
				data[1] = 'a';
				data[2] = 'i';
				data[3] = 't';
			} else
				data[0] = 'W';
		}
		return;
	}
	SPI_FlashReadPageByte(ReadPage, length, (uint8_t *) data);
	ReadPage++;
}
