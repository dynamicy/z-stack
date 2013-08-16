//-------------------------------------------------------------------
// Filename: M270.c
// Description: hal M270 module library Ä~¹q¾¹/«öÁä(DI/DO)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "M270.h"

//-------------------------------------------------------------------
void M270_Init(void)
{
    MCU_IO_INPUT(HAL_BOARD_KEY_1_PORT, HAL_BOARD_KEY_1_PIN, MCU_IO_PULLUP);
    MCU_IO_INPUT(HAL_BOARD_KEY_2_PORT, HAL_BOARD_KEY_2_PIN, MCU_IO_PULLUP);
    MCU_IO_OUTPUT(HAL_BOARD_RELAY_PORT, HAL_BOARD_RELAY_PIN,1);
    HAL_RELAY_OFF();
}

//-------------------------------------------------------------------
void M270_SetDO(uint8 udo)
{
	if(udo==0)
		HAL_RELAY_OFF();
	else
		HAL_RELAY_ON();
}

//-------------------------------------------------------------------
uint8 M270_GetDI(void)
{
	uint8 udi;

	udi=0x03;
	if(HAL_KEY_1_PUSHED())
		udi&=~(0x01);
	if(HAL_KEY_2_PUSHED())
		udi&=~(0x02);
	return udi;
}
