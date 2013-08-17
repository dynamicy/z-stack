//-------------------------------------------------------------------
// Filename: M160.h
// Description: hal M160 PWM library header file
//-------------------------------------------------------------------
#define HAL_BOARD_IO_CW_PORT                0
#define HAL_BOARD_IO_CW_PIN                 4

#define HAL_BOARD_IO_CCW_PORT                0
#define HAL_BOARD_IO_CCW_PIN                 5

#define HAL_CW_ON()                 MCU_IO_SET_HIGH(HAL_BOARD_IO_CW_PORT, HAL_BOARD_IO_CW_PIN)
#define HAL_CW_OFF()                MCU_IO_SET_LOW(HAL_BOARD_IO_CW_PORT, HAL_BOARD_IO_CW_PIN)
#define HAL_CW_OUTPUT()             MCU_IO_OUTPUT(HAL_BOARD_IO_CW_PORT, HAL_BOARD_IO_CW_PIN, 0)

#define HAL_CCW_ON()                MCU_IO_SET_HIGH(HAL_BOARD_IO_CCW_PORT, HAL_BOARD_IO_CCW_PIN)
#define HAL_CCW_OFF()               MCU_IO_SET_LOW(HAL_BOARD_IO_CCW_PORT, HAL_BOARD_IO_CCW_PIN)
#define HAL_CCW_OUTPUT()            MCU_IO_OUTPUT(HAL_BOARD_IO_CCW_PORT, HAL_BOARD_IO_CCW_PIN, 0)

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void M280_Init(void);
void M280_CW(uint8 duty);
void M280_CCW(uint8 duty);
void M280_Stop(void);
