/**************************************************************************************************
  Filename:       hal_lcd.c
  Revised:        $Date: 2009-11-09 20:03:17 -0800 (Mon, 09 Nov 2009) $
  Revision:       $Revision: 21106 $

  Description:    This file contains the interface to the HAL LCD Service.


  Copyright 2007 - 2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/**************************************************************************************************
 *                                           INCLUDES
 **************************************************************************************************/
#include "hal_types.h"
#include "hal_lcd.h"
#include "hal_keypad.h"
#include "OSAL.h"
#include "OnBoard.h"
#include "hal_timer.h"
#include <string.h>

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  #include "DebugTrace.h"
#endif

/*******************************************************************************
 *                                          CONSTANTS
 ******************************************************************************/
/* LCD lines */
#define LCD_LINE_COUNT       2
#define LCD_LINE_LENGTH      16
#define LCD_MAX_BUF          25

/* LCD Control */
#define HAL_LCD_EN_PORT      P2_0
#define HAL_LCD_EN_PIN       BV(0)
#define HAL_LCD_EN_DDR	     P2DIR
#define HAL_LCD_EN_SEL	     P2SEL
#define HAL_LCD_RS_PORT      P0_6
#define HAL_LCD_RS_PIN       BV(6)
#define HAL_LCD_RS_DDR	     P0DIR
#define HAL_LCD_RS_SEL	     P0SEL
#define HAL_LCD_DATA_PORT    P1
#define HAL_LCD_DATA_DDR     P1DIR
#define HAL_LCD_DATA_SEL     P1SEL

/* LED Control */
#define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
#define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()
#define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )

/* LCD Setting */
#define HAL_LCD_EN_SET()               st(HAL_LCD_EN_PORT = 1;)
#define HAL_LCD_EN_CLR()               st(HAL_LCD_EN_PORT = 0;)
#define HAL_LCD_RS_SET()               st(HAL_LCD_RS_PORT = 1;)
#define HAL_LCD_RS_CLR()               st(HAL_LCD_RS_PORT = 0;)
#define HAL_LCD_DATA(val)              st(P1 = val;)

/* Setting IO for output */
#define HAL_LCD_EN_OUTPUT()		st(P2SEL &= ~BV(0); P2DIR |= BV(0);)
#define HAL_LCD_RS_OUTPUT()		st(P0SEL &= ~BV(6); P0DIR |= BV(6);)
#define HAL_LCD_DATA_OUTPUT()		st(P1SEL &= 0x00; P1DIR |= 0xFF;)

/* Setting IO_DIR input, output */
#define HAL_LCD_RS_DIR_INPUT()		st(P0DIR &= ~BV(6);)
#define HAL_LCD_RS_DIR_OUTPUT()		st(P0DIR |= BV(6);)

/**************Defines for HW LCD*****************/
/* Function Set */
#define DOT_5X10_OFF                    0x00
#define DOT_5X10_ON                     0x04
#define DUAL_LINE_OFF                   0x00
#define DUAL_LINE_ON                    0x08
#define BUS_8BIT_OFF                    0x00
#define BUS_8BIT_ON                     0x10
#define FUNCTION_SET(options)           lcdCommand(0x20 | (options))

/* Display clear */
#define DISPLAY_CLEAR()                 lcdCommand(0x01)

/* Set display control */
#define DISPLAY_CTRL_ON                 0x04
#define DISPLAY_CTRL_OFF                0x00
#define DISPLAY_CTRL_BLINK_ON           0x01
#define DISPLAY_CTRL_BLINK_OFF          0x00
#define DISPLAY_CTRL_CURSOR_ON          0x02
#define DISPLAY_CTRL_CURSOR_OFF         0x00
#define SET_DISPLAY_CTRL(options)       lcdCommand(0x08 | (options))

/* Set enter mode */
#define INCREMENT_ON                    0x02
#define INCREMENT_OFF                   0x00
#define DISPLAY_SHIFT_ON                0x01
#define DISPLAY_SHIFT_OFF               0x00
#define SET_ENTER_MODE(options)         lcdCommand(0x04 | (options))

