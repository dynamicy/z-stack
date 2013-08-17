/**************************************************************************************************
  Filename:       hal_PA2591.h
  Revised:        $Date: 2009-03-09 05:27:36 -0700 (Mon, 09 Mar 2009) $
  Revision:       $Revision: 19340 $

  Description:    This file contains the interface to the UART Service.

  Copyright 2005-2009 Texas Instruments Incorporated. All rights reserved.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

/***************************************************************************************************
 *                                             INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "hal_board.h"

/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                             TYPEDEFS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                           GENERAL VARIABLES
 ***************************************************************************************************/
/***************************************************
// PORT, PIN Setting
 **************************************************/
// PA_Enable
#define HAL_PA_EN_PORT    	P1_1
#define HAL_PA_EN_PIN     	BV(1)
#define HAL_PA_EN_DDR     	P1DIR
#define HAL_PA_EN_SEL     	P1SEL

// Enable PA2591(XT200 : P1_0, MCU_PA : P1_4)
#define HAL_EN_2591_PORT	P1_0
#define HAL_EN_2591_PIN	BV(0)
#define HAL_EN_2591_DDR	P1DIR
#define HAL_EN_2591_SEL	P1SEL

// HGM/LGM(High gain mode/low gain mode)
#define HAL_HGM_PORT		P0_7
#define HAL_HGM_PIN		BV(7)
#define HAL_HGM_DDR		P0DIR
#define HAL_HGM_SEL		P0SEL

#define HAL_PA_EN_OUTPUT() 	st(P1SEL &= ~BV(1); HAL_PA_EN_PORT = 1; P1DIR |= BV(1);)
#define HAL_PA_EN_HIGH()	st(HAL_PA_EN_PORT = 1;)
#define HAL_PA_EN_LOW()		st(HAL_PA_EN_PORT = 0;)

#ifdef XT200_PA 
  #define HAL_EN_2591_OUTPUT() 	st(P1SEL &= ~BV(0); HAL_EN_2591_PORT = 1; P1DIR |= BV(0);)
#else
  #define HAL_EN_2591_OUTPUT() 	st(P1SEL &= ~BV(4); HAL_EN_2591_PORT = 1; P1DIR |= BV(4);)
#endif
  #define HAL_EN_2591_HIGH()	st(HAL_EN_2591_PORT = 1;)
  #define HAL_EN_2591_LOW()	st(HAL_EN_2591_PORT = 0;)

#define HAL_HGM_OUTPUT() 	st(P0SEL &= ~BV(7); HAL_HGM_PORT = 1; P0DIR |= BV(7);)
#define HAL_HGM_HIGH()		st(HAL_HGM_PORT = 1;)
#define HAL_HGM_LOW()		st(HAL_HGM_PORT = 0;)
   
/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/
/*
 *  Initilization the PA2591 IO pin
 */
extern void HalPA2591Init(void);

/*
 *  Enable the RS485 at the startup for high
 */
extern void HalPA2591Enable ( void );

/*
 *  Disable the RS485 at the stop for low
 */
extern void HalPA2591HGM ( void );

/*
 * Read a buffer from the RS485
 */
extern void HalPA2591LGM ( void );