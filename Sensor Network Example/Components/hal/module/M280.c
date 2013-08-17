//-------------------------------------------------------------------
// Filename: M280.c
// Description: hal M280 Motor PWM library °¨¹F±±¨î(PWM)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
/*
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "M280.h"
*/
#include "hal_defs.h"
#include "ioCC2530.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "M280.h"

uint8 M280_MotorCount;
uint8 M280_MotorValue;
uint8 M280_MotorCW;

//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn      timerPwm_ISR
// @brief   ISR framework for the 1k timer component
// @param   none
// @return  none
//-------------------------------------------------------------------
HAL_ISR_FUNCTION(T4_ISR, T4_VECTOR)
{
    M280_MotorCount++;
    if (M280_MotorCount >= 100)
    {
        M280_MotorCount = 0;
    }
    if (M280_MotorCount >= M280_MotorValue)
    {
        HAL_CW_OFF();
        HAL_CCW_OFF();
    }
    else
    {
        if (M280_MotorCW == 1)
        {
            HAL_CW_ON();
        }
        else
        {
            HAL_CCW_ON();
        }
    }
}

//-------------------------------------------------------------------
// @fn      M280_Init
// @brief   Set up timer 4 to generate an interrupt 10 kHz for PWM
// @return  none
//-------------------------------------------------------------------
void M280_Init(void)
{
    HAL_CW_OUTPUT();
    HAL_CCW_OUTPUT();
    HAL_CW_OFF();
    HAL_CCW_OFF();

    MCU_IO_OUTPUT(HAL_BOARD_IO_DO_PORT, HAL_BOARD_IO_DO_PIN, 1);
    MCU_IO_OUTPUT(HAL_BOARD_IO_DO_PORT, HAL_BOARD_IO_DO_PIN, 1);

    // Set prescaler divider value to 128 (8KHz)
    //T4CTL |= 0xE0; // 0x80
    T4CTL |= 0x80; // 0x80

    T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
}

//-------------------------------------------------------------------
void M280_CW(uint8 duty)
{
    M280_MotorCount = 0;
    M280_MotorValue = duty;
    M280_MotorCW = 1;
    T4CTL |= 0x10; // Start timer
    T4IE = 1; // Enable interrupt
}

//-------------------------------------------------------------------
void M280_CCW(uint8 duty)
{
    M280_MotorCount = 0;
    M280_MotorValue = duty;
    M280_MotorCW = 0;
    T4CTL |= 0x10; // Start timer
    T4IE = 1; // Enable interrupt
}

//-------------------------------------------------------------------
void M280_Stop(void)
{
    T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt

    HAL_CW_OFF();
    HAL_CCW_OFF();
}