/* Set display control */
#define SHITF_DISPLAY_ON                0x04
#define SHITF_DISPLAY_OFF               0x00
#define SHITF_RIGHT_ON                  0x02
#define SHITF_RIGHT_OFF                 0x00
#define SET_CURSOR_SHIFT(options)       lcdCommand(0x10 | (options))

/* Set Display Start Line */
#define LINE1                           0x00
#define LINE2                           0x40
#define SET_DISPLAY_LINE(line)          lcdCommand(0x80 | (line))

/* Set DD/ CGRAM address */
#define SET_DDRAM_ADDR(charIndex)       lcdCommand(0x80 | (charIndex))
#define SET_GCRAM_CHAR(specIndex)       lcdCommand(0x40 | (specIndex))

/* Set ICONRAM address */
#define CONTRAST_CTRL_REGISTER          0x10
#define SET_ICONRAM_ADDR(addr)          lcdCommand(0x40 | (addr))

/*******************************************************************************
 *                                           MACROS
 ******************************************************************************/

#define HAL_IO_SET(port, pin, val)        HAL_IO_SET_PREP(port, pin, val)
#define HAL_IO_SET_PREP(port, pin, val)   st( P##port##_##pin## = val; )

#define HAL_CONFIG_IO_OUTPUT(port, pin, val)      HAL_CONFIG_IO_OUTPUT_PREP(port, pin, val)
#define HAL_CONFIG_IO_OUTPUT_PREP(port, pin, val) st( P##port##SEL &= ~BV(pin); \
                                                      P##port##_##pin## = val; \
                                                      P##port##DIR |= BV(pin); )

#define HAL_CONFIG_IO_PERIPHERAL(port, pin)      HAL_CONFIG_IO_PERIPHERAL_PREP(port, pin)
#define HAL_CONFIG_IO_PERIPHERAL_PREP(port, pin) st( P##port##SEL |= BV(pin); )

/**************************************************************************************************
 *                                       GLOBAL VARIABLES
 **************************************************************************************************/
static uint8 *Lcd_Line1;
uint8 pLedData;

/*******************************************************************************
 *                                       FUNCTIONS - API
 ******************************************************************************/
#if (HAL_LCD == TRUE)
void HalLcd_HW_WaitUs(uint16 i);
void HalLcd_HW_Clear(void);
void HalLcd_HW_ClearAllSpecChars(void);
void HalLcd_HW_SetContrast(uint8 value);
void HalLcd_HW_WriteLine(uint8 line, const char *pText);

static void initLcd(void);
static void lcdCommand(uint8 command);
static void lcdData(uint8 ch);
static void lcdUpdateChar(uint8 line, uint8 col, char ch);
#endif //LCD

/*******************************************************************************
 * @fn      HalLcdInit
 *
 * @brief   Initilize LCD Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 ******************************************************************************/
void HalLcdInit(void)
{
  Lcd_Line1 = NULL;
#if (HAL_LCD == TRUE)
  initLcd();
#endif
}

/*************************************************************************************************
 *                    LCD EMULATION FUNCTIONS
 *
 * Some evaluation boards are equipped with Liquid Crystal Displays
 * (LCD) which may be used to display diagnostic information. These
 * functions provide LCD emulation, sending the diagnostic strings
 * to Z-Tool via the RS232 serial port. These functions are enabled
 * when the "LCD_SUPPORTED" compiler flag is placed in the makefile.
 *
 * Most applications update both lines (1 and 2) of the LCD whenever
 * text is posted to the device. This emulator assumes that line 1 is
 * updated first (saved locally) and the formatting and send operation
 * is triggered by receipt of line 2. Nothing will be transmitted if
 * only line 1 is updated.
 *
 *************************************************************************************************/

/*******************************************************************************
 * @fn      HalLcdWriteString
 *
 * @brief   Write a string to the LCD
 *
 * @param   str    - pointer to the string that will be displayed
 *          option - display options
 *
 * @return  None
 ******************************************************************************/
