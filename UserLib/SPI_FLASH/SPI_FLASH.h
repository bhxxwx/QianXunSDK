/*
 * SPI_FLASH.h
 *
 *  Created on: 2020年5月22日
 *      Author: WangXiang
 */

#ifndef SPI_FLASH_SPI_FLASH_H_
#define SPI_FLASH_SPI_FLASH_H_
#include "UserConfig.h"
#include <stdarg.h>

#define  FLASH_WRITE_ENABLE_CMD 		0x06
#define  FLASH_WRITE_DISABLE_CMD		0x04
#define  FLASH_READ_SR_CMD				0x05
#define  FLASH_WRITE_SR_CMD				0x01
#define  FLASH_READ_DATA				0x03
#define  FLASH_FASTREAD_DATA			0x0b
#define  FLASH_WRITE_PAGE				0x02

#define CS_HIGH 	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);digitalWriteA(GPIO_Pin_4, HIGH)
#define CS_LOW  digitalWriteA(GPIO_Pin_4, LOW)

extern int SPI_printf(const char *fmt, ...);

/*----------------------对外调用接口----------------------*/
void SPI_INIT();
void WriteEN();
void SPI_EraseChip();
void SPI_FlashReset();
void SPI_FlashFindHeadPage();
void SPI_FlashLostPower();
void SPI_FlashAutoRead(char data[]);

//----------------------中间调用函数----------------------//
void SPI_write(u8 TxData);
int SPI_writeStr(uint32_t page, char *str);

uint8_t SPI_MasterSendReceiveByte(uint8_t spi_byte);

//void SPI_MasterSendReceiveMultipeByte(uint8_t *spi_byte);
//----------------------SPI Flash接口--------------------------------//
void SPI_WriteFlashPageByte(uint32_t page, uint8_t offset, char str[]);
void SPI_FlashReadPage(uint32_t page, char *str);
void SPI_FlashReadPageByte(uint32_t page, uint8_t number, uint8_t str[]);

void SPI_EraseSector(uint32_t sector);
void CheckBusy();

void ts_itoa(char **buf, unsigned int d, int base);
int ts_formatstring(char *buf, const char *fmt, va_list va);
int ts_formatlength(const char *fmt, va_list va);

#endif /* SPI_FLASH_SPI_FLASH_H_ */
