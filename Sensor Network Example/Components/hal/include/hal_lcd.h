/**************************************************************************************************
  Filename:       hal_lcd.h
  Revised:        $Date: 2007-07-06 10:42:24 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Description:    This file contains the interface to the LCD Service.

  Copyright 2005-2007 Texas Instruments Incorporated. All rights reserved.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef HAL_LCD_H
#define HAL_LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *                                          INCLUDES
 **************************************************************************************************/
#include "hal_board.h"

/**************************************************************************************************
 *                                         CONSTANTS
 **************************************************************************************************/
/* Constants and Defines */
enum
{
    HAL_LCD_RADIX_DEC, HAL_LCD_RADIX_HEX
};
enum
{
    HAL_LCD_LINE_1 = 1, HAL_LCD_LINE_2, 
};

/*
   This to support LCD with extended number of lines (more than 2).
   Don't use these if LCD doesn't support more than 2 lines
*/
#define HAL_LCD_LINE_3      0x03
#define HAL_LCD_LINE_4      0x04
#define HAL_LCD_LINE_5      0x05
#define HAL_LCD_LINE_6      0x06
#define HAL_LCD_LINE_7      0x07
#define HAL_LCD_LINE_8      0x08

/* Max number of chars on a single LCD line */
#define HAL_LCD_MAX_CHARS   16
#define HAL_LCD_MAX_BUFF    25

/**************************************************************************************************
 *                                          MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                         TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/
extern uint8 pLedData;
  
/**************************************************************************************************
 *                                     FUNCTIONS - API
 **************************************************************************************************/

/*
 * Initialize LCD Service
 */
extern void HalLcdInit(void);

/*
 * Write a string to the LCD
 */
extern void HalLcdWriteString ( char *str, uint8 option);

/*
 * Write a value to the LCD
 */
extern void HalLcdWriteValue ( uint32 value, const uint8 radix, uint8 option);

/*
 * Write a value to the LCD
 */
extern void HalLcdWriteScreen( char *line1, char *line2 );

/*
 * Write a Char to the LCD
 */
extern void HalLcdWriteChar( uint8 line, uint8 col, char ch );

/*
 * Write a string followed by a value to the LCD
 */
extern void HalLcdWriteStringValue( char *title, uint16 value, uint8 format, uint8 line );

/*
 * Write a string followed by 2 values to the LCD
 */
extern void HalLcdWriteStringValueValue( char *title, uint16 value1, uint8 format1, uint16 value2, uint8 format2, uint8 line );

/*
 * Write a percentage bar to the LCD (百分比的Bar)
 */
extern void HalLcdDisplayPercentBar( char *title, uint8 value );

/*
 * Convert int32 to Text
 */
extern char *convInt32ToText(int32 value);

/*
 * Display the Uint8 to the LCD
 */
extern void halLcdDisplayUint8(uint8 line, uint8 col, uint8 radix, uint8 value);

/*
 * Display the Uint16 to the LCD
 */
void halLcdDisplayUint16(uint8 line, uint8 col, uint8 radix, uint16 value);

/*
 * Clear the display on LCD
 */
extern void HalLcd_HW_Clear(void);

/*
 * Clear one line on display
 */
extern void HalLcd_ClearLine(uint8 line);

/*
 * Write a character on LCD
 */
extern void HalLcd_WriteChar(uint8 line, uint8 col, char text);

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