void HalLcdWriteString ( char *str, uint8 option)
{
#if (HAL_LCD == TRUE)

  uint8 strLen = 0;
  uint8 totalLen = 0;
  uint8 *buf;
  uint8 tmpLen;

  if ( Lcd_Line1 == NULL )
  {
    Lcd_Line1 = osal_mem_alloc( HAL_LCD_MAX_CHARS+1 );
  }

  strLen = (uint8)osal_strlen( (char*)str );

  /* Check boundries */
  if ( strLen > HAL_LCD_MAX_CHARS )
    strLen = HAL_LCD_MAX_CHARS;

  if ( option == HAL_LCD_LINE_1 )
  {
    /* Line 1 gets saved for later */
    osal_memcpy( Lcd_Line1, str, strLen );
    Lcd_Line1[strLen] = '\0';
  }
  else
  {
    /* Line 2 triggers action */
    tmpLen = (uint8)osal_strlen( (char*)Lcd_Line1 );
    totalLen =  tmpLen + 1 + strLen + 1;
    buf = osal_mem_alloc( totalLen );
    if ( buf != NULL )
    {
      /* Concatenate strings */
      osal_memcpy( buf, Lcd_Line1, tmpLen );
      buf[tmpLen++] = ' ';
      osal_memcpy( &buf[tmpLen], str, strLen );
      buf[tmpLen+strLen] = '\0';

/*****************¥h°£HalLcdWriteString±quart¿é¥X******************************/      
//      /* Send it out */
//#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
//
//#if defined(SERIAL_DEBUG_SUPPORTED)
//      debug_str( (uint8*)buf );
//#endif //LCD_SUPPORTED
//
//#endif //ZTOOL_P1
/******************************************************************************/
      
      /* Free mem */
      osal_mem_free( buf );
    }
  }
  /* Display the string */
  HalLcd_HW_WriteLine (option, str);
#endif //HAL_LCD
}

/**************************************************************************************************
 * @fn      HalLcdWriteValue
 *
 * @brief   Write a value to the LCD
 *
 * @param   value  - value that will be displayed
 *          radix  - 8, 10, 16
 *          option - display options
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteValue ( uint32 value, const uint8 radix, uint8 option)
{
#if (HAL_LCD == TRUE)
  uint8 buf[LCD_MAX_BUF];

  _ltoa( value, &buf[0], radix );
  HalLcdWriteString( (char*)buf, option );
#endif
}

/**************************************************************************************************
 * @fn      HalLcdWriteScreen
 *
 * @brief   Write a value to the LCD
 *
 * @param   line1  - string that will be displayed on line 1
 *          line2  - string that will be displayed on line 2
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteScreen( char *line1, char *line2 )
{
#if (HAL_LCD == TRUE)
  HalLcdWriteString( line1, 1 );
  HalLcdWriteString( line2, 2 );
#endif
}

/**************************************************************************************************
 * @fn      HalLcdWriteStringValue
 *
 * @brief   Write a string followed by a value to the LCD
 *
 * @param   title  - Title that will be displayed before the value
 *          value  - value
 *          format - redix
 *          line   - line number
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteStringValue( char *title, uint16 value, uint8 format, uint8 line )
{
#if (HAL_LCD == TRUE)
  uint8 tmpLen;
  uint8 buf[LCD_MAX_BUF];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  osal_memcpy( buf, title, tmpLen );
  buf[tmpLen] = ' ';
  err = (uint32)(value);
  _ltoa( err, &buf[tmpLen+1], format );
  HalLcdWriteString( (char*)buf, line );		
#endif
}

/**************************************************************************************************
 * @fn      HalLcdWriteStringValue
 *
 * @brief   Write a string followed by a value to the LCD
 *
 * @param   title   - Title that will be displayed before the value
 *          value1  - value #1
 *          format1 - redix of value #1
 *          value2  - value #2
 *          format2 - redix of value #2
 *          line    - line number
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteStringValueValue( char *title, uint16 value1, uint8 format1,
                                  uint16 value2, uint8 format2, uint8 line )
{

#if (HAL_LCD == TRUE)

  uint8 tmpLen;
  uint8 buf[LCD_MAX_BUF];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  if ( tmpLen )
  {
    osal_memcpy( buf, title, tmpLen );
    buf[tmpLen++] = ' ';
  }

  err = (uint32)(value1);
  _ltoa( err, &buf[tmpLen], format1 );
  tmpLen = (uint8)osal_strlen( (char*)buf );

  buf[tmpLen++] = ',';
  buf[tmpLen++] = ' ';
  err = (uint32)(value2);
  _ltoa( err, &buf[tmpLen], format2 );

  HalLcdWriteString( (char *)buf, line );		

#endif
}

/**************************************************************************************************
 * @fn      HalLcdDisplayPercentBar
 *
 * @brief   Display percentage bar on the LCD
 *
 * @param   title   -
 *          value   -
 *
 * @return  None
 **************************************************************************************************/
