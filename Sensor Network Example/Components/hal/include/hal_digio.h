//-------------------------------------------------------------------
// Filename: hal_digio.h
// Description: HAL digital IO functionality
//-------------------------------------------------------------------

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

//-------------------------------------------------------------------
// CONSTANTS AND DEFINES
//-------------------------------------------------------------------
enum
{
    HAL_DIGIO_INPUT, HAL_DIGIO_OUTPUT
};

enum
{
    HAL_DIGIO_INT_FALLING_EDGE, HAL_DIGIO_INT_RISING_EDGE
};

#define HAL_DIGIO_OK       0
#define HAL_DIGIO_ERROR  (~0)

//-------------------------------------------------------------------
// TYPEDEFS
//-------------------------------------------------------------------
typedef struct
{
    uint8 port; // port number
    uint8 pin; // pin number
    uint8 pin_bm; // pin bitmask
    uint8 dir; // direction (input or output)
    uint8 initval; // initial value
} digioConfig;

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
/***************************************************
 * Configure the pin as specified by p
 **************************************************/
extern uint8 halDigioConfig(const digioConfig *p);

/***************************************************
 * Clear output pin
 **************************************************/
extern uint8 halDigioClear(const digioConfig *p);

/***************************************************
 * Toggle output pin
 **************************************************/
extern uint8 halDigioToggle(const digioConfig *p);

/***************************************************
 * Set output pin
 **************************************************/
extern uint8 halDigioSet(const digioConfig *p);

/***************************************************
 * Get value on input pin
 **************************************************/
extern uint8 halDigioGet(const digioConfig *p);

/************************************************************************
 * Connect function to IO interrupt
 ***********************************************************************/
extern uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func);

/***************************************************
 * Enable interrupt on IO pin
 **************************************************/
extern uint8 halDigioIntEnable(const digioConfig *p);

/***************************************************
 * Disable interrupt on IO pin
 **************************************************/
extern uint8 halDigioIntDisable(const digioConfig *p);

/***************************************************
 * Clear interrupt flag
 **************************************************/
extern uint8 halDigioIntClear(const digioConfig *p);

/***************************************************
 * Set edge for IO interrupt
 **************************************************/
extern uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge);