//-------------------------------------------------------------------
// Filename: hal_relay.c
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_mcu.h"
#include "hal_key.h"
#include "hal_board.h"
#include "hal_relay.h"

//-------------------------------------------------------------------
void RELAY_Init(void)
{   
    // switch init
    SWITCH1_INPUT();
    SWITCH2_INPUT();
    
    // relay init
    HAL_RELAY1_OUTPUT();
    HAL_RELAY2_OUTPUT();
    HAL_RELAY1_OFF();
    HAL_RELAY2_OFF();
}

//-------------------------------------------------------------------
void RELAY_SetDO(uint8 udo)
{
	if(udo == 0)
        {
          HAL_RELAY2_ON();
          HAL_RELAY1_OFF();
        }
	else
          {
            HAL_RELAY1_ON();
            HAL_RELAY2_OFF();
          }
}

//-------------------------------------------------------------------
uint8 RELAY_GetDI(void)
{
	uint8 udi;

	udi=0x03;
        
	if (HAL_SW_1_PUSH())
        {
	  udi&=~(0x01);
	}
        
        if (HAL_SW_2_PUSH())
        {
	  udi&=~(0x02);
	}
        return udi;
}