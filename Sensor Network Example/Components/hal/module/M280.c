//-------------------------------------------------------------------
// Filename: M160.c
// Description: hal M160 PWM library
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "ioCC2530.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "M280.h"

//-------------------------------------------------------------------
// LOCAL VARIABLES
//-------------------------------------------------------------------
uint8 M280_DutyCount;
uint8 M280_DutyValue;

//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn      timerPwm_ISR
// @brief   ISR framework for the 1k timer component
// @param   none
// @return  none
//-------------------------------------------------------------------
//HAL_ISR_FUNCTION(T4_ISR, T4_VECTOR)
HAL_ISR_FUNCTION(T4_ISR, T4_VECTOR)
{
    M280_DutyCount++;
    if (M280_DutyCount >= 100)
    {
        M280_DutyCount = 0;
    }
    if (M280_DutyCount >= M280_DutyValue)
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
void M280_Init(void)
{
    HAL_DO_OUTPUT();

    // Set prescaler divider value to 128 (8KHz)
    X_T4CTL |= 0x80;

    X_T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
}

//-------------------------------------------------------------------
// @fn          M160_On
// @brief       Turn M160 on.
// @param       uint8 duty
// @return      none
//-------------------------------------------------------------------
void M280_On(uint8 duty)
{
    M280_DutyCount = 0;
    M280_DutyValue = duty;

    X_T4CTL |= 0x10; // Start timer
    T4IE = 1; // Enable interrupt
}

//-------------------------------------------------------------------
// @fn          M160_Off
// @brief       Turn M160 off.
// @return      none
//-------------------------------------------------------------------
void M280_Off(void)
{
    X_T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
    HAL_DO_OFF();
}

