//-------------------------------------------------------------------
// Filename: M270.h
// Description: hal M270 module library header file
//-------------------------------------------------------------------

/***************************************************
// Switch KEY1,KEY2
 **************************************************/
// BOARD_KEY_1
#define HAL_BOARD_KEY_1_PORT P0_4
#define HAL_BOARD_KEY_1_PIN  BV(4)
#define HAL_BOARD_KEY_1_DDR  P0DIR
#define HAL_BOARD_KEY_1_SEL  P0SEL

// BOARD_KEY_2
#define HAL_BOARD_KEY_2_PORT P0_5
#define HAL_BOARD_KEY_2_PIN  BV(5)
#define HAL_BOARD_KEY_2_DDR  P0DIR
#define HAL_BOARD_KEY_2_SEL  P0SEL

// Relay_1
#define HAL_BOARD_KEY_2_PORT P0_7
#define HAL_BOARD_KEY_2_PIN  BV(7)
#define HAL_BOARD_KEY_2_DDR  P0DIR
#define HAL_BOARD_KEY_2_SEL  P0SEL

#define HAL_RELAY_OFF()              	MCU_IO_SET_HIGH(HAL_BOARD_RELAY_PORT, HAL_BOARD_RELAY_PIN)
#define HAL_RELAY_ON()               	MCU_IO_SET_LOW(HAL_BOARD_RELAY_PORT, HAL_BOARD_RELAY_PIN)

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void M270_Init(void);
void M270_SetDO(uint8 udo);
uint8 M270_GetDI(void);