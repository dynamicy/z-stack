//-------------------------------------------------------------------
// Filename: M190.c
// Description: hal M190 library ?¯è??»é˜»(A/D)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
/*
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "hal_adc.h"
*/
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "hal_adc.h"
#include "M190.h"

//-------------------------------------------------------------------
// @fn      M190_Init
// @brief    Set up port M190 ad
// @return  none
//-------------------------------------------------------------------
void M190_Init(void)
{
    // Analog input
    //MCU_IO_PERIPHERAL(HAL_BOARD_IO_ADC_PORT, HAL_BOARD_IO_ADC_CH);
    HAL_ADC_PERIPHERAL();
}

//-------------------------------------------------------------------
// @fn      M190_GetValue
// @brief   Get this M190 module value
// @param   none
// @return  none
//-------------------------------------------------------------------
uint16 M190_GetValue(void)
{
  
//    uint16 adcValue;
//    adcValue = adcSampleSingle(ADC_REF_AVDD, ADC_12_BIT, HAL_BOARD_IO_ADC_CH);
//    return adcValue;
    uint16 adcValue;
    adcValue = adcSampleSingle(HAL_ADC_REF_AVDD, ADC_12_BIT, 0);
    return adcValue;
}
