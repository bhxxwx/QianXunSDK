/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      <http://www.st.com/SLA0044>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include "usbd_usr.h"
#include <stdbool.h>
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USBD_USR 
* @brief    This file includes the user application layer
* @{
*/
extern bool VCP_ok;
/** @defgroup USBD_USR_Private_TypesDefinitions
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Defines
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Macros
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Variables
* @{
*/

USBD_Usr_cb_TypeDef USR_cb = {
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,


  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,
};

/**
* @}
*/

/** @defgroup USBD_USR_Private_Constants
* @{
*/

/**
* @}
*/



/** @defgroup USBD_USR_Private_FunctionPrototypes
* @{
*/
/**
* @}
*/


/** @defgroup USBD_USR_Private_Functions
* @{
*/

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{

}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed)
{
//  switch (speed)
//  {
//  case USB_OTG_SPEED_HIGH:
//#if ! defined (USE_STM32446_EVAL) && ! defined (USE_STM32469I_EVAL)
//    LCD_LOG_SetFooter((uint8_t *) "     USB Device Library V1.2.1 [HS]");
//#endif
//    break;
//
//  case USB_OTG_SPEED_FULL:
//#if ! defined (USE_STM32446_EVAL) && ! defined (USE_STM32469I_EVAL)
//    LCD_LOG_SetFooter((uint8_t *) "     USB Device Library V1.2.1 [FS]");
//#endif
//    break;
//  default:
//#if ! defined (USE_STM32446_EVAL) & ! defined (USE_STM32469I_EVAL)
//    LCD_LOG_SetFooter((uint8_t *) "     USB Device Library V1.2.1 [??]");
//#endif
//    break;
//  }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured(void)
{
	VCP_ok=true;
//#if ! defined (USE_STM32446_EVAL) && ! defined (USE_STM32469I_EVAL)
//  LCD_UsrLog("> VCP Interface configured.\n");
//#endif
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
	VCP_ok=false;
//#if ! defined (USE_STM32446_EVAL) && ! defined (USE_STM32469I_EVAL)
//  LCD_UsrLog("> USB Device in Suspend Mode.\n");
//  /* Users can do their application actions here for the USB-Reset */
//#endif
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
	VCP_ok=true;
//#if ! defined (USE_STM32446_EVAL) && ! defined (USE_STM32469I_EVAL)
//  LCD_UsrLog("> USB Device in Idle Mode.\n");
//  /* Users can do their application actions here for the USB-Reset */
//#endif
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected(void)
{

//#if ! defined (USE_STM32446_EVAL) && ! defined (USE_STM32469I_EVAL)
//  LCD_UsrLog("> USB Device Connected.\n");
//#endif
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected(void)
{
	VCP_ok=false;
//#if ! defined (USE_STM32446_EVAL) && ! defined (USE_STM32469I_EVAL)
//  LCD_UsrLog("> USB Device Disconnected.\n");
//#endif
}

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
