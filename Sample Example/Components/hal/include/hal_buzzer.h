//-------------------------------------------------------------------
// Filename: hal_buzzer.h
// Description: hal buzzer library header file
//-------------------------------------------------------------------
#define BUZZER_FREQ		2730 
//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
/***************************************************************
// Buzzer PORT,PIN
***************************************************************/
//Buzzer
#define HAL_BUZZER_IO_PORT         P0_1
#define HAL_BUZZER_IO_PIN          BV(1)
#define HAL_BUZZER_IO_DDR          P0DIR
#define HAL_BUZZER_IO_SEL          P0SEL

/***************************************************************
// Buzzer status setting
***************************************************************/
#define HAL_BUZZER_OFF()           st(HAL_BUZZER_IO_PORT = 1;)
#define HAL_BUZZER_ON()            st(HAL_BUZZER_IO_PORT = 0;)
#define HAL_BUZZER_TGL()           st(HAL_BUZZER_IO_PORT ^= 1;)

/***************************************************************
// Buzzer input, output setting
***************************************************************/
#define HAL_BUZZER_OUTPUT()        st(P0SEL &= ~BV(1); P0DIR |= BV(1);)

/**************************************************************************************************
 *                                     FUNCTIONS - API
 **************************************************************************************************/
/*
 * Initilization the Buzzer
 */
extern void halBuzzerInit(void);

/*
 * Stop the halBuzzer off
 */
extern void halBuzzerOff(void);

/*
 * Start the halBuzzer on
 */
extern void halBuzzerOn(uint16 ms);
