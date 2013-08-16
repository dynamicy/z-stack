//-------------------------------------------------------------------
// Filename: M110.h
// Description: M110 module library header file
//-------------------------------------------------------------------
/***************************************************
// A/D PORT, PIN Setting
 **************************************************/
// A/D
#define HAL_ADC_PORT        P0_0
#define HAL_ADC_CH          BV(0)
#define HAL_ADC_DDR         P0DIR
#define HAL_ADC_SEL         P0SEL

/***************************************************
// SPI Command PIN Function
 **************************************************/
// Peripheral
#define HAL_ADC_PERIPHERAL()    st( P0SEL |= BV(0); )

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void M110_Init(void);
uint16 M110_GetValue(void);
