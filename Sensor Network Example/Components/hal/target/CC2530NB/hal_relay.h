//-------------------------------------------------------------------
// Filename: hal_relay.h
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// PORT, PIN Setting
//-------------------------------------------------------------------
// RELAY_1
#define HAL_RELAY_1_PORT    P1_0
#define HAL_RELAY_1_PIN     BV(0)
#define HAL_RELAY_1_DDR     P1DIR
#define HAL_RELAY_1_SEL     P1SEL
// RELAY_2
#define HAL_RELAY_2_PORT    P1_1
#define HAL_RELAY_2_PIN     BV(1)
#define HAL_RELAY_2_DDR     P1DIR
#define HAL_RELAY_2_SEL     P1SEL

#define HAL_RELAY1_OFF()    st(HAL_RELAY_1_PORT = 1;)
#define HAL_RELAY1_ON()     st(HAL_RELAY_1_PORT = 0;)
#define HAL_RELAY2_OFF()    st(HAL_RELAY_2_PORT = 1;)
#define HAL_RELAY2_ON()     st(HAL_RELAY_2_PORT = 0;)

#define HAL_RELAY1_OUTPUT() st(P1SEL &= ~BV(0); HAL_RELAY_1_PORT = 1; P1DIR |= BV(0);)
#define HAL_RELAY1_INPUT()  st(P1SEL &= ~BV(0); P1DIR &= ~BV(0); P1INP &= ~BV(0);)
#define HAL_RELAY2_OUTPUT() st(P1SEL &= ~BV(1); HAL_RELAY_2_PORT = 1; P1DIR |= BV(1);)
#define HAL_RELAY2_INPUT()  st(P1SEL &= ~BV(1); P1DIR &= ~BV(1); P1INP &= ~BV(1);)

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
extern void RELAY_Init(void);
extern void RELAY_SetDO(uint8 udo);
extern uint8 RELAY_GetDI(void);