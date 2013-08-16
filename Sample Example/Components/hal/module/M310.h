//-------------------------------------------------------------------
// Filename: M310.h
// Description: hal M310 module library header file
//-------------------------------------------------------------------
/***************************************************
// SEN PORT, PIN Setting
 **************************************************/
// SEN1
#define HAL_SEN_1_PORT    P0_4
#define HAL_SEN_1_PIN     BV(4)
#define HAL_SEN_2_PORT    P0_6
#define HAL_SEN_2_PIN     BV(6)

#define HAL_SEN_1_DDR       P0DIR
#define HAL_SEN_1_SEL       P0SEL
#define HAL_SEN_2_DDR       P0DIR
#define HAL_SEN_2_SEL       P0SEL

#define HAL_LED_1_PORT    P0_5
#define HAL_LED_1_PIN     BV(5)
#define HAL_LED_2_PORT    P0_7
#define HAL_LED_2_PIN     BV(7)

#define HAL_LED_DDR       P0DIR
#define HAL_LED_SEL       P0SEL

#define HAL_SEN_1_DETECT()  !(P0 & BV(4))
#define HAL_SEN_2_DETECT()  !(P0 & BV(6))

// Setting SEN Input and Output
#define HAL_SEN_1_INPUT()   st(P0SEL &= ~BV(4); P0DIR &= ~BV(4);)
#define HAL_SEN_1_OUTPUT()  st(P0SEL &= ~BV(4); P0DIR |= BV(4);)
#define HAL_SEN_2_INPUT()   st(P0SEL &= ~BV(6); P0DIR &= ~BV(6);)
#define HAL_SEN_2_OUTPUT()  st(P0SEL &= ~BV(6); P0DIR |= BV(6);)

// Setting LED Input and Output
#define HAL_LED_1_INPUT()   st(P0SEL &= ~BV(5); P0DIR &= ~BV(5);)
#define HAL_LED_1_OUTPUT()  st(P0SEL &= ~BV(5); P0_5 = 1; P0DIR |= BV(5);)
#define HAL_LED_2_INPUT()   st(P0SEL &= ~BV(7); P0DIR &= ~BV(7);)
#define HAL_LED_2_OUTPUT()  st(P0SEL &= ~BV(7); P0_7 = 1; P0DIR |= BV(7);)

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
extern uint8 udi;
extern void M310_Init(void);
extern uint8 M310_GetSensor(void);