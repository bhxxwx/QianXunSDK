/*
 * usb_interface.h
 *
 *  Created on: 2020年9月2日
 *      Author: wxujs
 */

#ifndef STM32_USB_OTG_DRIVER_INC_USB_INTERFACE_H_
#define STM32_USB_OTG_DRIVER_INC_USB_INTERFACE_H_

#include "USB/inc/Queue.h"
#include "usb_core.h"
#include "usbd_core.h"
#include <stdarg.h>

#define USB_OTG_Rx_Queue_Size 1000
#define USB_OTG_Tx_Queue_Size 1000




extern int USB_printf(const char *fmt, ...);
void ts_itoa(char **buf, unsigned int d, int base);
int ts_formatstring(char *buf, const char *fmt, va_list va);
int ts_formatlength(const char *fmt, va_list va);

void inqueue_RX(uint8_t *pbuff,uint16_t count);
void USB_OTG_VCP_init();

uint32_t USB_OTG_RxRead(uint8_t *buffter, uint32_t buffterSize);
uint32_t USB_OTG_RxAvailable();

uint32_t USB_OTG_TxWrite(uint8_t *buffter, uint32_t writeLen);
uint32_t USB_OTG_TxRead(uint8_t *buffter, uint32_t buffterSize);
#endif /* STM32_USB_OTG_DRIVER_INC_USB_INTERFACE_H_ */
