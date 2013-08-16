//-------------------------------------------------------------------
// Filename: M100.c
// Description: S35190 RTC library RTC®ÉÄÁ(I2C)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_timer.h"
#include "hal_board.h"
#include "hal_adc.h"
#include "hal_rtc.h"

//-------------------------------------------------------------------
// LOCAL FUNCTION
//-------------------------------------------------------------------
uint8 GetRTCData(void);
void SetRTCCommand(uint8 c);
void SetRTCData(uint8 c);
uint8 ConvertBCD(uint8 ch);
uint8 ConvertHEX(uint8 ch);


uint16 Year;
uint8 Month, Day, Hour, Min, Sec;
//-------------------------------------------------------------------
uint8 GetRTCData(void)
{
    uint8 i, c;

    HAL_RTC_SIO_DIR_IN();
    c = 0;
    for (i = 0; i < 8; i++)
    {
        halMcuWaitUs(20);
        HAL_RTC_SCK_CLR();
        halMcuWaitUs(20);
        HAL_RTC_SCK_SET();
        halMcuWaitUs(20);
        if (HAL_RTC_SIO_VAL())
        {
            c |= (0x01 << i);
        }
    }
    HAL_RTC_SIO_DIR_OUT();
    return c;
}


//-------------------------------------------------------------------
void SetRTCCommand(uint8 c)
{
    uint8 i;

    for (i = 0; i < 8; i++)
    {
        if (c &(0x80 >> i))
        {
            HAL_RTC_SIO_SET();
        }
        else
        {
            HAL_RTC_SIO_CLR();
        }

        halMcuWaitUs(20);
        HAL_RTC_SCK_CLR();
        halMcuWaitUs(20);
        HAL_RTC_SCK_SET();
        halMcuWaitUs(20);
    }
    HAL_RTC_SIO_SET();
}

//-------------------------------------------------------------------
void SetRTCData(uint8 c)
{
    uint8 i;

    for (i = 0; i < 8; i++)
    {
        if (c &(0x01 << i))
        {
            HAL_RTC_SIO_SET();
        }
        else
        {
            HAL_RTC_SIO_CLR();
        }

        halMcuWaitUs(20);
        HAL_RTC_SCK_CLR();
        halMcuWaitUs(20);
        HAL_RTC_SCK_SET();
        halMcuWaitUs(20);
    }
    HAL_RTC_SIO_SET();
}

//-------------------------------------------------------------------
uint8 ConvertBCD(uint8 ch)
{
    return (((ch / 10) *16) + (ch % 10));
}

//-------------------------------------------------------------------
uint8 ConvertHEX(uint8 ch)
{
    return (((ch / 16) *10) + (ch % 16));
}

//-------------------------------------------------------------------
void RTC_Init(void)
{
    uint8 ch,flag=0;

    HAL_RTC_SIO_DIR_OUT();
    HAL_RTC_SCK_DIR_OUT();
    HAL_RTC_CS_DIR_OUT();
    
    HAL_RTC_CS_DIS();
    HAL_RTC_SCK_SET();
    HAL_RTC_SIO_SET();

    HAL_RTC_CS_EN();
    SetRTCCommand(0x61); // Read Register
    ch = GetRTCData();
    halMcuWaitUs(200);
    if (ch &0x80)
    {
        SetRTCCommand(0x60);
        SetRTCData(0x01);
        flag=1;
    }
    HAL_RTC_CS_DIS();

    HAL_RTC_CS_EN();
    SetRTCCommand(0x63); // Read Register
    ch = GetRTCData();
    halMcuWaitUs(200);
    if (ch &0x80)
    {
        SetRTCCommand(0x60);
        SetRTCData(0x01);
        flag=1;
    }
    HAL_RTC_CS_DIS();

    if(flag==1)
    {
        HAL_RTC_CS_EN();
        SetRTCCommand(0x60); // Write Register
        SetRTCData(0x02);
        HAL_RTC_CS_DIS();
        HAL_RTC_CS_EN();
        SetRTCCommand(0x62); // Write Register
        SetRTCData(0x08);
        HAL_RTC_CS_DIS();
    }
}

//-------------------------------------------------------------------
void SetDateTime(void)
{
    uint8 y;

    HAL_RTC_CS_EN();

    if (Year < 2000 || Year > 2099)
    {
        Year = 2000;
    }
    y = (uint8)(Year - 2000);

    SetRTCCommand(0x64);
    SetRTCData(ConvertBCD(y));
    SetRTCData(ConvertBCD(Month));
    SetRTCData(ConvertBCD(Day));
    SetRTCData(ConvertBCD(0));
    SetRTCData(ConvertBCD(Hour));
    SetRTCData(ConvertBCD(Min));
    SetRTCData(ConvertBCD(Sec));

    HAL_RTC_CS_DIS();
}

//-------------------------------------------------------------------
void GetDateTime(void)
{
    uint8 y;

    HAL_RTC_CS_EN();

    SetRTCCommand(0x65);
    y = ConvertHEX(GetRTCData());
    Month = ConvertHEX(GetRTCData());
    Day = ConvertHEX(GetRTCData());
    ConvertHEX(GetRTCData());
    Hour = ConvertHEX(GetRTCData() &0x3F);
    Min = ConvertHEX(GetRTCData());
    Sec = ConvertHEX(GetRTCData() &0x7F);

    HAL_RTC_CS_DIS();

    Year = 2000+(uint16)y;
    if (Year < 2000 || Year > 2099)
    {
        Year = 2000;
    }
}
