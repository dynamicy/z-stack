//-------------------------------------------------------------------
// Filename: hal_buzzer.c
// Description: hal buzzer library
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_types.h"
#include "hal_board.h"
#include "hal_timer.h"
#include "hal_buzzer.h"

//-------------------------------------------------------------------
// @fn      halBuzzerInit
// @brief    Set up timer 1  to generate an interrupt freq
// @param       uint16 freq
// @return  none
//-------------------------------------------------------------------
void halBuzzerInit(void)
{  
    // Set the buzzer for output
    HAL_BUZZER_OUTPUT();
}

//-------------------------------------------------------------------
// @fn          halBuzzerOff
// @brief       Turn Buzzer off.
// @return      none
//-------------------------------------------------------------------
void halBuzzerOff(void)
{
    HAL_BUZZER_OFF();
}

//-------------------------------------------------------------------
// @fn          halBuzzer
// @brief       Turn Buzzer on.
// @param       uint16 freq
//-------------------------------------------------------------------
void halBuzzerOn(uint16 ms)
{
    int i;
    
    for(i=0; i<ms; i++)
    {
       HAL_BUZZER_TGL();
       halMcuWaitUs(200);
    }
    HAL_BUZZER_OFF();
}
