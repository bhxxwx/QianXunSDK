/*
 * usb_interface.c
 *
 *  Created on: 2020年9月2日
 *      Author: wxujs
 */
#ifdef STM32F10X_CL
#include "usb_interface.h"
#include "UserConfig.h"
#include "USB/inc/Queue.h"
#include "usb_core.h"
static QUEUE8_t m_QueueUsbComRx = { 0 };
static QUEUE8_t m_QueueUsbComTx = { 0 };
static uint8_t m_UsbComRxBuf[USB_OTG_Rx_Queue_Size] = { 0 };
static uint8_t m_UsbComTxBuf[USB_OTG_Tx_Queue_Size] = { 0 };
uint8_t Txbuf[USB_OTG_Tx_Queue_Size];

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;
uint8_t Rxbuffer[64];
__IO uint32_t receive_count = 1;
extern __IO uint32_t data_sent;
bool TxFlag = false, VCP_ok = false;
extern __IO uint32_t data_sent;

__attribute__((alias("USB_iprintf"))) int USB_printf(const char *fmt, ...);

/*
 * STM32F10X_CL系列单片机USB OTG外设初始化，并且初始化消息队列
 */
void USB_OTG_VCP_init()
{
	data_sent = 1;
	QUEUE_PacketCreate(&m_QueueUsbComRx, m_UsbComRxBuf, sizeof(m_UsbComRxBuf));
	QUEUE_PacketCreate(&m_QueueUsbComTx, m_UsbComTxBuf, sizeof(m_UsbComTxBuf));
	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
	DCD_EP_PrepareRx(&USB_OTG_dev, CDC_OUT_EP, Rxbuffer, receive_count);
}

/*
 * USB_OTG接收到的数据入队列
 * 		pbuff: 队列名称
 * 		count: 数据数量
 */
void inqueue_RX(uint8_t *pbuff, uint16_t count)
{
	QUEUE_PacketIn(&m_QueueUsbComRx, pbuff, count);
}

/*
 * USB_OTG数据输出
 * 		buffer: 输出数据保存的位置
 * 		bufferSize: 希望从队列中获取的数据长度(位数)
 * 	return: 本次从队列中读取数据的数量
 */
uint32_t USB_OTG_RxRead(uint8_t *buffter, uint32_t buffterSize)
{
	return QUEUE_PacketOut(&m_QueueUsbComRx, buffter, buffterSize);
}

/*
 * 当前USB_OTG_Rx缓冲区待读取的数据长度
 */
uint32_t USB_OTG_RxAvailable()
{
	return QUEUE_PacketLengthGet(&m_QueueUsbComRx);
}

uint32_t USB_OTG_TxWrite(uint8_t *buffter, uint32_t writeLen)
{
	return QUEUE_PacketIn(&m_QueueUsbComTx, buffter, writeLen);
}

uint32_t USB_OTG_TxRead(uint8_t *buffter, uint32_t buffterSize)
{
	return QUEUE_PacketOut(&m_QueueUsbComTx, buffter, buffterSize);
}
static uint32_t count = 0;
int USB_iprintf(const char *fmt, ...)
{
	if(USB_OTG_dev.regs.INEP_REGS[1]->DIEPCTL==0x498040||USB_OTG_dev.regs.INEP_REGS[1]->DIEPCTL==0x488040)
	{//端点缓冲区为空
		VCP_ok=true;
	}
	if (data_sent == 0 || !VCP_ok)
		return 0;
	int length = 0;
	va_list va;
	va_start(va, fmt);
	length = ts_formatlength(fmt, va);
	va_end(va);
	{
		char buf[length];
		va_start(va, fmt);
		length = ts_formatstring(buf, fmt, va);

		USB_OTG_TxWrite((uint8_t *) buf, length);
//		DCD_EP_Tx(&USB_OTG_dev, CDC_IN_EP, (uint8_t *)buf, length);
//		DCD_EP_Tx(&USB_OTG_dev, CDC_IN_EP, buf, length);

		for (; QUEUE_PacketLengthGet(&m_QueueUsbComTx) > 0;)
		{
			data_sent = 0;
			count = USB_OTG_TxRead(Txbuf, 64);
			if (count)
				DCD_EP_Tx(&USB_OTG_dev, CDC_IN_EP, Txbuf, count);
			while (data_sent == 0)
			{
				delay_us(500);
				uint16_t delay_count=500;
				while(delay_count)
				if(USB_OTG_dev.regs.INEP_REGS[1]->DIEPCTL!=0x498040||USB_OTG_dev.regs.INEP_REGS[1]->DIEPCTL!=0x488040)
				{//端点缓冲的数据没有被发送完
					delay_count--;
					delay_us(5);//延时等待
				}
				VCP_ok=false;//超时,代表VCP没有准备好,或者USB接收端没打开
				break;
			}
		}
		va_end(va);
	}
	return length;
}

/*-----------------------------------USB_OTG 中断----------------------------------------*/
void OTG_FS_WKUP_IRQHandler(void)
{
	if (USB_OTG_dev.cfg.low_power)
	{
		*(uint32_t *) (0xE000ED10) &= 0xFFFFFFF9;
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line18);
}

extern uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);
#ifdef USE_USB_OTG_HS
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
	USBD_OTG_ISR_Handler(&USB_OTG_dev);
}
#endif
