//-------------------------------------------------------------------
// Filename: hal_keypad.c
// Description: HAL keypad control implementation file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_lcd.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "hal_keypad.h"
#include "hal_buzzer.h"

//-------------------------------------------------------------------
// GENERAL VARIABLES
//-------------------------------------------------------------------
uint8 keypad_TaskID;

//-------------------------------------------------------------------
// LOCAL VARIABLES
//-------------------------------------------------------------------
uint8 halKeypadResetCount;

/**************************************************
 * @fn      halKeypadI2cStart
 * @brief   Initilize the keypad I2C
 *************************************************/
void halKeypadI2cStart(void)
{
    HAL_KEYPAD_SCL_SET();
    HAL_KEYPAD_SDA_SET();
    halMcuWaitUs(4);
    HAL_KEYPAD_SDA_CLR();
    halMcuWaitUs(4);
    HAL_KEYPAD_SCL_CLR();
    halMcuWaitUs(4);
}

/*************************************************
 * @fn      halKeypadI2cStop
 * @brief   Stop the keypad I2C
 ************************************************/
void halKeypadI2cStop(void)
{
    HAL_KEYPAD_SDA_CLR();
    halMcuWaitUs(4);
    HAL_KEYPAD_SCL_SET();
    halMcuWaitUs(4);
    HAL_KEYPAD_SDA_SET();
    halMcuWaitUs(4);
}

/*************************************************
 * @fn      halKeypadI2cAck
 * @brief   The keypad I2C Ack
 ************************************************/
void halKeypadI2cAck(void)
{
    HAL_KEYPAD_SDA_CLR();
    HAL_KEYPAD_SCL_SET();
    halMcuWaitUs(4);
    HAL_KEYPAD_SCL_CLR();
    halMcuWaitUs(4);
}

/************************************************
 * @fn      halKeypadI2cNack
 * @brief   The keypad I2C NoAck
 ***********************************************/
void halKeypadI2cNack(void)
{
    HAL_KEYPAD_SDA_SET();
    HAL_KEYPAD_SCL_SET();
    halMcuWaitUs(4);
    HAL_KEYPAD_SCL_CLR();
    halMcuWaitUs(4);
}

/**********************************************
 * @fn      halKeypadI2cWrite
 * @brief   The keypad I2C Write Command
 *********************************************/
void halKeypadI2cWrite(uint8 d)
{
    uint8 i;

    for (i = 0; i < 8; i++)
    {
        if (d &0x80)
        {
            HAL_KEYPAD_SDA_SET();
        }
        else
        {
            HAL_KEYPAD_SDA_CLR();
        }
        HAL_KEYPAD_SCL_SET();
        d = d << 1;
        halMcuWaitUs(4);
        HAL_KEYPAD_SCL_CLR();
        halMcuWaitUs(4);
    }
}

/*******************************************
 * @fn      halKeypadI2cRead
 * @brief   The keypad I2C Read Command
 ******************************************/
uint8 halKeypadI2cRead(void)
{
    uint8 res = 0;
    uint8 cnt;

    HAL_KEYPAD_SDA_SET();
    HAL_KEY_SDA_DIR_IN();
    halMcuWaitUs(4);
    for (cnt = 0; cnt < 8; cnt++)
    {
        HAL_KEYPAD_SCL_SET();
        halMcuWaitUs(4);
        res <<= 1;
        if (HAL_KEYPAD_SDA_VAL())
        {
            res |= 0x01;
        }
        halMcuWaitUs(4);
        HAL_KEYPAD_SCL_CLR();
        halMcuWaitUs(4);
    }
    HAL_KEYPAD_SDA_OUT();
    
    return res;
}

/**********************************************************
 * @fn      halKeypadWriteRegister
 * @brief   Write the keypad I2C register
 *********************************************************/
void halKeypadWriteRegister(uint8 addr, uint8 ch)
{
    halKeypadI2cStart();
    halKeypadI2cWrite(0xE0);
    halKeypadI2cAck();
    halKeypadI2cWrite(0x00);
    halKeypadI2cAck();
    halKeypadI2cWrite(addr); //addr
    halKeypadI2cAck();
    halKeypadI2cWrite(ch); //data
    halKeypadI2cAck();
    halKeypadI2cStop();
}

/********************************************************
 * @fn      halKeypadReadRegister
 * @brief   Read the keypad I2C Register
 *******************************************************/
