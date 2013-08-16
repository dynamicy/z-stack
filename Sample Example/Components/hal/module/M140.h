//-------------------------------------------------------------------
// Filename: M140.h (·Å«×°»´ú)_SPI
// Description: hal M140 library header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------


/***************************************************
// SPI PORT, PIN Setting
 **************************************************/
// SPI_MISO
#define HAL_SPI_MISO_PORT    P0_6
#define HAL_SPI_MISO_PIN     BV(6)
#define HAL_SPI_MISO_DDR     P0DIR
#define HAL_SPI_MISO_SEL     P0SEL
// SPI_MOSI
#define HAL_SPI_MOSI_PORT    P0_4
#define HAL_SPI_MOSI_PIN     BV(4)
#define HAL_SPI_MOSI_DDR     P0DIR
#define HAL_SPI_MOSI_SEL     P0SEL
// SPI_CLK
#define HAL_SPI_CLK_PORT     P0_5
#define HAL_SPI_CLK_PIN      BV(5)
#define HAL_SPI_CLK_DDR      P0DIR
#define HAL_SPI_CLK_SEL      P0SEL
// SPI_CS
#define HAL_SPI_CS_PORT      P0_7
#define HAL_SPI_CS_PIN       BV(7)
#define HAL_SPI_CS_DDR       P0DIR
#define HAL_SPI_CS_SEL       P0SEL

/***************************************************
// SPI Command PIN Function
 **************************************************/   
// Setting SPI_MOSI,CLK,CS,MISO high and low
#define HAL_SPI_MOSI_SET()  st(HAL_SPI_MOSI_PORT = 1;)
#define HAL_SPI_MOSI_CLR()  st(HAL_SPI_MOSI_PORT = 0;)
#define HAL_SPI_CLK_SET()   st(HAL_SPI_CLK_PORT = 1;)
#define HAL_SPI_CLK_CLR()   st(HAL_SPI_CLK_PORT = 0;)
#define HAL_SPI_CS_EN()     st(HAL_SPI_CS_PORT = 0;)
#define HAL_SPI_CS_DIS()    st(HAL_SPI_CS_PORT = 1;) 

//SPI MISO value setting
#define HAL_SPI_MISO_VAL()  (HAL_SPI_MISO_PORT)

// Setting SPI Input and Output
#define HAL_SPI_MOSI_INPUT()  st(P0SEL &= ~BV(4); P0DIR &= ~BV(4);)
#define HAL_SPI_MOSI_OUTPUT() st(P0SEL &= ~BV(4); P0DIR |= BV(4);)
#define HAL_SPI_CLK_INPUT()   st(P0SEL &= ~BV(5); P0DIR &= ~BV(5);)
#define HAL_SPI_CLK_OUTPUT()  st(P0SEL &= ~BV(5); P0DIR |= BV(5);)
#define HAL_SPI_CS_INPUT()    st(P0SEL &= ~BV(7); P0DIR &= ~BV(7);
#define HAL_SPI_CS_OUTPUT()   st(P0SEL &= ~BV(7); P0DIR |= BV(7);)
#define HAL_SPI_MISO_INPUT()  st(P0SEL &= ~BV(6); P0DIR &= ~BV(6);)
#define HAL_SPI_MISO_OUTPUT() st(P0SEL &= ~BV(6); P0DIR |= BV(6);)

/***************************************************
 * Initilization the M140 module
 **************************************************/
extern void M140_Init(void);

/***************************************************
 * Get the M140 module value
 **************************************************/
extern uint16 M140_GetValue(void);

/***************************************************
 * Enable the M140 module
 **************************************************/
extern void M140_Enable(void);

