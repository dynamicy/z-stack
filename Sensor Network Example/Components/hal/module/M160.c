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
#include "M160.h"
#include "hal_timer.h"
#include "hal_lcd.h"

//-------------------------------------------------------------------
// LOCAL VARIABLES
//-------------------------------------------------------------------
uint8 M160_DutyCount = 0;
uint8 M160_DutyValue = 50;
//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn      timerPwm_ISR
// @brief   ISR framework for the 1k timer component
// @param   none
// @return  none
//-------------------------------------------------------------------
/*
HAL_ISR_FUNCTION(M160_ISR, T4_VECTOR)
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
*/

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
    HAL_DO_OUTPUT();

    // Set prescaler divider value to 128 (8KHz)
    /*
    X_T4CTL |= 0x80;

    X_T4CTL &= ~(0x10); // Stop timer
    T4IE = 0; // Disable interrupt
    */
    uint8 status =
    HalTimerConfig(HAL_TIMER_0,
                   HAL_TIMER_MODE_CTC,
                   HAL_TIMER_CHANNEL_SINGLE,
                   HAL_TIMER_CH_MODE_OUTPUT_COMPARE,
                   1,
                   M160_Timer_Call_Back);
    HalTimerStart(HAL_TIMER_0,100);
}

//-------------------------------------------------------------------
// @fn          M160_On
// @brief       Turn M160 on.
// @param       uint8 duty
// @return      none
//-------------------------------------------------------------------
void M160_On(uint8 duty)
{
    //M160_DutyCount = 0;
    M160_DutyValue = duty;

    //X_T4CTL |= 0x10; // Start timer
    //T4IE = 1; // Enable interrupt
}

//-------------------------------------------------------------------
// @fn          M160_Off
// @brief       Turn M160 off.
// @return      none
//-------------------------------------------------------------------
void M160_Off(void)
{
    //X_T4CTL &= ~(0x10); // Stop timer
    //T4IE = 0; // Disable interrupt
    HAL_DO_OFF();
}

void M160_Timer_Call_Back(uint8 timerId, uint8 channel, uint8 channelMode){
  M160_DutyCount++;
  if (M160_DutyCount >= 100)
      M160_DutyCount = 0;
  if (M160_DutyCount >= M160_DutyValue)
      HAL_DO_ON();
      //HAL_DO_OFF();
  else
      //HAL_DO_ON();
      HAL_DO_OFF();
}