uint8 halKeypadReadRegister(uint8 addr)
{
    uint8 ch;

    halKeypadI2cStart();
    halKeypadI2cWrite(0xE0);
    halKeypadI2cAck();
    halKeypadI2cWrite(0x00);
    halKeypadI2cAck();
    halKeypadI2cWrite(addr);
    halKeypadI2cAck();
    halKeypadI2cStop();

    halKeypadI2cStart();
    halKeypadI2cWrite(0xE1);
    halKeypadI2cAck();
    ch = halKeypadI2cRead();
    halKeypadI2cNack();
    halKeypadI2cStop();

    return ch;
}

/*****************************************************
 * @fn      halKeypadSensorData
 * @brief   
 ****************************************************/
uint16 halKeypadSensorData(void)
{
    uint16 val;
    halKeypadI2cStart();
    halKeypadI2cWrite(0xE0);
    halKeypadI2cAck();
    halKeypadI2cWrite(0x00);
    halKeypadI2cAck();
    halKeypadI2cWrite(0x08); // 0x08 and 0x09 is read sensor data
    halKeypadI2cAck();
    halKeypadI2cStop();

    halKeypadI2cStart();
    halKeypadI2cWrite(0xE1);
    halKeypadI2cAck();
    val = halKeypadI2cRead();
    halKeypadI2cAck();
    val = (val << 8) | halKeypadI2cRead();
    halKeypadI2cNack();
    halKeypadI2cStop();
    
    return val;
}

/********************************************************
 * @fn      halKeypadInit
 * @brief   Initializes the keypad functionality
 *******************************************************/
void halKeypadInit(void)
{   
    HAL_KEYPAD_SCL_OUT();
    HAL_KEYPAD_SDA_OUT();
    HAL_KEY_SDA_DIR_IN();
   
    halKeypadWriteRegister(0x00, 0x62);
    halKeypadWriteRegister(0x02, 0xF7); // Sense1 bit0-3
    halKeypadWriteRegister(0x03, 0x07); // Sense2 bit0-3
    halKeypadWriteRegister(0x04, 0x00);
    halKeypadWriteRegister(0x05, 0x00);
    halKeypadWriteRegister(0x0E, 0x30);
    halKeypadWriteRegister(0x00, 0x6A); //Reset
}

/*******************************************************************************
 * @fn      halKeypadPushed
 * @brief   This function detects if the keypad is being pushed. 
 * @param   void
 * @return  uint8
 *          '0'~'9' 'A' 'B' 'C' 'D' 'E' 'F' 'U' 'N' '#' '*' : keypad is being pushed
 *          0: Button is not being pushed
 ******************************************************************************/
uint8 halKeypadPushed(void)
{
    HAL_KEYPAD_SCL_OUT();
    HAL_KEYPAD_SDA_OUT();
  
    uint16 val;
    uint8 ch;
    
    val = halKeypadSensorData();
    halLcdDisplayUint16(HAL_LCD_LINE_4,0,HAL_LCD_RADIX_HEX,val);
    switch (val)
    {
        case 0x0404: // 0
            ch = '0';
            break;
        case 0x0082: // 1
            ch = '1';
            break;
        case 0x0084: // 2
            ch = '2';
            break;
        case 0x0088: // 3
            ch = '3';
            break;
        case 0x0102: // 4
            ch = '4';
            break;
        case 0x0104: // 5
            ch = '5';
            break;
        case 0x0108: // 6
            ch = '6';
            break;
        case 0x0202: // 7
            ch = '7';
            break;
        case 0x0204: // 8
            ch = '8';
            break;
        case 0x0208: // 9
            ch = '9';
            break;
        case 0x0402: // *
            ch = '*';
            break;
        case 0x0408: // #
            ch = '#';
            break;
        case 0x0090: // A
            ch = 'A';
            break;
        case 0x0110: // B
            ch = 'B';
            break;
        case 0x0210: // C
            ch = 'C';
            break;
        case 0x00A0: // D
            ch = 'D';
            break;
        case 0x0120: // E
            ch = 'E';
            break;
        case 0x0220: // F
            ch = 'F';
            break;
        case 0x0410: // U
            ch = 'U';
            break;
        case 0x0420: // N
            ch = 'N';
            break;
        default:
            ch = 0;
            break;
    }
    halKeypadResetCount++;
    if (halKeypadResetCount > 100)
    {
        halKeypadResetCount = 0;
        halKeypadWriteRegister(0x00, 0x6A); //Reset
    }
//    if (ch > 0)
//    {
//		halBuzzerOn(BUZZER_FREQ);
//		halMcuWaitMs(20);
//		halBuzzerOff();
//    }
    return ch;


}