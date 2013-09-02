//-------------------------------------------------------------------
// Filename: M160.h
// Description: hal M160 PWM library header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------

/***************************************************
// DO PORT, PIN Setting
 **************************************************/
// DO 
#define HAL_DO_PORT    P0_0
#define HAL_DO_PIN     BV(0)
#define HAL_DO_DDR     P0DIR
#define HAL_DO_SEL     P0SEL

/***************************************************
// DO Command PIN Function
***************************************************/   
// Setting DO on, off
#define HAL_DO_OFF() st(HAL_DO_PORT = 1;)
#define HAL_DO_ON()  st(HAL_DO_PORT = 0;)

// Setting DO Output
#define HAL_DO_OUTPUT() st(P0SEL &= ~BV(0); P0DIR |= BV(0);)

/***************************************************
 * Initilization the M160 module
 **************************************************/
void M160_Init(void);

/***************************************************
 * Turn on the M160 module
 **************************************************/
void M160_On(uint8 duty);

/***************************************************
 * Turn off the M160 module
 **************************************************/
void M160_Off(void);



void M160_Timer_Call_Back(uint8 timerId, uint8 channel, uint8 channelMode);
