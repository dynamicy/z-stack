#define HAL_ADC_PORT        P0_0
#define HAL_ADC_CH          BV(0)
#define HAL_ADC_DDR         P0DIR
#define HAL_ADC_SEL         P0SEL

// Peripheral
#define HAL_ADC_PERIPHERAL()    st( P0SEL |= BV(0); )

void M170_Init(void);
uint16 M170_GetValue(void);