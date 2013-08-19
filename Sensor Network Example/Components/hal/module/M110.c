#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "hal_adc.h"
#include "M110.h"

void M110_Init(void)
{
    HAL_ADC_PERIPHERAL();
}

uint16 M110_GetValue(void)
{
    uint16 adcValue;
    adcValue = adcSampleSingle(HAL_ADC_REF_AVDD, ADC_12_BIT, 0);
    return adcValue;
}
