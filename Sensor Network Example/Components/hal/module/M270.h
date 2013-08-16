//-------------------------------------------------------------------
// Filename: M270.h
// Description: hal M270 module library header file
//-------------------------------------------------------------------

/***************************************************
// DO PORT, PIN Setting
 **************************************************/
// DO
#define HAL_RELAY_PORT    P0_7
#define HAL_RELAY_PIN     BV(7)
#define HAL_RELAY_DDR     P0DIR
#define HAL_RELAY_SEL     P0SEL

// DO value setting
#define HAL_RELAY_OFF()   st(HAL_RELAY_PORT = 1;)
#define HAL_RELAY_ON()    st(HAL_RELAY_PORT = 0;)

// DO port Setting
#define HAL_IO_RELAY_OUTPUT()  st(P0SEL &= ~BV(7); P0DIR |= BV(7);)

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------

/***************************************************
 * Initilization the M270 module
 **************************************************/
void M270_Init(void);

/***************************************************
 * Get DO the M270 module
 **************************************************/
void M270_SetDO(uint8 udo);

/***************************************************
 * Get DI the M270 module
 **************************************************/
uint8 M270_GetDI(void);