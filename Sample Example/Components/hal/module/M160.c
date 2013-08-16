//-------------------------------------------------------------------
// Filename: M160.c
// Description: hal M160 PWM library
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "M160.h"

//-------------------------------------------------------------------
// LOCAL VARIABLES
//-------------------------------------------------------------------
uint8 M160_DutyCount;
uint8 M160_DutyValue;

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
    M160_DutyCount++;
    if (M160_DutyCount >= 100)
    {
        M160_DutyCount = 0;
    }
    if (M160_DutyCount >= M160_DutyValue)
    {
        HAL_DO_OFF();
    }
    else
    {
        HAL_DO_ON();
    }
}

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn      M160_Init
// @brief    Set up timer 4 to generate an interrupt 1 kHz for PWM
// @return  none
//-------------------------------------------------------------------
void M160_Init(void)
{
    MCU_IO_OUTPUT(HAL_BOARD_IO_DO_PORT, HAL_BOARD_IO_DO_PIN, 1);

    // Set prescaler divider value to 128 (8KHz)
    T4CTL |= 0x80;

    T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
}

//-------------------------------------------------------------------
// @fn          M160_On
// @brief       Turn M160 on.
// @param       uint8 duty
// @return      none
//-------------------------------------------------------------------
void M160_On(uint8 duty)
{
    M160_DutyCount = 0;
    M160_DutyValue = duty;

    T4CTL |= 0x10; // Start timer
    T4IE = 1; // Enable interrupt
}

//-------------------------------------------------------------------
// @fn          M160_Off
// @brief       Turn M160 off.
// @return      none
//-------------------------------------------------------------------
void M160_Off(void)
{
    T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
    HAL_DO_OFF();
}