void HalLcdDisplayPercentBar( char *title, uint8 value )
{
#if (HAL_LCD == TRUE)

  uint8 percent;
  uint8 leftOver;
  uint8 buf[17];
  uint32 err;
  uint8 x;

  /* Write the title: */
  HalLcdWriteString( title, HAL_LCD_LINE_1 );

  if ( value > 100 )
    value = 100;

  /* convert to blocks */
  percent = (uint8)(value / 10);
  leftOver = (uint8)(value % 10);

  /* Make window */
  osal_memcpy( buf, "[          ]  ", 15 );

  for ( x = 0; x < percent; x ++ )
  {
    buf[1+x] = '>';
  }

  if ( leftOver >= 5 )
    buf[1+x] = '+';

  err = (uint32)value;
  _ltoa( err, (uint8*)&buf[13], 10 );

  HalLcdWriteString( (char*)buf, HAL_LCD_LINE_2 );

#endif

}


#if (HAL_LCD == TRUE)
/**************************************************************************************************
 *                                    HARDWARE LCD
 **************************************************************************************************/

/*******************************************************************************
 * @fn      initLcd
 *
 * @brief   Initilize HW LCD Driver.
 *
 * @param   None
 *
 * @return  None
 ******************************************************************************/
void initLcd(void)
{   
  
    // LCD Setting output
    HAL_LCD_EN_OUTPUT();
    HAL_LCD_RS_OUTPUT();	
    HAL_LCD_DATA_OUTPUT();
    
    // Perform the initialization sequence
    HAL_LCD_RS_DIR_OUTPUT();  
    HAL_LCD_EN_CLR();
    halMcuWaitMs(30);

    FUNCTION_SET(BUS_8BIT_ON);
    halMcuWaitMs(2);
    FUNCTION_SET(BUS_8BIT_ON);
    halMcuWaitMs(2);
    FUNCTION_SET(BUS_8BIT_ON);
    halMcuWaitMs(2);
    FUNCTION_SET(BUS_8BIT_ON | DUAL_LINE_ON);
    halMcuWaitMs(2);
    FUNCTION_SET(BUS_8BIT_ON | DUAL_LINE_ON);
    halMcuWaitMs(2);
    SET_CURSOR_SHIFT(SHITF_DISPLAY_OFF);
    SET_ENTER_MODE(INCREMENT_ON);
    DISPLAY_CLEAR();
    halMcuWaitMs(2);
    // Clear the display
    HalLcd_HW_Clear();
    SET_DISPLAY_CTRL(DISPLAY_CTRL_ON | DISPLAY_CTRL_BLINK_OFF | DISPLAY_CTRL_CURSOR_OFF);
}

/*******************************************************************************
 * @fn      HalLcd_HW_Control
 *
 * @brief   Write 1 command to the LCD
 *
 * @param   uint8 cmd - command to be written to the LCD
 *
 * @return  None
 ******************************************************************************/
//-------------------------------------------------------------------
// @fn          lcdCommand
// @brief       Send command to display
// @param       uint8 command
// @return      none
//-------------------------------------------------------------------
static void lcdCommand(uint8 command)
{
    HAL_LCD_RS_DIR_OUTPUT();
    HAL_LCD_RS_CLR();
    HAL_LCD_DATA(command);
    HAL_LCD_EN_SET();
    halMcuWaitUs(2);
    HAL_LCD_EN_CLR();
    halMcuWaitUs(80);
    HAL_LCD_DATA(pLedData);
}

