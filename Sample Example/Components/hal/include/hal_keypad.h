//-------------------------------------------------------------------
// Filename: hal_keypad.h
// Description:  HAL keypad control header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// GLOBAL FUNCTIONS and MACROS
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//MACROS
//-------------------------------------------------------------------

/**********************************************
// Keypad I2C PORT,PIN
***********************************************/
//KEYPAD SDA
#define HAL_KEYPAD_SDA_PORT         P2_3
#define HAL_KEYPAD_SDA_PIN          BV(3)
#define HAL_KEYPAD_SDA_DDR          P2DIR
#define HAL_KEYPAD_SDA_SEL          P2SEL
//KEYPAD SCL
#define HAL_KEYPAD_SCL_PORT         P2_4
#define HAL_KEYPAD_SCL_PIN          BV(4)
#define HAL_KEYPAD_SCL_DDR          P2DIR
#define HAL_KEYPAD_SCL_SEL          P2SEL

// Keypad I2C
#define HAL_BOARD_IO_KEY_SCL_PORT           2
#define HAL_BOARD_IO_KEY_SCL_PIN            4
#define HAL_BOARD_IO_KEY_SDA_PORT           2
#define HAL_BOARD_IO_KEY_SDA_PIN            3

/**********************************************
// Keypad status setting
***********************************************/
// Keypad SDA, SCL high, low setting
#define HAL_KEYPAD_SCL_SET()             st(HAL_KEYPAD_SCL_PORT = 1;)
#define HAL_KEYPAD_SCL_CLR()             st(HAL_KEYPAD_SCL_PORT = 0;)
#define HAL_KEYPAD_SDA_SET()             st(HAL_KEYPAD_SDA_PORT = 1;)
#define HAL_KEYPAD_SDA_CLR()             st(HAL_KEYPAD_SDA_PORT = 0;)
//// Keypad SDA, SCL input , output setting
#define HAL_KEYPAD_SDA_OUT()             st(P2SEL &= ~BV(3); P2DIR |= BV(3);)
#define HAL_KEYPAD_SCL_OUT()             st(P2SEL &= ~BV(4); P2DIR |= BV(4);)
#define HAL_KEY_SDA_DIR_IN()             st(P2SEL &= ~BV(3); P2DIR &= ~BV(3); P2INP |= BV(3);)
// Keypad value setting
#define HAL_KEYPAD_SDA_VAL()             (HAL_KEYPAD_SDA_PORT)

/********************************************************************
// @fn      halKeypadInit
// @brief   Initializes the keypad functionality
********************************************************************/
extern void halKeypadInit(void);

/********************************************************************
// @fn     halKeypadPushed
// @brief  This function detects if the keypad is being pushed.
********************************************************************/
extern uint8 halKeypadPushed(void);

/*******************************************************************************
 *                                 FUNCTIONS - API
 ******************************************************************************/
/*
 * Start the keypad I2C
 */
extern void halKeypadI2cStart(void);

/*
 * Stop the keypad I2C
 */
extern void halKeypadI2cStop(void);

/*
 * The keypad I2C Ack
 */
extern void halKeypadI2cAck(void);

/*
 * The keypad I2C NoAck
 */
extern void halKeypadI2cNack(void);

/*
 * The keypad I2C Write
 */
extern void halKeypadI2cWrite(uint8 d);

/*
 * The keypad I2C Read
 */
extern uint8 halKeypadI2cRead(void);

/*
 * The keypad I2C Write Register
 */
extern void halKeypadWriteRegister(uint8 addr, uint8 ch);

/*
 * The keypad I2C Read Register
 */
extern uint8 halKeypadReadRegister(uint8 addr);

/*
 * The keypad Sensor Data
 */
extern uint16 halKeypadSensorData(void);
 