//-------------------------------------------------------------------
// Filename: M170.c
// Description: hal M170 library
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "hal_adc.h"
#include "M170.h"

//-----------------------------------------
// INCLUDES
//-----------------------------------------
void M170_Init(void)
{
    // Analog input
    HAL_ADC_PERIPHERAL();
}
//----------------------------------------
// INCLUDES
//----------------------------------------
uint16 M170_GetValue(void)
{
    uint16 adcValue;
    adcValue = adcSampleSingle(HAL_ADC_REF_AVDD, ADC_12_BIT, 0);
    return adcValue;
}