/*******************************************************************************
 * @fn      HalLcd_HW_Write
 *
 * @brief   Write 1 byte to the LCD
 *
 * @param   uint8 data - data to be written to the LCD
 *
 * @return  None
 ******************************************************************************/
//-------------------------------------------------------------------
// @fn          lcdData
// @brief       Write character to display
// @param       uint8 ch - character to display
// @return      none
//-------------------------------------------------------------------
static void lcdData(uint8 ch)
{
    HAL_LCD_RS_DIR_OUTPUT(); 
    HAL_LCD_RS_SET();
    HAL_LCD_DATA(ch);
    HAL_LCD_EN_SET();
    halMcuWaitUs(2);
    HAL_LCD_EN_CLR();
    halMcuWaitUs(80);
    HAL_LCD_DATA(pLedData);
}

/**************************************************************************************************
 * @fn          HalLcd_HW_SetContrast
 *
 * @brief       Set display contrast
 *
 * @param       uint8 value - contrast value
 *
 * @return      none
 **************************************************************************************************/
void HalLcd_HW_SetContrast(uint8 value)
{
  SET_ICONRAM_ADDR(CONTRAST_CTRL_REGISTER);
  lcdData(value);
}

/*******************************************************************************
 * @fn      HalLcd_HW_Clear
 *
 * @brief   Clear the HW LCD
 *
 * @param   None
 *
 * @return  None
 ******************************************************************************/
void HalLcd_HW_Clear(void)
{
    uint8 n;
    SET_DISPLAY_LINE(LINE1);
    for (n = 0; n < LCD_LINE_LENGTH; n++)
    {
        lcdData(' ');
    }
    SET_DISPLAY_LINE(LINE2);
    for (n = 0; n < LCD_LINE_LENGTH; n++)
    {
        lcdData(' ');
    }
}
/*******************************************************************************
 * @fn      HalLcd_ClearLine
 *
 * @brief   Clear one line on display
 *
 * @param   uint8 line - display line
 *
 * @return  None
 ******************************************************************************/
void HalLcd_ClearLine(uint8 line)
{
   uint8 n;
    if (line == HAL_LCD_LINE_2)
    {
        SET_DISPLAY_LINE(LINE2);
    }
    else
    {
        SET_DISPLAY_LINE(LINE1);
    }
    for (n = 0; n < LCD_LINE_LENGTH; n++)
    {
        lcdData(' ');
    }
}


/**************************************************************************************************
 * @fn      HalLcd_HW_ClearAllSpecChars
 *
 * @brief   Clear all special chars
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_ClearAllSpecChars(void)
{
  uint8 n = 0;

  SET_GCRAM_CHAR(0);
  for (n = 0; n < (8 * 8); n++)
  {
    lcdData(0x00);
  }
}

/*******************************************************************************
 * @fn      HalLcd_HW_WriteChar
 *
 * @brief   Write one char to the display
 *
 * @param   uint8 line - line number that the char will be displayed
 *          uint8 col - colum where the char will be displayed
 *
 * @return  None
 ******************************************************************************/
/************************************************************
 * @fn      HalLcd_HW_WriteChar
************************************************************/
static void lcdUpdateChar( uint8 line, uint8 col, char ch )
{
    if (line == HAL_LCD_LINE_2)
    {
        SET_DISPLAY_LINE(LINE2 + col);
    }
    else
    {
        SET_DISPLAY_LINE(LINE1 + col);
    }
    lcdData(ch);
}
/*********************************************************************/
void HalLcdWriteChar( uint8 line, uint8 col, char ch )
{
    if (line == HAL_LCD_LINE_2)
    {
        SET_DISPLAY_LINE(LINE2 + col);
    }
    else
    {
        SET_DISPLAY_LINE(LINE1 + col);
    }
    lcdData(ch);
}

/**************************************************************************************************
 * @fn          halLcdWriteLine
 *
 * @brief       Write one line on display
 *
 * @param       uint8 line - display line
 *              char *pText - text buffer to write
 *
 * @return      none
 **************************************************************************************************/
