//-------------------------------------------------------------------
// Filename: M100.h
// Description: RTC library header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// PORT, PIN Setting
//-------------------------------------------------------------------
// RTC_SCK
#define HAL_RTC_SCK_PORT    P1_3
#define HAL_RTC_SCK_PIN     BV(3)
#define HAL_RTC_SCK_DDR     P1DIR
#define HAL_RTC_SCK_SEL     P1SEL
// RTC_SIO
#define HAL_RTC_SIO_PORT    P1_2
#define HAL_RTC_SIO_PIN     BV(2)
#define HAL_RTC_SIO_DDR     P1DIR
#define HAL_RTC_SIO_SEL     P1SEL
// RTC_CS
#define HAL_RTC_CS_PORT    P1_4
#define HAL_RTC_CS_PIN     BV(4)
#define HAL_RTC_CS_DDR     P1DIR
#define HAL_RTC_CS_SEL     P1SEL


#define HAL_RTC_SCK_DIR_OUT()          st(P1SEL &= ~BV(3); HAL_RTC_SCK_PORT = 1; P1DIR |= BV(3);)
#define HAL_RTC_SCK_SET()              st(HAL_RTC_SCK_PORT = 1;)
#define HAL_RTC_SCK_CLR()              st(HAL_RTC_SCK_PORT = 0;)

#define HAL_RTC_SIO_DIR_OUT()          st(P1SEL &= ~BV(2); HAL_RTC_SIO_PORT = 1; P1DIR |= BV(2);)
#define HAL_RTC_SIO_DIR_IN()           st(P1SEL &= ~BV(2); P1DIR &= ~BV(2); P1INP &= ~BV(2);)
#define HAL_RTC_SIO_SET()              st(HAL_RTC_SIO_PORT = 1;)
#define HAL_RTC_SIO_CLR()              st(HAL_RTC_SIO_PORT = 0;)
#define HAL_RTC_SIO_VAL()              (HAL_RTC_SIO_PORT & BV(2))

#define HAL_RTC_CS_DIR_OUT()           st(P1SEL &= ~BV(4); HAL_RTC_CS_PORT = 1; P1DIR |= BV(4);)
#define HAL_RTC_CS_EN()                st(HAL_RTC_CS_PORT = 1;)
#define HAL_RTC_CS_DIS()               st(HAL_RTC_CS_PORT = 0;)

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
extern uint16 Year;
extern uint8 Month, Day, Hour, Min, Sec;

void RTC_Init(void);
void SetDateTime(void);
void GetDateTime(void);
