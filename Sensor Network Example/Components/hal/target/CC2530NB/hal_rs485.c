/**************************************************************************************************
  Filename:       hal_rs485.c
  Revised:        $Date: 2009-06-12 09:16:43 -0700 (Fri, 12 Jun 2009) $
  Revision:       $Revision: 20142 $

  Description: This file contains the interface to the H/W UART driver.

  Copyright 2006-2009 Texas Instruments Incorporated. All rights reserved.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "hal_defs.h"
#include "hal_types.h"

/* APP */
#if defined(End_Device) || defined(Router_Device)
  #include "zcl_zigbee device.h"
#endif

/* HAL */
#include "hal_board_cfg.h"
#include "mt_uart.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "hal_rs485.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */


/******************************************************************************
 * @fn      HalRS485Init
 * @brief   Initilization the RS485 to transmit/receive
 *****************************************************************************/
void HalRS485Init(void)
{ 
  // Set the RS485 Enable pin to output
  HAL_RS485_EN_OUTPUT();
  // Initilization the Enable pin to low
  HAL_RS485_EN_LOW();
}

/******************************************************************************
 * @fn      HalRS485EnTX
 * @brief   Enable the RS485 to transmit state
 *****************************************************************************/
void HalRS485EnTX(void)
{
  HAL_RS485_EN_HIGH();
  halMcuWaitUs(500);
}

/******************************************************************************
 * @fn      HalRS485DisTX
 * @brief   Disable the RS485 to transmit state
 *****************************************************************************/
void HalRS485DisTX (void)
{
  halMcuWaitUs(500);
  HAL_RS485_EN_LOW();
}

/******************************************************************************
 * @fn      HalRS485EnRX
 * @brief   Enable the RS485 to receive state
 *****************************************************************************/
void HalRS485EnRX ( void )
{
  HAL_RS485_EN_LOW();
  halMcuWaitUs(500);
}

/******************************************************************************
******************************************************************************/
