//-------------------------------------------------------------------
// Filename: M200.h (·ÅÀã«×°»´ú)_I2C
// Description: hal M200 module library header file
//-------------------------------------------------------------------
/********************************************************************
// I2C PORT,PIN
********************************************************************/ 
// SDA
#define HAL_I2C_SDA_PORT       P0_4
#define HAL_I2C_SDA_PIN        BV(4)
#define HAL_I2C_SDA_DDR        P0DIR
#define HAL_I2C_SDA_SEL        P0SEL
// SCL
#define HAL_I2C_SCL_PORT       P0_5
#define HAL_I2C_SCL_PIN        BV(5)
#define HAL_I2C_SCL_DDR        P0DIR
#define HAL_I2C_SCL_SEL        P0SEL

/******************************************************************
// I2C status setting
******************************************************************/
// High, Low status setting       
#define HAL_I2C_SDA_SET()             st(HAL_I2C_SDA_PORT = 1;)
#define HAL_I2C_SDA_CLR()             st(HAL_I2C_SDA_PORT = 0;)
#define HAL_I2C_SCL_SET()             st(HAL_I2C_SCL_PORT = 1;)
#define HAL_I2C_SCL_CLR()             st(HAL_I2C_SCL_PORT = 0;)
// Input, Output status setting
#define HAL_I2C_SDA_DIR_OUT()         st(P0SEL &= ~BV(4); P0DIR |= BV(4);)
#define HAL_I2C_SCL_DIR_OUT()         st(P0SEL &= ~BV(5); P0DIR |= BV(5);)
#define HAL_I2C_SDA_DIR_IN()          st(P0SEL &= ~BV(4); P0DIR &= ~BV(4); P0INP |= BV(4);)
// Get value setting
#define HAL_I2C_SDA_VAL()             (HAL_I2C_SDA_PORT) 

//-------------------------------
// General variable
//-------------------------------
extern uint16 M200_Humi;
extern uint16 M200_Temp;

//--------------------------------------------------------------
// GLOBAL FUNCTIONS
//--------------------------------------------------------------
//-------------------------------
// Initilization the M200 module
//-------------------------------
extern void M200_Init(void);

//-------------------------------
// Initilization the M200 module
//-------------------------------
extern void M200_GetValue(void);

//-----------------------------------------
// Initilization the I2C of the M200 module 
//-----------------------------------------
extern void SHT_Start(void);

//-------------------------------
// Write a byte to M200 module
//-------------------------------
extern uint8 SHT_WriteByte(uint8 dat);

//-------------------------------
// read a byte to  M200 module
//-------------------------------
extern uint8 SHT_ReadByte(uint8 ack);

//-------------------------------
// reset the M200 module connect
//-------------------------------
extern void SHT_ConnectReset(void);