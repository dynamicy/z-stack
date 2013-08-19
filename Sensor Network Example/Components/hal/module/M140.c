//-------------------------------------------------------------------
// Filename: M140.c
// Description: hal M140 library
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "hal_keypad.h"
#include "hal_timer.h"
#include "M140.h"

//-------------------------------------------------------------------
// LOCAL VARIABLES
//-------------------------------------------------------------------
uint16 M140_Value;

uint8 M140_SpiRW(uint8 ch);
void ADT7301_Start(void);
void ADT7301_Read(void);

//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
//--------------------------------------------
// The M140 module SPI read, write
//--------------------------------------------
uint8 M140_SpiRW(uint8 ch)
{
    uint8 i, cmd, ret;

    cmd = ch;
    for (i = 0; i < 8; i++)
    {
        HAL_SPI_CLK_CLR();
        if (cmd &0x80)
        {
            HAL_SPI_MOSI_SET();
        }
        else
        {
            HAL_SPI_MOSI_CLR();
        }
        cmd <<= 1;
        ret <<= 1;
        HAL_SPI_CLK_SET();
        if (HAL_SPI_MISO_VAL())
        {
            ret |= 0x01;
        }
    }
    HAL_SPI_MOSI_SET();
    return ret;
}

//--------------------------------------------
// Start the ADT7301
//--------------------------------------------
void ADT7301_Start(void)
{
    HAL_SPI_CS_EN();
    M140_SpiRW(0x00);
    M140_SpiRW(0x00);
    HAL_SPI_CS_DIS();
}

//--------------------------------------------
// Read the ADT7301 
//--------------------------------------------
void ADT7301_Read(void)
{
    HAL_SPI_CS_EN();
    M140_Value = M140_SpiRW(0x20);
    M140_Value = (M140_Value << 8) + M140_SpiRW(0x00);
    HAL_SPI_CS_DIS();
}

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
//--------------------------------------------
// Initilization M140 module
//--------------------------------------------
void M140_Init(void)
{   
    HAL_SPI_MISO_INPUT();
    HAL_SPI_MOSI_OUTPUT();
    HAL_SPI_CLK_OUTPUT();
    HAL_SPI_CS_OUTPUT();
    
    ADT7301_Start();
    halMcuWaitMs(2);
    ADT7301_Read();
}

//--------------------------------------------
// Get the M140 sensor module value
//--------------------------------------------
uint16 M140_GetValue(void)
{
    ADT7301_Start();
    halMcuWaitMs(2);
    ADT7301_Read();
    return M140_Value;
}