void HalLcd_HW_WriteLine(uint8 line, const char *pText)
{
  uint8 count;
  uint8 totalLength = (uint8)osal_strlen( (char *)pText );

  /* Write the content first */
  for (count=0; count<totalLength; count++)
  {
    lcdUpdateChar(line, count, (*(pText++)));
  }

  /* Write blank spaces to rest of the line */
  for(count=totalLength; count<LCD_LINE_LENGTH;count++)
  {
    lcdUpdateChar(line, count, ' ');
  }
}

/*******************************************************************************
 * @fn      HalLcd_HW_WaitUs
 *
 * @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
 *
 * @param   x us. range[0-65536]
 *
 * @return  None
 ******************************************************************************/
void HalLcd_HW_WaitUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}

/*******************************************************************************
* @fn          convInt32ToText
* @brief       Converts 32 bit int to text
* @param       int32 value
* @return      char* - pointer to text buffer which is a file scope allocated array
 ******************************************************************************/
char *convInt32ToText(int32 value)
{
    static char pValueToTextBuffer[12];
    char *pLast;
    char *pFirst;
    char last;
    uint8 negative;

    pLast = pValueToTextBuffer;

    // Record the sign of the value
    negative = (value < 0);
    value = ABS(value);

    // Print the value in the reverse order
    do
    {
        *(pLast++) = '0' + (uint8)(value % 10);
        value /= 10;
    }
    while (value);

    // Add the '-' when the number is negative, and terminate the string
    if (negative)
    {
        *(pLast++) = '-';
    }
    *(pLast--) = 0x00;

    // Now reverse the string
    pFirst = pValueToTextBuffer;
    while (pLast > pFirst)
    {
        last =  *pLast;
        *(pLast--) =  *pFirst;
        *(pFirst++) = last;
    }
    return pValueToTextBuffer;
}

static const char hex[] = 
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};
/*******************************************************************************
* @fn          halLcdDisplayUint8
* @brief       Write a 8 bit value on the specified line on the LCD
* @param       uint8 value - value to display
*              uint8 radix - must be HAL_LCD_RADIX_DEC
*              uint8 line - line on display
* @return      none
 ******************************************************************************/
void halLcdDisplayUint8(uint8 line, uint8 col, uint8 radix, uint8 value)
{
    if (radix == HAL_LCD_RADIX_DEC)
    {
        uint8 n = 0;
        char *pValue = convInt32ToText((uint32)value);
        while (*pValue)
        {
            HalLcdWriteChar(line, col + n++, *(pValue++));
        }
        while (n < 3)
        {
            HalLcdWriteChar(line, col + n++, ' ');
        }
    }
    else if (radix == HAL_LCD_RADIX_HEX)
    {
        HalLcdWriteChar(line, col, hex[(value &0xF0) >> 4]);
        HalLcdWriteChar(line, col + 1, hex[(value &0x0F)]);
    }
}

/*******************************************************************************
 @fn          halLcdDisplayUint16
 @brief       Write a 16 bit value on the specified line on the LCD
 @param       uint16 value - value to display
              uint8 radix - must be HAL_LCD_RADIX_DEC
              uint8 line - line on display
 @return      none
  *****************************************************************************/
void halLcdDisplayUint16(uint8 line, uint8 col, uint8 radix, uint16 value)
{
    if (radix == HAL_LCD_RADIX_DEC)
    {
        uint8 n = 0;
        char *pValue = convInt32ToText((uint32)value);
        while (*pValue)
        {
            lcdUpdateChar(line, col + n++, *(pValue++));
        }
        while (n < 5)
        {
            lcdUpdateChar(line, col + n++, ' ');
        }
    }
    else if (radix == HAL_LCD_RADIX_HEX)
    {
        lcdUpdateChar(line, col, hex[(value &0xF000) >> 12]);
        lcdUpdateChar(line, col + 1, hex[(value &0x0F00) >> 8]);
        lcdUpdateChar(line, col + 2, hex[(value &0x00F0) >> 4]);
        lcdUpdateChar(line, col + 3, hex[(value &0x000F)]);
    }
}
#endif
/**************************************************************************************************
**************************************************************************************************/



